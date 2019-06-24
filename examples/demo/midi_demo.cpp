#include <algorithm>
#include <chrono>
#include <functional>
#include <thread>
#include <utility>
#include <vector>

#include "MidiFile.h"
#include "audio_output/pa_audio_output.h"
#include "barelymusician/base/logging.h"
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
using ::barelyapi::examples::BasicSynthInstrument;
using ::barelyapi::examples::BasicSynthInstrumentParam;
using ::barelyapi::examples::PaAudioOutput;
using ::barelyapi::examples::WinConsoleInput;
using ::smf::MidiFile;

using Int64 = long long int;

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

MessageBuffer BuildMidiScore(const smf::MidiEventList& midi_events,
                             int ticks_per_beat, int samples_per_beat) {
  const auto get_timestamp = [ticks_per_beat, samples_per_beat](int tick) {
    return static_cast<int>(static_cast<Int64>(samples_per_beat) *
                            static_cast<Int64>(tick) /
                            static_cast<Int64>(ticks_per_beat));
  };
  MessageBuffer score;
  const float max_velocity = 127.0f;
  for (int i = 0; i < midi_events.size(); ++i) {
    const auto& midi_event = midi_events[i];
    if (midi_event.isNoteOn()) {
      const float index = static_cast<float>(midi_event.getKeyNumber());
      const float intensity =
          static_cast<float>(midi_event.getVelocity()) / max_velocity;
      const int timestamp = get_timestamp(midi_event.tick);
      PushNoteOnMessage(index, intensity, timestamp, &score);
    } else if (midi_event.isNoteOff()) {
      const float index = static_cast<float>(midi_event.getKeyNumber());
      const int timestamp = get_timestamp(midi_event.tick);
      PushNoteOffMessage(index, timestamp, &score);
    }
  }
  return score;
}

}  // namespace

int main(int argc, char* argv[]) {
  PaAudioOutput audio_output;
  WinConsoleInput input_manager;

  MidiFile midi_file;
  CHECK(midi_file.read(kMidiFileName)) << "Failed to read " << kMidiFileName;

  const int num_tracks = midi_file.getTrackCount();
  const int ticks_per_quarter = midi_file.getTPQ();
  LOG(INFO) << "Initializing " << kMidiFileName << " for MIDI playback ("
            << num_tracks << " tracks, " << ticks_per_quarter << " TPQ)";

  const int samples_per_beat =
      static_cast<int>(static_cast<float>(kSampleRate) *
                       barelyapi::kSecondsFromMinutes / kTempo);

  std::vector<std::pair<BasicSynthInstrument, MessageBuffer>> performers;
  for (int i = 0; i < num_tracks; ++i) {
    // Create score.
    const MessageBuffer score =
        BuildMidiScore(midi_file[i], ticks_per_quarter, samples_per_beat);
    if (score.Empty()) {
      continue;
    }
    // Create instrument.
    BasicSynthInstrument instrument(kSampleInterval, kNumInstrumentVoices);
    instrument.SetFloatParam(BasicSynthInstrumentParam::kOscillatorType,
                             static_cast<float>(OscillatorType::kSquare));
    instrument.SetFloatParam(BasicSynthInstrumentParam::kEnvelopeAttack, 0.0f);
    instrument.SetFloatParam(BasicSynthInstrumentParam::kEnvelopeRelease, 0.2f);
    instrument.SetFloatParam(BasicSynthInstrumentParam::kGain, 0.1f);
    // Create performer.
    performers.emplace_back(std::move(instrument), std::move(score));
  }
  LOG(INFO) << "Number of performers: " << performers.size();

  // Audio process callback.
  std::vector<float> temp_buffer(kNumChannels * kNumFrames);
  int timestamp = 0;
  const auto process_callback = [&performers, &temp_buffer,
                                 &timestamp](float* output) {
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
