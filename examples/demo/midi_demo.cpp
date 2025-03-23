#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <functional>
#include <span>
#include <string>
#include <thread>
#include <tuple>
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
using ::barely::Engine;
using ::barely::Instrument;
using ::barely::Performer;
using ::barely::Task;
using ::barely::TaskState;
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
constexpr float kInstrumentOscShape = 0.5f;
constexpr float kInstrumentEnvelopeAttack = 0.0f;
constexpr float kInstrumentEnvelopeRelease = 0.2f;
constexpr int kInstrumentVoiceCount = 16;
constexpr float kInstrumentGain = 0.1f;

// Midi file name.
constexpr char kMidiFileName[] = "midi/sample.mid";

constexpr double kTempo = 132.0;

// Builds the score for the given `midi_events`.
bool BuildScore(const smf::MidiEventList& midi_events, int ticks_per_beat, Instrument& instrument,
                Performer& performer, std::vector<Task>& tasks) {
  const auto get_position_fn = [ticks_per_beat](int tick) -> double {
    return static_cast<double>(tick) / static_cast<double>(ticks_per_beat);
  };
  bool has_notes = false;
  tasks.reserve(2 * midi_events.size());
  for (int i = 0; i < midi_events.size(); ++i) {
    const auto& midi_event = midi_events[i];
    if (midi_event.isNoteOn()) {
      const double position = get_position_fn(midi_event.tick);
      const double duration = get_position_fn(midi_event.getTickDuration());
      const float pitch = static_cast<float>(midi_event.getKeyNumber() - 60) / 12.0f;
      const float intensity = static_cast<float>(midi_event.getVelocity()) / 127.0f;
      tasks.emplace_back(
          performer.CreateTask(position, duration, [&, pitch, intensity](TaskState state) noexcept {
            if (state == TaskState::kBegin) {
              instrument.SetNoteOn(pitch, intensity);
            } else if (state == TaskState::kEnd) {
              instrument.SetNoteOff(pitch);
            }
          }));
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

  Engine engine(kSampleRate);
  engine.SetTempo(kTempo);

  std::vector<std::tuple<Instrument, Performer, std::vector<Task>, size_t>> tracks;
  tracks.reserve(track_count);
  for (int i = 0; i < track_count; ++i) {
    tracks.emplace_back(engine.CreateInstrument(), engine.CreatePerformer(), std::vector<Task>{},
                        tracks.size() + 1);
    auto& [instrument, performer, tasks, track_index] = tracks.back();
    // Build the score to perform.
    if (!BuildScore(midi_file[i], ticks_per_quarter, instrument, performer, tasks)) {
      ConsoleLog() << "Empty MIDI track: " << i;
      tracks.pop_back();
      continue;
    }
    // Set the instrument settings.
    instrument.SetNoteOnCallback([track_index](float pitch) {
      ConsoleLog() << "MIDI track #" << track_index << ": NoteOn(" << pitch << ")";
    });
    instrument.SetNoteOffCallback([track_index](float pitch) {
      ConsoleLog() << "MIDI track #" << track_index << ": NoteOff(" << pitch << ")";
    });
    instrument.SetControl(ControlType::kGain, kInstrumentGain);
    instrument.SetControl(ControlType::kOscMix, 1.0f);
    instrument.SetControl(ControlType::kOscShape, kInstrumentOscShape);
    instrument.SetControl(ControlType::kAttack, kInstrumentEnvelopeAttack);
    instrument.SetControl(ControlType::kRelease, kInstrumentEnvelopeRelease);
    instrument.SetControl(ControlType::kVoiceCount, kInstrumentVoiceCount);
  }
  ConsoleLog() << "Number of active MIDI tracks: " << tracks.size();

  // Audio process callback.
  std::vector<float> mix_buffer(kSampleCount);
  const auto process_callback = [&](std::span<float> output_samples) {
    std::fill_n(output_samples.begin(), kSampleCount, 0.0f);
    for (auto& [instrument, performer, tasks, _] : tracks) {
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
  engine.Update(kLookahead);
  for (auto& [instrument, performer, tasks, _] : tracks) {
    performer.Start();
  }

  while (!quit) {
    input_manager.Update();
    engine.Update(clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  ConsoleLog() << "Stopping audio stream";
  for (auto& [instrument, performer, tasks, _] : tracks) {
    performer.Stop();
    tasks.clear();
  }
  audio_output.Stop();

  return 0;
}
