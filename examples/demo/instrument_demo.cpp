#include <algorithm>
#include <cctype>
#include <chrono>
#include <memory>
#include <thread>

#include "barelymusician/common/logging.h"
#include "barelymusician/composition/note_utils.h"
#include "barelymusician/engine/task_runner.h"
#include "examples/common/audio_output.h"
#include "examples/common/input_manager.h"
#include "examples/instruments/synth_instrument.h"

namespace {

using ::barelyapi::OscillatorType;
using ::barelyapi::TaskRunner;
using ::barelyapi::examples::AudioOutput;
using ::barelyapi::examples::InputManager;
using ::barelyapi::examples::SynthInstrument;
using ::barelyapi::examples::SynthInstrumentParam;

// System audio settings.
constexpr int kSampleRate = 48000;
constexpr int kNumChannels = 2;
constexpr int kNumFrames = 512;

constexpr int kNumMaxTasks = 100;

// Instrument settings.
constexpr float kGain = 0.125f;
constexpr int kNumVoices = 16;
constexpr OscillatorType kOscillatorType = OscillatorType::kSaw;
constexpr float kEnvelopeAttack = 0.05f;
constexpr float kEnvelopeRelease = 0.125f;

// Note settings.
constexpr float kRootPitch = barelyapi::kPitchC3;
constexpr float kNoteIntensity = 1.0f;
constexpr char kOctaveKeys[] = {'A', 'W', 'S', 'E', 'D', 'F', 'T',
                                'G', 'Y', 'H', 'U', 'J', 'K'};
constexpr float kMaxOffsetOctaves = 3.0f;

// Returns the pitch for the given |key| and |offset_octaves|.
float PitchFromKey(const InputManager::Key& key, float offset_octaves) {
  const auto it = std::find(std::cbegin(kOctaveKeys), std::cend(kOctaveKeys),
                            std::toupper(key));
  if (it == std::cend(kOctaveKeys)) {
    return -1.0f;
  }
  const float distance =
      static_cast<float>(std::distance(std::cbegin(kOctaveKeys), it));
  return kRootPitch + barelyapi::kNumSemitones * offset_octaves + distance;
}

}  // namespace

int main(int /*argc*/, char* /*argv*/[]) {
  AudioOutput audio_output;
  InputManager input_manager;

  TaskRunner task_runner(kNumMaxTasks);

  SynthInstrument instrument(kSampleRate);
  instrument.SetParam(SynthInstrumentParam::kNumVoices,
                      static_cast<float>(kNumVoices));
  instrument.SetParam(SynthInstrumentParam::kGain, kGain);
  instrument.SetParam(SynthInstrumentParam::kOscillatorType,
                      static_cast<float>(kOscillatorType));
  instrument.SetParam(SynthInstrumentParam::kEnvelopeAttack, kEnvelopeAttack);
  instrument.SetParam(SynthInstrumentParam::kEnvelopeRelease, kEnvelopeRelease);

  float offset_octaves = 0.0f;

  // Audio process callback.
  const auto process_callback = [&](float* output) {
    task_runner.Run();
    instrument.Process(output, kNumChannels, kNumFrames);
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

    // Shift octaves.
    const auto upper_key = std::toupper(key);
    if (upper_key == 'Z' || upper_key == 'X') {
      // Stop current notes first.
      task_runner.Add([&, offset_octaves]() {
        for (const char key : kOctaveKeys) {
          instrument.NoteOff(PitchFromKey(key, offset_octaves));
        }
      });
      // Update offset.
      if (upper_key == 'Z') {
        --offset_octaves;
      } else {
        ++offset_octaves;
      }
      offset_octaves =
          std::clamp(offset_octaves, -kMaxOffsetOctaves, kMaxOffsetOctaves);
      LOG(INFO) << "Octave offset set to " << offset_octaves;
      return;
    }

    // Play note.
    const float pitch = PitchFromKey(key, offset_octaves);
    if (pitch < 0.0f) {
      return;
    }
    task_runner.Add([&, pitch]() { instrument.NoteOn(pitch, kNoteIntensity); });
    LOG(INFO) << "NoteOn(" << pitch << ", " << kNoteIntensity << ")";
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Key up callback.
  const auto key_up_callback = [&](const InputManager::Key& key) {
    // Stop note.
    const float pitch = PitchFromKey(key, offset_octaves);
    if (pitch < 0.0f) {
      return;
    }
    task_runner.Add([&, pitch]() { instrument.NoteOff(pitch); });
    LOG(INFO) << "NoteOff(" << pitch << ")";
  };
  input_manager.SetKeyUpCallback(key_up_callback);

  // Start the demo.
  LOG(INFO) << "Starting audio stream";
  audio_output.Start(kSampleRate, kNumChannels, kNumFrames);

  while (!quit) {
    input_manager.Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  LOG(INFO) << "Stopping audio stream";
  audio_output.Stop();

  return 0;
}
