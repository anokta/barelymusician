#include <cctype>
#include <chrono>
#include <cstdint>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/rational.h"
#include "barelymusician/composition/pitch.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/instruments/synth_instrument.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"

namespace {

using ::barely::Musician;
using ::barely::OscillatorType;
using ::barely::Rational;
using ::barely::SynthInstrument;
using ::barely::Task;
using ::barely::TaskDefinition;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;

// System audio settings.
constexpr int kFrameRate = 48000;
constexpr int kChannelCount = 2;
constexpr int kFrameCount = 1024;

constexpr std::int64_t kLookahead = kFrameRate / 10;

// Instrument settings.
constexpr Rational kGain = Rational(1, 10);
constexpr OscillatorType kOscillatorType = OscillatorType::kSaw;
constexpr Rational kAttack = 0;
constexpr Rational kRelease = Rational(1, 10);

constexpr int kInitialTempo = 120;
constexpr int kTempoIncrement = 10;

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int /*argc*/, char* /*argv*/[]) {
  AudioOutput audio_output;
  InputManager input_manager;

  AudioClock audio_clock(kFrameRate);

  Musician musician(kFrameRate);
  musician.SetTempo(kInitialTempo);

  auto instrument = musician.CreateInstrument<SynthInstrument>();
  instrument.SetControl(SynthInstrument::Control::kGain, kGain);
  instrument.SetControl(SynthInstrument::Control::kOscillatorType,
                        static_cast<int>(kOscillatorType));
  instrument.SetControl(SynthInstrument::Control::kAttack, kAttack);
  instrument.SetControl(SynthInstrument::Control::kRelease, kRelease);
  instrument.SetNoteOnEvent([](Rational pitch, Rational /*intensity*/) {
    ConsoleLog() << "Note{" << barely::MidiFromPitch(pitch) << "}";
  });

  auto performer = musician.CreatePerformer();
  performer.SetLooping(true);
  performer.SetLoopBeginPosition(3);
  performer.SetLoopLength(5);

  const auto play_note_fn = [&](Rational duration, Rational pitch) {
    return [&instrument, &performer, pitch, duration]() {
      instrument.SetNoteOn(pitch);
      performer.ScheduleOneOffTask([&instrument, pitch]() { instrument.SetNoteOff(pitch); },
                                   performer.GetPosition() + duration);
    };
  };

  std::vector<std::pair<Rational, TaskDefinition::Callback>> score;
  score.emplace_back(0, play_note_fn(1, barely::kPitchC4));
  score.emplace_back(1, play_note_fn(1, barely::kPitchD4));
  score.emplace_back(2, play_note_fn(1, barely::kPitchE4));
  score.emplace_back(3, play_note_fn(1, barely::kPitchF4));
  score.emplace_back(4, play_note_fn(1, barely::kPitchG4));
  score.emplace_back(5, play_note_fn(Rational(1, 3), barely::kPitchG4));
  score.emplace_back(5 + Rational(1, 3), play_note_fn(Rational(1, 3), barely::kPitchA4));
  score.emplace_back(5 + Rational(2, 3), play_note_fn(Rational(1, 3), barely::kPitchB4));
  score.emplace_back(6, play_note_fn(2, barely::kPitchC4 + 1));

  std::unordered_map<int, Task> tasks;
  int index = 0;
  for (const auto& [position, callback] : score) {
    tasks.emplace(index++, performer.CreateTask(callback, position));
  }

  // Audio process callback.
  const auto process_callback = [&](double* output) {
    instrument.Process(output, kChannelCount, kFrameCount, audio_clock.GetTimestamp());
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
    if (const int index = static_cast<int>(key - '0'); index > 0 && index < 10) {
      // Toggle score.
      if (const auto it = tasks.find(index - 1); it != tasks.end()) {
        tasks.erase(it);
        ConsoleLog() << "Removed note " << index;
      } else {
        const auto& [position, callback] = score[index - 1];
        tasks.emplace(index - 1, performer.CreateTask(callback, position));
        ConsoleLog() << "Added note " << index;
      }
      return;
    }
    // Adjust tempo.
    int tempo = musician.GetTempo();
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
        performer.SetPosition(0);
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
