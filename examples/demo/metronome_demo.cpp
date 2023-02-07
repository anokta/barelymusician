#include <cctype>
#include <chrono>
#include <thread>

#include "barelymusician/barelymusician.h"
#include "barelymusician/composition/note_pitch.h"
#include "barelymusician/instruments/synth_instrument.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"
#include "examples/performers/metronome.h"

namespace {

using ::barely::Instrument;
using ::barely::Musician;
using ::barely::OscillatorType;
using ::barely::SynthControl;
using ::barely::SynthInstrument;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;
using ::barely::examples::Metronome;

// System audio settings.
constexpr int kFrameRate = 48000;
constexpr int kChannelCount = 2;
constexpr int kFrameCount = 1024;

constexpr double kLookahead = 0.1;

// Metronome settings.
constexpr OscillatorType kOscillatorType = OscillatorType::kSquare;
constexpr double kGain = 0.25;
constexpr double kAttack = 0.0;
constexpr double kRelease = 0.025;
constexpr int kVoiceCount = 1;

constexpr double kBarPitch = barely::kPitchA4;
constexpr double kBeatPitch = barely::kPitchA3;

constexpr int kBeatCount = 4;
constexpr double kInitialTempo = 120.0;
constexpr double kTempoIncrement = 10.0;

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
  AudioOutput audio_output;
  InputManager input_manager;

  AudioClock audio_clock(kFrameRate);

  Musician musician;
  musician.SetTempo(kInitialTempo);

  // Create metronome instrument.
  Instrument instrument =
      musician.CreateInstrument(SynthInstrument::GetDefinition(), kFrameRate);
  instrument.SetControl(SynthControl::kGain, kGain);
  instrument.SetControl(SynthControl::kOscillatorType, kOscillatorType);
  instrument.SetControl(SynthControl::kAttack, kAttack);
  instrument.SetControl(SynthControl::kRelease, kRelease);
  instrument.SetControl(SynthControl::kVoiceCount, kVoiceCount);

  // Add beat event.
  Metronome metronome(musician.CreatePerformer());
  metronome.SetBeatCallback([&](int beat) {
    ConsoleLog() << "Tick " << (beat / kBeatCount) << "."
                 << (beat % kBeatCount);
    const double pitch = (beat % kBeatCount == 0) ? kBarPitch : kBeatPitch;
    instrument.SetNoteOn(pitch);
    instrument.SetNoteOff(pitch);
    ++beat;
  });

  // Audio process callback.
  const auto process_callback = [&](double* output) {
    instrument.Process(output, kChannelCount, kFrameCount,
                       audio_clock.GetTimestamp());
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
          ConsoleLog() << "Stopped playback";
        } else {
          metronome.Start();
          ConsoleLog() << "Started playback";
        }
        return;
      case '\r':
        metronome.Reset();
        ConsoleLog() << "Reset playback";
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
    musician.SetTempo(tempo);
    ConsoleLog() << "Tempo set to " << musician.GetTempo() << " bpm";
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start(kFrameRate, kChannelCount, kFrameCount);
  metronome.Start();

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
