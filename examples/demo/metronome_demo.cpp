#include <barelymusician.h>

#include <algorithm>
#include <cctype>
#include <chrono>
#include <thread>

#include "common/audio_clock.h"
#include "common/audio_output.h"
#include "common/console_log.h"
#include "common/input_manager.h"

namespace {

using ::barely::Engine;
using ::barely::InstrumentControlType;
using ::barely::TaskEventType;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;

// System audio settings.
constexpr int kSampleRate = 48000;
constexpr int kChannelCount = 2;
constexpr int kFrameCount = 1024;

constexpr double kLookahead = 0.1;

// Metronome settings.
constexpr float kOscShape = 0.75f;
constexpr float kGain = 0.25f;
constexpr float kAttack = 0.0f;
constexpr float kRelease = 0.05f;
constexpr int kVoiceCount = 1;

constexpr float kBarPitch = 1.0f;
constexpr float kBeatPitch = 0.0f;

constexpr int kBeatCount = 4;
constexpr double kInitialTempo = 120.0;
constexpr double kTempoIncrement = 10.0;

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
  InputManager input_manager;

  AudioClock audio_clock(kSampleRate);
  AudioOutput audio_output(kSampleRate, kChannelCount, kFrameCount);

  Engine engine(kSampleRate);
  engine.SetTempo(kInitialTempo);

  // Create the metronome instrument.
  auto instrument = engine.CreateInstrument({{
      {InstrumentControlType::kGain, kGain},
      {InstrumentControlType::kOscMix, 1.0f},
      {InstrumentControlType::kOscShape, kOscShape},
      {InstrumentControlType::kAttack, kAttack},
      {InstrumentControlType::kRelease, kRelease},
      {InstrumentControlType::kVoiceCount, kVoiceCount},
  }});

  // Create the metronome with a beat callback.
  auto metronome = engine.CreatePerformer();
  metronome.SetLooping(true);
  int beat = 0;
  engine.CreateTask(metronome, 0.0, 1e-6, 0, [&](TaskEventType type) {
    if (type != TaskEventType::kBegin) {
      return;
    }
    const int current_bar = (beat / kBeatCount) + 1;
    const int current_beat = (beat % kBeatCount) + 1;
    ++beat;
    ConsoleLog() << "Tick " << current_bar << "." << current_beat;
    const float pitch = current_beat == 1 ? kBarPitch : kBeatPitch;
    instrument.SetNoteOn(pitch);
    instrument.SetNoteOff(pitch);
  });

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
    // Adjust tempo.
    double tempo = engine.GetTempo();
    switch (std::toupper(key)) {
      case ' ':
        if (metronome.IsPlaying()) {
          metronome.Stop();
          ConsoleLog() << "Metronome stopped";
        } else {
          metronome.Start();
          ConsoleLog() << "Metronome started";
        }
        return;
      case '\r':
        metronome.Stop();
        metronome.SetPosition(0.0);
        ConsoleLog() << "Metronome reset";
        return;
      case 'O':
        tempo -= kTempoIncrement;
        break;
      case 'P':
        tempo += kTempoIncrement;
        break;
      case '1':
        tempo *= 0.5;
        break;
      case '2':
        tempo *= 2.0;
        break;
      case 'R':
        tempo = kInitialTempo;
        break;
      default:
        return;
    }
    tempo = std::clamp(tempo, 0.0, static_cast<double>(kSampleRate));
    engine.SetTempo(tempo);
    ConsoleLog() << "Tempo set to " << engine.GetTempo() << " bpm";
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start();
  engine.Update(kLookahead);
  metronome.Start();

  ConsoleLog() << "Play the metronome using the keyboard keys:";
  ConsoleLog() << "  * Use space key to start or stop the metronome";
  ConsoleLog() << "  * Use enter key to reset the metronome";
  ConsoleLog() << "  * Use 12 keys to halve and double the tempo";
  ConsoleLog() << "  * Use OP keys to increment and decrement the tempo";
  ConsoleLog() << "  * Use R key to reset the tempo";

  while (!quit) {
    input_manager.Update();
    engine.Update(audio_clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  audio_output.Stop();

  return 0;
}
