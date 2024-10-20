#include <algorithm>
#include <cctype>
#include <chrono>
#include <thread>

#include "barelymusician/barelymusician.h"
#include "barelymusician/components/metronome.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"

namespace {

using ::barely::ControlType;
using ::barely::Metronome;
using ::barely::Musician;
using ::barely::OscillatorShape;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;

// System audio settings.
constexpr int kFrameRate = 48000;
constexpr int kChannelCount = 2;
constexpr int kFrameCount = 1024;

constexpr double kLookahead = 0.1;

// Metronome settings.
constexpr OscillatorShape kOscillatorShape = OscillatorShape::kSquare;
constexpr double kGain = 0.25;
constexpr double kAttack = 0.0;
constexpr double kRelease = 0.025;
constexpr int kVoiceCount = 1;

constexpr double kBarPitch = 1.0;
constexpr double kBeatPitch = 0.0;

constexpr int kBeatCount = 4;
constexpr double kInitialTempo = 120.0;
constexpr double kTempoIncrement = 10.0;

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
  InputManager input_manager;

  AudioClock audio_clock(kFrameRate);
  AudioOutput audio_output(kFrameRate, kChannelCount, kFrameCount);

  Musician musician(kFrameRate);
  musician.SetTempo(kInitialTempo);

  // Create the metronome instrument.
  auto instrument = musician.AddInstrument();
  instrument.SetControl(ControlType::kGain, kGain);
  instrument.SetControl(ControlType::kOscillatorShape, kOscillatorShape);
  instrument.SetControl(ControlType::kAttack, kAttack);
  instrument.SetControl(ControlType::kRelease, kRelease);
  instrument.SetControl(ControlType::kVoiceCount, kVoiceCount);

  // Create the metronome with a beat callback.
  Metronome metronome(musician);
  metronome.SetBeatCallback([&](int beat) {
    const int current_bar = (beat / kBeatCount) + 1;
    const int current_beat = (beat % kBeatCount) + 1;
    ConsoleLog() << "Tick " << current_bar << "." << current_beat;
    const double pitch = current_beat == 1 ? kBarPitch : kBeatPitch;
    instrument.SetNoteOn(pitch);
    instrument.SetNoteOff(pitch);
  });

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
        metronome.Reset();
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
    tempo = std::clamp(tempo, 0.0, static_cast<double>(kFrameRate));
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
