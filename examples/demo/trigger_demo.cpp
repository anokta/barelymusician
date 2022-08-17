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
using ::barely::Sequencer;
using ::barely::SynthInstrument;
using ::barely::SynthParameter;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;

// System audio settings.
constexpr int kFrameRate = 48000;
constexpr int kNumChannels = 2;
constexpr int kNumFrames = 512;

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
  return static_cast<int>(barely::kNumSemitones * pitch) + 69;
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

  std::vector<std::tuple<double, double, double>> notes;
  std::vector<std::pair<double, double>> triggers;

  Sequencer sequencer = engine.CreateSequencer();
  sequencer.SetLooping(true);

  const auto play_note_fn = [&](int scale_index,
                                double duration) -> Sequencer::EventCallback {
    const double pitch =
        barely::kPitchD3 +
        barely::GetPitch(barely::kPitchMajorScale, scale_index);
    return [&instrument, &sequencer, duration, pitch]() {
      instrument.StartNote(pitch, kGain);
      sequencer.ScheduleOneOffEvent(
          sequencer.GetPosition() + duration,
          [&instrument, pitch]() { instrument.StopNote(pitch); });
    };
  };

  // Trigger 1.
  triggers.emplace_back(0.0, 1.0);
  sequencer.AddEvent(0.0, play_note_fn(0, 1.0));
  // Trigger 2.
  triggers.emplace_back(1.0, 1.0);
  sequencer.AddEvent(1.0, play_note_fn(1, 1.0));
  // Trigger 3.
  triggers.emplace_back(2.0, 1.0);
  sequencer.AddEvent(2.0, play_note_fn(2, 1.0));
  // Trigger 4.
  triggers.emplace_back(3.0, 1.0);
  sequencer.AddEvent(3.0, play_note_fn(3, 2.0 / 3.0));
  sequencer.AddEvent(3.0 + 2.0 / 3.0, play_note_fn(4, 1.0 / 3.0));
  // Trigger 5.
  triggers.emplace_back(4.0, 1.0);
  sequencer.AddEvent(4.0, play_note_fn(5, 1.0 / 3.0));
  sequencer.AddEvent(4.0 + 1.0 / 3.0, play_note_fn(6, 1.0 / 3.0));
  sequencer.AddEvent(4.0 + 2.0 / 3.0, play_note_fn(7, 1.0 / 3.0));
  // Trigger 6.
  triggers.emplace_back(5.0, 2.0);
  sequencer.AddEvent(5.0, play_note_fn(8, 2.0));

  // Audio process callback.
  const auto process_callback = [&](double* output) {
    instrument.Process(output, kNumChannels, kNumFrames,
                       audio_clock.GetTimestamp());
    audio_clock.Update(kNumFrames);
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
      sequencer.Stop();
      instrument.StopAllNotes();
      sequencer.SetLoopBeginPosition(triggers[index].first);
      sequencer.SetLoopLength(triggers[index].second);
      sequencer.SetPosition(triggers[index].first);
      sequencer.Start();
      return;
    }
    // Adjust tempo.
    double tempo = engine.GetTempo();
    switch (std::toupper(key)) {
      case ' ':
        if (sequencer.IsPlaying()) {
          instrument.StopAllNotes();
          sequencer.Stop();
          ConsoleLog() << "Stopped playback";
        } else {
          sequencer.Start();
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
  audio_output.Start(kFrameRate, kNumChannels, kNumFrames);

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
