#include <array>
#include <chrono>
#include <span>
#include <thread>
#include <utility>
#include <vector>

#include "barelycomposer.h"
#include "barelymusician.h"
#include "common/audio_clock.h"
#include "common/audio_output.h"
#include "common/console_log.h"
#include "common/input_manager.h"

namespace {

using ::barely::ControlType;
using ::barely::Musician;
using ::barely::OscillatorShape;
using ::barely::Scale;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;

// System audio settings.
constexpr int kSampleRate = 48000;
constexpr int kSampleCount = 512;

constexpr double kLookahead = 0.05;

constexpr std::array<double, 7> kMajor = {
    0.0, 2.0 / 12.0, 4.0 / 12.0, 5.0 / 12.0, 7.0 / 12.0, 9.0 / 12.0, 11.0 / 12.0,
};

// Instrument settings.
constexpr double kGain = -20.0;
constexpr OscillatorShape kOscillatorShape = OscillatorShape::kSaw;
constexpr double kAttack = 0.0;
constexpr double kRelease = 0.1;

constexpr double kInitialTempo = 120.0;

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int /*argc*/, char* /*argv*/[]) {
  InputManager input_manager;

  AudioClock audio_clock(kSampleRate);
  AudioOutput audio_output(kSampleRate, kSampleCount);

  Musician musician(kSampleRate);
  musician.SetTempo(kInitialTempo);

  auto instrument = musician.AddInstrument();
  instrument.SetControl(ControlType::kGain, kGain);
  instrument.SetControl(ControlType::kOscillatorShape, kOscillatorShape);
  instrument.SetControl(ControlType::kAttack, kAttack);
  instrument.SetControl(ControlType::kRelease, kRelease);
  instrument.SetNoteOnEvent(
      [](double pitch, double /*intensity*/) { ConsoleLog() << "Note(" << pitch << ")"; });

  std::vector<std::pair<double, double>> triggers;

  auto performer = musician.AddPerformer();

  const auto play_note_fn = [&](int degree, double duration) {
    return [&instrument, &performer, duration, pitch = Scale(kMajor).GetPitch(degree)]() {
      instrument.SetNoteOn(pitch);
      performer.ScheduleOneOffTask(
          [&instrument, &performer, pitch]() { instrument.SetNoteOff(pitch); },
          performer.GetPosition() + duration);
    };
  };

  // Trigger 1.
  triggers.emplace_back(0.0, 1.0);
  performer.AddTask(play_note_fn(0, 1.0), 0.0);
  // Trigger 2.
  triggers.emplace_back(1.0, 1.0);
  performer.AddTask(play_note_fn(1, 1.0), 1.0);
  // Trigger 3.
  triggers.emplace_back(2.0, 1.0);
  performer.AddTask(play_note_fn(2, 1.0), 2.0);
  // Trigger 4.
  triggers.emplace_back(3.0, 1.0);
  performer.AddTask(play_note_fn(3, 0.66), 3.0);
  performer.AddTask(play_note_fn(4, 0.34), 3.66);
  // Trigger 5.
  triggers.emplace_back(4.0, 1.0);
  performer.AddTask(play_note_fn(5, 0.33), 4.0);
  performer.AddTask(play_note_fn(6, 0.33), 4.33);
  performer.AddTask(play_note_fn(7, 0.34), 4.66);
  // Trigger 6.
  triggers.emplace_back(5.0, 2.0);
  performer.AddTask(play_note_fn(8, 2.0), 5.0);

  // Audio process callback.
  const auto process_callback = [&](std::span<double> output_samples) {
    instrument.Process(output_samples, audio_clock.GetTimestamp());
    audio_clock.Update(static_cast<int>(output_samples.size()));
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
      instrument.SetAllNotesOff();
      performer.SetPosition(triggers[index].first);
      performer.ScheduleOneOffTask(
          [&]() {
            performer.Stop();
            instrument.SetAllNotesOff();
          },
          triggers[index].first + triggers[index].second);
      performer.Start();
      return;
    }
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start();
  musician.Update(kLookahead);

  while (!quit) {
    input_manager.Update();
    musician.Update(audio_clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  ConsoleLog() << "Stopping audio stream";
  performer.Stop();
  audio_output.Stop();

  return 0;
}
