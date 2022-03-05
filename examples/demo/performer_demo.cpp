#include <cassert>
#include <cctype>
#include <chrono>
#include <cmath>
#include <memory>
#include <thread>
#include <utility>
#include <variant>
#include <vector>

#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"
#include "examples/common/random.h"
#include "examples/composition/note_pitch.h"
#include "examples/instruments/synth_instrument.h"
#include "platforms/api/barelymusician.h"

namespace {

using ::barely::Instrument;
using ::barely::IsOk;
using ::barely::Musician;
using ::barely::NoteReference;
using ::barely::Sequence;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;
using ::barely::examples::SynthInstrument;
using ::barely::examples::SynthInstrumentParameter;
using ::barelyapi::OscillatorType;
using ::barelyapi::Random;

// System audio settings.
constexpr int kSampleRate = 48000;
constexpr int kNumChannels = 2;
constexpr int kNumFrames = 1024;

constexpr double kLookahead = 0.1;

// Instrument settings.
constexpr double kGain = 0.2;
constexpr OscillatorType kOscillatorType = OscillatorType::kSaw;
constexpr double kAttack = 0.0f;
constexpr double kRelease = 0.1f;

constexpr double kInitialTempo = 120.0;
constexpr double kTempoIncrement = 10.0;

// Returns the MIDI key number for the given `pitch`.
int MidiKeyNumberFromPitch(float pitch) {
  return static_cast<int>(barelyapi::kNumSemitones * pitch) + 69;
}

}  // namespace

int main(int /*argc*/, char* /*argv*/[]) {
  AudioOutput audio_output;
  InputManager input_manager;

  AudioClock audio_clock(kSampleRate);

  Musician musician;
  musician.SetTempo(kInitialTempo);

  Instrument performer =
      musician.CreateInstrument(SynthInstrument::GetDefinition(), kSampleRate);
  performer.SetGain(kGain);
  performer.SetParameter(SynthInstrumentParameter::kEnvelopeAttack, kAttack);
  performer.SetParameter(SynthInstrumentParameter::kEnvelopeRelease, kRelease);
  performer.SetParameter(SynthInstrumentParameter::kOscillatorType,
                         static_cast<double>(kOscillatorType));
  performer.SetNoteOnCallback(
      [](float pitch, float intensity, double /*timestamp*/) {
        ConsoleLog() << "Note{" << MidiKeyNumberFromPitch(pitch) << ", "
                     << intensity << "}";
      });

  Instrument metronome =
      musician.CreateInstrument(SynthInstrument::GetDefinition(), kSampleRate);
  metronome.SetGain(0.5 * kGain);
  metronome.SetParameter(SynthInstrumentParameter::kEnvelopeAttack, kAttack);
  metronome.SetParameter(SynthInstrumentParameter::kEnvelopeRelease, 0.025);
  metronome.SetParameter(SynthInstrumentParameter::kOscillatorType,
                         static_cast<double>(OscillatorType::kSquare));

  const auto build_note = [](float pitch, double duration,
                             float intensity = 0.25f) {
    return BarelyNoteDefinition{
        .duration_definition = {.duration = duration},
        .intensity_definition = {.intensity = intensity},
        .pitch_definition = {.absolute_pitch = pitch}};
  };
  std::vector<std::pair<double, BarelyNoteDefinition>> notes;
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
  std::vector<NoteReference> note_references;
  for (const auto& [position, note] : notes) {
    note_references.push_back(sequence.AddNote(position, note));
  }

  bool use_conductor = false;

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
  std::vector<float> temp_buffer(kNumChannels * kNumFrames);
  const auto process_callback = [&](float* output) {
    std::fill_n(output, kNumChannels * kNumFrames, 0.0f);
    for (auto* instrument : {&metronome, &performer}) {
      instrument->Process(audio_clock.GetTimestamp(), temp_buffer.data(),
                          kNumChannels, kNumFrames);
      std::transform(temp_buffer.cbegin(), temp_buffer.cend(), output, output,
                     std::plus<>());
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
      if (IsOk(sequence.RemoveNote(note_references[index]))) {
        ConsoleLog() << "Removed note " << index;
      } else {
        note_references[index] =
            sequence.AddNote(notes[index].first, notes[index].second);
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
        // TODO: Add back conductor adjustment.
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
  audio_output.Start(kSampleRate, kNumChannels, kNumFrames);
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
