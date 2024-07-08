#include <algorithm>
#include <array>
#include <cctype>
#include <chrono>
#include <iomanip>
#include <iterator>
#include <optional>
#include <thread>

#include "barelymusician/barelymusician.h"
#include "barelymusician/components/repeater.h"
#include "barelymusician/composition/pitch.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/instruments/synth_instrument.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"

namespace {

using ::barely::OscillatorType;
using ::barely::Repeater;
using ::barely::RepeaterStyle;
using ::barely::ScopedMusician;
using ::barely::SynthInstrument;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;

// System audio settings.
constexpr int kFrameRate = 48000;
constexpr int kChannelCount = 2;
constexpr int kFrameCount = 256;

constexpr double kLookahead = 0.1;

// Arpeggiator settings.
constexpr double kGain = 0.125;
constexpr OscillatorType kOscillatorType = OscillatorType::kSquare;
constexpr double kAttack = 0.0;
constexpr double kRelease = 0.05;
constexpr int kVoiceCount = 16;

constexpr double kInitialRate = 2.0;
constexpr double kInitialTempo = 135.0;
constexpr RepeaterStyle kInitialStyle = RepeaterStyle::kForward;

// Note settings.
constexpr double kRootPitch = barely::kPitchC4;
constexpr std::array<char, 13> kOctaveKeys = {'A', 'W', 'S', 'E', 'D', 'F', 'T',
                                              'G', 'Y', 'H', 'U', 'J', 'K'};
constexpr double kMaxOffsetOctaves = 3.0;

// Returns the pitch for a given `key`.
std::optional<double> PitchFromKey(const InputManager::Key& key) {
  const auto it = std::find(kOctaveKeys.begin(), kOctaveKeys.end(), std::toupper(key));
  if (it == kOctaveKeys.end()) {
    return std::nullopt;
  }
  const double distance = static_cast<double>(std::distance(kOctaveKeys.begin(), it));
  return kRootPitch + distance / barely::kSemitoneCount;
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int /*argc*/, char* /*argv*/[]) {
  AudioOutput audio_output;
  InputManager input_manager;

  AudioClock audio_clock(kFrameRate);

  ScopedMusician musician;
  musician.SetTempo(kInitialTempo);

  auto instrument = musician.CreateInstrument<SynthInstrument>(kFrameRate);
  instrument.GetControl(SynthInstrument::Control::kGain).SetValue(kGain);
  instrument.GetControl(SynthInstrument::Control::kOscillatorType).SetValue(kOscillatorType);
  instrument.GetControl(SynthInstrument::Control::kAttack).SetValue(kAttack);
  instrument.GetControl(SynthInstrument::Control::kRelease).SetValue(kRelease);
  instrument.GetControl(SynthInstrument::Control::kVoiceCount).SetValue(kVoiceCount);

  auto repeater = musician.CreateComponent<Repeater>();
  repeater.SetInstrument(&instrument);
  repeater.SetRate(kInitialRate);
  repeater.SetStyle(kInitialStyle);

  instrument.SetNoteOnEvent([&repeater](double pitch, double /*intensity*/) {
    if (repeater.IsPlaying()) {
      ConsoleLog() << std::setprecision(2) << "Note(" << pitch << ")";
    }
  });

  // Audio process callback.
  audio_output.SetProcessCallback([&](double* output) {
    instrument.Process(output, kChannelCount, kFrameCount, audio_clock.GetTimestamp());
    audio_clock.Update(kFrameCount);
  });

  // Key down callback.
  double offset_octaves = 0.0;
  int length = 1;
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
      if (!repeater.IsPlaying()) {
        instrument.SetAllNotesOff();
      }
      if (upper_key == 'Z') {
        --offset_octaves;
      } else {
        ++offset_octaves;
      }
      offset_octaves = std::clamp(offset_octaves, -kMaxOffsetOctaves, kMaxOffsetOctaves);
      ConsoleLog() << "Octave offset set to " << offset_octaves;
      return;
    }

    // Play note.
    if (const auto pitch_or = PitchFromKey(key)) {
      const double pitch = offset_octaves + *pitch_or;
      if (!repeater.IsPlaying()) {
        instrument.SetNoteOn(pitch);
      }
      repeater.Push(pitch, length);
      ConsoleLog() << "Note(" << pitch << ") added";
      return;
    }

    switch (upper_key) {
      case '0':
        repeater.Push(/*pitch_or=*/std::nullopt, length);
        ConsoleLog() << "Silence added";
        break;
      case '1':
        length = 1;
        ConsoleLog() << "Set note length to 1";
        break;
      case '2':
        length = 2;
        ConsoleLog() << "Set note length to 2";
        break;
      case '\b':
        repeater.Pop();
        ConsoleLog() << "Last note removed";
        break;
      case 'R':
        repeater.Clear();
        ConsoleLog() << "Repeater cleared";
        break;
      case ' ':
        if (repeater.IsPlaying()) {
          repeater.Stop();
          ConsoleLog() << "Repeater stopped";
        } else {
          instrument.SetAllNotesOff();
          repeater.Start();
          ConsoleLog() << "Repeater started";
        }
        break;
      default:
        break;
    }
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Key up callback.
  const auto key_up_callback = [&](const InputManager::Key& key) {
    // Stop note.
    if (const auto pitch = PitchFromKey(key)) {
      if (!repeater.IsPlaying()) {
        instrument.SetNoteOff(offset_octaves + *pitch);
      }
    }
  };
  input_manager.SetKeyUpCallback(key_up_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start(kFrameRate, kChannelCount, kFrameCount);

  ConsoleLog() << "Play the repeater using the keyboard keys:";
  ConsoleLog() << "  * Use space key to start or stop the repeater";
  ConsoleLog() << "  * Use ASDFFGHJK keys to push the white notes in an octave";
  ConsoleLog() << "  * Use WETYU keys to push the black notes in an octave";
  ConsoleLog() << "  * Use ZX keys to shift the octave up and down";
  ConsoleLog() << "  * Use 0 key to push silence";
  ConsoleLog() << "  * Use 12 keys to set note length";
  ConsoleLog() << "  * Use backspace key to pop last note";

  while (!quit) {
    input_manager.Update();
    musician.Update(audio_clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  ConsoleLog() << "Stopping audio stream";
  audio_output.Stop();

  return 0;
}
