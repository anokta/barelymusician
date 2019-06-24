#include <algorithm>
#include <chrono>
#include <functional>
#include <thread>
#include <utility>
#include <vector>

#include "MidiFile.h"
#include "audio_output/pa_audio_output.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/base/sequencer.h"
#include "barelymusician/base/transport.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/dsp/dsp_utils.h"
#include "barelymusician/instrument/instrument.h"
#include "barelymusician/instrument/instrument_utils.h"
#include "barelymusician/message/message_buffer.h"
#include "instruments/basic_synth_instrument.h"
#include "util/input_manager/win_console_input.h"

namespace {

using ::barelyapi::Instrument;
using ::barelyapi::MessageBuffer;
using ::barelyapi::Note;
using ::barelyapi::OscillatorType;
using ::barelyapi::Process;
using ::barelyapi::PushNoteOffMessage;
using ::barelyapi::PushNoteOnMessage;
using ::barelyapi::SamplesFromBeats;
using ::barelyapi::Sequencer;
using ::barelyapi::Transport;
using ::barelyapi::examples::BasicSynthInstrument;
using ::barelyapi::examples::BasicSynthInstrumentParam;
using ::barelyapi::examples::PaAudioOutput;
using ::barelyapi::examples::WinConsoleInput;
using ::smf::MidiFile;

// System audio settings.
const int kSampleRate = 48000;
const int kNumChannels = 2;
const int kNumFrames = 512;

const float kSampleInterval = 1.0f / static_cast<float>(kSampleRate);

// Sequencer settings.
const float kTempo = 132.0f;

// Ensemble settings.
const int kNumInstrumentVoices = 16;

// Midi file name.
const char kMidiFileName[] = "data/midi/sample.mid";

std::vector<Note> GetMidiScore(const smf::MidiEventList& midi_events,
                               int ticks_per_quarter) {
  std::vector<Note> score;
  const float ticks_per_beat = static_cast<float>(ticks_per_quarter);
  const float max_velocity = 127.0f;
  for (int i = 0; i < midi_events.size(); ++i) {
    const auto& midi_event = midi_events[i];
    if (midi_event.isNoteOn()) {
      Note note;
      note.index = static_cast<float>(midi_event.getKeyNumber());
      note.intensity =
          static_cast<float>(midi_event.getVelocity()) / max_velocity;
      note.start_beat = static_cast<float>(midi_event.tick) / ticks_per_beat;
      note.duration_beats =
          static_cast<float>(midi_event.getTickDuration()) / ticks_per_beat;
      score.push_back(note);
    }
  }
  return score;
}

std::vector<Note> GetBeatNotes(const std::vector<Note>& score,
                               const Transport& transport) {
  std::vector<Note> notes;
  const float beat = static_cast<float>(transport.beat);
  const auto compare_beat = [](const Note& note, float start_beat) {
    return note.start_beat < start_beat;
  };
  const auto begin =
      std::lower_bound(score.begin(), score.end(), beat, compare_beat);
  const auto end =
      std::lower_bound(begin, score.end(), beat + 1.0f, compare_beat);
  for (auto it = begin; it != end; ++it) {
    notes.push_back(*it);
  }
  return notes;
}

}  // namespace

int main(int argc, char* argv[]) {
  PaAudioOutput audio_output;
  WinConsoleInput input_manager;

  MidiFile midi_file;
  CHECK(midi_file.read(kMidiFileName)) << "Failed to read " << kMidiFileName;
  midi_file.linkNotePairs();

  const int num_tracks = midi_file.getTrackCount();
  const int ticks_per_quarter = midi_file.getTPQ();
  LOG(INFO) << "Initializing " << kMidiFileName << " for MIDI playback ("
            << num_tracks << " tracks, " << ticks_per_quarter << " TPQ)";

  Sequencer sequencer(kSampleRate);
  sequencer.SetTempo(kTempo);

  std::vector<std::vector<Note>> scores;
  std::vector<std::pair<BasicSynthInstrument, MessageBuffer>> performers;
  for (int i = 0; i < num_tracks; ++i) {
    // Create instrument.
    const auto score = GetMidiScore(midi_file[i], ticks_per_quarter);
    if (score.empty()) {
      continue;
    }
    scores.push_back(score);
    // Create instrument.
    BasicSynthInstrument instrument(kSampleInterval, kNumInstrumentVoices);
    instrument.SetFloatParam(BasicSynthInstrumentParam::kOscillatorType,
                             static_cast<float>(OscillatorType::kSquare));
    instrument.SetFloatParam(BasicSynthInstrumentParam::kEnvelopeAttack, 0.0f);
    instrument.SetFloatParam(BasicSynthInstrumentParam::kEnvelopeRelease, 0.2f);
    instrument.SetFloatParam(BasicSynthInstrumentParam::kGain, 0.1f);
    // Create performer.
    performers.emplace_back(std::move(instrument), MessageBuffer());
  }
  LOG(INFO) << "Number of performers: " << performers.size();

  // Beat callback.
  const auto beat_callback = [&performers, &scores](const Transport& transport,
                                                    int start_sample,
                                                    int num_samples_per_beat) {
    const int num_performers = static_cast<int>(performers.size());
    for (int i = 0; i < num_performers; ++i) {
      MessageBuffer* message_buffer = &performers[i].second;
      for (const Note& note : GetBeatNotes(scores[i], transport)) {
        const int start_offset_samples =
            start_sample +
            SamplesFromBeats(note.start_beat, num_samples_per_beat);
        PushNoteOnMessage(note.index, note.intensity, start_offset_samples,
                          message_buffer);
        const int end_offset_samples =
            start_offset_samples +
            SamplesFromBeats(note.duration_beats, num_samples_per_beat);
        PushNoteOffMessage(note.index, end_offset_samples, message_buffer);
      }
    }
  };
  sequencer.RegisterBeatCallback(beat_callback);

  // Audio process callback.
  std::vector<float> temp_buffer(kNumChannels * kNumFrames);
  int timestamp = 0;
  const auto process_callback = [&sequencer, &performers, &temp_buffer,
                                 &timestamp](float* output) {
    sequencer.Update(kNumFrames);
    std::fill_n(output, kNumChannels * kNumFrames, 0.0f);
    for (auto& performer : performers) {
      Instrument* instrument = &performer.first;
      const auto messages = performer.second.GetIterator(timestamp, kNumFrames);
      Process(instrument, messages, temp_buffer.data(), kNumChannels,
              kNumFrames);
      std::transform(temp_buffer.begin(), temp_buffer.end(), output, output,
                     std::plus<float>());
    }
    timestamp += kNumFrames;
  };
  audio_output.SetProcessCallback(process_callback);

  // Key down callback.
  bool quit = false;
  const auto key_down_callback = [&quit](const WinConsoleInput::Key& key) {
    if (static_cast<int>(key) == 27) {
      // ESC pressed, quit the app.
      quit = true;
      return;
    }
  };
  input_manager.RegisterKeyDownCallback(key_down_callback);

  // Start the demo.
  LOG(INFO) << "Starting audio stream";

  input_manager.Initialize();
  audio_output.Start(kSampleRate, kNumChannels, kNumFrames);

  while (!quit) {
    input_manager.Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  // Stop the demo.
  LOG(INFO) << "Stopping audio stream";

  audio_output.Stop();
  input_manager.Shutdown();

  return 0;
}
