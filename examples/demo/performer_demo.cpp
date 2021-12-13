#include <cctype>
#include <chrono>
#include <cmath>
#include <memory>
#include <thread>
#include <utility>
#include <variant>
#include <vector>

#include "barelymusician/common/id.h"
#include "barelymusician/common/random.h"
#include "barelymusician/common/status.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/composition/note_duration.h"
#include "barelymusician/composition/note_intensity.h"
#include "barelymusician/composition/note_pitch.h"
#include "barelymusician/engine/conductor_definition.h"
#include "barelymusician/engine/musician.h"
#include "barelymusician/engine/param_definition.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"
#include "examples/instruments/synth_instrument.h"

namespace {

using ::barely::ConductorDefinition;
using ::barely::ConductorState;
using ::barely::Id;
using ::barely::IsOk;
using ::barely::Musician;
using ::barely::Note;
using ::barely::NoteDuration;
using ::barely::NoteIntensity;
using ::barely::NotePitch;
using ::barely::OscillatorType;
using ::barely::ParamDefinition;
using ::barely::ParamDefinitionMap;
using ::barely::Random;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;
using ::barely::examples::SynthInstrument;
using ::barely::examples::SynthInstrumentParam;

// System audio settings.
constexpr int kSampleRate = 48000;
constexpr int kNumChannels = 2;
constexpr int kNumFrames = 1024;

constexpr double kLookahead = 0.1;

// Instrument settings.
constexpr int kNumVoices = 4;
constexpr float kGain = 0.2f;
constexpr OscillatorType kOscillatorType = OscillatorType::kSaw;
constexpr float kAttack = 0.0f;
constexpr float kRelease = 0.1f;

constexpr double kInitialTempo = 120.0;
constexpr double kTempoIncrement = 10.0;

// Returns the MIDI key number for the given |pitch|.
int MidiKeyNumberFromPitch(float pitch) {
  return static_cast<int>(barely::kNumSemitones * pitch) + 69;
}

}  // namespace

int main(int /*argc*/, char* /*argv*/[]) {
  AudioOutput audio_output;
  InputManager input_manager;

  AudioClock audio_clock(kSampleRate);

  Musician musician(kSampleRate);
  musician.SetPlaybackTempo(kInitialTempo);

  const Id performer_instrument_id = musician.AddInstrument(
      SynthInstrument::GetDefinition(),
      {{SynthInstrumentParam::kNumVoices, ParamDefinition{kNumVoices}},
       {SynthInstrumentParam::kGain, ParamDefinition{kGain}},
       {SynthInstrumentParam::kOscillatorType,
        ParamDefinition{static_cast<int>(kOscillatorType)}},
       {SynthInstrumentParam::kEnvelopeAttack, ParamDefinition{kAttack}},
       {SynthInstrumentParam::kEnvelopeRelease, ParamDefinition{kRelease}}});
  const Id metronome_id = musician.AddInstrument(
      SynthInstrument::GetDefinition(),
      {{SynthInstrumentParam::kNumVoices, ParamDefinition{kNumVoices}},
       {SynthInstrumentParam::kGain, ParamDefinition{0.5f * kGain}},
       {SynthInstrumentParam::kOscillatorType,
        ParamDefinition{static_cast<int>(OscillatorType::kSquare)}},
       {SynthInstrumentParam::kEnvelopeAttack, ParamDefinition{kAttack}},
       {SynthInstrumentParam::kEnvelopeRelease, ParamDefinition{0.025f}}});

  musician.SetInstrumentNoteOnCallback(
      [&](Id instrument_id, float note_pitch, float note_intensity) {
        if (instrument_id == performer_instrument_id) {
          ConsoleLog() << "Note{" << MidiKeyNumberFromPitch(note_pitch) << ", "
                       << note_intensity << "}";
        }
      });

  const auto build_note = [](float pitch, double duration,
                             float intensity = 0.25f) {
    return Note{.pitch = pitch, .intensity = intensity, .duration = duration};
  };
  std::vector<std::pair<double, Note>> notes;
  notes.emplace_back(0.0, build_note(barely::kPitchC4, 1.0));
  notes.emplace_back(1.0, build_note(barely::kPitchD4, 1.0));
  notes.emplace_back(2.0, build_note(barely::kPitchE4, 1.0));
  notes.emplace_back(3.0, build_note(barely::kPitchF4, 1.0));
  notes.emplace_back(4.0, build_note(barely::kPitchG4, 1.0));
  notes.emplace_back(5.0, build_note(barely::kPitchG4, 1.0 / 3.0));
  notes.emplace_back(5.0 + 1.0 / 3.0, build_note(barely::kPitchA5, 1.0 / 3.0));
  notes.emplace_back(5.0 + 2.0 / 3.0, build_note(barely::kPitchB5, 1.0 / 3.0));
  notes.emplace_back(6.0, build_note(barely::kPitchC5, 2.0));

  const Id performer_id = musician.AddPerformer();
  musician.AddPerformerInstrument(performer_id, performer_instrument_id);
  musician.SetPerformerBeginPosition(performer_id, 2.0);
  musician.SetPerformerEndPosition(performer_id, 19.5);
  musician.SetPerformerBeginOffset(performer_id, -1.0);
  musician.SetPerformerLoop(performer_id, true);
  musician.SetPerformerLoopBeginOffset(performer_id, 3.0);
  musician.SetPerformerLoopLength(performer_id, 5.0);
  std::vector<Id> note_ids;
  for (const auto& [position, note] : notes) {
    note_ids.push_back(GetStatusOrValue(
        musician.AddPerformerNote(performer_id, position, note)));
  }

  bool use_conductor = false;
  Random random;

  double reset_position = false;
  musician.SetPlaybackBeatCallback([&](double /*position*/) {
    musician.SetInstrumentNoteOn(metronome_id, barely::kPitchC3, 1.0);
    musician.SetInstrumentNoteOff(metronome_id, barely::kPitchC3);
    if (reset_position) {
      reset_position = false;
      musician.SetPlaybackPosition(0.0);
    }
    ConsoleLog() << "Beat: " << musician.GetPlaybackPosition();
  });

  // Audio process callback.
  std::vector<float> temp_buffer(kNumChannels * kNumFrames);
  const auto process_callback = [&](float* output) {
    std::fill_n(output, kNumChannels * kNumFrames, 0.0f);
    for (const Id instrument_id : {performer_instrument_id, metronome_id}) {
      musician.ProcessInstrument(instrument_id, audio_clock.GetTimestamp(),
                                 temp_buffer.data(), kNumChannels, kNumFrames);
      std::transform(temp_buffer.cbegin(), temp_buffer.cend(), output, output,
                     std::plus<float>());
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
      if (IsOk(musician.RemovePerformerNote(performer_id, note_ids[index]))) {
        ConsoleLog() << "Removed note " << index;
        note_ids[index] = barely::kInvalidId;
      } else {
        note_ids[index] = GetStatusOrValue(musician.AddPerformerNote(
            performer_id, notes[index].first, notes[index].second));
        ConsoleLog() << "Added note " << index;
      }
      return;
    }
    // Adjust tempo.
    double tempo = musician.GetPlaybackTempo();
    switch (std::toupper(key)) {
      case ' ':
        if (musician.IsPlaying()) {
          musician.StopPlayback();
          ConsoleLog() << "Stopped playback";
        } else {
          musician.StartPlayback();
          ConsoleLog() << "Started playback";
        }
        return;
      case 'L':
        if (GetStatusOrValue(musician.IsPerformerLooping(performer_id))) {
          musician.SetPerformerLoop(performer_id, false);
          ConsoleLog() << "Loop turned off";
        } else {
          musician.SetPerformerLoop(performer_id, true);
          ConsoleLog() << "Loop turned on";
        }
        return;
      case 'C':
        use_conductor = !use_conductor;
        musician.SetConductor(
            use_conductor
                ? ConductorDefinition{
                      .transform_note_duration_fn =
                          [&](ConductorState*,
                              const NoteDuration& note_duration) {
                            return std::get<double>(note_duration) * 0.25 *
                                   static_cast<double>(
                                       random.DrawUniform(0, 4));
                          },
                      .transform_note_intensity_fn =
                          [&](ConductorState*,
                              const NoteIntensity& note_intensity) {
                            return std::get<float>(note_intensity) * 0.25f *
                                   static_cast<float>(random.DrawUniform(1, 4));
                          },
                      .transform_note_pitch_fn =
                          [&](ConductorState*, const NotePitch& note_pitch) {
                            return std::get<float>(note_pitch) +
                                   static_cast<float>(
                                       random.DrawUniform(-1, 1));
                          },
                      .transform_playback_tempo_fn =
                          [&](ConductorState*, double playback_tempo) {
                            return 1.25 * playback_tempo;
                          }}
                : ConductorDefinition{},
            ParamDefinitionMap{});
        ConsoleLog() << "Conductor turned " << (use_conductor ? "on" : "off");
        return;
      case 'P':
        reset_position = true;
        return;
      case 'O':
        musician.SetPlaybackPosition(0.0);
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
    musician.SetPlaybackTempo(tempo);
    ConsoleLog() << "Tempo set to " << tempo << " BPM";
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start(kSampleRate, kNumChannels, kNumFrames);
  musician.StartPlayback();

  while (!quit) {
    input_manager.Update();
    musician.Update(audio_clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  ConsoleLog() << "Stopping audio stream";
  musician.StopPlayback();
  audio_output.Stop();

  return 0;
}
