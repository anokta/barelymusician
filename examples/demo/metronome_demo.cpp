#include <atomic>
#include <cctype>
#include <chrono>
#include <cmath>
#include <memory>
#include <thread>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/base/types.h"
#include "barelymusician/dsp/dsp_utils.h"
#include "barelymusician/engine/engine.h"
#include "examples/instruments/basic_synth_instrument.h"
#include "examples/util/audio_output.h"
#include "examples/util/input_manager.h"

namespace {

using ::barelyapi::Engine;
using ::barelyapi::int64;
using ::barelyapi::OscillatorType;
using ::barelyapi::examples::AudioOutput;
using ::barelyapi::examples::BasicSynthInstrument;
using ::barelyapi::examples::BasicSynthInstrumentParam;
using ::barelyapi::examples::InputManager;

// System audio settings.
constexpr int kSampleRate = 48000;
constexpr int kNumChannels = 2;
constexpr int kNumFrames = 1024;

constexpr int64 kLookahead = 4 * kNumFrames;

// Metronome settings.
constexpr int kNumVoices = 1;
constexpr float kGain = 0.5f;
constexpr OscillatorType kOscillatorType = OscillatorType::kSquare;
constexpr float kAttack = 0.0f;
constexpr float kRelease = 0.025f;

constexpr double kTickDuration = 0.005f;
constexpr float kBarPitch = barelyapi::kPitchA4;
constexpr float kBeatPitch = barelyapi::kPitchA3;

constexpr int kNumBeats = 4;
constexpr double kInitialTempo = 120.0;
constexpr double kTempoIncrement = 10.0;

}  // namespace

int main(int /*argc*/, char* /*argv*/[]) {
  AudioOutput audio_output;
  InputManager input_manager;

  Engine engine;
  engine.SetTempo(kInitialTempo);

  const auto metronome_id = GetValue(engine.Create(
      BasicSynthInstrument::GetDefinition(kSampleRate),
      {{static_cast<int>(BasicSynthInstrumentParam::kNumVoices),
        static_cast<float>(kNumVoices)},
       {static_cast<int>(BasicSynthInstrumentParam::kGain), kGain},
       {static_cast<int>(BasicSynthInstrumentParam::kOscillatorType),
        static_cast<float>(kOscillatorType)},
       {static_cast<int>(BasicSynthInstrumentParam::kEnvelopeAttack), kAttack},
       {static_cast<int>(BasicSynthInstrumentParam::kEnvelopeRelease),
        kRelease}}));

  // Beat callback.
  const auto beat_callback = [&](int64, int beat) {
    const int current_bar = beat / kNumBeats;
    const int current_beat = beat % kNumBeats;
    LOG(INFO) << "Tick " << current_bar << "." << current_beat;
    const double position = static_cast<double>(beat);
    const float pitch = (current_beat == 0) ? kBarPitch : kBeatPitch;
    engine.ScheduleNote(metronome_id, position, kTickDuration, pitch, kGain);
  };
  engine.SetBeatCallback(beat_callback);

  // Audio process callback.
  std::atomic<int64> timestamp = 0;
  const auto process_callback = [&](float* output) {
    engine.Process(metronome_id, timestamp, output, kNumChannels, kNumFrames);
    timestamp += kNumFrames;
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
    double tempo = engine.GetTempo();
    switch (std::toupper(key)) {
      case ' ':
        if (engine.IsPlaying()) {
          engine.Stop();
          LOG(INFO) << "Stopped playback";
        } else {
          engine.Start(timestamp + kLookahead);
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
    engine.SetTempo(tempo);
    LOG(INFO) << "Tempo set to " << tempo;
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  LOG(INFO) << "Starting audio stream";
  audio_output.Start(kSampleRate, kNumChannels, kNumFrames);
  engine.Start(timestamp + kLookahead);

  while (!quit) {
    input_manager.Update();
    engine.Update(kSampleRate, timestamp + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  LOG(INFO) << "Stopping audio stream";
  engine.Stop();
  audio_output.Stop();

  return 0;
}
