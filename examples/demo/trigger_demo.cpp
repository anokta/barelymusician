#include <chrono>
#include <cstdint>
#include <thread>
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
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;

// System audio settings.
constexpr int kFrameRate = 48000;
constexpr int kChannelCount = 2;
constexpr int kFrameCount = 512;

constexpr std::int64_t kLookahead = kFrameRate / 20;

// Instrument settings.
constexpr Rational kGain = Rational(1, 10);
constexpr OscillatorType kOscillatorType = OscillatorType::kSaw;
constexpr Rational kAttack = 0;
constexpr Rational kRelease = Rational(1, 10);

constexpr int kInitialTempo = 120;

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

  std::vector<std::pair<Rational, Rational>> triggers;
  std::vector<Task> tasks;

  auto performer = musician.CreatePerformer();

  const auto play_note_fn = [&](int scale_index, Rational duration) {
    const Rational pitch =
        barely::kPitchD4 + barely::PitchFromScale(barely::kPitchMajorScale, scale_index);
    return [&instrument, &performer, duration, pitch]() {
      instrument.SetNoteOn(pitch);
      performer.ScheduleOneOffTask(
          [&instrument, &performer, pitch]() { instrument.SetNoteOff(pitch); },
          performer.GetPosition() + duration);
    };
  };

  // Trigger 1.
  triggers.emplace_back(0, 1);
  tasks.push_back(performer.CreateTask(play_note_fn(0, 1), 0));
  // Trigger 2.
  triggers.emplace_back(1, 1);
  tasks.push_back(performer.CreateTask(play_note_fn(1, 1), 1));
  // Trigger 3.
  triggers.emplace_back(2, 1);
  tasks.push_back(performer.CreateTask(play_note_fn(2, 1), 2));
  // Trigger 4.
  triggers.emplace_back(3, 1);
  tasks.push_back(performer.CreateTask(play_note_fn(3, Rational(2, 3)), 3));
  tasks.push_back(performer.CreateTask(play_note_fn(4, Rational(1, 3)), 3 + Rational(2, 3)));
  // Trigger 5.
  triggers.emplace_back(4, 1);
  tasks.push_back(performer.CreateTask(play_note_fn(5, Rational(1, 3)), 4));
  tasks.push_back(performer.CreateTask(play_note_fn(6, Rational(1, 3)), 4 + Rational(1, 3)));
  tasks.push_back(performer.CreateTask(play_note_fn(7, Rational(1, 3)), 4 + Rational(2, 3)));
  // Trigger 6.
  triggers.emplace_back(5, 2);
  tasks.push_back(performer.CreateTask(play_note_fn(8, 2), 5));

  // Stopper.
  auto stopper = performer.CreateTask([&performer]() { performer.Stop(); }, 0,
                                      /*process_order=*/-1);

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
    if (const int index = static_cast<int>(key - '1');
        index >= 0 && index < static_cast<int>(triggers.size())) {
      performer.Stop();
      instrument.SetAllNotesOff();
      performer.SetPosition(triggers[index].first);
      stopper.SetPosition(triggers[index].first + triggers[index].second);
      performer.Start();
      return;
    }
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start(kFrameRate, kChannelCount, kFrameCount);

  while (!quit) {
    input_manager.Update();
    musician.Update(audio_clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  ConsoleLog() << "Stopping audio stream";
  tasks.clear();
  performer.Stop();
  audio_output.Stop();

  return 0;
}
