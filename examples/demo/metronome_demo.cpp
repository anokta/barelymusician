#include <atomic>
#include <cctype>
#include <chrono>
#include <cmath>
#include <memory>
#include <thread>

#include "barelymusician/common/logging.h"
#include "barelymusician/dsp/dsp_utils.h"
#include "barelymusician/engine/engine.h"
#include "examples/common/audio_output.h"
#include "examples/common/input_manager.h"
#include "examples/instruments/synth_instrument.h"

namespace {

using ::barelyapi::Engine;
using ::barelyapi::OscillatorType;
using ::barelyapi::examples::AudioOutput;
using ::barelyapi::examples::InputManager;
using ::barelyapi::examples::SynthInstrument;
using ::barelyapi::examples::SynthInstrumentParam;

// System audio settings.
constexpr int kSampleRate = 48000;
constexpr int kNumChannels = 2;
constexpr int kNumFrames = 1024;

constexpr double kLookahead = 0.1;

// Metronome settings.
constexpr int kNumVoices = 1;
constexpr float kGain = 0.5f;
constexpr OscillatorType kOscillatorType = OscillatorType::kSquare;
constexpr float kAttack = 0.0f;
constexpr float kRelease = 0.025f;

constexpr double kTickDuration = 0.005f;
constexpr float kBarPitch = 0.0f;
constexpr float kBeatPitch = -1.0f;

constexpr int kNumBeats = 4;
constexpr double kInitialTempo = 120.0;
constexpr double kTempoIncrement = 10.0;

}  // namespace

int main(int /*argc*/, char* /*argv*/[]) {
  AudioOutput audio_output;
  InputManager input_manager;

  Engine engine(kSampleRate);
  engine.SetPlaybackTempo(kInitialTempo);

  const auto metronome_id = engine.CreateInstrument(
      SynthInstrument::GetDefinition(),
      {{static_cast<int>(SynthInstrumentParam::kNumVoices),
        static_cast<float>(kNumVoices)},
       {static_cast<int>(SynthInstrumentParam::kGain), kGain},
       {static_cast<int>(SynthInstrumentParam::kOscillatorType),
        static_cast<float>(kOscillatorType)},
       {static_cast<int>(SynthInstrumentParam::kEnvelopeAttack), kAttack},
       {static_cast<int>(SynthInstrumentParam::kEnvelopeRelease), kRelease}});

  // Beat callback.
  const auto beat_callback = [&](double, int beat) {
    const int current_bar = beat / kNumBeats;
    const int current_beat = beat % kNumBeats;
    LOG(INFO) << "Tick " << current_bar << "." << current_beat;
    const double position = static_cast<double>(beat);
    const float pitch = (current_beat == 0) ? kBarPitch : kBeatPitch;
    engine.ScheduleInstrumentNote(metronome_id, position, kTickDuration, pitch,
                                  kGain);
  };
  engine.SetBeatCallback(beat_callback);

  // Audio process callback.
  std::atomic<double> timestamp = 0.0;
  const auto process_callback = [&](float* output) {
    engine.ProcessInstrument(metronome_id, timestamp, output, kNumChannels,
                             kNumFrames);
    timestamp +=
        static_cast<double>(kNumFrames) / static_cast<double>(kSampleRate);
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
    double tempo = engine.GetPlaybackTempo();
    switch (std::toupper(key)) {
      case ' ':
        if (engine.IsPlaying()) {
          engine.StopPlayback();
          LOG(INFO) << "Stopped playback";
        } else {
          engine.StartPlayback();
          LOG(INFO) << "Started playback";
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
    engine.SetPlaybackTempo(tempo);
    LOG(INFO) << "Tempo set to " << tempo;
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  LOG(INFO) << "Starting audio stream";
  audio_output.Start(kSampleRate, kNumChannels, kNumFrames);
  engine.Update(timestamp + kLookahead);
  engine.StartPlayback();

  while (!quit) {
    input_manager.Update();
    engine.Update(timestamp + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  LOG(INFO) << "Stopping audio stream";
  engine.StopPlayback();
  audio_output.Stop();

  return 0;
}
