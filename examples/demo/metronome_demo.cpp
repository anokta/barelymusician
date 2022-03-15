#include <cctype>
#include <chrono>
#include <thread>

#include "barelymusician/api/conductor.h"
#include "barelymusician/api/instrument.h"
#include "barelymusician/api/presets/instruments.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"
#include "examples/composition/note_pitch.h"

namespace {

using ::barely::Conductor;
using ::barely::Instrument;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;
using ::barely::presets::CreateInstrument;
using ::barely::presets::InstrumentType;
using ::barely::presets::OscillatorType;
using ::barely::presets::SynthParameter;

// System audio settings.
constexpr int kFrameRate = 48000;
constexpr int kNumChannels = 2;
constexpr int kNumFrames = 1024;

constexpr double kLookahead = 0.1;

// Metronome settings.
constexpr int kNumVoices = 1;
constexpr double kGain = 0.25;
constexpr OscillatorType kOscillatorType = OscillatorType::kSquare;
constexpr double kAttack = 0.0;
constexpr double kRelease = 0.025;

constexpr double kBarPitch = barelyapi::kPitchA4;
constexpr double kBeatPitch = barelyapi::kPitchA3;

constexpr int kNumBeats = 4;
constexpr double kInitialTempo = 120.0;
constexpr double kTempoIncrement = 10.0;

}  // namespace

int main(int /*argc*/, char* /*argv*/[]) {
  AudioOutput audio_output;
  InputManager input_manager;

  AudioClock audio_clock(kFrameRate);

  // Create metronome instrument.
  Instrument metronome = CreateInstrument(InstrumentType::kSynth, kFrameRate);
  metronome.SetParameter(SynthParameter::kAttack, kAttack);
  metronome.SetParameter(SynthParameter::kRelease, kRelease);
  metronome.SetParameter(SynthParameter::kOscillatorType,
                         static_cast<double>(kOscillatorType));
  metronome.SetParameter(SynthParameter::kNumVoices,
                         static_cast<double>(kNumVoices));

  // Create conductor.
  Conductor conductor;
  conductor.SetTempo(kInitialTempo);

  // Beat callback.
  const auto beat_callback = [&](double position, double timestamp) {
    const int current_bar = static_cast<int>(position) / kNumBeats;
    const int current_beat = static_cast<int>(position) % kNumBeats;
    ConsoleLog() << "Tick " << current_bar << "." << current_beat;
    const double pitch = (current_beat == 0) ? kBarPitch : kBeatPitch;
    metronome.StartNote(pitch, kGain, timestamp);
    metronome.StopNote(pitch, timestamp);
  };
  conductor.SetBeatCallback(beat_callback);

  // Audio process callback.
  const auto process_callback = [&](double* output) {
    metronome.Process(output, kNumChannels, kNumFrames,
                      audio_clock.GetTimestamp());
    audio_clock.Update(kNumFrames);
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
    double tempo = conductor.GetTempo();
    switch (std::toupper(key)) {
      case ' ':
        if (conductor.IsPlaying()) {
          conductor.Stop();
          ConsoleLog() << "Stopped playback";
        } else {
          conductor.Start();
          ConsoleLog() << "Started playback";
        }
        return;
      case '-':
        tempo -= kTempoIncrement;
        break;
      case '+':
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
    conductor.SetTempo(tempo);
    ConsoleLog() << "Tempo set to " << conductor.GetTempo() << " bpm";
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start(kFrameRate, kNumChannels, kNumFrames);
  conductor.Start();

  while (!quit) {
    input_manager.Update();
    conductor.Update(audio_clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  ConsoleLog() << "Stopping audio stream";
  conductor.Stop();
  audio_output.Stop();

  return 0;
}
