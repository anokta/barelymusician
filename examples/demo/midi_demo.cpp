#include <algorithm>
#include <cassert>
#include <chrono>
#include <functional>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "MidiEventList.h"
#include "MidiFile.h"
#include "barelymusician/barelymusician.h"
#include "barelymusician/composition/midi.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/instruments/synth_instrument.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"
#include "examples/data/data.h"

namespace {

using ::barely::Instrument;
using ::barely::IntensityFromMidiVelocity;
using ::barely::MidiNumberFromPitch;
using ::barely::MidiVelocityFromIntensity;
using ::barely::Musician;
using ::barely::NotePtr;
using ::barely::OscillatorType;
using ::barely::Performer;
using ::barely::PitchFromMidiNumber;
using ::barely::SynthInstrument;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::GetDataFilePath;
using ::barely::examples::InputManager;
using ::smf::MidiFile;

// System audio settings.
constexpr int kFrameRate = 48000;
constexpr int kChannelCount = 2;
constexpr int kFrameCount = 512;

constexpr double kLookahead = 0.1;

// Instrument settings.
constexpr OscillatorType kInstrumentOscillatorType = OscillatorType::kSquare;
constexpr double kInstrumentEnvelopeAttack = 0.0;
constexpr double kInstrumentEnvelopeRelease = 0.2;
constexpr int kInstrumentVoiceCount = 16;
constexpr double kInstrumentGain = 1.0 / static_cast<double>(kInstrumentVoiceCount);

// Midi file name.
constexpr char kMidiFileName[] = "midi/sample.mid";

constexpr double kTempo = 132.0;

// Builds the score for the given `midi_events`.
bool BuildScore(const smf::MidiEventList& midi_events, int track_index, int ticks_per_beat,
                Instrument& instrument, Performer& performer) {
  const auto get_position_fn = [ticks_per_beat](int tick) -> double {
    return static_cast<double>(tick) / static_cast<double>(ticks_per_beat);
  };
  bool has_notes = false;
  for (int i = 0; i < midi_events.size(); ++i) {
    const auto& midi_event = midi_events[i];
    if (midi_event.isNoteOn()) {
      const double position = get_position_fn(midi_event.tick);
      const double duration = get_position_fn(midi_event.getTickDuration());
      const double pitch = PitchFromMidiNumber(midi_event.getKeyNumber());
      const double intensity = IntensityFromMidiVelocity(midi_event.getVelocity());
      performer.ScheduleOneOffTask(
          [&instrument, &performer, track_index, position, duration, pitch, intensity]() mutable {
            performer.ScheduleOneOffTask(
                [track_index, pitch,
                 note = NotePtr::Create(instrument, pitch, intensity)]() mutable {
                  NotePtr::Destroy(note);
                  ConsoleLog() << "MIDI track #" << track_index
                               << ": NoteOff(key: " << MidiNumberFromPitch(pitch) << ")";
                },
                position + duration);
            ConsoleLog() << "MIDI track #" << track_index
                         << ": NoteOn(key: " << MidiNumberFromPitch(pitch)
                         << ", velocity: " << MidiVelocityFromIntensity(intensity) << ")";
          },
          position);
      has_notes = true;
    }
  }
  return has_notes;
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int /*argc*/, char* argv[]) {
  AudioOutput audio_output;
  InputManager input_manager;

  MidiFile midi_file;
  const std::string midi_file_path = GetDataFilePath(kMidiFileName, argv);
  [[maybe_unused]] const bool success = midi_file.read(midi_file_path);
  assert(success && midi_file.isAbsoluteTicks());
  midi_file.linkNotePairs();

  const int track_count = midi_file.getTrackCount();
  const int ticks_per_quarter = midi_file.getTPQ();
  ConsoleLog() << "Initializing " << kMidiFileName << " for MIDI playback (" << track_count
               << " tracks, " << ticks_per_quarter << " TPQ)";

  AudioClock clock(kFrameRate);

  Musician musician(kFrameRate);
  musician.SetTempo(kTempo);

  std::vector<std::pair<Instrument, Performer>> tracks;
  tracks.reserve(track_count);
  for (int i = 0; i < track_count; ++i) {
    tracks.emplace_back(Instrument(musician, SynthInstrument::GetDefinition()),
                        Performer(musician));
    auto& [instrument, performer] = tracks.back();
    // Build the score to perform.
    if (!BuildScore(midi_file[i], static_cast<int>(tracks.size() + 1), ticks_per_quarter,
                    instrument, performer)) {
      ConsoleLog() << "Empty MIDI track: " << i;
      tracks.pop_back();
      continue;
    }
    // Set the instrument settings.
    instrument.GetControl(SynthInstrument::Control::kGain).SetValue(kInstrumentGain);
    instrument.GetControl(SynthInstrument::Control::kOscillatorType)
        .SetValue(kInstrumentOscillatorType);
    instrument.GetControl(SynthInstrument::Control::kAttack).SetValue(kInstrumentEnvelopeAttack);
    instrument.GetControl(SynthInstrument::Control::kRelease).SetValue(kInstrumentEnvelopeRelease);
    instrument.GetControl(SynthInstrument::Control::kVoiceCount).SetValue(kInstrumentVoiceCount);
  }
  ConsoleLog() << "Number of active MIDI tracks: " << tracks.size();

  // Audio process callback.
  std::vector<double> mix_buffer(kChannelCount * kFrameCount);
  const auto process_callback = [&](double* output) {
    std::fill_n(output, kChannelCount * kFrameCount, 0.0);
    for (auto& [instrument, performer] : tracks) {
      instrument.Process(mix_buffer.data(), kChannelCount, kFrameCount, clock.GetTimestamp());
      std::transform(mix_buffer.begin(), mix_buffer.end(), output, output, std::plus<>());
    }
    clock.Update(kFrameCount);
  };
  audio_output.SetProcessCallback(process_callback);

  // Key down callback.
  bool quit = false;
  const auto key_down_callback = [&quit](const InputManager::Key& key) {
    if (static_cast<int>(key) == 27) {
      // ESC pressed, quit the app.
      quit = true;
      return;
    }
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start(kFrameRate, kChannelCount, kFrameCount);
  for (auto& [instrument, performer] : tracks) {
    performer.Start();
  }

  while (!quit) {
    input_manager.Update();
    musician.Update(clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  audio_output.Stop();

  return 0;
}
