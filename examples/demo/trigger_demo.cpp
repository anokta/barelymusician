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
#include "barelymusician/composition/pitch.h"
#include "barelymusician/instruments/synth_instrument.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"

namespace {

using ::barely::Musician;
using ::barely::OscillatorType;
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

  auto instrument =
      musician.CreateInstrument(SynthInstrument::GetDefinition(), kFrameRate);
  instrument.SetControl(SynthInstrument::Control::kGain, kGain);
  instrument.SetControl(SynthInstrument::Control::kOscillatorType,
                        kOscillatorType);
  instrument.SetControl(SynthInstrument::Control::kAttack, kAttack);
  instrument.SetControl(SynthInstrument::Control::kRelease, kRelease);
  instrument.SetNoteOnEvent([](double pitch, double /*intensity*/) {
    ConsoleLog() << "Note{" << MidiKeyNumberFromPitch(pitch) << "}";
  });

  std::vector<std::tuple<double, double, double>> notes;
  std::vector<std::pair<double, double>> triggers;

  auto performer = musician.CreatePerformer();

  const auto play_note_fn = [&](int scale_index, double duration) {
    const double pitch =
        barely::kPitchD3 +
        barely::PitchFromScale(barely::kPitchMajorScale, scale_index);
    return [&instrument, &performer, duration, pitch]() {
      instrument.SetNoteOn(pitch);
      performer
          .CreateTask([&instrument, &performer,
                       pitch]() { instrument.SetNoteOff(pitch); },
                      /*is_one_off=*/true, performer.GetPosition() + duration)
          .Release();
    };
  };

  // Trigger 1.
  triggers.emplace_back(0.0, 1.0);
  performer.CreateTask(play_note_fn(0, 1.0), /*is_one_off=*/false, 0.0)
      .Release();
  // Trigger 2.
  triggers.emplace_back(1.0, 1.0);
  performer.CreateTask(play_note_fn(1, 1.0), /*is_one_off=*/false, 1.0)
      .Release();
  // Trigger 3.
  triggers.emplace_back(2.0, 1.0);
  performer.CreateTask(play_note_fn(2, 1.0), /*is_one_off=*/false, 2.0)
      .Release();
  // Trigger 4.
  triggers.emplace_back(3.0, 1.0);
  performer.CreateTask(play_note_fn(3, 0.66), /*is_one_off=*/false, 3.0)
      .Release();
  performer.CreateTask(play_note_fn(4, 0.34), /*is_one_off=*/false, 3.66)
      .Release();
  // Trigger 5.
  triggers.emplace_back(4.0, 1.0);
  performer.CreateTask(play_note_fn(5, 0.33), /*is_one_off=*/false, 4.0)
      .Release();
  performer.CreateTask(play_note_fn(6, 0.33), /*is_one_off=*/false, 4.33)
      .Release();
  performer.CreateTask(play_note_fn(7, 0.34), /*is_one_off=*/false, 4.66)
      .Release();
  // Trigger 6.
  triggers.emplace_back(5.0, 2.0);
  performer.CreateTask(play_note_fn(8, 2.0), /*is_one_off=*/false, 5.0)
      .Release();

  // Stopper.
  auto stopper = performer.CreateTask([&performer]() { performer.Stop(); },
                                      /*is_one_off=*/false, 0.0,
                                      /*process_order=*/-1);

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
  performer.Stop();
  audio_output.Stop();

  return 0;
}
