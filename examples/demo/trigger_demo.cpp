#include <cassert>
#include <cctype>
#include <chrono>
#include <cmath>
#include <memory>
#include <thread>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/random.h"
#include "barelymusician/instruments/synth_instrument.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"
#include "examples/composition/note_pitch.h"

namespace {

using ::barely::Instrument;
using ::barely::Musician;
using ::barely::Note;
using ::barely::OscillatorType;
using ::barely::Random;
using ::barely::Sequence;
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
constexpr double kGain = 0.2;
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

  Random random;

  Musician musician;
  musician.SetTempo(kInitialTempo);

  Instrument metronome =
      musician.CreateInstrument(SynthInstrument::GetDefinition(), kFrameRate);
  metronome.SetParameter(SynthParameter::kOscillatorType,
                         OscillatorType::kSquare);
  metronome.SetParameter(SynthParameter::kAttack, kAttack);
  metronome.SetParameter(SynthParameter::kRelease, 0.025);

  Instrument synth =
      musician.CreateInstrument(SynthInstrument::GetDefinition(), kFrameRate);
  synth.SetParameter(SynthParameter::kOscillatorType, kOscillatorType);
  synth.SetParameter(SynthParameter::kAttack, kAttack);
  synth.SetParameter(SynthParameter::kRelease, kRelease);
  synth.SetNoteOnCallback(
      [](double pitch, double intensity, double /*timestamp*/) {
        ConsoleLog() << "Note{" << MidiKeyNumberFromPitch(pitch) << ", "
                     << intensity << "}";
      });

  Sequence sequence = musician.CreateSequence();
  sequence.SetInstrument(&synth);

  std::vector<std::pair<double, double>> triggers;
  std::vector<Note> notes;
  const auto create_note_fn = [&](int scale_index, double position,
                                  double duration, double intensity = 1.0) {
    return sequence.CreateNote(
        position, duration,
        barely::kPitchD3 +
            barely::GetPitch(barely::kPitchMajorScale, scale_index),
        intensity);
  };

  // Trigger 1.
  triggers.emplace_back(0.0, 1.0);
  notes.push_back(create_note_fn(0, 0.0, 1.0));
  // Trigger 2.
  triggers.emplace_back(1.0, 1.0);
  notes.push_back(create_note_fn(1, 1.0, 1.0));
  // Trigger 3.
  triggers.emplace_back(2.0, 1.0);
  notes.push_back(create_note_fn(2, 2.0, 1.0));
  // Trigger 4.
  triggers.emplace_back(3.0, 1.0);
  notes.push_back(create_note_fn(3, 3.0, 2.0 / 3.0));
  notes.push_back(create_note_fn(4, 3.0 + 2.0 / 3.0, 1.0 / 3.0));
  // Trigger 5.
  triggers.emplace_back(4.0, 1.0);
  notes.push_back(create_note_fn(5, 4.0, 1.0 / 3.0));
  notes.push_back(create_note_fn(6, 4.0 + 1.0 / 3.0, 1.0 / 3.0));
  notes.push_back(create_note_fn(7, 4.0 + 2.0 / 3.0, 1.0 / 3.0));
  // Trigger 6.
  triggers.emplace_back(5.0, 2.0);
  notes.push_back(create_note_fn(8, 5.0, 2.0));

  bool enable_metronome = false;
  const auto beat_callback = [&](double position, double /*timestamp*/) {
    if (position >= sequence.GetEndPosition()) {
      musician.Stop();
    }
    if (enable_metronome) {
      metronome.StartNote(barely::kPitchC3, 1.0);
      metronome.StopNote(barely::kPitchC3);
      ConsoleLog() << "Beat: " << musician.GetPosition();
    }
  };
  musician.SetBeatCallback(beat_callback);

  // Audio process callback.
  std::vector<double> gains = {kGain, 0.5 * kGain};
  std::vector<double> temp_buffer(kNumChannels * kNumFrames);
  const auto process_callback = [&](double* output) {
    std::fill_n(output, kNumChannels * kNumFrames, 0.0);
    int i = 0;
    for (auto* instrument : {&synth, &metronome}) {
      const double gain = gains[i++];
      instrument->Process(temp_buffer.data(), kNumChannels, kNumFrames,
                          audio_clock.GetTimestamp());
      std::transform(temp_buffer.cbegin(), temp_buffer.cend(), output, output,
                     [&](double sample, double output_sample) {
                       return gain * sample + output_sample;
                     });
    }
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
      musician.Stop();
      musician.SetPosition(0.0);
      sequence.SetBeginOffset(triggers[index].first);
      sequence.SetEndPosition(triggers[index].second);
      musician.Start();
      return;
    }
    // Adjust tempo.
    double tempo = musician.GetTempo();
    switch (std::toupper(key)) {
      case ' ':
        if (musician.IsPlaying()) {
          musician.Stop();
          ConsoleLog() << "Stopped playback";
        } else {
          musician.Start();
          ConsoleLog() << "Started playback";
        }
        return;
      case 'L':
        if (sequence.IsLooping()) {
          sequence.SetLooping(false);
          ConsoleLog() << "Loop turned off";
        } else {
          sequence.SetLooping(true);
          ConsoleLog() << "Loop turned on";
        }
        return;
      case 'M':
        enable_metronome = !enable_metronome;
        ConsoleLog() << "Metronome is " << (enable_metronome ? "on" : "off");
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
  audio_output.Start(kFrameRate, kNumChannels, kNumFrames);

  while (!quit) {
    input_manager.Update();
    musician.Update(audio_clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  ConsoleLog() << "Stopping audio stream";
  musician.Stop();
  audio_output.Stop();

  return 0;
}
