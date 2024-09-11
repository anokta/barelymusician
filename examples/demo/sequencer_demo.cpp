#include <cctype>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/composition/scale.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/instruments/synth_instrument.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"

namespace {

using ::barely::Instrument;
using ::barely::Musician;
using ::barely::OscillatorType;
using ::barely::Performer;
using ::barely::PitchClass;
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

constexpr double kLookahead = 0.1;

// Instrument settings.
constexpr double kGain = 0.1;
constexpr OscillatorType kOscillatorType = OscillatorType::kSaw;
constexpr double kAttack = 0.0;
constexpr double kRelease = 0.1;

constexpr double kInitialTempo = 120.0;
constexpr double kTempoIncrement = 10.0;

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int /*argc*/, char* /*argv*/[]) {
  AudioOutput audio_output;
  InputManager input_manager;

  AudioClock audio_clock(kFrameRate);

  Musician musician(kFrameRate);
  musician.SetTempo(kInitialTempo);

  Instrument instrument(musician, SynthInstrument::GetDefinition());
  instrument.SetControl(SynthInstrument::Control::kGain, kGain);
  instrument.SetControl(SynthInstrument::Control::kOscillatorType, kOscillatorType);
  instrument.SetControl(SynthInstrument::Control::kAttack, kAttack);
  instrument.SetControl(SynthInstrument::Control::kRelease, kRelease);
  instrument.SetNoteOnEvent(
      [](double note, double /*intensity*/) { ConsoleLog() << "Note{" << note << "}"; });

  Performer performer(musician);
  performer.SetLooping(true);
  performer.SetLoopBeginPosition(3.0);
  performer.SetLoopLength(5.0);

  const auto play_note_fn = [&](double duration, double note) {
    return [&instrument, &performer, note, duration]() {
      instrument.SetNoteOn(note);
      performer.ScheduleOneOffTask([&instrument, note]() { instrument.SetNoteOff(note); },
                                   performer.GetPosition() + duration);
    };
  };

  std::vector<std::pair<double, TaskDefinition::Callback>> score;
  score.emplace_back(0.0, play_note_fn(1.0, barely::GetNote(PitchClass::kC)));
  score.emplace_back(1.0, play_note_fn(1.0, barely::GetNote(PitchClass::kD)));
  score.emplace_back(2.0, play_note_fn(1.0, barely::GetNote(PitchClass::kE)));
  score.emplace_back(3.0, play_note_fn(1.0, barely::GetNote(PitchClass::kF)));
  score.emplace_back(4.0, play_note_fn(1.0, barely::GetNote(PitchClass::kG)));
  score.emplace_back(5.0, play_note_fn(1.0 / 3.0, barely::GetNote(PitchClass::kG)));
  score.emplace_back(5 + 1.0 / 3.0, play_note_fn(1.0 / 3.0, barely::GetNote(PitchClass::kA)));
  score.emplace_back(5 + 2.0 / 3.0, play_note_fn(1.0 / 3.0, barely::GetNote(PitchClass::kB)));
  score.emplace_back(6.0, play_note_fn(2.0, barely::GetNote(PitchClass::kC, 5)));

  std::unordered_map<int, Task> tasks;
  int index = 0;
  for (const auto& [position, callback] : score) {
    tasks.emplace(index++, Task(performer, callback, position));
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
      if (tasks.erase(index - 1) > 0) {
        ConsoleLog() << "Removed note " << index;
      } else {
        const auto& [position, callback] = score[index - 1];
        tasks.emplace(index - 1, Task(performer, callback, position));
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
