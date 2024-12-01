#include <cctype>
#include <chrono>
#include <span>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include "barelymusician.h"
#include "common/audio_clock.h"
#include "common/audio_output.h"
#include "common/console_log.h"
#include "common/input_manager.h"

namespace {

using ::barely::ControlType;
using ::barely::Musician;
using ::barely::OscillatorShape;
using ::barely::TaskEvent;
using ::barely::TaskHandle;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;

// System audio settings.
constexpr int kSampleRate = 48000;
constexpr int kSampleCount = 1024;

constexpr double kLookahead = 0.1;

// Instrument settings.
constexpr float kGain = -20.0f;
constexpr OscillatorShape kOscillatorShape = OscillatorShape::kSaw;
constexpr float kAttack = 0.0f;
constexpr float kRelease = 0.1f;

constexpr double kInitialTempo = 120.0;
constexpr double kTempoIncrement = 10.0;

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
  instrument.SetNoteOnEvent({[](float pitch, float /*intensity*/, void* /*user_data*/) {
    ConsoleLog() << "Note(" << pitch << ")";
  }});

  auto performer = musician.AddPerformer();
  performer.SetLooping(true);
  performer.SetLoopBeginPosition(3.0);
  performer.SetLoopLength(5.0);

  const auto play_note_fn = [&](double duration, float pitch) {
    return [&instrument, &performer, pitch, duration]() {
      instrument.SetNoteOn(pitch);
      performer.ScheduleOneOffTask([&instrument, pitch]() { instrument.SetNoteOff(pitch); },
                                   performer.GetPosition() + duration);
    };
  };

  std::vector<std::pair<double, TaskEvent::Callback>> score;
  score.emplace_back(0.0, play_note_fn(1.0, 0.0f));
  score.emplace_back(1.0, play_note_fn(1.0, 2.0f / 12.0f));
  score.emplace_back(2.0, play_note_fn(1.0, 4.0f / 12.0f));
  score.emplace_back(3.0, play_note_fn(1.0, 5.0f / 12.0f));
  score.emplace_back(4.0, play_note_fn(1.0, 7.0f / 12.0f));
  score.emplace_back(5.0, play_note_fn(1.0 / 3.0, 7.0f / 12.0f));
  score.emplace_back(5 + 1.0 / 3.0, play_note_fn(1.0 / 3.0, 9.0f / 12.0f));
  score.emplace_back(5 + 2.0 / 3.0, play_note_fn(1.0 / 3.0, 11.0f / 12.0f));
  score.emplace_back(6.0, play_note_fn(2.0, 1.0f));

  std::unordered_map<int, TaskHandle> tasks;
  int index = 0;
  for (const auto& [position, callback] : score) {
    tasks.emplace(index++, performer.AddTask(callback, position));
  }

  // Audio process callback.
  const auto process_callback = [&](std::span<float> output_samples) {
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
    if (const int index = static_cast<int>(key - '0'); index > 0 && index < 10) {
      // Toggle score.
      if (const auto it = tasks.find(index - 1); it != tasks.end()) {
        performer.RemoveTask(it->second);
        tasks.erase(it);
        ConsoleLog() << "Removed note " << index;
      } else {
        const auto& [position, callback] = score[index - 1];
        tasks.emplace(index - 1, performer.AddTask(callback, position));
        ConsoleLog() << "Added note " << index;
      }
      return;
    }
    // Adjust tempo.
    double tempo = musician.GetTempo();
    switch (std::toupper(key)) {
      case ' ':
        if (performer.IsPlaying()) {
          performer.Stop();
          instrument.SetAllNotesOff();
          ConsoleLog() << "Stopped playback";
        } else {
          performer.Start();
          ConsoleLog() << "Started playback";
        }
        return;
      case 'L':
        if (performer.IsLooping()) {
          performer.SetLooping(false);
          ConsoleLog() << "Loop turned off";
        } else {
          performer.SetLooping(true);
          ConsoleLog() << "Loop turned on";
        }
        return;
      case 'P':
        instrument.SetAllNotesOff();
        performer.SetPosition(0.0);
        return;
      case '-':
        tempo -= kTempoIncrement;
        break;
      case '+':
        tempo += kTempoIncrement;
        break;
      case 'R':
        tempo = kInitialTempo;
        break;
      default:
        return;
    }
    musician.SetTempo(tempo);
    ConsoleLog() << "Tempo set to " << musician.GetTempo() << " bpm";
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start();
  musician.Update(kLookahead);
  performer.Start();

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
