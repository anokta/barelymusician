#include <algorithm>
#include <cctype>
#include <chrono>
#include <memory>
#include <optional>
#include <thread>

#include "barelymusician/common/logging.h"
#include "barelymusician/composition/note_utils.h"
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
constexpr int kNumFrames = 512;

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

// Returns the pitch for the given |key|.
std::optional<float> PitchFromKey(const InputManager::Key& key) {
  const auto it = std::find(std::cbegin(kOctaveKeys), std::cend(kOctaveKeys),
                            std::toupper(key));
  if (it == std::cend(kOctaveKeys)) {
    return std::nullopt;
  }
  const float distance =
      static_cast<float>(std::distance(std::cbegin(kOctaveKeys), it));
  return kRootPitch + distance / barelyapi::kNumSemitones;
}

}  // namespace

int main(int /*argc*/, char* /*argv*/[]) {
  AudioOutput audio_output;
  InputManager input_manager;

  Engine engine(kSampleRate);
  const int instrument_id = engine.CreateInstrument(
      SynthInstrument::GetDefinition(),
      {{SynthInstrumentParam::kNumVoices, static_cast<float>(kNumVoices)},
       {SynthInstrumentParam::kGain, kGain},
       {SynthInstrumentParam::kOscillatorType,
        static_cast<float>(kOscillatorType)},
       {SynthInstrumentParam::kEnvelopeAttack, kEnvelopeAttack},
       {SynthInstrumentParam::kEnvelopeRelease, kEnvelopeRelease}});
  engine.SetNoteOnCallback([](int, float pitch, float intensity) {
    LOG(INFO) << "NoteOn(" << pitch << ", " << intensity << ")";
  });
  engine.SetNoteOffCallback(
      [](int, float pitch) { LOG(INFO) << "NoteOff(" << pitch << ") "; });

  // Audio process callback.
  audio_output.SetProcessCallback([&](float* output) {
    engine.ProcessInstrument(instrument_id, 0.0, output, kNumChannels,
                             kNumFrames);
  });

  // Key down callback.
  float offset_octaves = 0.0f;
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
      engine.SetAllInstrumentNotesOff(instrument_id);
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
    if (const auto pitch = PitchFromKey(key)) {
      engine.SetInstrumentNoteOn(instrument_id, offset_octaves + *pitch,
                                 kNoteIntensity);
    }
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Key up callback.
  const auto key_up_callback = [&](const InputManager::Key& key) {
    // Stop note.
    if (const auto pitch = PitchFromKey(key)) {
      engine.SetInstrumentNoteOff(instrument_id, offset_octaves + *pitch);
    }
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
