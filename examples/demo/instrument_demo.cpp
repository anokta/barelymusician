#include <algorithm>
#include <array>
#include <cctype>
#include <chrono>
#include <cmath>
#include <iterator>
#include <optional>
#include <thread>

#include "barelymusician/barelymusician.h"
#include "barelymusician/composition/scale.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/instruments/synth_instrument.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"

namespace {

using ::barely::Instrument;
using ::barely::Musician;
using ::barely::OscillatorType;
using ::barely::PitchClass;
using ::barely::Scale;
using ::barely::ScaleType;
using ::barely::SynthInstrument;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;

// System audio settings.
constexpr int kFrameRate = 48000;
constexpr int kChannelCount = 2;
constexpr int kFrameCount = 256;

// Instrument settings.
constexpr double kGain = 0.125;
constexpr OscillatorType kOscillatorType = OscillatorType::kSaw;
constexpr double kAttack = 0.05;
constexpr double kRelease = 0.125;
constexpr int kVoiceCount = 16;

// Note settings.
constexpr double kRootNote = 440.0;
constexpr std::array<char, 13> kOctaveKeys = {'A', 'W', 'S', 'E', 'D', 'F', 'T',
                                              'G', 'Y', 'H', 'U', 'J', 'K'};
constexpr double kMaxOctave = 3.0;

// Returns the note for a given `key`.
std::optional<double> NoteFromKey(const Scale& scale, int octave, const InputManager::Key& key) {
  const auto it = std::find(kOctaveKeys.begin(), kOctaveKeys.end(), std::toupper(key));
  if (it == kOctaveKeys.end()) {
    return std::nullopt;
  }
  return scale.GetNote(octave * scale.GetNoteCount() +
                       static_cast<int>(std::distance(kOctaveKeys.begin(), it)));
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int /*argc*/, char* /*argv*/[]) {
  AudioOutput audio_output;
  InputManager input_manager;

  Musician musician(kFrameRate);

  Instrument instrument(musician, SynthInstrument::GetDefinition());
  instrument.SetControl(SynthInstrument::Control::kGain, kGain);
  instrument.SetControl(SynthInstrument::Control::kOscillatorType, kOscillatorType);
  instrument.SetControl(SynthInstrument::Control::kAttack, kAttack);
  instrument.SetControl(SynthInstrument::Control::kRelease, kRelease);
  instrument.SetControl(SynthInstrument::Control::kVoiceCount, kVoiceCount);

  instrument.SetNoteOnEvent([](double note, double intensity) {
    ConsoleLog() << "NoteOn(" << note << ", " << intensity << ")";
  });
  instrument.SetNoteOffEvent([](double note) { ConsoleLog() << "NoteOff(" << note << ") "; });

  const Scale scale = barely::CreateScale(ScaleType::kChromatic, kRootNote);

  // Audio process callback.
  audio_output.SetProcessCallback([&](double* output) {
    instrument.Process(output, kChannelCount, kFrameCount, /*timestamp=*/0.0);
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
      ConsoleLog() << "Octave offset set to " << octave;
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
    if (const auto note = NoteFromKey(scale, octave, key)) {
      instrument.SetNoteOn(*note, intensity);
    }
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Key up callback.
  const auto key_up_callback = [&](const InputManager::Key& key) {
    // Stop note.
    if (const auto note = NoteFromKey(scale, octave, key)) {
      instrument.SetNoteOff(*note);
    }
  };
  input_manager.SetKeyUpCallback(key_up_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start(kFrameRate, kChannelCount, kFrameCount);

  ConsoleLog() << "Play the instrument using the keyboard keys:";
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
