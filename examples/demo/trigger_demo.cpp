#include <barelymusician.h>

#include <array>
#include <chrono>
#include <thread>
#include <utility>
#include <vector>

#include "common/audio_clock.h"
#include "common/audio_output.h"
#include "common/console_log.h"
#include "common/input_manager.h"

namespace {

using ::barely::Engine;
using ::barely::InstrumentControlType;
using ::barely::NoteEventType;
using ::barely::Scale;
using ::barely::Task;
using ::barely::TaskEventType;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;

// System audio settings.
constexpr int kSampleRate = 48000;
constexpr int kChannelCount = 2;
constexpr int kFrameCount = 512;

constexpr double kLookahead = 0.05;

constexpr std::array<float, 7> kMajor = {
    0.0f, 2.0f / 12.0f, 4.0f / 12.0f, 5.0f / 12.0f, 7.0f / 12.0f, 9.0f / 12.0f, 11.0f / 12.0f,
};

// Instrument settings.
constexpr float kGain = 0.1f;
constexpr float kOscShape = 1.0f;
constexpr float kAttack = 0.0f;
constexpr float kRelease = 0.1f;

constexpr float kInitialTempo = 120.0f;

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int /*argc*/, char* /*argv*/[]) {
  InputManager input_manager;

  AudioClock audio_clock(kSampleRate);
  AudioOutput audio_output(kSampleRate, kChannelCount, kFrameCount);

  Engine engine(kSampleRate, kFrameCount);
  engine.SetControl(barely::EngineControlType::kTempo, kInitialTempo);

  auto instrument = engine.CreateInstrument({{
      {InstrumentControlType::kGain, kGain},
      {InstrumentControlType::kOscMix, 1.0f},
      {InstrumentControlType::kOscShape, kOscShape},
      {InstrumentControlType::kAttack, kAttack},
      {InstrumentControlType::kRelease, kRelease},
  }});
  instrument.SetNoteEventCallback([](NoteEventType type, float pitch) {
    if (type == NoteEventType::kBegin) {
      ConsoleLog() << "Note(" << pitch << ")";
    }
  });

  std::vector<std::pair<double, double>> triggers;
  std::vector<Task> tasks;

  auto performer = engine.CreatePerformer();
  double stop_position = 0.0;

  const auto play_note_fn = [&](int degree) {
    return [&, pitch = Scale(kMajor).GetPitch(degree)](TaskEventType type) {
      if (type == TaskEventType::kBegin) {
        instrument.SetNoteOn(pitch);
      } else if (type == TaskEventType::kEnd) {
        instrument.SetNoteOff(pitch);
        if (stop_position == performer.GetPosition()) {
          performer.Stop();
        }
      }
    };
  };

  // Trigger 1.
  triggers.emplace_back(0.0, 1.0);
  tasks.emplace_back(performer.CreateTask(0.0, 1.0, 0, play_note_fn(0)));
  // Trigger 2.
  triggers.emplace_back(1.0, 1.0);
  tasks.emplace_back(performer.CreateTask(1.0, 1.0, 0, play_note_fn(1)));
  // Trigger 3.
  triggers.emplace_back(2.0, 1.0);
  tasks.emplace_back(performer.CreateTask(2.0, 1.0, 0, play_note_fn(2)));
  // Trigger 4.
  triggers.emplace_back(3.0, 1.0);
  tasks.emplace_back(performer.CreateTask(3.0, 0.66, 0, play_note_fn(3)));
  tasks.emplace_back(performer.CreateTask(3.66, 0.34, 0, play_note_fn(4)));
  // Trigger 5.
  triggers.emplace_back(4.0, 1.0);
  tasks.emplace_back(performer.CreateTask(4.0, 0.33, 0, play_note_fn(5)));
  tasks.emplace_back(performer.CreateTask(4.33, 0.33, 0, play_note_fn(6)));
  tasks.emplace_back(performer.CreateTask(4.66, 0.34, 0, play_note_fn(7)));
  // Trigger 6.
  triggers.emplace_back(5.0, 2.0);
  tasks.emplace_back(performer.CreateTask(5.0, 2.0, 0, play_note_fn(8)));

  // Audio process callback.
  audio_output.SetProcessCallback(
      [&](float* output_samples, int output_channel_count, int output_frame_count) {
        engine.Process(output_samples, output_channel_count, output_frame_count,
                       audio_clock.GetTimestamp());
        audio_clock.Update(output_frame_count);
      });

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
      performer.SetPosition(triggers[index].first);
      stop_position = triggers[index].first + triggers[index].second;
      performer.Start();
      return;
    }
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start();
  engine.Update(kLookahead);

  while (!quit) {
    input_manager.Update();
    engine.Update(audio_clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  return 0;
}
