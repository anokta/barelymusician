#include <cctype>
#include <chrono>
#include <memory>
#include <thread>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/id.h"
#include "barelymusician/common/logging.h"
#include "barelymusician/composition/note_pitch.h"
#include "barelymusician/engine/instrument_manager.h"
#include "barelymusician/engine/transport.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/input_manager.h"
#include "examples/instruments/synth_instrument.h"

namespace {

using ::barelyapi::BarelyMusician;
using ::barelyapi::Id;
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
constexpr int kNumVoices = 1;
constexpr float kGain = 0.5f;
constexpr OscillatorType kOscillatorType = OscillatorType::kSquare;
constexpr float kAttack = 0.0f;
constexpr float kRelease = 0.025f;

constexpr float kBarPitch = barelyapi::kPitchA4;
constexpr float kBeatPitch = barelyapi::kPitchA3;

constexpr int kNumBeats = 4;
constexpr double kInitialTempo = 120.0;
constexpr double kTempoIncrement = 10.0;

}  // namespace

int main(int /*argc*/, char* /*argv*/[]) {
  AudioOutput audio_output;
  InputManager input_manager;

  AudioClock audio_clock(kSampleRate);

  BarelyMusician barelymusician(kSampleRate);
  barelymusician.SetPlaybackTempo(kInitialTempo);

  // Create metronome instrument.
  const Id metronome_id = barelymusician.CreateInstrument(
      SynthInstrument::GetDefinition(),
      {{SynthInstrumentParam::kNumVoices, static_cast<float>(kNumVoices)},
       {SynthInstrumentParam::kGain, kGain},
       {SynthInstrumentParam::kOscillatorType,
        static_cast<float>(kOscillatorType)},
       {SynthInstrumentParam::kEnvelopeAttack, kAttack},
       {SynthInstrumentParam::kEnvelopeRelease, kRelease}});

  // Beat callback.
  const auto beat_callback = [&](double position) {
    const int current_bar = static_cast<int>(position) / kNumBeats;
    const int current_beat = static_cast<int>(position) % kNumBeats;
    LOG(INFO) << "Tick " << current_bar << "." << current_beat;
    const float pitch = (current_beat == 0) ? kBarPitch : kBeatPitch;
    barelymusician.SetInstrumentNoteOn(metronome_id, pitch, kGain);
    barelymusician.SetInstrumentNoteOff(metronome_id, pitch);
  };
  barelymusician.SetPlaybackBeatCallback(beat_callback);

  // Audio process callback.
  const auto process_callback = [&](float* output) {
    barelymusician.ProcessInstrument(metronome_id, audio_clock.GetTimestamp(),
                                     output, kNumChannels, kNumFrames);
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
    double tempo = barelymusician.GetPlaybackTempo();
    switch (std::toupper(key)) {
      case ' ':
        if (barelymusician.IsPlaying()) {
          barelymusician.StopPlayback();
          LOG(INFO) << "Stopped playback";
        } else {
          barelymusician.StartPlayback();
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
    barelymusician.SetPlaybackTempo(tempo);
    LOG(INFO) << "Tempo set to " << tempo << " BPM";
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  LOG(INFO) << "Starting audio stream";
  audio_output.Start(kSampleRate, kNumChannels, kNumFrames);
  barelymusician.StartPlayback();

  while (!quit) {
    input_manager.Update();
    barelymusician.Update(audio_clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  LOG(INFO) << "Stopping audio stream";
  audio_output.Stop();

  return 0;
}
