#include <algorithm>
#include <cctype>
#include <chrono>
#include <iterator>
#include <optional>
#include <thread>

#include "barelymusician/barelymusician.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"
#include "examples/composition/note_pitch.h"

namespace {

using ::barely::Instrument;
using ::barely::InstrumentType;
using ::barely::Musician;
using ::barely::OscillatorType;
using ::barely::SynthParameter;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;

// System audio settings.
constexpr int kFrameRate = 48000;
constexpr int kNumChannels = 2;
constexpr int kNumFrames = 256;

// Instrument settings.
constexpr OscillatorType kOscillatorType = OscillatorType::kSaw;
constexpr double kGain = 0.125;
constexpr double kAttack = 0.05;
constexpr double kRelease = 0.125;
constexpr int kNumVoices = 16;

// Note settings.
constexpr double kRootPitch = barelyapi::kPitchC3;
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
  return kRootPitch + distance / barelyapi::kNumSemitones;
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int /*argc*/, char* /*argv*/[]) {
  AudioOutput audio_output;
  InputManager input_manager;

  Musician musician;

  Instrument instrument =
      musician.CreateInstrument(InstrumentType::kSynth, kFrameRate);
  instrument.SetParameter(SynthParameter::kOscillatorType, kOscillatorType);
  instrument.SetParameter(SynthParameter::kAttack, kAttack);
  instrument.SetParameter(SynthParameter::kRelease, kRelease);
  instrument.SetParameter(SynthParameter::kNumVoices, kNumVoices);

  instrument.SetNoteOnCallback(
      [](double pitch, double intensity, double /*timestamp*/) {
        ConsoleLog() << "NoteOn(" << pitch << ", " << intensity << ")";
      });
  instrument.SetNoteOffCallback([](double pitch, double /*timestamp*/) {
    ConsoleLog() << "NoteOff(" << pitch << ") ";
  });

  // Audio process callback.
  audio_output.SetProcessCallback([&](double* output) {
    instrument.Process(output, kNumChannels, kNumFrames, 0.0);
    for (int i = 0; i < kNumChannels * kNumFrames; ++i) {
      output[i] *= kGain;
    }
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
      instrument.StopAllNotes();
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
      instrument.StartNote(offset_octaves + *pitch);
    }
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Key up callback.
  const auto key_up_callback = [&](const InputManager::Key& key) {
    // Stop note.
    if (const auto pitch = PitchFromKey(key)) {
      instrument.StopNote(offset_octaves + *pitch);
    }
  };
  input_manager.SetKeyUpCallback(key_up_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start(kFrameRate, kNumChannels, kNumFrames);

  while (!quit) {
    input_manager.Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  ConsoleLog() << "Stopping audio stream";
  audio_output.Stop();

  return 0;
}
