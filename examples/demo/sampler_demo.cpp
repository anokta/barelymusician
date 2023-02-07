#include <algorithm>
#include <cctype>
#include <chrono>
#include <iterator>
#include <optional>
#include <thread>

#include "barelymusician/barelymusician.h"
#include "barelymusician/composition/note_pitch.h"
#include "barelymusician/instruments/sampler_instrument.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"
#include "examples/common/wav_file.h"
#include "tools/cpp/runfiles/runfiles.h"

namespace {

using ::barely::Instrument;
using ::barely::Musician;
using ::barely::SamplerControl;
using ::barely::SamplerInstrument;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;
using ::barely::examples::WavFile;
using ::bazel::tools::cpp::runfiles::Runfiles;

// System audio settings.
constexpr int kFrameRate = 48000;
constexpr int kChannelCount = 2;
constexpr int kFrameCount = 256;

// Instrument settings.
constexpr char kSamplePath[] = "barelymusician/examples/data/audio/sample.wav";
constexpr bool kLoop = true;
constexpr double kGain = 0.25;
constexpr double kAttack = 0.0125;
constexpr double kRelease = 0.125;
constexpr int kVoiceCount = 16;

// Note settings.
constexpr double kRootPitch = barely::kPitchC3;
constexpr char kOctaveKeys[] = {'A', 'W', 'S', 'E', 'D', 'F', 'T',
                                'G', 'Y', 'H', 'U', 'J', 'K'};
constexpr double kMaxOffsetOctaves = 3.0;

// Returns the pitch for the given `key`.
std::optional<double> PitchFromKey(const InputManager::Key& key) {
  const auto* it = std::find(std::cbegin(kOctaveKeys), std::cend(kOctaveKeys),
                             std::toupper(key));
  if (it == std::cend(kOctaveKeys)) {
    return std::nullopt;
  }
  const double distance =
      static_cast<double>(std::distance(std::cbegin(kOctaveKeys), it));
  return kRootPitch + distance / barely::kSemitoneCount;
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int /*argc*/, char* argv[]) {
  std::unique_ptr<Runfiles> runfiles(Runfiles::Create(argv[0]));
  assert(runfiles);
  WavFile sample_file;
  const bool success = sample_file.Load(runfiles->Rlocation(kSamplePath));
  assert(success);

  AudioOutput audio_output;
  InputManager input_manager;

  Musician musician;

  Instrument instrument =
      musician.CreateInstrument(SamplerInstrument::GetDefinition(), kFrameRate);
  instrument.SetControl(SamplerControl::kGain, kGain);
  instrument.SetControl(SamplerControl::kRootPitch, kRootPitch);
  instrument.SetControl(SamplerControl::kLoop, kLoop);
  instrument.SetControl(SamplerControl::kAttack, kAttack);
  instrument.SetControl(SamplerControl::kRelease, kRelease);
  instrument.SetControl(SamplerControl::kVoiceCount, kVoiceCount);
  instrument.SetData(
      sample_file.GetData().data(),
      static_cast<int>(sample_file.GetData().size() * sizeof(double)));

  instrument.SetNoteOnEventCallback([](double pitch, double intensity) {
    ConsoleLog() << "NoteOn(" << pitch << ", " << intensity << ")";
  });
  instrument.SetNoteOffEventCallback(
      [](double pitch) { ConsoleLog() << "NoteOff(" << pitch << ") "; });

  // Audio process callback.
  audio_output.SetProcessCallback([&](double* output) {
    instrument.Process(output, kChannelCount, kFrameCount, 0.0);
  });

  // Key down callback.
  double offset_octaves = 0.0;
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
      instrument.SetAllNotesOff();
      if (upper_key == 'Z') {
        --offset_octaves;
      } else {
        ++offset_octaves;
      }
      offset_octaves =
          std::clamp(offset_octaves, -kMaxOffsetOctaves, kMaxOffsetOctaves);
      ConsoleLog() << "Octave offset set to " << offset_octaves;
      return;
    }

    // Play note.
    if (const auto pitch = PitchFromKey(key)) {
      instrument.SetNoteOn(offset_octaves + *pitch);
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

  while (!quit) {
    input_manager.Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  ConsoleLog() << "Stopping audio stream";
  audio_output.Stop();

  return 0;
}
