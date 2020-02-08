#include <algorithm>
#include <chrono>
#include <memory>
#include <thread>
#include <utility>
#include <vector>

#include "MidiFile.h"
#include "audio_output/pa_audio_output.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/engine/clock.h"
#include "barelymusician/engine/performer.h"
#include "barelymusician/instrument/instrument.h"
#include "instruments/basic_synth_instrument.h"
#include "util/input_manager/win_console_input.h"

namespace {

using ::barelyapi::Clock;
using ::barelyapi::OscillatorType;
using ::barelyapi::Performer;
using ::barelyapi::examples::BasicSynthInstrument;
using ::barelyapi::examples::BasicSynthInstrumentParam;
using ::barelyapi::examples::PaAudioOutput;
using ::barelyapi::examples::WinConsoleInput;
using ::smf::MidiFile;

// System audio settings.
const int kSampleRate = 48000;
const int kNumChannels = 2;
const int kNumFrames = 512;

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

// Schedules |midi_events| to be performed by |performer|.
bool ScheduleMidiEvents(const smf::MidiEventList& midi_events,
                        int ticks_per_beat, Performer* performer) {
  const auto get_position = [ticks_per_beat](int tick) -> double {
    return static_cast<double>(tick) / static_cast<double>(ticks_per_beat);
  };
  bool any_scheduled = false;
  for (int i = 0; i < midi_events.size(); ++i) {
    const auto& midi_event = midi_events[i];
    if (midi_event.isNoteOn()) {
      const float index = static_cast<float>(midi_event.getKeyNumber());
      const float intensity =
          static_cast<float>(midi_event.getVelocity()) / kMaxVelocity;
      const double position = get_position(midi_event.tick);
      performer->ScheduleNoteOn(position, index, intensity);
      any_scheduled = true;
    } else if (midi_event.isNoteOff()) {
      const float index = static_cast<float>(midi_event.getKeyNumber());
      const double position = get_position(midi_event.tick);
      performer->ScheduleNoteOff(position, index);
      any_scheduled = true;
    }
  }
  return any_scheduled;
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

  Clock clock(kSampleRate);
  clock.SetTempo(kTempo);

  std::vector<Performer> performers;
  for (int i = 0; i < num_tracks; ++i) {
    // Create performer.
    Performer performer;
    if (!ScheduleMidiEvents(midi_file[i], ticks_per_quarter, &performer)) {
      LOG(WARNING) << "Empty MIDI track: " << i;
      continue;
    }
    performer.SetNoteOnCallback([i](float index, float intensity) {
      LOG(INFO) << "MIDI track #" << i << ": NoteOn(" << index << ", "
                << intensity << ")";
    });
    performer.SetNoteOffCallback([i](float index) {
      LOG(INFO) << "MIDI track #" << i << ": NoteOff(" << index << ") ";
    });
    // Set instrument.
    auto instrument = std::make_unique<BasicSynthInstrument>(
        kSampleRate, kNumInstrumentVoices);
    instrument->SetFloatParam(BasicSynthInstrumentParam::kOscillatorType,
                              static_cast<float>(kInstrumentOscillatorType));
    instrument->SetFloatParam(BasicSynthInstrumentParam::kEnvelopeAttack,
                              kInstrumentEnvelopeAttack);
    instrument->SetFloatParam(BasicSynthInstrumentParam::kEnvelopeRelease,
                              kInstrumentEnvelopeRelease);
    instrument->SetFloatParam(BasicSynthInstrumentParam::kGain,
                              kInstrumentGain);
    performer.SetInstrument(std::move(instrument));
    // Add performer.
    performers.push_back(std::move(performer));
  }
  LOG(INFO) << "Number of active MIDI tracks: " << performers.size();

  // Audio process callback.
  std::vector<float> temp_buffer(kNumChannels * kNumFrames);
  const auto process_callback = [&clock, &performers,
                                 &temp_buffer](float* output) {
    const double start_position = clock.GetPosition();
    clock.UpdatePosition(kNumFrames);
    const double end_position = clock.GetPosition();

    std::fill_n(output, kNumChannels * kNumFrames, 0.0f);
    for (Performer& performer : performers) {
      performer.Process(start_position, end_position, temp_buffer.data(),
                        kNumChannels, kNumFrames);
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
