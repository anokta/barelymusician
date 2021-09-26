#include <atomic>
#include <cctype>
#include <chrono>
#include <cmath>
#include <memory>
#include <thread>

#include "barelymusician/common/id.h"
#include "barelymusician/common/logging.h"
#include "barelymusician/composition/note_utils.h"
#include "barelymusician/engine/clock.h"
#include "barelymusician/engine/instrument_manager.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/input_manager.h"
#include "examples/instruments/synth_instrument.h"

namespace {

using ::barelyapi::Clock;
using ::barelyapi::Id;
using ::barelyapi::InstrumentManager;
using ::barelyapi::OscillatorType;
using ::barelyapi::examples::AudioClock;
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
constexpr Id kMetronomeId = 1;
constexpr int kNumVoices = 1;
constexpr float kGain = 0.5f;
constexpr OscillatorType kOscillatorType = OscillatorType::kSquare;
constexpr float kAttack = 0.0f;
constexpr float kRelease = 0.025f;

constexpr double kTickDuration = 0.005f;
constexpr float kBarPitch = barelyapi::kPitchA4;
constexpr float kBeatPitch = barelyapi::kPitchA3;

constexpr int kNumBeats = 4;
constexpr double kInitialTempo = 2.0;
constexpr double kTempoIncrement = 0.1;

}  // namespace

int main(int /*argc*/, char* /*argv*/[]) {
  AudioOutput audio_output;
  InputManager input_manager;

  AudioClock audio_clock(kSampleRate);
  InstrumentManager instrument_manager(kSampleRate);

  instrument_manager.Create(
      kMetronomeId, 0.0, SynthInstrument::GetDefinition(),
      {{static_cast<int>(SynthInstrumentParam::kNumVoices),
        static_cast<float>(kNumVoices)},
       {static_cast<int>(SynthInstrumentParam::kGain), kGain},
       {static_cast<int>(SynthInstrumentParam::kOscillatorType),
        static_cast<float>(kOscillatorType)},
       {static_cast<int>(SynthInstrumentParam::kEnvelopeAttack), kAttack},
       {static_cast<int>(SynthInstrumentParam::kEnvelopeRelease), kRelease}});

  double is_playing = true;
  double tempo = kInitialTempo;
  Clock clock;
  clock.SetTempo(tempo);

  // Beat callback.
  const auto beat_callback = [&](double position, double timestamp) {
    const int current_bar = std::abs(static_cast<int>(position)) / kNumBeats;
    const int current_beat = std::abs(static_cast<int>(position)) % kNumBeats;
    LOG(INFO) << "Tick " << ((position < 0.0) ? "-" : "") << current_bar << "."
              << current_beat;
    const float pitch = (current_beat == 0) ? kBarPitch : kBeatPitch;
    instrument_manager.SetNoteOn(kMetronomeId, timestamp, pitch, kGain);
    const double end_timestamp = clock.GetTimestampAtPosition(
        position + ((tempo > 0.0) ? kTickDuration : -kTickDuration));
    instrument_manager.SetNoteOff(kMetronomeId, end_timestamp, pitch);
  };
  clock.SetBeatCallback(beat_callback);

  // Audio process callback.
  const auto process_callback = [&](float* output) {
    instrument_manager.Process(kMetronomeId, audio_clock.GetTimestamp(), output,
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
    switch (std::toupper(key)) {
      case ' ':
        if (is_playing) {
          is_playing = false;
          clock.SetTempo(0.0);
          LOG(INFO) << "Stopped playback";
        } else {
          is_playing = true;
          clock.SetTempo(tempo);
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
      case 'X':
        tempo *= -1.0;
        break;
      case 'R':
        tempo = kInitialTempo;
        break;
      default:
        return;
    }
    if (is_playing) {
      clock.SetTempo(tempo);
    }
    LOG(INFO) << "Tempo set to " << (60.0 * tempo) << " BPM";
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  LOG(INFO) << "Starting audio stream";
  audio_output.Start(kSampleRate, kNumChannels, kNumFrames);

  while (!quit) {
    input_manager.Update();
    instrument_manager.Update();
    clock.UpdatePosition(audio_clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  LOG(INFO) << "Stopping audio stream";
  audio_output.Stop();

  return 0;
}
