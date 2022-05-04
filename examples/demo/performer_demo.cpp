#include <cassert>
#include <cctype>
#include <chrono>
#include <cmath>
#include <memory>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <utility>
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
constexpr int kNumFrames = 1024;

constexpr double kLookahead = 0.1;

// Instrument settings.
constexpr double kGain = 0.2;
constexpr OscillatorType kOscillatorType = OscillatorType::kSaw;
constexpr double kAttack = 0.0;
constexpr double kRelease = 0.1;

constexpr double kInitialTempo = 120.0;
constexpr double kTempoIncrement = 10.0;

constexpr double kIntensity = 0.25;

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

  Instrument performer =
      musician.CreateInstrument(SynthInstrument::GetDefinition(), kFrameRate);
  performer.SetParameter(SynthParameter::kOscillatorType, kOscillatorType);
  performer.SetParameter(SynthParameter::kAttack, kAttack);
  performer.SetParameter(SynthParameter::kRelease, kRelease);
  performer.SetNoteOnCallback(
      [](double pitch, double intensity, double /*timestamp*/) {
        ConsoleLog() << "Note{" << MidiKeyNumberFromPitch(pitch) << ", "
                     << intensity << "}";
      });

  Instrument metronome =
      musician.CreateInstrument(SynthInstrument::GetDefinition(), kFrameRate);
  metronome.SetParameter(SynthParameter::kOscillatorType,
                         OscillatorType::kSquare);
  metronome.SetParameter(SynthParameter::kAttack, kAttack);
  metronome.SetParameter(SynthParameter::kRelease, 0.025);

  std::vector<std::tuple<double, double, double>> score;
  score.emplace_back(0.0, 1.0, barely::kPitchC4);
  score.emplace_back(1.0, 1.0, barely::kPitchD4);
  score.emplace_back(2.0, 1.0, barely::kPitchE4);
  score.emplace_back(3.0, 1.0, barely::kPitchF4);
  score.emplace_back(4.0, 1.0, barely::kPitchG4);
  score.emplace_back(5.0, 1.0 / 3.0, barely::kPitchG4);
  score.emplace_back(5.0 + 1.0 / 3.0, 1.0 / 3.0, barely::kPitchA5);
  score.emplace_back(5.0 + 2.0 / 3.0, 1.0 / 3.0, barely::kPitchB5);
  score.emplace_back(6.0, 2.0, barely::kPitchC5);

  Sequence sequence = musician.CreateSequence();
  sequence.SetInstrument(&performer);
  sequence.SetBeginPosition(2.0);
  sequence.SetEndPosition(19.5);
  sequence.SetBeginOffset(-1.0);
  sequence.SetLooping(true);
  sequence.SetLoopBeginOffset(3.0);
  sequence.SetLoopLength(5.0);
  std::unordered_map<int, Note> notes;
  notes.reserve(score.size());
  int index = 0;
  for (const auto& [position, duration, pitch] : score) {
    notes.emplace(index++,
                  sequence.CreateNote(position, duration, pitch, kIntensity));
  }

  bool reset_position = false;
  const auto beat_callback = [&](double /*position*/, double timestamp) {
    metronome.StartNoteAt(timestamp, barely::kPitchC3, 1.0);
    metronome.StopNoteAt(timestamp, barely::kPitchC3);
    if (reset_position) {
      reset_position = false;
      musician.SetPosition(0.0);
    }
    ConsoleLog() << "Beat: " << musician.GetPosition();
  };
  musician.SetBeatCallback(beat_callback);

  // Audio process callback.
  std::vector<double> gains = {kGain, 0.5 * kGain};
  std::vector<double> temp_buffer(kNumChannels * kNumFrames);
  const auto process_callback = [&](double* output) {
    std::fill_n(output, kNumChannels * kNumFrames, 0.0);
    int i = 0;
    for (auto* instrument : {&performer, &metronome}) {
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
    if (const int index = static_cast<int>(key - '0');
        index > 0 && index < 10) {
      // Toggle score.
      if (notes.erase(index - 1) > 0) {
        ConsoleLog() << "Removed note " << index;
      } else {
        const auto& note = score[index - 1];
        notes.emplace(index - 1,
                      sequence.CreateNote(std::get<0>(note), std::get<1>(note),
                                          std::get<2>(note), kIntensity));
        ConsoleLog() << "Added note " << index;
      }
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
      case 'P':
        reset_position = true;
        return;
      case 'O':
        musician.SetPosition(0.0);
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
  musician.Start();

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
