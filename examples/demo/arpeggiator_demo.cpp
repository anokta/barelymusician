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

using ::barely::Arpeggiator;
using ::barely::ArpeggiatorStyle;
using ::barely::ControlType;
using ::barely::Engine;
using ::barely::NoteEventType;
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
constexpr float kGain = 0.125f;
constexpr float kOscShape = 0.75f;
constexpr float kAttack = 0.0f;
constexpr float kRelease = 0.05f;
constexpr int kVoiceCount = 16;

constexpr float kInitialGateRatio = 0.5f;
constexpr double kInitialRate = 4.0;
constexpr double kInitialTempo = 100.0;
constexpr ArpeggiatorStyle kInitialStyle = ArpeggiatorStyle::kUp;

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

  AudioClock audio_clock(kFrameRate);
  AudioOutput audio_output(kFrameRate, kChannelCount, kFrameCount);

  Engine engine(kFrameRate);
  engine.SetTempo(kInitialTempo);

  auto instrument = engine.CreateInstrument({{
      {ControlType::kGain, kGain},
      {ControlType::kOscMix, 1.0f},
      {ControlType::kOscShape, kOscShape},
      {ControlType::kAttack, kAttack},
      {ControlType::kRelease, kRelease},
      {ControlType::kVoiceCount, kVoiceCount},
  }});
  instrument.SetNoteEventCallback([](NoteEventType type, float pitch) {
    if (type == NoteEventType::kOn) {
      ConsoleLog() << "Note(" << pitch << ")";
    }
  });

  Arpeggiator arpeggiator(engine);
  arpeggiator.SetInstrument(&instrument);
  arpeggiator.SetGateRatio(kInitialGateRatio);
  arpeggiator.SetRate(kInitialRate);
  arpeggiator.SetStyle(kInitialStyle);

  // Audio process callback.
  audio_output.SetProcessCallback([&](float* samples, int channel_count, int frame_count) {
    engine.Process(samples, channel_count, frame_count, audio_clock.GetTimestamp());
    audio_clock.Update(frame_count);
  });

  // Key down callback.
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
      arpeggiator.SetAllNotesOff();
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
      arpeggiator.SetNoteOn(*pitch_or);
    }
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Key up callback.
  const auto key_up_callback = [&](const InputManager::Key& key) {
    // Stop note.
    if (const auto pitch_or = KeyToPitch(octave_shift, key)) {
      arpeggiator.SetNoteOff(*pitch_or);
    }
  };
  input_manager.SetKeyUpCallback(key_up_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start();
  engine.Update(kLookahead);

  ConsoleLog() << "Play the arpeggiator using the keyboard keys:";
  ConsoleLog() << "  * Use ASDFFGHJK keys to play the white notes in an octave";
  ConsoleLog() << "  * Use WETYU keys to play the black notes in an octave";
  ConsoleLog() << "  * Use ZX keys to set the octave up and down";

  while (!quit) {
    input_manager.Update();
    engine.Update(audio_clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  return 0;
}
