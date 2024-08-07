#include <algorithm>
#include <array>
#include <cassert>
#include <cctype>
#include <chrono>
#include <iomanip>
#include <iterator>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/composition/pitch.h"
#include "barelymusician/effects/low_pass_effect.h"
#include "barelymusician/instruments/sampler_instrument.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"
#include "examples/common/wav_file.h"
#include "examples/data/data.h"

namespace {

using ::barely::Effect;
using ::barely::Instrument;
using ::barely::LowPassEffect;
using ::barely::Musician;
using ::barely::Note;
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
constexpr double kGain = 0.25;
constexpr bool kLoop = true;
constexpr double kAttack = 0.0125;
constexpr double kRelease = 0.125;
constexpr int kVoiceCount = 16;

constexpr char kSamplePath[] = "audio/sample.wav";

constexpr double kLowPassCutoffFrequency = 2000;

// Note settings.
constexpr double kRootPitch = barely::kPitchC4;
constexpr std::array<char, 13> kOctaveKeys = {'A', 'W', 'S', 'E', 'D', 'F', 'T',
                                              'G', 'Y', 'H', 'U', 'J', 'K'};
constexpr int kMaxOctave = 3;

// Returns the sample data from a given `file_path`.
std::vector<double> GetSampleData(const std::string& file_path) {
  WavFile sample_file;
  [[maybe_unused]] const bool success = sample_file.Load(file_path);
  assert(success);

  const double frame_rate = static_cast<double>(sample_file.GetFrameRate());
  const auto& sample_data = sample_file.GetData();

  std::vector<double> data;
  data.reserve(sample_data.size() + 1);
  data.push_back(frame_rate);
  data.insert(data.end(), sample_data.begin(), sample_data.end());
  return data;
}

// Returns the pitch for a given `key`.
std::optional<double> PitchFromKey(int octave, const InputManager::Key& key) {
  const auto it = std::find(kOctaveKeys.begin(), kOctaveKeys.end(), std::toupper(key));
  if (it == kOctaveKeys.end()) {
    return std::nullopt;
  }
  const double distance = static_cast<double>(std::distance(kOctaveKeys.begin(), it));
  return octave + kRootPitch + distance / barely::kSemitoneCount;
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int /*argc*/, char* argv[]) {
  AudioOutput audio_output;
  InputManager input_manager;

  Musician musician(kFrameRate);

  Instrument instrument(musician, SamplerInstrument::GetDefinition());
  instrument.GetControl(SamplerInstrument::Control::kGain).SetValue(kGain);
  instrument.GetControl(SamplerInstrument::Control::kRootPitch).SetValue(kRootPitch);
  instrument.GetControl(SamplerInstrument::Control::kLoop).SetValue(kLoop);
  instrument.GetControl(SamplerInstrument::Control::kAttack).SetValue(kAttack);
  instrument.GetControl(SamplerInstrument::Control::kRelease).SetValue(kRelease);
  instrument.GetControl(SamplerInstrument::Control::kVoiceCount).SetValue(kVoiceCount);

  Effect effect(musician, LowPassEffect::GetDefinition());
  effect.GetControl(LowPassEffect::Control::kCutoffFrequency).SetValue(kLowPassCutoffFrequency);

  instrument.SetData(GetSampleData(GetDataFilePath(kSamplePath, argv)));

  int octave = 0;
  std::unordered_map<InputManager::Key, Note> notes;

  // Key down callback.
  bool quit = false;
  double intensity = 1.0;
  const auto key_down_callback = [&](const InputManager::Key& key) {
    if (static_cast<int>(key) == 27) {
      // ESC pressed, quit the app.
      quit = true;
      return;
    }

    const auto upper_key = std::toupper(key);

    if (upper_key == 'Z' || upper_key == 'X') {
      // Stop all notes.
      notes.clear();

      // Shift octaves.
      if (upper_key == 'Z') {
        --octave;
      } else {
        ++octave;
      }
      octave = std::clamp(octave, -kMaxOctave, kMaxOctave);
      ConsoleLog() << "Octave set to " << octave;
      return;
    }

    // Change intensity.
    if (upper_key == 'C' || upper_key == 'V') {
      if (upper_key == 'C') {
        intensity -= 0.25;
      } else {
        intensity += 0.25;
      }
      intensity = std::clamp(intensity, 0.0, 1.0);
      ConsoleLog() << "Note intensity set to " << intensity;
      return;
    }

    // Start note.
    if (const auto pitch = PitchFromKey(octave, key)) {
      [[maybe_unused]] const auto success =
          notes.emplace(key, Note(instrument, *pitch, intensity)).second;
      assert(success);
      ConsoleLog() << std::setprecision(2) << "NoteOn(" << *pitch << ", " << intensity << ")";
    }
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Key up callback.
  const auto key_up_callback = [&](const InputManager::Key& key) {
    if (notes.erase(key) > 0) {
      ConsoleLog() << std::setprecision(2) << "NoteOff(" << *PitchFromKey(octave, key) << ") ";
    }
  };
  input_manager.SetKeyUpCallback(key_up_callback);

  // Audio process callback.
  audio_output.SetProcessCallback([&](double* output) {
    instrument.Process(output, kChannelCount, kFrameCount, /*timestamp=*/0.0);
  });

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
