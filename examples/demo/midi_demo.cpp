#include <algorithm>
#include <cassert>
#include <chrono>
#include <functional>
#include <span>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "MidiEventList.h"
#include "MidiFile.h"
#include "barelymusician.h"
#include "common/audio_clock.h"
#include "common/audio_output.h"
#include "common/console_log.h"
#include "common/input_manager.h"
#include "data/data.h"

namespace {

using ::barely::ControlType;
using ::barely::InstrumentHandle;
using ::barely::Musician;
using ::barely::OscillatorShape;
using ::barely::PerformerHandle;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::GetDataFilePath;
using ::barely::examples::InputManager;
using ::smf::MidiFile;

// System audio settings.
constexpr int kSampleRate = 48000;
constexpr int kSampleCount = 512;

constexpr double kLookahead = 0.1;

// Instrument settings.
constexpr OscillatorShape kInstrumentOscillatorShape = OscillatorShape::kSquare;
constexpr double kInstrumentEnvelopeAttack = 0.0;
constexpr double kInstrumentEnvelopeRelease = 0.2;
constexpr int kInstrumentVoiceCount = 16;
constexpr double kInstrumentGain = -6.0 * static_cast<double>(kInstrumentVoiceCount);

// Midi file name.
constexpr char kMidiFileName[] = "midi/sample.mid";

constexpr double kTempo = 132.0;

// Builds the score for the given `midi_events`.
bool BuildScore(const smf::MidiEventList& midi_events, int ticks_per_beat,
                InstrumentHandle& instrument, PerformerHandle& performer) {
  const auto get_position_fn = [ticks_per_beat](int tick) -> double {
    return static_cast<double>(tick) / static_cast<double>(ticks_per_beat);
  };
  bool has_notes = false;
  for (int i = 0; i < midi_events.size(); ++i) {
    const auto& midi_event = midi_events[i];
    if (midi_event.isNoteOn()) {
      const double position = get_position_fn(midi_event.tick);
      const double duration = get_position_fn(midi_event.getTickDuration());
      const double pitch = static_cast<double>(midi_event.getKeyNumber() - 60) / 12.0;
      const double intensity = static_cast<double>(midi_event.getVelocity()) / 127.0;
      performer.ScheduleOneOffTask(
          [&instrument, pitch, intensity]() mutable { instrument.SetNoteOn(pitch, intensity); },
          position);
      performer.ScheduleOneOffTask([&instrument, pitch]() mutable { instrument.SetNoteOff(pitch); },
                                   position + duration);
      has_notes = true;
    }
  }
  return has_notes;
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int /*argc*/, char* argv[]) {
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

  AudioClock clock(kSampleRate);
  AudioOutput audio_output(kSampleRate, kSampleCount);

  Musician musician(kSampleRate);
  musician.SetTempo(kTempo);

  std::vector<std::pair<InstrumentHandle, PerformerHandle>> tracks;
  tracks.reserve(track_count);
  for (int i = 0; i < track_count; ++i) {
    tracks.emplace_back(musician.AddInstrument(), musician.AddPerformer());
    auto& [instrument, performer] = tracks.back();
    // Build the score to perform.
    if (!BuildScore(midi_file[i], ticks_per_quarter, instrument, performer)) {
      ConsoleLog() << "Empty MIDI track: " << i;
      musician.RemoveInstrument(instrument);
      musician.RemovePerformer(performer);
      tracks.pop_back();
      continue;
    }
    // Set the instrument settings.
    const auto track_index = tracks.size() + 1;
    instrument.SetNoteOnEvent([track_index](double pitch, double intensity) {
      ConsoleLog() << "MIDI track #" << track_index << ": NoteOn(" << pitch << ", " << intensity
                   << ")";
    });
    instrument.SetNoteOffEvent([track_index](double pitch) {
      ConsoleLog() << "MIDI track #" << track_index << ": NoteOff(" << pitch << ")";
    });
    instrument.SetControl(ControlType::kGain, kInstrumentGain);
    instrument.SetControl(ControlType::kOscillatorShape, kInstrumentOscillatorShape);
    instrument.SetControl(ControlType::kAttack, kInstrumentEnvelopeAttack);
    instrument.SetControl(ControlType::kRelease, kInstrumentEnvelopeRelease);
    instrument.SetControl(ControlType::kVoiceCount, kInstrumentVoiceCount);
  }
  ConsoleLog() << "Number of active MIDI tracks: " << tracks.size();

  // Audio process callback.
  std::vector<double> mix_buffer(kSampleCount);
  const auto process_callback = [&](std::span<double> output_samples) {
    std::fill_n(output_samples.begin(), kSampleCount, 0.0);
    for (auto& [instrument, performer] : tracks) {
      instrument.Process(mix_buffer, clock.GetTimestamp());
      std::transform(mix_buffer.begin(), mix_buffer.end(), output_samples.begin(),
                     output_samples.begin(), std::plus<>());
    }
    clock.Update(static_cast<int>(output_samples.size()));
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
  audio_output.Start();
  musician.Update(kLookahead);
  for (auto& [instrument, performer] : tracks) {
    performer.Start();
  }

  while (!quit) {
    input_manager.Update();
    musician.Update(clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  ConsoleLog() << "Stopping audio stream";
  for (auto& [instrument, performer] : tracks) {
    performer.Stop();
    instrument.SetAllNotesOff();
  }
  audio_output.Stop();

  return 0;
}
