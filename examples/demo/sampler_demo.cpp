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
constexpr double kRootNote = 440.0;
constexpr std::array<char, 13> kOctaveKeys = {'A', 'W', 'S', 'E', 'D', 'F', 'T',
                                              'G', 'Y', 'H', 'U', 'J', 'K'};
constexpr double kMaxOctave = 3.0;

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

// Returns the note for a given `key`.
std::optional<double> NoteFromKey(int octave, const InputManager::Key& key) {
  const auto it = std::find(kOctaveKeys.begin(), kOctaveKeys.end(), std::toupper(key));
  if (it == kOctaveKeys.end()) {
    return std::nullopt;
  }
  int distance = static_cast<int>(std::distance(kOctaveKeys.begin(), it));
  if (distance == barely::kSemitoneCount) {
    ++octave;
    distance = 0;
  }
  return std::pow(2.0, octave) * kRootNote * barely::kSemitoneRatios[distance];
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int /*argc*/, char* argv[]) {
  AudioOutput audio_output;
  InputManager input_manager;

  Musician musician(kFrameRate);

  Instrument instrument(musician, SamplerInstrument::GetDefinition());
  instrument.SetControl(SamplerInstrument::Control::kGain, kGain);
  instrument.SetControl(SamplerInstrument::Control::kRootNote, kRootNote);
  instrument.SetControl(SamplerInstrument::Control::kLoop, kLoop);
  instrument.SetControl(SamplerInstrument::Control::kAttack, kAttack);
  instrument.SetControl(SamplerInstrument::Control::kRelease, kRelease);
  instrument.SetControl(SamplerInstrument::Control::kVoiceCount, kVoiceCount);

  Effect effect(musician, LowPassEffect::GetDefinition());
  effect.SetControl(LowPassEffect::Control::kCutoffFrequency, kLowPassCutoffFrequency);

  instrument.SetData(GetSampleData(GetDataFilePath(kSamplePath, argv)));

  instrument.SetNoteOnEvent([](double note, double intensity) {
    ConsoleLog() << "NoteOn(" << note << ", " << intensity << ")";
  });
  instrument.SetNoteOffEvent([](double note) { ConsoleLog() << "NoteOff(" << note << ") "; });

  // Audio process callback.
  audio_output.SetProcessCallback([&](double* output) {
    instrument.Process(output, kChannelCount, kFrameCount, /*timestamp=*/0.0);
    effect.Process(output, kChannelCount, kFrameCount, /*timestamp=*/0.0);
  });

  // Key down callback.
  double intensity = 1.0;
  double octave = 0.0;
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
        --octave;
      } else {
        ++octave;
      }
      octave = std::clamp(octave, -kMaxOctave, kMaxOctave);
      ConsoleLog() << "Octave set to " << octave;
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
    if (const auto note = NoteFromKey(octave, key)) {
      instrument.SetNoteOn(*note, intensity);
    }
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Key up callback.
  const auto key_up_callback = [&](const InputManager::Key& key) {
    // Stop note.
    if (const auto note = NoteFromKey(octave, key)) {
      instrument.SetNoteOff(*note);
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
