#include <algorithm>
#include <array>
#include <cassert>
#include <cctype>
#include <chrono>
#include <cmath>
#include <iterator>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/instruments/sampler_instrument.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"
#include "examples/common/wav_file.h"
#include "examples/data/data.h"

namespace {

using ::barely::Instrument;
using ::barely::Musician;
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
constexpr std::array<char, 13> kOctaveKeys = {'A', 'W', 'S', 'E', 'D', 'F', 'T',
                                              'G', 'Y', 'H', 'U', 'J', 'K'};
constexpr int kRootPitch = 60;
constexpr int kOctavePitchCount = 12;
constexpr int kMaxOctaveShift = 4;

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
std::optional<int> PitchFromKey(int octave_shift, const InputManager::Key& key) {
  const auto it = std::find(kOctaveKeys.begin(), kOctaveKeys.end(), std::toupper(key));
  if (it == kOctaveKeys.end()) {
    return std::nullopt;
  }
  return kRootPitch + octave_shift * kOctavePitchCount +
         static_cast<int>(std::distance(kOctaveKeys.begin(), it));
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int /*argc*/, char* argv[]) {
  InputManager input_manager;

  AudioOutput audio_output(kFrameRate, kChannelCount, kFrameCount);

  Musician musician(kFrameRate);

  Instrument instrument(musician, SamplerInstrument::GetDefinition());
  instrument.SetControl(SamplerInstrument::Control::kGain, kGain);
  instrument.SetControl(SamplerInstrument::Control::kRootPitch, kRootPitch);
  instrument.SetControl(SamplerInstrument::Control::kLoop, kLoop);
  instrument.SetControl(SamplerInstrument::Control::kAttack, kAttack);
  instrument.SetControl(SamplerInstrument::Control::kRelease, kRelease);
  instrument.SetControl(SamplerInstrument::Control::kVoiceCount, kVoiceCount);

  instrument.SetData(GetSampleData(GetDataFilePath(kSamplePath, argv)));

  instrument.SetNoteOnEvent([](int pitch, double intensity) {
    ConsoleLog() << "NoteOn(" << pitch << ", " << intensity << ")";
  });
  instrument.SetNoteOffEvent([](int pitch) { ConsoleLog() << "NoteOff(" << pitch << ") "; });

  // Audio process callback.
  audio_output.SetProcessCallback([&](double* output) {
    instrument.Process(output, kChannelCount, kFrameCount, /*timestamp=*/0.0);
  });

  // Key down callback.
  double intensity = 1.0;
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
        intensity -= 0.25;
      } else {
        intensity += 0.25;
      }
      intensity = std::clamp(intensity, 0.0, 1.0);
      ConsoleLog() << "Note intensity set to " << intensity;
      return;
    }

    // Play note.
    if (const auto pitch_or = PitchFromKey(octave_shift, key)) {
      instrument.SetNoteOn(*pitch_or, intensity);
    }
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Key up callback.
  const auto key_up_callback = [&](const InputManager::Key& key) {
    // Stop note.
    if (const auto pitch_or = PitchFromKey(octave_shift, key)) {
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
