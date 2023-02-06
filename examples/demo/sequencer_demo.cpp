#include <cassert>
#include <cctype>
#include <chrono>
#include <cmath>
#include <memory>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/composition/note_pitch.h"
#include "barelymusician/dsp/gain_processor.h"
#include "barelymusician/instruments/synth_instrument.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"

namespace {

using ::barely::GainProcessor;
using ::barely::Instrument;
using ::barely::Musician;
using ::barely::OscillatorType;
using ::barely::Performer;
using ::barely::SynthControl;
using ::barely::SynthInstrument;
using ::barely::TaskCallback;
using ::barely::TaskType;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;

// System audio settings.
constexpr int kFrameRate = 48000;
constexpr int kChannelCount = 2;
constexpr int kFrameCount = 1024;

constexpr int kLookahead = 100'000;

// Instrument settings.
constexpr double kGain = 0.1;
constexpr OscillatorType kOscillatorType = OscillatorType::kSaw;
constexpr double kAttack = 0.0;
constexpr double kRelease = 0.1;

constexpr double kInitialTempo = 120.0;
constexpr double kTempoIncrement = 10.0;

// Returns the MIDI key number for the given `pitch`.
int MidiKeyNumberFromPitch(double pitch) {
  return static_cast<int>(barely::kSemitoneCount * pitch) + 69;
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int /*argc*/, char* /*argv*/[]) {
  AudioOutput audio_output;
  InputManager input_manager;

  AudioClock audio_clock(kFrameRate);

  Musician musician;
  musician.SetTempo(kInitialTempo);

  Instrument instrument =
      musician.CreateInstrument(SynthInstrument::GetDefinition(), kFrameRate);
  instrument.SetControl(SynthControl::kOscillatorType, kOscillatorType);
  instrument.SetControl(SynthControl::kAttack, kAttack);
  instrument.SetControl(SynthControl::kRelease, kRelease);
  instrument.SetNoteOnEventCallback([](double pitch) {
    ConsoleLog() << "Note{" << MidiKeyNumberFromPitch(pitch) << "}";
  });

  GainProcessor gain(kFrameRate);
  gain.SetGain(kGain);

  Performer performer = musician.CreatePerformer();
  performer.SetLooping(true);
  performer.SetLoopBeginPosition(3.0);
  performer.SetLoopLength(5.0);

  const auto play_note_fn = [&](double duration, double pitch) -> TaskCallback {
    return [&instrument, &performer, pitch, duration]() {
      instrument.SetNoteOn(pitch);
      performer.CreateTask(
          [&instrument, pitch]() { instrument.SetNoteOff(pitch); },
          performer.GetPosition() + duration, TaskType::kOneOff);
    };
  };

  std::vector<std::pair<double, TaskCallback>> score;
  score.emplace_back(0.0, play_note_fn(1.0, barely::kPitchC4));
  score.emplace_back(1.0, play_note_fn(1.0, barely::kPitchD4));
  score.emplace_back(2.0, play_note_fn(1.0, barely::kPitchE4));
  score.emplace_back(3.0, play_note_fn(1.0, barely::kPitchF4));
  score.emplace_back(4.0, play_note_fn(1.0, barely::kPitchG4));
  score.emplace_back(5.0, play_note_fn(1.0 / 3.0, barely::kPitchG4));
  score.emplace_back(5 + 1.0 / 3.0, play_note_fn(1.0 / 3.0, barely::kPitchA5));
  score.emplace_back(5 + 2.0 / 3.0, play_note_fn(1.0 / 3.0, barely::kPitchB5));
  score.emplace_back(6.0, play_note_fn(2.0, barely::kPitchC5));

  std::unordered_map<int, Performer::TaskReference> tasks;
  int index = 0;
  for (const auto& [position, callback] : score) {
    tasks.emplace(index++, performer.CreateTask(callback, position,
                                                TaskType::kRecurring));
  }

  // Audio process callback.
  const auto process_callback = [&](double* output) {
    instrument.Process(output, kChannelCount, kFrameCount,
                       audio_clock.GetTimestamp());
    gain.Process(output, kChannelCount, kFrameCount);
    audio_clock.Update(kFrameCount);
  };
  audio_output.SetProcessCallback(process_callback);

  // Key down callback.
  bool quit = false;
  const auto key_down_callback = [&](const InputManager::Key& key) {
    if (static_cast<int>(key) == 27) {
      // ESC pressed, quit the app.
      quit = true;
      return;
    }
    if (const int index = static_cast<int>(key - '0');
        index > 0 && index < 10) {
      // Toggle score.
      if (const auto it = tasks.find(index - 1);
          it != tasks.end() && performer.DestroyTask(it->second).IsOk()) {
        tasks.erase(it);
        ConsoleLog() << "Removed note " << index;
      } else {
        const auto& [position, callback] = score[index - 1];
        tasks.emplace(index - 1, performer.CreateTask(callback, position,
                                                      TaskType::kRecurring));
        ConsoleLog() << "Added note " << index;
      }
      return;
    }
    // Adjust tempo.
    double tempo = musician.GetTempo();
    switch (std::toupper(key)) {
      case ' ':
        if (performer.IsPlaying()) {
          performer.Stop();
          instrument.SetAllNotesOff();
          ConsoleLog() << "Stopped playback";
        } else {
          performer.Start();
          ConsoleLog() << "Started playback";
        }
        return;
      case 'L':
        if (performer.IsLooping()) {
          performer.SetLooping(false);
          ConsoleLog() << "Loop turned off";
        } else {
          performer.SetLooping(true);
          ConsoleLog() << "Loop turned on";
        }
        return;
      case 'P':
        instrument.SetAllNotesOff();
        performer.SetPosition(0.0);
        return;
      case '-':
        tempo -= kTempoIncrement;
        break;
      case '+':
        tempo += kTempoIncrement;
        break;
      case 'R':
        tempo = kInitialTempo;
        break;
      default:
        return;
    }
    musician.SetTempo(tempo);
    ConsoleLog() << "Tempo set to " << musician.GetTempo() << " bpm";
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start(kFrameRate, kChannelCount, kFrameCount);
  performer.Start();

  while (!quit) {
    input_manager.Update();
    musician.Update(audio_clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  ConsoleLog() << "Stopping audio stream";
  performer.Stop();
  audio_output.Stop();

  return 0;
}
