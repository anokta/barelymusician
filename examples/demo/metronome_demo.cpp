#include <cctype>
#include <chrono>
#include <thread>

#include "barelymusician/composition/note_pitch.h"
#include "barelymusician/engine/musician.h"
#include "barelymusician/engine/param_definition.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"
#include "examples/instruments/synth_instrument.h"

namespace {

using ::barely::Musician;
using ::barely::OscillatorType;
using ::barely::ParamDefinition;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;
using ::barely::examples::SynthInstrument;
using ::barely::examples::SynthInstrumentParam;

// System audio settings.
constexpr int kSampleRate = 48000;
constexpr int kNumChannels = 2;
constexpr int kNumFrames = 1024;

constexpr double kLookahead = 0.1;

// Metronome settings.
constexpr int kNumVoices = 1;
constexpr float kGain = 0.25f;
constexpr OscillatorType kOscillatorType = OscillatorType::kSquare;
constexpr float kAttack = 0.0f;
constexpr float kRelease = 0.025f;

constexpr float kBarPitch = barely::kPitchA4;
constexpr float kBeatPitch = barely::kPitchA3;

constexpr int kNumBeats = 4;
constexpr double kInitialTempo = 120.0;
constexpr double kTempoIncrement = 10.0;

}  // namespace

int main(int /*argc*/, char* /*argv*/[]) {
  AudioOutput audio_output;
  InputManager input_manager;

  AudioClock audio_clock(kSampleRate);

  Musician musician(kSampleRate);
  musician.SetPlaybackTempo(kInitialTempo);

  // Create metronome instrument.
  const auto metronome_id =
      musician.AddInstrument(SynthInstrument::GetDefinition());
  musician.SetInstrumentParam(metronome_id,
                              SynthInstrumentParam::kEnvelopeAttack, kAttack);
  musician.SetInstrumentParam(metronome_id,
                              SynthInstrumentParam::kEnvelopeRelease, kRelease);
  musician.SetInstrumentParam(metronome_id,
                              SynthInstrumentParam::kOscillatorType,
                              static_cast<float>(kOscillatorType));
  musician.SetInstrumentParam(metronome_id, SynthInstrumentParam::kNumVoices,
                              static_cast<float>(kNumVoices));

  // Beat callback.
  const auto beat_callback = [&](double position) {
    const int current_bar = static_cast<int>(position) / kNumBeats;
    const int current_beat = static_cast<int>(position) % kNumBeats;
    ConsoleLog() << "Tick " << current_bar << "." << current_beat;
    const float pitch = (current_beat == 0) ? kBarPitch : kBeatPitch;
    musician.SetInstrumentNoteOn(metronome_id, pitch, kGain);
    musician.SetInstrumentNoteOff(metronome_id, pitch);
  };
  musician.SetPlaybackBeatCallback(beat_callback);

  // Audio process callback.
  const auto process_callback = [&](float* output) {
    musician.ProcessInstrument(metronome_id, audio_clock.GetTimestamp(), output,
                               kNumChannels, kNumFrames);
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
    double tempo = musician.GetPlaybackTempo();
    switch (std::toupper(key)) {
      case ' ':
        if (musician.IsPlaying()) {
          musician.StopPlayback();
          ConsoleLog() << "Stopped playback";
        } else {
          musician.StartPlayback();
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
    musician.SetPlaybackTempo(tempo);
    ConsoleLog() << "Tempo set to " << tempo << " BPM";
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start(kSampleRate, kNumChannels, kNumFrames);
  musician.StartPlayback();

  while (!quit) {
    input_manager.Update();
    musician.Update(audio_clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  ConsoleLog() << "Stopping audio stream";
  musician.StopPlayback();
  audio_output.Stop();

  return 0;
}
