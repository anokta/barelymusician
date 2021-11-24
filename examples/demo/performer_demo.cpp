#include <cctype>
#include <chrono>
#include <cmath>
#include <memory>
#include <thread>
#include <utility>
#include <variant>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/id.h"
#include "barelymusician/common/logging.h"
#include "barelymusician/common/random.h"
#include "barelymusician/common/status.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/composition/note_duration.h"
#include "barelymusician/composition/note_intensity.h"
#include "barelymusician/composition/note_pitch.h"
#include "barelymusician/engine/conductor_definition.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/input_manager.h"
#include "examples/instruments/synth_instrument.h"

namespace {

using ::barelyapi::BarelyMusician;
using ::barelyapi::ConductorDefinition;
using ::barelyapi::ConductorState;
using ::barelyapi::Id;
using ::barelyapi::IsOk;
using ::barelyapi::Note;
using ::barelyapi::NoteDuration;
using ::barelyapi::NoteIntensity;
using ::barelyapi::NotePitch;
using ::barelyapi::OscillatorType;
using ::barelyapi::Random;
using ::barelyapi::examples::AudioClock;
using ::barelyapi::examples::AudioOutput;
using ::barelyapi::examples::InputManager;
using ::barelyapi::examples::SynthInstrument;
using ::barelyapi::examples::SynthInstrumentParam;

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
  return static_cast<int>(barelyapi::kNumSemitones * pitch) + 69;
}

}  // namespace

int main(int /*argc*/, char* /*argv*/[]) {
  AudioOutput audio_output;
  InputManager input_manager;

  AudioClock audio_clock(kSampleRate);

  BarelyMusician barelymusician(kSampleRate);
  barelymusician.SetPlaybackTempo(kInitialTempo);

  const Id performer_instrument_id = barelymusician.AddInstrument(
      SynthInstrument::GetDefinition(),
      {{SynthInstrumentParam::kNumVoices, static_cast<float>(kNumVoices)},
       {SynthInstrumentParam::kGain, kGain},
       {SynthInstrumentParam::kOscillatorType,
        static_cast<float>(kOscillatorType)},
       {SynthInstrumentParam::kEnvelopeAttack, kAttack},
       {SynthInstrumentParam::kEnvelopeRelease, kRelease}});
  const Id metronome_id = barelymusician.AddInstrument(
      SynthInstrument::GetDefinition(),
      {{SynthInstrumentParam::kNumVoices, static_cast<float>(kNumVoices)},
       {SynthInstrumentParam::kGain, 0.5f * kGain},
       {SynthInstrumentParam::kOscillatorType,
        static_cast<float>(OscillatorType::kSquare)},
       {SynthInstrumentParam::kEnvelopeAttack, kAttack},
       {SynthInstrumentParam::kEnvelopeRelease, 0.025f}});

  barelymusician.SetInstrumentNoteOnCallback(
      [&](Id instrument_id, float note_pitch, float note_intensity) {
        if (instrument_id == performer_instrument_id) {
          LOG(INFO) << "Note{" << MidiKeyNumberFromPitch(note_pitch) << ", "
                    << note_intensity << "}";
        }
      });

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

  const Id performer_id = barelymusician.AddPerformer();
  barelymusician.AddPerformerInstrument(performer_id, performer_instrument_id);
  barelymusician.SetPerformerBeginPosition(performer_id, 2.0);
  barelymusician.SetPerformerEndPosition(performer_id, 19.5);
  barelymusician.SetPerformerBeginOffset(performer_id, -1.0);
  barelymusician.SetPerformerLooping(performer_id, true);
  barelymusician.SetPerformerLoopBeginOffset(performer_id, 3.0);
  barelymusician.SetPerformerLoopLength(performer_id, 5.0);
  std::vector<Id> note_ids;
  for (const auto& [position, note] : notes) {
    note_ids.push_back(GetStatusOrValue(
        barelymusician.AddPerformerNote(performer_id, position, note)));
  }

  bool use_conductor = false;
  Random random;

  double reset_position = false;
  barelymusician.SetPlaybackBeatCallback([&](double /*position*/) {
    barelymusician.SetInstrumentNoteOn(metronome_id, barelyapi::kPitchC3, 1.0);
    barelymusician.SetInstrumentNoteOff(metronome_id, barelyapi::kPitchC3);
    if (reset_position) {
      reset_position = false;
      barelymusician.SetPlaybackPosition(0.0);
    }
    LOG(INFO) << "Beat: " << barelymusician.GetPlaybackPosition();
  });

  // Audio process callback.
  std::vector<float> temp_buffer(kNumChannels * kNumFrames);
  const auto process_callback = [&](float* output) {
    std::fill_n(output, kNumChannels * kNumFrames, 0.0f);
    for (const Id instrument_id : {performer_instrument_id, metronome_id}) {
      barelymusician.ProcessInstrument(
          instrument_id, audio_clock.GetTimestamp(), temp_buffer.data(),
          kNumChannels, kNumFrames);
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
      if (IsOk(barelymusician.RemovePerformerNote(performer_id,
                                                  note_ids[index]))) {
        LOG(INFO) << "Removed note " << index;
        note_ids[index] = barelyapi::kInvalidId;
      } else {
        note_ids[index] = GetStatusOrValue(barelymusician.AddPerformerNote(
            performer_id, notes[index].first, notes[index].second));
        LOG(INFO) << "Added note " << index;
      }
      return;
    }
    // Adjust tempo.
    double tempo = barelymusician.GetPlaybackTempo();
    switch (std::toupper(key)) {
      case ' ':
        if (barelymusician.IsPlaying()) {
          barelymusician.StopPlayback();
          LOG(INFO) << "Stopped playback";
        } else {
          barelymusician.StartPlayback();
          LOG(INFO) << "Started playback";
        }
        return;
      case 'L':
        if (GetStatusOrValue(barelymusician.IsPerformerLooping(performer_id))) {
          barelymusician.SetPerformerLooping(performer_id, false);
          LOG(INFO) << "Looping turned off";
        } else {
          barelymusician.SetPerformerLooping(performer_id, true);
          LOG(INFO) << "Looping turned on";
        }
        return;
      case 'C':
        use_conductor = !use_conductor;
        barelymusician.SetConductor(
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
                : ConductorDefinition{});
        LOG(INFO) << "Conductor turned " << (use_conductor ? "on" : "off");
        return;
      case 'P':
        reset_position = true;
        return;
      case 'O':
        barelymusician.SetPlaybackPosition(0.0);
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
    barelymusician.SetPlaybackTempo(tempo);
    LOG(INFO) << "Tempo set to " << tempo << " BPM";
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  LOG(INFO) << "Starting audio stream";
  audio_output.Start(kSampleRate, kNumChannels, kNumFrames);
  barelymusician.StartPlayback();

  while (!quit) {
    input_manager.Update();
    barelymusician.Update(audio_clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  LOG(INFO) << "Stopping audio stream";
  barelymusician.StopPlayback();
  audio_output.Stop();

  return 0;
}
