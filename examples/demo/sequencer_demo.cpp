#include <cctype>
#include <chrono>
#include <cmath>
#include <map>
#include <memory>
#include <thread>
#include <utility>
#include <variant>
#include <vector>

#include "barelymusician/common/id.h"
#include "barelymusician/common/logging.h"
#include "barelymusician/common/random.h"
#include "barelymusician/common/status.h"
#include "barelymusician/composition/note_pitch.h"
#include "barelymusician/composition/sequencer.h"
#include "barelymusician/engine/conductor_definition.h"
#include "barelymusician/engine/instrument_manager.h"
#include "barelymusician/engine/transport.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/input_manager.h"
#include "examples/instruments/synth_instrument.h"

namespace {

using ::barelyapi::ConductorDefinition;
using ::barelyapi::ConductorState;
using ::barelyapi::GetStatusOrValue;
using ::barelyapi::Id;
using ::barelyapi::InstrumentManager;
using ::barelyapi::IsOk;
using ::barelyapi::Note;
using ::barelyapi::NoteDuration;
using ::barelyapi::NoteIntensity;
using ::barelyapi::NotePitch;
using ::barelyapi::NoteSequence;
using ::barelyapi::OscillatorType;
using ::barelyapi::Random;
using ::barelyapi::Sequencer;
using ::barelyapi::StatusOr;
using ::barelyapi::Transport;
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
constexpr Id kInstrumentId = 1;
constexpr int kNumVoices = 4;
constexpr float kGain = 0.2f;
constexpr OscillatorType kOscillatorType = OscillatorType::kSaw;
constexpr float kAttack = 0.0f;
constexpr float kRelease = 0.1f;

constexpr Id kMetronomeId = 2;
constexpr double kInitialTempo = 2.0;
constexpr double kTempoIncrement = 0.1;

constexpr Id kSequenceId = 10;

// Returns the MIDI key number for the given |pitch|.
int MidiKeyNumberFromPitch(float pitch) {
  return static_cast<int>(barelyapi::kNumSemitones * pitch) + 69;
}

}  // namespace

int main(int /*argc*/, char* /*argv*/[]) {
  AudioOutput audio_output;
  InputManager input_manager;

  AudioClock audio_clock(kSampleRate);

  InstrumentManager instrument_manager(kSampleRate);
  Transport transport;
  transport.SetTempo(kInitialTempo);

  // Create metronome instrument.
  instrument_manager.Create(
      kInstrumentId, 0.0, SynthInstrument::GetDefinition(),
      {{SynthInstrumentParam::kNumVoices, static_cast<float>(kNumVoices)},
       {SynthInstrumentParam::kGain, kGain},
       {SynthInstrumentParam::kOscillatorType,
        static_cast<float>(kOscillatorType)},
       {SynthInstrumentParam::kEnvelopeAttack, kAttack},
       {SynthInstrumentParam::kEnvelopeRelease, kRelease}});

  instrument_manager.Create(
      kMetronomeId, 0.0, SynthInstrument::GetDefinition(),
      {{SynthInstrumentParam::kNumVoices, static_cast<float>(kNumVoices)},
       {SynthInstrumentParam::kGain, 0.5f * kGain},
       {SynthInstrumentParam::kOscillatorType,
        static_cast<float>(OscillatorType::kSquare)},
       {SynthInstrumentParam::kEnvelopeAttack, kAttack},
       {SynthInstrumentParam::kEnvelopeRelease, 0.025f}});

  instrument_manager.SetNoteOnCallback([](Id instrument_id, double timestamp,
                                          float note_pitch,
                                          float note_intensity) {
    if (instrument_id == kInstrumentId) {
      LOG(INFO) << "Note{" << MidiKeyNumberFromPitch(note_pitch) << ", "
                << note_intensity << "} at: " << timestamp;
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

  Sequencer sequencer;
  sequencer.CreateSequence(kSequenceId);
  sequencer.AddInstrument(kSequenceId, kInstrumentId);
  auto* sequence = GetStatusOrValue(sequencer.GetSequence(kSequenceId));
  sequence->SetStartPosition(2.0);
  sequence->SetEndPosition(20.0);
  sequence->SetLooping(true);
  sequence->SetLoopLength(5.0);
  sequence->SetLoopStartOffset(3.0);
  sequence->SetStartOffset(-1.0);
  int note_index = 0;
  for (const auto& [position, note] : notes) {
    sequence->Add(++note_index, position, note);
  }

  // Transport update callback.
  double reset_position = false;
  const auto update_callback =
      [&](double begin_position, double end_position,
          const Transport::GetTimestampFn& get_timestamp_fn) {
        for (auto& [position, instrument_id_event_pair] :
             sequencer.Process(begin_position, end_position)) {
          auto& [instrument_id, event] = instrument_id_event_pair;
          instrument_manager.ProcessEvent(
              instrument_id, get_timestamp_fn(position), std::move(event));
        }
      };
  transport.SetUpdateCallback(update_callback);

  transport.SetBeatCallback([&](double position) {
    instrument_manager.SetNoteOn(kMetronomeId, transport.GetTimestamp(),
                                 barelyapi::kPitchC3, 1.0);
    instrument_manager.SetNoteOff(kMetronomeId, transport.GetTimestamp(),
                                  barelyapi::kPitchC3);
    if (reset_position) {
      reset_position = false;
      transport.SetPosition(0.0);
      position = transport.GetPosition();
    }
    LOG(INFO) << "Beat: " << position;
  });

  // Audio process callback.
  std::vector<float> temp_buffer(kNumChannels * kNumFrames);
  const auto process_callback = [&](float* output) {
    std::fill_n(output, kNumChannels * kNumFrames, 0.0f);
    for (int i = 0; i < 2; ++i) {
      instrument_manager.Process(static_cast<Id>(i + 1),
                                 audio_clock.GetTimestamp(), temp_buffer.data(),
                                 kNumChannels, kNumFrames);
      std::transform(temp_buffer.cbegin(), temp_buffer.cend(), output, output,
                     std::plus<float>());
    }
    audio_clock.Update(kNumFrames);
  };
  audio_output.SetProcessCallback(process_callback);

  bool use_conductor = false;
  Random random;
  // Key down callback.
  bool quit = false;
  const auto key_down_callback = [&](const InputManager::Key& key) {
    if (static_cast<int>(key) == 27) {
      // ESC pressed, quit the app.
      quit = true;
      return;
    }
    if (const int id = static_cast<int>(key - '0'); id > 0 && id < 10) {
      // Toggle notes.
      if (IsOk(sequence->Remove(static_cast<Id>(id)))) {
        LOG(INFO) << "Removed note " << id;
      } else {
        sequence->Add(static_cast<Id>(id), notes[id - 1].first,
                      notes[id - 1].second);
        LOG(INFO) << "Added note " << id;
      }
      return;
    }
    // Adjust tempo.
    double tempo = transport.GetTempo();
    switch (std::toupper(key)) {
      case ' ':
        if (transport.IsPlaying()) {
          transport.Stop();
          sequencer.Stop();
          instrument_manager.SetAllNotesOff(transport.GetTimestamp());
          LOG(INFO) << "Stopped playback";
        } else {
          transport.Start();
          LOG(INFO) << "Started playback";
        }
        return;
      case 'L':
        if (sequence->IsLooping()) {
          sequence->SetLooping(false);
          LOG(INFO) << "Looping turned off";
        } else {
          sequence->SetLooping(true);
          LOG(INFO) << "Looping turned on";
        }
        return;
      case 'C':
        use_conductor = !use_conductor;
        sequencer.SetConductor(
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
                          }}
                : ConductorDefinition{});
        LOG(WARNING) << "Conductor is " << (use_conductor ? "on" : "off");
        return;
      case 'P':
        reset_position = true;
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
    transport.SetTempo(tempo);
    LOG(INFO) << "Tempo set to " << transport.GetTempo() << " BPM";
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  LOG(INFO) << "Starting audio stream";
  audio_output.Start(kSampleRate, kNumChannels, kNumFrames);
  transport.Start();

  while (!quit) {
    input_manager.Update();
    transport.Update(audio_clock.GetTimestamp() + kLookahead);
    instrument_manager.Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  LOG(INFO) << "Stopping audio stream";
  audio_output.Stop();

  return 0;
}
