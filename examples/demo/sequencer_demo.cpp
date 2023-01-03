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
#include "barelymusician/instruments/synth_instrument.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"

namespace {

using ::barely::Engine;
using ::barely::Instrument;
using ::barely::OscillatorType;
using ::barely::Sequencer;
using ::barely::SynthInstrument;
using ::barely::SynthParameter;
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

  Engine engine;
  engine.SetTempo(kInitialTempo);

  Instrument instrument =
      engine.CreateInstrument(SynthInstrument::GetDefinition(), kFrameRate);
  instrument.SetParameter(SynthParameter::kOscillatorType, kOscillatorType);
  instrument.SetParameter(SynthParameter::kAttack, kAttack);
  instrument.SetParameter(SynthParameter::kRelease, kRelease);
  instrument.SetNoteOnCallback(
      [](double pitch, [[maybe_unused]] double intensity) {
        ConsoleLog() << "Note{" << MidiKeyNumberFromPitch(pitch) << "}";
      });

  Sequencer sequencer = engine.CreateSequencer();
  sequencer.SetLooping(true);
  sequencer.SetLoopBeginPosition(3.0);
  sequencer.SetLoopLength(5.0);

  const auto play_note_fn = [&](double duration,
                                double pitch) -> Sequencer::EventCallback {
    return [&instrument, &sequencer, pitch, duration]() {
      instrument.StartNote(pitch, kGain);
      sequencer.ScheduleOneOffEvent(
          sequencer.GetPosition() + duration,
          [&instrument, pitch]() { instrument.StopNote(pitch); });
    };
  };

  std::vector<std::pair<double, Sequencer::EventCallback>> score;
  score.emplace_back(0.0, play_note_fn(1.0, barely::kPitchC4));
  score.emplace_back(1.0, play_note_fn(1.0, barely::kPitchD4));
  score.emplace_back(2.0, play_note_fn(1.0, barely::kPitchE4));
  score.emplace_back(3.0, play_note_fn(1.0, barely::kPitchF4));
  score.emplace_back(4.0, play_note_fn(1.0, barely::kPitchG4));
  score.emplace_back(5.0, play_note_fn(1.0 / 3.0, barely::kPitchG4));
  score.emplace_back(5.0 + 1.0 / 3.0,
                     play_note_fn(1.0 / 3.0, barely::kPitchA5));
  score.emplace_back(5.0 + 2.0 / 3.0,
                     play_note_fn(1.0 / 3.0, barely::kPitchB5));
  score.emplace_back(6.0, play_note_fn(2.0, barely::kPitchC5));

  std::unordered_map<int, Sequencer::EventReference> events;
  int index = 0;
  for (const auto& [position, callback] : score) {
    events.emplace(index++, sequencer.AddEvent(position, callback));
  }

  // Audio process callback.
  const auto process_callback = [&](double* output) {
    instrument.Process(output, kChannelCount, kFrameCount,
                       audio_clock.GetTimestamp());
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
      if (const auto it = events.find(index - 1);
          it != events.end() && sequencer.RemoveEvent(it->second).IsOk()) {
        events.erase(it);
        ConsoleLog() << "Removed note " << index;
      } else {
        const auto& [position, callback] = score[index - 1];
        events.emplace(index - 1, sequencer.AddEvent(position, callback));
        ConsoleLog() << "Added note " << index;
      }
      return;
    }
    // Adjust tempo.
    double tempo = engine.GetTempo();
    switch (std::toupper(key)) {
      case ' ':
        if (sequencer.IsPlaying()) {
          sequencer.Stop();
          instrument.StopAllNotes();
          ConsoleLog() << "Stopped playback";
        } else {
          sequencer.Start();
          ConsoleLog() << "Started playback";
        }
        return;
      case 'L':
        if (sequencer.IsLooping()) {
          sequencer.SetLooping(false);
          ConsoleLog() << "Loop turned off";
        } else {
          sequencer.SetLooping(true);
          ConsoleLog() << "Loop turned on";
        }
        return;
      case 'P':
        instrument.StopAllNotes();
        sequencer.SetPosition(0.0);
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
    engine.SetTempo(tempo);
    ConsoleLog() << "Tempo set to " << engine.GetTempo() << " bpm";
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start(kFrameRate, kChannelCount, kFrameCount);
  sequencer.Start();

  while (!quit) {
    input_manager.Update();
    engine.Update(audio_clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  ConsoleLog() << "Stopping audio stream";
  sequencer.Stop();
  audio_output.Stop();

  return 0;
}
