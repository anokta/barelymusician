#include <algorithm>
#include <cctype>
#include <chrono>
#include <span>
#include <thread>

#include "barelymusician.h"
#include "common/audio_clock.h"
#include "common/audio_output.h"
#include "common/console_log.h"
#include "common/input_manager.h"

namespace {

using ::barely::ControlType;
using ::barely::Musician;
using ::barely::OscillatorShape;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;

// System audio settings.
constexpr int kSampleRate = 48000;
constexpr int kSampleCount = 1024;

constexpr double kLookahead = 0.1;

// Metronome settings.
constexpr OscillatorShape kOscillatorShape = OscillatorShape::kSquare;
constexpr float kGain = -12.0f;
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
  AudioOutput audio_output(kSampleRate, kSampleCount);

  Musician musician(kSampleRate);
  musician.SetTempo(kInitialTempo);

  // Create the metronome instrument.
  auto instrument = musician.CreateInstrument();
  instrument.SetControl(ControlType::kGain, kGain);
  instrument.SetControl(ControlType::kOscillatorShape, kOscillatorShape);
  instrument.SetControl(ControlType::kAttack, kAttack);
  instrument.SetControl(ControlType::kRelease, kRelease);
  instrument.SetControl(ControlType::kVoiceCount, kVoiceCount);

  // Create the metronome with a beat callback.
  auto metronome = musician.CreatePerformer();
  metronome.SetBeatCallback([&]() {
    const int beat = static_cast<int>(metronome.GetPosition());
    const int current_bar = (beat / kBeatCount) + 1;
    const int current_beat = (beat % kBeatCount) + 1;
    ConsoleLog() << "Tick " << current_bar << "." << current_beat;
    const float pitch = current_beat == 1 ? kBarPitch : kBeatPitch;
    instrument.SetNoteOn(pitch);
    instrument.SetNoteOff(pitch);
  });

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
    // Adjust tempo.
    double tempo = musician.GetTempo();
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
    musician.SetTempo(tempo);
    ConsoleLog() << "Tempo set to " << musician.GetTempo() << " bpm";
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start();
  musician.Update(kLookahead);
  metronome.Start();

  ConsoleLog() << "Play the metronome using the keyboard keys:";
  ConsoleLog() << "  * Use space key to start or stop the metronome";
  ConsoleLog() << "  * Use enter key to reset the metronome";
  ConsoleLog() << "  * Use 12 keys to halve and double the tempo";
  ConsoleLog() << "  * Use OP keys to increment and decrement the tempo";
  ConsoleLog() << "  * Use R key to reset the tempo";

  while (!quit) {
    input_manager.Update();
    musician.Update(audio_clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  ConsoleLog() << "Stopping audio stream";
  metronome.Stop();
  audio_output.Stop();

  return 0;
}
