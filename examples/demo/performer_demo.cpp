#include <cassert>
#include <cctype>
#include <chrono>
#include <cmath>
#include <memory>
#include <thread>
#include <utility>
#include <variant>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/random.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"
#include "examples/composition/note_pitch.h"

namespace {

using ::barely::Instrument;
using ::barely::InstrumentType;
using ::barely::Musician;
using ::barely::NoteDefinition;
using ::barely::NotePitchDefinition;
using ::barely::OscillatorType;
using ::barely::Sequence;
using ::barely::SynthParameter;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;
// TODO(#99): Move `Random` to public api.
using ::barelyapi::Random;

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

// Returns the MIDI key number for the given `pitch`.
int MidiKeyNumberFromPitch(double pitch) {
  return static_cast<int>(barelyapi::kNumSemitones * pitch) + 69;
}

}  // namespace

int main(int /*argc*/, char* /*argv*/[]) {
  AudioOutput audio_output;
  InputManager input_manager;

  AudioClock audio_clock(kFrameRate);

  Random random;

  Musician musician;
  musician.SetTempo(kInitialTempo);

  Instrument performer =
      musician.CreateInstrument(InstrumentType::kSynth, kFrameRate);
  performer.SetParameter(SynthParameter::kOscillatorType, kOscillatorType);
  performer.SetParameter(SynthParameter::kAttack, kAttack);
  performer.SetParameter(SynthParameter::kRelease, kRelease);
  performer.SetNoteOnCallback(
      [](double pitch, double intensity, double /*timestamp*/) {
        ConsoleLog() << "Note{" << MidiKeyNumberFromPitch(pitch) << ", "
                     << intensity << "}";
      });

  Instrument metronome =
      musician.CreateInstrument(InstrumentType::kSynth, kFrameRate);
  metronome.SetParameter(SynthParameter::kOscillatorType,
                         OscillatorType::kSquare);
  metronome.SetParameter(SynthParameter::kAttack, kAttack);
  metronome.SetParameter(SynthParameter::kRelease, 0.025);

  const auto build_note = [](double pitch, double duration,
                             double intensity = 0.25) {
    return NoteDefinition(duration, NotePitchDefinition::AbsolutePitch(pitch),
                          intensity);
  };
  std::vector<std::pair<double, NoteDefinition>> notes;
  notes.emplace_back(0.0, build_note(barelyapi::kPitchC4, 1.0));
  notes.emplace_back(1.0, build_note(barelyapi::kPitchD4, 1.0));
  notes.emplace_back(2.0, build_note(barelyapi::kPitchE4, 1.0));
  notes.emplace_back(3.0, build_note(barelyapi::kPitchF4, 1.0));
  notes.emplace_back(4.0, build_note(barelyapi::kPitchG4, 1.0));
  notes.emplace_back(5.0, build_note(barelyapi::kPitchG4, 1.0 / 3.0));
  notes.emplace_back(5.0 + 1.0 / 3.0,
                     build_note(barelyapi::kPitchA5, 1.0 / 3.0));
  notes.emplace_back(5.0 + 2.0 / 3.0,
                     build_note(barelyapi::kPitchB5, 1.0 / 3.0));
  notes.emplace_back(6.0, build_note(barelyapi::kPitchC5, 2.0));

  Sequence sequence = musician.CreateSequence();
  sequence.SetInstrument(&performer);
  sequence.SetBeginPosition(2.0);
  sequence.SetEndPosition(19.5);
  sequence.SetBeginOffset(-1.0);
  sequence.SetLooping(true);
  sequence.SetLoopBeginOffset(3.0);
  sequence.SetLoopLength(5.0);
  std::vector<BarelyId> note_references;
  note_references.reserve(notes.size());
  for (const auto& [position, note] : notes) {
    note_references.push_back(sequence.AddNote(note, position));
  }

  bool use_conductor = false;
  musician.SetAdjustNoteCallback([&](NoteDefinition* definition) {
    if (use_conductor) {
      definition->duration = 0.25 *
                             static_cast<double>(random.DrawUniform(1, 4)) *
                             definition->duration;
      definition->pitch.absolute_pitch +=
          static_cast<double>(random.DrawUniform(-1, 1));
      definition->intensity = 0.5 *
                              static_cast<double>(random.DrawUniform(1, 4)) *
                              definition->intensity;
    }
  });

  bool reset_position = false;
  const auto beat_callback = [&](double /*position*/, double /*timestamp*/) {
    metronome.StartNote(barelyapi::kPitchC3, 1.0);
    metronome.StopNote(barelyapi::kPitchC3);
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
      // Toggle notes.
      if (sequence.RemoveNote(note_references[index - 1]).IsOk()) {
        ConsoleLog() << "Removed note " << index;
      } else {
        note_references[index - 1] =
            sequence.AddNote(notes[index - 1].second, notes[index - 1].first);
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
      case 'C':
        use_conductor = !use_conductor;
        ConsoleLog() << "Conductor turned " << (use_conductor ? "on" : "off");
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
