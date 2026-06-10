#include <barelymusician.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <chrono>
#include <iterator>
#include <optional>
#include <thread>

#include "common/audio_clock.h"
#include "common/audio_output.h"
#include "common/console_log.h"
#include "common/input_manager.h"
#include "composition/repeater.h"

namespace {

using ::barely::Engine;
using ::barely::InstrumentControlType;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;
using ::barely::examples::Repeater;

// System audio settings.
constexpr int kSampleRate = 48000;
constexpr int kChannelCount = 2;
constexpr int kFrameCount = 256;

constexpr double kLookahead = 0.1;

// Repeater settings.
constexpr float kGain = 0.9f;
constexpr float kOscShape = 0.75f;
constexpr float kAttack = 0.0f;
constexpr float kRelease = 0.1f;
constexpr int kVoiceCount = 16;

constexpr Repeater::Mode kInitialMode = Repeater::Mode::kForward;
constexpr double kInitialRate = 2.0;
constexpr double kInitialTempo = 135.0;

// Note settings.
constexpr int kKeyCount = 13;
constexpr std::array<char, kKeyCount> kOctaveKeys = {'A', 'W', 'S', 'E', 'D', 'F', 'T',
                                                     'G', 'Y', 'H', 'U', 'J', 'K'};
constexpr float kRootPitch = 0.0f;
constexpr int kMaxOctaveShift = 4;

std::optional<int> KeyToIndex(const InputManager::Key& key) {
  const auto it = std::find(kOctaveKeys.begin(), kOctaveKeys.end(), std::toupper(key));
  if (it == kOctaveKeys.end()) {
    return std::nullopt;
  }
  return static_cast<int>(std::distance(kOctaveKeys.begin(), it));
}

float IndexToPitch(int octave_shift, int index) {
  return kRootPitch + static_cast<float>(octave_shift) + static_cast<float>(index) / 12.0f;
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int /*argc*/, char* /*argv*/[]) {
  InputManager input_manager;

  AudioClock audio_clock(kSampleRate);
  AudioOutput audio_output(kSampleRate, kChannelCount, kFrameCount);

  Engine engine(kSampleRate);
  engine.SetTempo(kInitialTempo);

  auto instrument = engine.CreateInstrument();
  instrument.SetControl(InstrumentControlType::kGain, kGain);
  instrument.SetControl(InstrumentControlType::kOscMix, 1.0f);
  instrument.SetControl(InstrumentControlType::kOscShape, kOscShape);
  instrument.SetControl(InstrumentControlType::kAttack, kAttack);
  instrument.SetControl(InstrumentControlType::kRelease, kRelease);
  instrument.SetControl(InstrumentControlType::kVoiceCount, kVoiceCount);

  Repeater repeater(engine, instrument);
  repeater.SetMode(kInitialMode);
  repeater.SetRate(kInitialRate);
  repeater.SetNoteCallback([&repeater](float pitch) { ConsoleLog() << "Note(" << pitch << ")"; });

  // Audio process callback.
  audio_output.SetProcessCallback(
      [&](float* output_samples, int output_channel_count, int output_frame_count) {
        engine.Process(output_samples, output_channel_count, output_frame_count,
                       audio_clock.GetTimestamp());
        audio_clock.Update(output_frame_count);
      });

  // Key down callback.
  std::array<bool, kKeyCount> keys = {};
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
      if (repeater.IsPlaying()) {
        keys = {};
      } else {
        for (int i = 0; i < kKeyCount; ++i) {
          if (keys[i]) {
            instrument.SetNoteOff(IndexToPitch(octave_shift, i));
            keys[i] = false;
          }
        }
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
    if (const auto index_or = KeyToIndex(key)) {
      const float pitch = IndexToPitch(octave_shift, *index_or);
      if (repeater.IsPlaying()) {
        repeater.Stop();
        repeater.Start(pitch);
      } else {
        instrument.SetNoteOn(pitch);
        keys[*index_or] = true;
        repeater.Push(pitch, length);
        ConsoleLog() << "Note(" << pitch << ") added";
      }
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
          for (int i = 0; i < kKeyCount; ++i) {
            if (keys[i]) {
              instrument.SetNoteOff(IndexToPitch(octave_shift, i));
              keys[i] = false;
            }
          }
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
    if (const auto index_or = KeyToIndex(key)) {
      const float pitch = IndexToPitch(octave_shift, *index_or);
      instrument.SetNoteOff(pitch);
      keys[*index_or] = false;
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

  audio_output.Stop();

  return 0;
}
