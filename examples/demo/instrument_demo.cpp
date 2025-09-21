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

using ::barely::ArpMode;
using ::barely::ControlType;
using ::barely::Engine;
using ::barely::EngineControlType;
using ::barely::NoteEventType;
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
constexpr float kDelayLowPassFrequency = 1000.0f;
constexpr float kTempo = 128.0f;

// Instrument settings.
constexpr float kGain = 0.125f;
constexpr float kOscShape = 0.75f;
constexpr float kAttack = 0.005f;
constexpr float kRelease = 0.125f;
constexpr int kVoiceCount = 16;
constexpr float kDelaySend = 0.1f;
constexpr ArpMode kArpMode = ArpMode::kUp;
constexpr float kArpGateRatio = 0.5f;
constexpr float kArpRate = 2.0f;

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
  AudioOutput audio_output(kSampleRate, kChannelCount, kFrameCount);

  Engine engine(kSampleRate, kFrameCount);
  engine.SetControl(EngineControlType::kDelayTime, kDelayTime);
  engine.SetControl(EngineControlType::kDelayFeedback, kDelayFeedback);
  engine.SetControl(EngineControlType::kDelayLowPassFrequency, kDelayLowPassFrequency);
  engine.SetControl(barely::EngineControlType::kTempo, kTempo);

  auto instrument = engine.CreateInstrument({{
      {ControlType::kGain, kGain},
      {ControlType::kOscMix, 1.0f},
      {ControlType::kOscShape, kOscShape},
      {ControlType::kAttack, kAttack},
      {ControlType::kRelease, kRelease},
      {ControlType::kVoiceCount, kVoiceCount},
      {ControlType::kDelaySend, kDelaySend},
      {ControlType::kArpGateRatio, kArpGateRatio},
      {ControlType::kArpRate, kArpRate},
  }});
  instrument.SetNoteEventCallback([](NoteEventType type, float pitch) {
    ConsoleLog() << "Note" << (type == NoteEventType::kBegin ? "On" : "Off") << "(" << pitch << ")";
  });

  // Audio process callback.
  audio_output.SetProcessCallback(
      [&](float* output_samples, int output_channel_count, int output_frame_count) {
        engine.Process(output_samples, output_channel_count, output_frame_count,
                       audio_clock.GetTimestamp());
        audio_clock.Update(output_frame_count);
      });

  // Key down callback.
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
    if (upper_key == '0') {
      instrument.SetControl<ArpMode>(
          ControlType::kArpMode,
          (instrument.GetControl<ArpMode>(ControlType::kArpMode) == ArpMode::kNone)
              ? kArpMode
              : ArpMode::kNone);
    }

    // Play note.
    if (const auto pitch_or = KeyToPitch(octave_shift, key)) {
      instrument.SetNoteOn(*pitch_or, gain);
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
  engine.Update(kLookahead);

  ConsoleLog() << "Play the instrument using the keyboard keys:";
  ConsoleLog() << "  * Use ASDFFGHJK keys to play the white notes in an octave";
  ConsoleLog() << "  * Use WETYU keys to play the black notes in an octave";
  ConsoleLog() << "  * Use ZX keys to set the octave up and down";
  ConsoleLog() << "  * Use CV keys to set the note gain up and down";
  ConsoleLog() << "  * Use 0 key to toggle the arpeggiator on and off";

  while (!quit) {
    input_manager.Update();
    engine.Update(audio_clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }

  return 0;
}
