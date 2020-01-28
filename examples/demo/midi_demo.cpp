#include <algorithm>
#include <chrono>
#include <functional>
#include <memory>
#include <thread>
#include <utility>
#include <variant>
#include <vector>

#include "MidiFile.h"
#include "audio_output/pa_audio_output.h"
#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/instrument/instrument.h"
#include "barelymusician/message/message_buffer.h"
#include "barelymusician/message/message_data.h"
#include "instruments/basic_synth_instrument.h"
#include "util/input_manager/win_console_input.h"

namespace {

using ::barelyapi::Instrument;
using ::barelyapi::MessageBuffer;
using ::barelyapi::NoteOffData;
using ::barelyapi::NoteOnData;
using ::barelyapi::OscillatorType;
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
const double kTempo = 132.0;

// Performer settings.
const int kNumInstrumentVoices = 16;
const float kInstrumentGain = 1.0f / static_cast<float>(kNumInstrumentVoices);
const float kInstrumentEnvelopeAttack = 0.0f;
const float kInstrumentEnvelopeRelease = 0.2f;
const OscillatorType kInstrumentOscillatorType = OscillatorType::kSquare;

const float kMaxVelocity = 127.0f;

// Midi file name.
const char kMidiFileName[] = "data/midi/sample.mid";

// Instrument message processor.
struct MessageProcessor {
  // Processes |NoteOffData|.
  void operator()(const NoteOffData& note_off_data) {
    instrument->NoteOff(note_off_data.index);
  }

  // Processes |NoteOnData|.
  void operator()(const NoteOnData& note_on_data) {
    instrument->NoteOn(note_on_data.index, note_on_data.intensity);
  }

  // Instrument to process.
  Instrument* instrument;
};

MessageBuffer BuildScore(const smf::MidiEventList& midi_events,
                         int ticks_per_beat, int samples_per_beat) {
  const auto get_timestamp = [ticks_per_beat, samples_per_beat](int tick) {
    return static_cast<double>(samples_per_beat) * static_cast<double>(tick) /
           static_cast<double>(ticks_per_beat);
  };

  MessageBuffer score;
  for (int i = 0; i < midi_events.size(); ++i) {
    const auto& midi_event = midi_events[i];
    if (midi_event.isNoteOn()) {
      const float index = static_cast<float>(midi_event.getKeyNumber());
      const float intensity =
          static_cast<float>(midi_event.getVelocity()) / kMaxVelocity;
      const double timestamp = get_timestamp(midi_event.tick);
      score.Push({NoteOnData{index, intensity}, timestamp});
    } else if (midi_event.isNoteOff()) {
      const float index = static_cast<float>(midi_event.getKeyNumber());
      const double timestamp = get_timestamp(midi_event.tick);
      score.Push({NoteOffData{index}, timestamp});
    }
  }
  return score;
}

void ProcessInstrument(float* output, int num_channels, int num_frames,
                       const MessageBuffer::Iterator& notes,
                       Instrument* instrument) {
  int frame = 0;
  // Process notes.
  for (auto it = notes.cbegin; it != notes.cend; ++it) {
    const int note_frame = static_cast<int>(it->timestamp - notes.timestamp);
    if (frame < note_frame) {
      instrument->Process(&output[num_channels * frame], num_channels,
                          note_frame - frame);
      frame = note_frame;
    }
    std::visit(MessageProcessor{instrument}, it->data);
  }
  // Process the rest of the buffer.
  if (frame < num_frames) {
    instrument->Process(&output[num_channels * frame], num_channels,
                        num_frames - frame);
  }
}

}  // namespace

int main(int argc, char* argv[]) {
  PaAudioOutput audio_output;
  WinConsoleInput input_manager;

  MidiFile midi_file;
  CHECK(midi_file.read(kMidiFileName)) << "Failed to read " << kMidiFileName;
  CHECK(midi_file.isAbsoluteTicks()) << "Events should be in absolute ticks";

  const int num_tracks = midi_file.getTrackCount();
  const int ticks_per_quarter = midi_file.getTPQ();
  LOG(INFO) << "Initializing " << kMidiFileName << " for MIDI playback ("
            << num_tracks << " tracks, " << ticks_per_quarter << " TPQ)";

  const int samples_per_beat =
      static_cast<int>(static_cast<double>(kSampleRate) *
                       barelyapi::kSecondsFromMinutes / kTempo);

  std::vector<std::pair<std::unique_ptr<Instrument>, MessageBuffer>> performers;

  for (int i = 0; i < num_tracks; ++i) {
    // Build score.
    MessageBuffer score =
        BuildScore(midi_file[i], ticks_per_quarter, samples_per_beat);
    if (score.Empty()) {
      LOG(WARNING) << "Empty track: " << i;
      continue;
    }
    // Create instrument.
    auto instrument = std::make_unique<BasicSynthInstrument>(
        kSampleInterval, kNumInstrumentVoices);
    instrument->SetFloatParam(BasicSynthInstrumentParam::kOscillatorType,
                              static_cast<float>(kInstrumentOscillatorType));
    instrument->SetFloatParam(BasicSynthInstrumentParam::kEnvelopeAttack,
                              kInstrumentEnvelopeAttack);
    instrument->SetFloatParam(BasicSynthInstrumentParam::kEnvelopeRelease,
                              kInstrumentEnvelopeRelease);
    instrument->SetFloatParam(BasicSynthInstrumentParam::kGain,
                              kInstrumentGain);
    // Create performer.
    performers.emplace_back(std::move(instrument), std::move(score));
  }
  LOG(INFO) << "Number of instruments: " << performers.size();

  // Audio process callback.
  std::vector<float> temp_buffer(kNumChannels * kNumFrames);
  int timestamp = 0;
  const auto process_callback = [&performers, &temp_buffer,
                                 &timestamp](float* output) {
    const double begin_timestamp = static_cast<double>(timestamp);
    timestamp += kNumFrames;
    const double end_timestamp = static_cast<double>(timestamp);

    std::fill_n(output, kNumChannels * kNumFrames, 0.0f);
    for (const auto& performer : performers) {
      const auto notes =
          performer.second.GetIterator(begin_timestamp, end_timestamp);
      ProcessInstrument(temp_buffer.data(), kNumChannels, kNumFrames, notes,
                        performer.first.get());
      std::transform(temp_buffer.cbegin(), temp_buffer.cend(), output, output,
                     std::plus<float>());
    }
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
  input_manager.SetKeyDownCallback(key_down_callback);

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
