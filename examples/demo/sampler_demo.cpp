#include <algorithm>
#include <array>
#include <cassert>
#include <cctype>
#include <chrono>
#include <iterator>
#include <optional>
#include <span>
#include <string>
#include <thread>
#include <vector>

#include "barelymusician.h"
#include "common/audio_output.h"
#include "common/console_log.h"
#include "common/input_manager.h"
#include "common/wav_file.h"
#include "data/data.h"

namespace {

using ::barely::ControlType;
using ::barely::Engine;
using ::barely::Slice;
using ::barely::SliceMode;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::GetDataFilePath;
using ::barely::examples::InputManager;
using ::barely::examples::WavFile;

// System audio settings.
constexpr int kSampleRate = 48000;
constexpr int kSampleCount = 256;

// Instrument settings.
constexpr float kGain = 0.25f;
constexpr bool kLoop = true;
constexpr float kAttack = 0.0125f;
constexpr float kRelease = 0.125f;
constexpr int kVoiceCount = 16;

constexpr char kSamplePath[] = "audio/sample.wav";

constexpr float kLowPassCutoffFrequency = 2000.0f;

// Note settings.
constexpr std::array<char, 13> kOctaveKeys = {'A', 'W', 'S', 'E', 'D', 'F', 'T',
                                              'G', 'Y', 'H', 'U', 'J', 'K'};
constexpr float kRootPitch = 0.0f;
constexpr int kMaxOctaveShift = 4;

// Returns the sample data from a given `file_path`.
std::vector<Slice> GetSampleData(const std::string& file_path) {
  WavFile sample_file;
  [[maybe_unused]] const bool success = sample_file.Load(file_path);
  assert(success);

  const static std::vector<float> samples = sample_file.GetData();
  return {Slice(kRootPitch, sample_file.GetSampleRate(), samples)};
}

// Returns the pitch for a given `key`.
std::optional<float> KeyToPitch(int octave_shift, const InputManager::Key& key) {
  const auto it = std::find(kOctaveKeys.begin(), kOctaveKeys.end(), std::toupper(key));
  if (it == kOctaveKeys.end()) {
    return std::nullopt;
  }
  return kRootPitch + static_cast<float>(octave_shift) +
         static_cast<float>(std::distance(kOctaveKeys.begin(), it)) / 12.0f;
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int /*argc*/, char* argv[]) {
  InputManager input_manager;

  AudioOutput audio_output(kSampleRate, kSampleCount);

  Engine engine(kSampleRate);

  auto instrument = engine.CreateInstrument();
  instrument.SetControl(ControlType::kGain, kGain);
  instrument.SetControl(ControlType::kSliceMode, SliceMode::kLoop);
  instrument.SetControl(ControlType::kAttack, kAttack);
  instrument.SetControl(ControlType::kRelease, kRelease);
  instrument.SetControl(ControlType::kVoiceCount, kVoiceCount);

  instrument.SetSampleData(GetSampleData(GetDataFilePath(kSamplePath, argv)));

  instrument.SetNoteOnCallback([](float pitch) { ConsoleLog() << "NoteOn(" << pitch << ")"; });
  instrument.SetNoteOffCallback([](float pitch) { ConsoleLog() << "NoteOff(" << pitch << ") "; });

  // Audio process callback.
  audio_output.SetProcessCallback([&](std::span<float> output_samples) {
    instrument.Process(output_samples, /*timestamp=*/0.0);
  });

  // Key down callback.
  float intensity = 1.0f;
  int octave_shift = 0;
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
        --octave_shift;
      } else {
        ++octave_shift;
      }
      octave_shift = std::clamp(octave_shift, -kMaxOctaveShift, kMaxOctaveShift);
      ConsoleLog() << "Octave shift set to " << octave_shift;
      return;
    }
    if (upper_key == 'C' || upper_key == 'V') {
      // Change intensity.
      if (upper_key == 'C') {
        intensity -= 0.25f;
      } else {
        intensity += 0.25f;
      }
      intensity = std::clamp(intensity, 0.0f, 1.0f);
      ConsoleLog() << "Note intensity set to " << intensity;
      return;
    }

    // Play note.
    if (const auto pitch_or = KeyToPitch(octave_shift, key)) {
      instrument.SetNoteOn(*pitch_or, intensity);
    }
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Key up callback.
  const auto key_up_callback = [&](const InputManager::Key& key) {
    // Stop note.
    if (const auto pitch_or = KeyToPitch(octave_shift, key)) {
      instrument.SetNoteOff(*pitch_or);
    }
  };
  input_manager.SetKeyUpCallback(key_up_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start();

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
