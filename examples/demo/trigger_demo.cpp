#include <chrono>
#include <thread>
#include <unordered_set>
#include <utility>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/composition/midi.h"
#include "barelymusician/composition/pitch.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/instruments/synth_instrument.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"

namespace {

using ::barely::Note;
using ::barely::OscillatorType;
using ::barely::ScopedMusician;
using ::barely::SynthInstrument;
using ::barely::Task;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;

// System audio settings.
constexpr int kFrameRate = 48000;
constexpr int kChannelCount = 2;
constexpr int kFrameCount = 512;

constexpr double kLookahead = 0.05;

// Instrument settings.
constexpr double kGain = 0.1;
constexpr OscillatorType kOscillatorType = OscillatorType::kSaw;
constexpr double kAttack = 0.0;
constexpr double kRelease = 0.1;

constexpr double kInitialTempo = 120.0;

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int /*argc*/, char* /*argv*/[]) {
  AudioOutput audio_output;
  InputManager input_manager;

  AudioClock audio_clock(kFrameRate);

  ScopedMusician musician;
  musician.SetTempo(kInitialTempo);

  auto instrument = musician.CreateInstrument<SynthInstrument>(kFrameRate);
  instrument.GetControl(SynthInstrument::Control::kGain).SetValue(kGain);
  instrument.GetControl(SynthInstrument::Control::kOscillatorType).SetValue(kOscillatorType);
  instrument.GetControl(SynthInstrument::Control::kAttack).SetValue(kAttack);
  instrument.GetControl(SynthInstrument::Control::kRelease).SetValue(kRelease);

  std::vector<std::pair<double, double>> triggers;
  std::vector<Task> tasks;

  auto performer = musician.CreatePerformer();

  std::unordered_set<Note> notes;
  const auto play_note_fn = [&](int scale_index, double duration) {
    const double pitch =
        barely::kPitchD3 + barely::PitchFromScale(barely::kPitchMajorScale, scale_index);
    return [&instrument, &performer, &notes, duration, pitch]() {
      ConsoleLog() << "Note{" << barely::MidiNumberFromPitch(pitch) << "}";
      const auto [it, success] = notes.emplace(instrument.CreateNote(pitch));
      performer.ScheduleOneOffTask(
          [&instrument, &performer, &notes, note = *it]() {
            instrument.DestroyNote(note);
            notes.erase(note);
          },
          performer.GetPosition() + duration);
    };
  };
  const auto stop_all_notes_fn = [&]() {
    if (!notes.empty()) {
      for (auto& note : notes) {
        instrument.DestroyNote(note);
      }
      notes.clear();
    }
  };

  // Trigger 1.
  triggers.emplace_back(0.0, 1.0);
  tasks.push_back(performer.CreateTask(play_note_fn(0, 1.0), 0.0));
  // Trigger 2.
  triggers.emplace_back(1.0, 1.0);
  tasks.push_back(performer.CreateTask(play_note_fn(1, 1.0), 1.0));
  // Trigger 3.
  triggers.emplace_back(2.0, 1.0);
  tasks.push_back(performer.CreateTask(play_note_fn(2, 1.0), 2.0));
  // Trigger 4.
  triggers.emplace_back(3.0, 1.0);
  tasks.push_back(performer.CreateTask(play_note_fn(3, 0.66), 3.0));
  tasks.push_back(performer.CreateTask(play_note_fn(4, 0.34), 3.66));
  // Trigger 5.
  triggers.emplace_back(4.0, 1.0);
  tasks.push_back(performer.CreateTask(play_note_fn(5, 0.33), 4.0));
  tasks.push_back(performer.CreateTask(play_note_fn(6, 0.33), 4.33));
  tasks.push_back(performer.CreateTask(play_note_fn(7, 0.34), 4.66));
  // Trigger 6.
  triggers.emplace_back(5.0, 2.0);
  tasks.push_back(performer.CreateTask(play_note_fn(8, 2.0), 5.0));

  // Audio process callback.
  const auto process_callback = [&](double* output) {
    instrument.Process(output, kChannelCount, kFrameCount, audio_clock.GetTimestamp());
    audio_clock.Update(kFrameCount);
  };
  audio_output.SetProcessCallback(process_callback);

  // Key down callback.
  bool quit = false;
  const auto key_down_callback = [&](const InputManager::Key& key) {
    if (static_cast<int>(key) == 27) {
      // ESC pressed, quit the app.
      quit = true;
      return;
    }
    if (const int index = static_cast<int>(key - '1');
        index >= 0 && index < static_cast<int>(triggers.size())) {
      performer.Stop();
      performer.CancelAllOneOffTasks();
      stop_all_notes_fn();
      performer.SetPosition(triggers[index].first);
      performer.ScheduleOneOffTask(
          [&]() {
            stop_all_notes_fn();
            performer.Stop();
          },
          triggers[index].first + triggers[index].second);
      performer.Start();
      return;
    }
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start(kFrameRate, kChannelCount, kFrameCount);

  while (!quit) {
    input_manager.Update();
    musician.Update(audio_clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  ConsoleLog() << "Stopping audio stream";
  tasks.clear();
  performer.Stop();
  audio_output.Stop();

  return 0;
}
