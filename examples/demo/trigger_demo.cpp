#include <cassert>
#include <cctype>
#include <chrono>
#include <cmath>
#include <memory>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <variant>
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
using ::barely::Performer;
using ::barely::SynthControl;
using ::barely::SynthInstrument;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;

// System audio settings.
constexpr int kFrameRate = 48000;
constexpr int kChannelCount = 2;
constexpr int kFrameCount = 512;

constexpr double kLookahead = 0.05;

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
  instrument.SetControl(SynthControl::kOscillatorType, kOscillatorType);
  instrument.SetControl(SynthControl::kAttack, kAttack);
  instrument.SetControl(SynthControl::kRelease, kRelease);
  instrument.SetNoteOnCallback(
      [](double pitch, [[maybe_unused]] double intensity) {
        ConsoleLog() << "Note{" << MidiKeyNumberFromPitch(pitch) << "}";
      });

  std::vector<std::tuple<double, double, double>> notes;
  std::vector<std::pair<double, double>> triggers;

  Performer performer = engine.CreatePerformer();
  performer.SetLooping(true);

  const auto play_note_fn = [&](int scale_index,
                                double duration) -> Performer::TaskCallback {
    const double pitch =
        barely::kPitchD3 +
        barely::GetPitch(barely::kPitchMajorScale, scale_index);
    return [&instrument, &performer, duration, pitch]() {
      instrument.StartNote(pitch, kGain);
      performer.ScheduleOneOffTask(
          performer.GetPosition() + duration,
          [&instrument, pitch]() { instrument.StopNote(pitch); });
    };
  };

  // Trigger 1.
  triggers.emplace_back(0.0, 1.0);
  performer.AddTask(0.0, play_note_fn(0, 1.0));
  // Trigger 2.
  triggers.emplace_back(1.0, 1.0);
  performer.AddTask(1.0, play_note_fn(1, 1.0));
  // Trigger 3.
  triggers.emplace_back(2.0, 1.0);
  performer.AddTask(2.0, play_note_fn(2, 1.0));
  // Trigger 4.
  triggers.emplace_back(3.0, 1.0);
  performer.AddTask(3.0, play_note_fn(3, 2.0 / 3.0));
  performer.AddTask(3.0 + 2.0 / 3.0, play_note_fn(4, 1.0 / 3.0));
  // Trigger 5.
  triggers.emplace_back(4.0, 1.0);
  performer.AddTask(4.0, play_note_fn(5, 1.0 / 3.0));
  performer.AddTask(4.0 + 1.0 / 3.0, play_note_fn(6, 1.0 / 3.0));
  performer.AddTask(4.0 + 2.0 / 3.0, play_note_fn(7, 1.0 / 3.0));
  // Trigger 6.
  triggers.emplace_back(5.0, 2.0);
  performer.AddTask(5.0, play_note_fn(8, 2.0));

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
    if (const int index = static_cast<int>(key - '1');
        index >= 0 && index < static_cast<int>(triggers.size())) {
      performer.Stop();
      instrument.StopAllNotes();
      performer.SetLoopBeginPosition(triggers[index].first);
      performer.SetLoopLength(triggers[index].second);
      performer.SetPosition(triggers[index].first);
      performer.Start();
      return;
    }
    // Adjust tempo.
    double tempo = engine.GetTempo();
    switch (std::toupper(key)) {
      case ' ':
        if (performer.IsPlaying()) {
          instrument.StopAllNotes();
          performer.Stop();
          ConsoleLog() << "Stopped playback";
        } else {
          performer.Start();
          ConsoleLog() << "Started playback";
        }
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

  while (!quit) {
    input_manager.Update();
    engine.Update(audio_clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  ConsoleLog() << "Stopping audio stream";
  performer.Stop();
  audio_output.Stop();

  return 0;
}
