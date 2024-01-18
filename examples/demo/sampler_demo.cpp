#include <algorithm>
#include <array>
#include <cassert>
#include <cctype>
#include <chrono>
#include <iterator>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/rational.h"
#include "barelymusician/composition/pitch.h"
#include "barelymusician/effects/low_pass_effect.h"
#include "barelymusician/instruments/sampler_instrument.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"
#include "examples/common/wav_file.h"
#include "examples/data/data.h"

namespace {

using ::barely::LowPassEffect;
using ::barely::Musician;
using ::barely::Rational;
using ::barely::SamplerInstrument;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::GetDataFilePath;
using ::barely::examples::InputManager;
using ::barely::examples::WavFile;

// System audio settings.
constexpr int kFrameRate = 48000;
constexpr int kChannelCount = 2;
constexpr int kFrameCount = 256;

// Instrument settings.
constexpr Rational kGain = Rational(1, 4);
constexpr bool kLoop = true;
constexpr Rational kAttack = Rational(1, 80);
constexpr Rational kRelease = Rational(1, 8);
constexpr int kVoiceCount = 16;

constexpr char kSamplePath[] = "audio/sample.wav";

constexpr Rational kLowPassCutoffFrequency = 2000;

// Note settings.
constexpr Rational kRootPitch = barely::kPitchC4;
constexpr std::array<char, 13> kOctaveKeys = {'A', 'W', 'S', 'E', 'D', 'F', 'T',
                                              'G', 'Y', 'H', 'U', 'J', 'K'};
constexpr int kMaxOffsetOctaves = 3;

// Returns the sample data from a given `file_path`.
std::vector<float> GetSampleData(const std::string& file_path) {
  WavFile sample_file;
  [[maybe_unused]] const bool success = sample_file.Load(file_path);
  assert(success);

  const float frame_rate = static_cast<float>(sample_file.GetFrameRate());
  const auto& sample_data = sample_file.GetData();

  std::vector<float> data;
  data.reserve(sample_data.size() + 1);
  data.push_back(frame_rate);
  data.insert(data.end(), sample_data.begin(), sample_data.end());
  return data;
}

// Returns the pitch for a given `key`.
std::optional<Rational> PitchFromKey(const InputManager::Key& key) {
  const auto it = std::find(kOctaveKeys.begin(), kOctaveKeys.end(), std::toupper(key));
  if (it == kOctaveKeys.end()) {
    return std::nullopt;
  }
  const Rational distance = std::distance(kOctaveKeys.begin(), it);
  return kRootPitch + distance / barely::kSemitoneCount;
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int /*argc*/, char* argv[]) {
  AudioOutput audio_output;
  InputManager input_manager;

  Musician musician(kFrameRate);

  auto instrument = musician.CreateInstrument<SamplerInstrument>();
  instrument.SetControl(SamplerInstrument::Control::kGain, kGain);
  instrument.SetControl(SamplerInstrument::Control::kRootPitch, kRootPitch);
  instrument.SetControl(SamplerInstrument::Control::kLoop, kLoop);
  instrument.SetControl(SamplerInstrument::Control::kAttack, kAttack);
  instrument.SetControl(SamplerInstrument::Control::kRelease, kRelease);
  instrument.SetControl(SamplerInstrument::Control::kVoiceCount, kVoiceCount);

  auto effect = instrument.CreateEffect<LowPassEffect>();
  effect.SetControl(LowPassEffect::Control::kCutoffFrequency, kLowPassCutoffFrequency);

  instrument.SetData(GetSampleData(GetDataFilePath(kSamplePath, argv)));

  instrument.SetNoteOnEvent([](Rational pitch, Rational intensity) {
    ConsoleLog() << "NoteOn(" << pitch << ", " << intensity << ")";
  });
  instrument.SetNoteOffEvent([](Rational pitch) { ConsoleLog() << "NoteOff(" << pitch << ") "; });

  // Audio process callback.
  audio_output.SetProcessCallback([&](float* output) {
    instrument.Process(output, kChannelCount, kFrameCount, /*timestamp=*/0);
  });

  // Key down callback.
  Rational intensity = 1;
  int offset_octaves = 0;
  bool quit = false;
  const auto key_down_callback = [&](const InputManager::Key& key) {
    if (static_cast<int>(key) == 27) {
      // ESC pressed, quit the app.
      quit = true;
      return;
    }

    const auto upper_key = std::toupper(key);
    if (upper_key == 'Z' || upper_key == 'X') {
      // Shift octaves.
      instrument.SetAllNotesOff();
      if (upper_key == 'Z') {
        --offset_octaves;
      } else {
        ++offset_octaves;
      }
      offset_octaves = std::clamp(offset_octaves, -kMaxOffsetOctaves, kMaxOffsetOctaves);
      ConsoleLog() << "Octave offset set to " << offset_octaves;
      return;
    }
    if (upper_key == 'C' || upper_key == 'V') {
      // Change intensity.
      if (upper_key == 'C') {
        intensity -= Rational(1, 4);
      } else {
        intensity += Rational(1, 4);
      }
      intensity = std::clamp(intensity, Rational(0), Rational(1));
      ConsoleLog() << "Note intensity set to " << intensity;
      return;
    }

    // Play note.
    if (const auto pitch = PitchFromKey(key)) {
      instrument.SetNoteOn(offset_octaves + *pitch, intensity);
    }
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Key up callback.
  const auto key_up_callback = [&](const InputManager::Key& key) {
    // Stop note.
    if (const auto pitch = PitchFromKey(key)) {
      instrument.SetNoteOff(offset_octaves + *pitch);
    }
  };
  input_manager.SetKeyUpCallback(key_up_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start(kFrameRate, kChannelCount, kFrameCount);

  ConsoleLog() << "Play the sampler using the keyboard keys:";
  ConsoleLog() << "  * Use ASDFFGHJK keys to play the white notes in an octave";
  ConsoleLog() << "  * Use WETYU keys to play the black notes in an octave";
  ConsoleLog() << "  * Use ZX keys to set the octave up and down";
  ConsoleLog() << "  * Use CV keys to to set the note intensity up and down";

  while (!quit) {
    input_manager.Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  ConsoleLog() << "Stopping audio stream";
  audio_output.Stop();

  return 0;
}
