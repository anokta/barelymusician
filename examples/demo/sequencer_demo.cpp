#include <cctype>
#include <chrono>
#include <span>
#include <thread>
#include <unordered_map>
#include <vector>

#include "barelymusician.h"
#include "common/audio_clock.h"
#include "common/audio_output.h"
#include "common/console_log.h"
#include "common/input_manager.h"

namespace {

using ::barely::ControlType;
using ::barely::Engine;
using ::barely::Task;
using ::barely::TaskState;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;

// System audio settings.
constexpr int kSampleRate = 48000;
constexpr int kSampleCount = 1024;

constexpr double kLookahead = 0.1;

// Instrument settings.
constexpr float kGain = 0.1f;
constexpr float kOscShape = 1.0f;
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

  Engine engine(kSampleRate);
  engine.SetTempo(kInitialTempo);

  auto instrument = engine.CreateInstrument();
  instrument.SetControl(ControlType::kGain, kGain);
  instrument.SetControl(ControlType::kOscMix, 1.0f);
  instrument.SetControl(ControlType::kOscShape, kOscShape);
  instrument.SetControl(ControlType::kAttack, kAttack);
  instrument.SetControl(ControlType::kRelease, kRelease);
  instrument.SetNoteOnCallback([](float pitch) { ConsoleLog() << "Note(" << pitch << ")"; });

  auto performer = engine.CreatePerformer();
  performer.SetLooping(true);
  performer.SetLoopBeginPosition(3.0);
  performer.SetLoopLength(5.0);

  struct SequencerNote {
    double position;
    double duration;
    float pitch;
  };
  std::vector<SequencerNote> score;
  score.push_back({0.0, 1.0, 0.0f});
  score.push_back({1.0, 1.0, 2.0f / 12.0f});
  score.push_back({2.0, 1.0, 4.0f / 12.0f});
  score.push_back({3.0, 1.0, 5.0f / 12.0f});
  score.push_back({4.0, 1.0, 7.0f / 12.0f});
  score.push_back({5.0, 1.0 / 3.0, 7.0f / 12.0f});
  score.push_back({5 + 1.0 / 3.0, 1.0 / 3.0, 9.0f / 12.0f});
  score.push_back({5 + 2.0 / 3.0, 1.0 / 3.0, 11.0f / 12.0f});
  score.push_back({6.0, 2.0, 1.0f});

  std::unordered_map<int, Task> tasks;
  const auto build_note_fn = [&](const SequencerNote& note) {
    return performer.CreateTask(note.position, note.duration,
                                [&instrument, pitch = note.pitch](TaskState state) {
                                  switch (state) {
                                    case TaskState::kBegin:
                                      instrument.SetNoteOn(pitch);
                                      break;
                                    case TaskState::kEnd:
                                      instrument.SetNoteOff(pitch);
                                      break;
                                    default:
                                      break;
                                  }
                                });
  };
  for (int i = 0; i < static_cast<int>(score.size()); ++i) {
    tasks.emplace(i, build_note_fn(score[i]));
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
        tasks.erase(it);
        ConsoleLog() << "Removed note " << index;
      } else {
        tasks.emplace(index - 1, build_note_fn(score[index - 1]));
        ConsoleLog() << "Added note " << index;
      }
      return;
    }
    // Adjust tempo.
    double tempo = engine.GetTempo();
    switch (std::toupper(key)) {
      case ' ':
        if (performer.IsPlaying()) {
          performer.Stop();
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
    engine.SetTempo(tempo);
    ConsoleLog() << "Tempo set to " << engine.GetTempo() << " bpm";
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start();
  engine.Update(kLookahead);
  performer.Start();

  while (!quit) {
    input_manager.Update();
    engine.Update(audio_clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  ConsoleLog() << "Stopping audio stream";
  performer.Stop();
  audio_output.Stop();

  return 0;
}
