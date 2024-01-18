#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <thread>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/rational.h"
#include "barelymusician/components/metronome.h"
#include "barelymusician/composition/pitch.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/instruments/synth_instrument.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"

namespace {

using ::barely::Metronome;
using ::barely::Musician;
using ::barely::OscillatorType;
using ::barely::Rational;
using ::barely::SynthInstrument;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;

// System audio settings.
constexpr int kFrameRate = 48000;
constexpr int kChannelCount = 2;
constexpr int kFrameCount = 1024;

constexpr std::int64_t kLookahead = kFrameRate / 10;

// Metronome settings.
constexpr OscillatorType kOscillatorType = OscillatorType::kSquare;
constexpr Rational kGain = Rational(1, 4);
constexpr Rational kAttack = 0;
constexpr Rational kRelease = Rational(1, 40);
constexpr int kVoiceCount = 1;

constexpr Rational kBarPitch = barely::kPitchA4;
const Rational kBeatPitch = kBarPitch - 1;

constexpr int kBeatCount = 4;
constexpr int kInitialTempo = 120;
constexpr int kTempoIncrement = 10;

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
  AudioOutput audio_output;
  InputManager input_manager;

  AudioClock audio_clock(kFrameRate);

  Musician musician(kFrameRate);
  musician.SetTempo(kInitialTempo);

  // Create the metronome instrument.
  auto instrument = musician.CreateInstrument<SynthInstrument>();
  instrument.SetControl(SynthInstrument::Control::kGain, kGain);
  instrument.SetControl(SynthInstrument::Control::kOscillatorType,
                        static_cast<int>(kOscillatorType));
  instrument.SetControl(SynthInstrument::Control::kAttack, kAttack);
  instrument.SetControl(SynthInstrument::Control::kRelease, kRelease);
  instrument.SetControl(SynthInstrument::Control::kVoiceCount, kVoiceCount);

  // Create the metronome with a beat callback.
  auto metronome = musician.CreateComponent<Metronome>();
  metronome.SetBeatCallback([&](int beat) {
    const int current_bar = (beat / kBeatCount) + 1;
    const int current_beat = (beat % kBeatCount) + 1;
    ConsoleLog() << "Tick " << current_bar << "." << current_beat;
    const Rational pitch = current_beat == 1 ? kBarPitch : kBeatPitch;
    instrument.SetNoteOn(pitch);
    instrument.SetNoteOff(pitch);
  });

  // Audio process callback.
  const auto process_callback = [&](float* output) {
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
    int tempo = musician.GetTempo();
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
        tempo /= 2;
        break;
      case '2':
        tempo *= 2;
        break;
      case 'R':
        tempo = kInitialTempo;
        break;
      default:
        return;
    }
    tempo = std::clamp(tempo, 0, kFrameRate);
    musician.SetTempo(tempo);
    ConsoleLog() << "Tempo set to " << musician.GetTempo() << " bpm";
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start(kFrameRate, kChannelCount, kFrameCount);
  metronome.Start();

  ConsoleLog() << "Play the metronome using the keyboard keys:";
  ConsoleLog() << "  * Use space key to start or stop the metronome";
  ConsoleLog() << "  * Use enter key to reset the metronome";
  ConsoleLog() << "  * Use 12 keys to halve and float the tempo";
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
