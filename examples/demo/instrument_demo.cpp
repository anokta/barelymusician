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

namespace {

using ::barely::Engine;
using ::barely::EngineControlType;
using ::barely::InstrumentControlType;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;

// System audio settings.
constexpr int kSampleRate = 48000;
constexpr int kChannelCount = 2;
constexpr int kFrameCount = 256;

constexpr double kLookahead = 0.05;

// Engine settings.
constexpr float kDelayTime = 0.5f;
constexpr float kDelayFeedback = 0.2;
constexpr float kDelayLpfCutoff = 0.2f;
constexpr double kTempo = 128.0;

// Instrument settings.
constexpr float kGain = 1.0f;
constexpr float kOscShape = 0.75f;
constexpr float kAttack = 0.005f;
constexpr float kRelease = 0.2f;
constexpr int kVoiceCount = 16;
constexpr float kDelaySend = 0.2f;

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
  return kRootPitch + static_cast<float>(octave_shift + index) / 12.0f;
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int /*argc*/, char* /*argv*/[]) {
  InputManager input_manager;

  AudioClock audio_clock(kSampleRate);
  AudioOutput audio_output(kSampleRate, kChannelCount, kFrameCount);

  Engine engine(kSampleRate);
  engine.SetControl(EngineControlType::kDelayTime, kDelayTime);
  engine.SetControl(EngineControlType::kDelayFeedback, kDelayFeedback);
  engine.SetControl(EngineControlType::kDelayLpfCutoff, kDelayLpfCutoff);
  engine.SetTempo(kTempo);

  auto instrument = engine.CreateInstrument();
  instrument.SetControl(InstrumentControlType::kGain, kGain);
  instrument.SetControl(InstrumentControlType::kOscMix, 1.0f);
  instrument.SetControl(InstrumentControlType::kOscShape, kOscShape);
  instrument.SetControl(InstrumentControlType::kAttack, kAttack);
  instrument.SetControl(InstrumentControlType::kRelease, kRelease);
  instrument.SetControl(InstrumentControlType::kVoiceCount, kVoiceCount);
  instrument.SetControl(InstrumentControlType::kDelaySend, kDelaySend);

  // Audio process callback.
  audio_output.SetProcessCallback(
      [&](float* output_samples, int output_channel_count, int output_frame_count) {
        engine.Process(output_samples, output_channel_count, output_frame_count,
                       audio_clock.GetTimestamp());
        audio_clock.Update(output_frame_count);
      });

  // Key down callback.
  std::array<bool, kKeyCount> keys = {};
  float gain = 1.0f;
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
      for (int i = 0; i < kKeyCount; ++i) {
        if (keys[i]) {
          const float pitch = IndexToPitch(octave_shift, i);
          instrument.SetNoteOff(pitch);
          ConsoleLog() << "NoteOff(" << pitch << ")";
          keys[i] = false;
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
    if (upper_key == 'C' || upper_key == 'V') {
      // Change gain.
      if (upper_key == 'C') {
        gain -= 0.25f;
      } else {
        gain += 0.25f;
      }
      gain = std::clamp(gain, 0.0f, 1.0f);
      ConsoleLog() << "Note gain set to " << gain;
      return;
    }

    // Play note.
    if (const auto index_or = KeyToIndex(key)) {
      const float pitch = IndexToPitch(octave_shift, *index_or);
      instrument.SetNoteOn(pitch, gain);
      keys[*index_or] = true;
      ConsoleLog() << "NoteOn(" << pitch << ")";
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
      ConsoleLog() << "NoteOff(" << pitch << ")";
    }
  };
  input_manager.SetKeyUpCallback(key_up_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start();
  engine.Update(kLookahead);

  ConsoleLog() << "Play the instrument using the keyboard keys:";
  ConsoleLog() << "  * Use ASDFFGHJK keys to play the white notes in an octave";
  ConsoleLog() << "  * Use WETYU keys to play the black notes in an octave";
  ConsoleLog() << "  * Use ZX keys to set the octave up and down";
  ConsoleLog() << "  * Use CV keys to set the note gain up and down";

  while (!quit) {
    input_manager.Update();
    engine.Update(audio_clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }

  audio_output.Stop();

  return 0;
}
