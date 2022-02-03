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
#include "barelymusician/engine/engine.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"
#include "examples/instruments/synth_instrument.h"

namespace {

using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;
using ::barely::examples::SynthInstrument;
using ::barely::examples::SynthInstrumentParam;
using ::barelyapi::ConductorDefinition;
using ::barelyapi::Engine;
using ::barelyapi::Id;
using ::barelyapi::IsOk;
using ::barelyapi::Note;
using ::barelyapi::NoteDuration;
using ::barelyapi::NoteIntensity;
using ::barelyapi::NotePitch;
using ::barelyapi::OscillatorType;
using ::barelyapi::Random;

// System audio settings.
constexpr int kSampleRate = 48000;
constexpr int kNumChannels = 2;
constexpr int kNumFrames = 1024;

constexpr double kLookahead = 0.1;

// Instrument settings.
constexpr float kGain = 0.2f;
constexpr OscillatorType kOscillatorType = OscillatorType::kSaw;
constexpr float kAttack = 0.0f;
constexpr float kRelease = 0.1f;

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

  Engine engine;
  engine.SetPlaybackTempo(kInitialTempo);

  const Id performer_instrument_id =
      engine.AddInstrument(SynthInstrument::GetDefinition(), kSampleRate);
  engine.SetInstrumentGain(performer_instrument_id, kGain);
  engine.SetInstrumentParam(performer_instrument_id,
                            SynthInstrumentParam::kEnvelopeAttack, kAttack);
  engine.SetInstrumentParam(performer_instrument_id,
                            SynthInstrumentParam::kEnvelopeRelease, kRelease);
  engine.SetInstrumentParam(performer_instrument_id,
                            SynthInstrumentParam::kOscillatorType,
                            static_cast<float>(kOscillatorType));
  engine.SetInstrumentNoteOnCallback(
      performer_instrument_id,
      [](float pitch, float intensity, double /*timestamp*/) {
        ConsoleLog() << "Note{" << MidiKeyNumberFromPitch(pitch) << ", "
                     << intensity << "}";
      });

  const Id metronome_id =
      engine.AddInstrument(SynthInstrument::GetDefinition(), kSampleRate);
  engine.SetInstrumentGain(metronome_id, 0.5f * kGain);
  engine.SetInstrumentParam(metronome_id, SynthInstrumentParam::kEnvelopeAttack,
                            kAttack);
  engine.SetInstrumentParam(metronome_id,
                            SynthInstrumentParam::kEnvelopeRelease, 0.025f);
  engine.SetInstrumentParam(metronome_id, SynthInstrumentParam::kOscillatorType,
                            static_cast<float>(OscillatorType::kSquare));

  const auto build_note = [](float pitch, double duration,
                             float intensity = 0.25f) {
    return Note{.pitch = pitch, .intensity = intensity, .duration = duration};
  };
  std::vector<std::pair<double, Note>> notes;
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

  const Id performer_id = engine.AddPerformer();
  engine.SetPerformerInstrument(performer_id, performer_instrument_id);
  engine.SetPerformerBeginPosition(performer_id, 2.0);
  engine.SetPerformerEndPosition(performer_id, 19.5);
  engine.SetPerformerBeginOffset(performer_id, -1.0);
  engine.SetPerformerLoop(performer_id, true);
  engine.SetPerformerLoopBeginOffset(performer_id, 3.0);
  engine.SetPerformerLoopLength(performer_id, 5.0);
  std::vector<Id> note_ids;
  for (const auto& [position, note] : notes) {
    note_ids.push_back(GetStatusOrValue(
        engine.AddPerformerNote(performer_id, position, note)));
  }

  bool use_conductor = false;
  Random random;

  bool reset_position = false;
  const auto beat_callback = [&](double /*position*/, double /*timestamp*/) {
    engine.SetInstrumentNoteOn(metronome_id, barelyapi::kPitchC3, 1.0);
    engine.SetInstrumentNoteOff(metronome_id, barelyapi::kPitchC3);
    if (reset_position) {
      reset_position = false;
      engine.SetPlaybackPosition(0.0);
    }
    ConsoleLog() << "Beat: " << engine.GetPlaybackPosition();
  };
  engine.SetPlaybackBeatCallback(beat_callback);

  // Audio process callback.
  std::vector<float> temp_buffer(kNumChannels * kNumFrames);
  const auto process_callback = [&](float* output) {
    std::fill_n(output, kNumChannels * kNumFrames, 0.0f);
    for (const Id instrument_id : {performer_instrument_id, metronome_id}) {
      engine.ProcessInstrument(instrument_id, audio_clock.GetTimestamp(),
                               temp_buffer.data(), kNumChannels, kNumFrames);
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
      if (IsOk(engine.RemovePerformerNote(performer_id, note_ids[index]))) {
        ConsoleLog() << "Removed note " << index;
        note_ids[index] = barelyapi::kInvalidId;
      } else {
        note_ids[index] = GetStatusOrValue(engine.AddPerformerNote(
            performer_id, notes[index].first, notes[index].second));
        ConsoleLog() << "Added note " << index;
      }
      return;
    }
    // Adjust tempo.
    double tempo = engine.GetPlaybackTempo();
    switch (std::toupper(key)) {
      case ' ':
        if (engine.IsPlaying()) {
          engine.StopPlayback();
          ConsoleLog() << "Stopped playback";
        } else {
          engine.StartPlayback();
          ConsoleLog() << "Started playback";
        }
        return;
      case 'L':
        if (GetStatusOrValue(engine.IsPerformerLooping(performer_id))) {
          engine.SetPerformerLoop(performer_id, false);
          ConsoleLog() << "Loop turned off";
        } else {
          engine.SetPerformerLoop(performer_id, true);
          ConsoleLog() << "Loop turned on";
        }
        return;
      case 'C':
        use_conductor = !use_conductor;
        engine.SetConductor(
            use_conductor
                ? ConductorDefinition{
                      .transform_note_duration_fn =
                          [&](void**, const NoteDuration& note_duration) {
                            return std::get<double>(note_duration) * 0.25 *
                                   static_cast<double>(
                                       random.DrawUniform(0, 4));
                          },
                      .transform_note_intensity_fn =
                          [&](void**, const NoteIntensity& note_intensity) {
                            return std::get<float>(note_intensity) * 0.25f *
                                   static_cast<float>(random.DrawUniform(1, 4));
                          },
                      .transform_note_pitch_fn =
                          [&](void**, const NotePitch& note_pitch) {
                            return std::get<float>(note_pitch) +
                                   static_cast<float>(
                                       random.DrawUniform(-1, 1));
                          },
                      .transform_playback_tempo_fn =
                          [&](void**, double playback_tempo) {
                            return 1.25 * playback_tempo;
                          }}
                : ConductorDefinition{});
        ConsoleLog() << "Conductor turned " << (use_conductor ? "on" : "off");
        return;
      case 'P':
        reset_position = true;
        return;
      case 'O':
        engine.SetPlaybackPosition(0.0);
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
    engine.SetPlaybackTempo(tempo);
    ConsoleLog() << "Tempo set to " << engine.GetPlaybackTempo() << " bpm";
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start(kSampleRate, kNumChannels, kNumFrames);
  engine.StartPlayback();

  while (!quit) {
    input_manager.Update();
    engine.Update(audio_clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  ConsoleLog() << "Stopping audio stream";
  engine.StopPlayback();
  audio_output.Stop();

  return 0;
}
