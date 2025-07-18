#include <barelymusician.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <chrono>
#include <iterator>
#include <optional>
#include <span>
#include <thread>

#include "common/audio_clock.h"
#include "common/audio_output.h"
#include "common/console_log.h"
#include "common/input_manager.h"

namespace {

using ::barely::ControlType;
using ::barely::Engine;
using ::barely::Repeater;
using ::barely::RepeaterStyle;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;

// System audio settings.
constexpr int kSampleRate = 48000;
constexpr int kSampleCount = 256;

constexpr double kLookahead = 0.1;

// Arpeggiator settings.
constexpr float kGain = 0.125f;
constexpr float kOscShape = 0.75f;
constexpr float kAttack = 0.0f;
constexpr float kRelease = 0.05f;
constexpr int kVoiceCount = 16;

constexpr double kInitialRate = 2.0;
constexpr double kInitialTempo = 135.0;
constexpr RepeaterStyle kInitialStyle = RepeaterStyle::kForward;

// Note settings.
constexpr std::array<char, 13> kOctaveKeys = {'A', 'W', 'S', 'E', 'D', 'F', 'T',
                                              'G', 'Y', 'H', 'U', 'J', 'K'};
constexpr float kRootPitch = 0.0f;
constexpr int kMaxOctaveShift = 4;

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
int main(int /*argc*/, char* /*argv*/[]) {
  InputManager input_manager;

  AudioClock audio_clock(kSampleRate);
  AudioOutput audio_output(kSampleRate, kSampleCount);

  Engine engine(kSampleRate);
  engine.SetTempo(kInitialTempo);

  auto instrument = engine.CreateInstrument({{
      {ControlType::kGain, kGain},
      {ControlType::kOscMix, 1.0f},
      {ControlType::kOscShape, kOscShape},
      {ControlType::kAttack, kAttack},
      {ControlType::kRelease, kRelease},
      {ControlType::kVoiceCount, kVoiceCount},
  }});

  Repeater repeater(engine);
  repeater.SetInstrument(&instrument);
  repeater.SetRate(kInitialRate);
  repeater.SetStyle(kInitialStyle);

  instrument.SetNoteOnCallback([&repeater](float pitch) {
    if (repeater.IsPlaying()) {
      ConsoleLog() << "Note(" << pitch << ")";
    }
  });

  // Audio process callback.
  audio_output.SetProcessCallback([&](std::span<float> output_samples) {
    instrument.Process(output_samples, audio_clock.GetTimestamp());
    audio_clock.Update(static_cast<int>(output_samples.size()));
  });

  // Key down callback.
  int octave_shift = 0;
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
        --octave_shift;
      } else {
        ++octave_shift;
      }
      octave_shift = std::clamp(octave_shift, -kMaxOctaveShift, kMaxOctaveShift);
      ConsoleLog() << "Octave shift set to " << octave_shift;
      return;
    }

    // Play note.
    if (const auto pitch_or = KeyToPitch(octave_shift, key)) {
      const float pitch = *pitch_or;
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
    if (const auto pitch_or = KeyToPitch(octave_shift, key)) {
      if (!repeater.IsPlaying()) {
        instrument.SetNoteOff(*pitch_or);
      }
    }
  };
  input_manager.SetKeyUpCallback(key_up_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start();
  engine.Update(kLookahead);

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
    engine.Update(audio_clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  return 0;
}
