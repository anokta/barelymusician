#include <algorithm>
#include <cassert>
#include <chrono>
#include <memory>
#include <thread>
#include <utility>
#include <vector>

#include "MidiFile.h"
#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
#include "barelymusician/composition/note_pitch.h"
#include "barelymusician/engine/engine.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"
#include "examples/instruments/synth_instrument.h"
#include "tools/cpp/runfiles/runfiles.h"

namespace {

using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;
using ::barely::examples::SynthInstrument;
using ::barely::examples::SynthInstrumentParameter;
using ::barelyapi::Engine;
using ::barelyapi::GetStatusOrValue;
using ::barelyapi::Id;
using ::barelyapi::OscillatorType;
using ::bazel::tools::cpp::runfiles::Runfiles;
using ::smf::MidiFile;

// System audio settings.
constexpr int kSampleRate = 48000;
constexpr int kNumChannels = 2;
constexpr int kNumFrames = 512;

constexpr double kLookahead = 0.1;

// Instrument settings.
constexpr int kNumInstrumentVoices = 16;
constexpr float kInstrumentGain =
    1.0f / static_cast<float>(kNumInstrumentVoices);
constexpr float kInstrumentEnvelopeAttack = 0.0f;
constexpr float kInstrumentEnvelopeRelease = 0.2f;
constexpr OscillatorType kInstrumentOscillatorType = OscillatorType::kSquare;

constexpr float kMaxVelocity = 127.0f;

// Midi file name.
constexpr char kMidiFileName[] = "barelymusician/examples/data/midi/sample.mid";

constexpr double kTempo = 132.0;

// Returns the pitch for the given `midi_key_number`.
float PitchFromMidiKeyNumber(int midi_key_number) {
  return static_cast<float>(midi_key_number - 69) / barelyapi::kNumSemitones;
}

// Returns the MIDI key number for the given `pitch`.
int MidiKeyNumberFromPitch(float pitch) {
  return static_cast<int>(barelyapi::kNumSemitones * pitch) + 69;
}

// Adds the score to `performer_id` from the given `midi_events`.
void AddScore(const smf::MidiEventList& midi_events, int ticks_per_beat,
              Engine* engine, Id performer_id) {
  const auto get_position = [ticks_per_beat](int tick) -> double {
    return static_cast<double>(tick) / static_cast<double>(ticks_per_beat);
  };
  for (int i = 0; i < midi_events.size(); ++i) {
    const auto& midi_event = midi_events[i];
    if (midi_event.isNoteOn()) {
      BarelyNoteDefinition note;
      note.pitch_definition.absolute_pitch =
          PitchFromMidiKeyNumber(midi_event.getKeyNumber());
      note.intensity_definition.intensity =
          static_cast<float>(midi_event.getVelocity()) / kMaxVelocity;
      note.duration_definition.duration =
          get_position(midi_event.getTickDuration());
      engine->AddPerformerNote(performer_id, get_position(midi_event.tick),
                               note);
    }
  }
}

}  // namespace

int main(int /*argc*/, char* argv[]) {
  std::unique_ptr<Runfiles> runfiles(Runfiles::Create(argv[0]));
  assert(runfiles);

  AudioOutput audio_output;
  InputManager input_manager;

  MidiFile midi_file;
  const std::string midi_file_path = runfiles->Rlocation(kMidiFileName);
  assert(midi_file.read(midi_file_path));
  assert(midi_file.isAbsoluteTicks());
  midi_file.linkNotePairs();

  const int num_tracks = midi_file.getTrackCount();
  const int ticks_per_quarter = midi_file.getTPQ();
  ConsoleLog() << "Initializing " << kMidiFileName << " for MIDI playback ("
               << num_tracks << " tracks, " << ticks_per_quarter << " TPQ)";

  AudioClock clock(kSampleRate);

  Engine engine;
  engine.SetPlaybackTempo(kTempo);

  std::vector<Id> instrument_ids;
  for (int i = 0; i < num_tracks; ++i) {
    // Build score.
    const Id performer_id = engine.AddPerformer();
    AddScore(midi_file[i], ticks_per_quarter, &engine, performer_id);
    if (GetStatusOrValue(engine.IsPerformerEmpty(performer_id))) {
      ConsoleLog() << "Empty MIDI track: " << i;
      engine.RemovePerformer(performer_id);
      continue;
    }
    // Add instrument.
    const Id instrument_id =
        engine.CreateInstrument(SynthInstrument::GetDefinition(), kSampleRate);
    engine.SetInstrumentNoteOnCallback(
        instrument_id,
        [](float pitch, float intensity, double /*timestamp*/,
           void* user_data) {
          // TODO: This won't work, but should be resolved by demo api refactor.
          ConsoleLog() << "MIDI track #" << *reinterpret_cast<Id*>(&user_data)
                       << ": NoteOn(" << MidiKeyNumberFromPitch(pitch) << ", "
                       << intensity << ")";
        },
        reinterpret_cast<void*>(instrument_id));
    engine.SetInstrumentNoteOffCallback(
        instrument_id,
        [](float pitch, double /*timestamp*/, void* user_data) {
          // TODO: This won't work, but should be resolved by demo api refactor.
          ConsoleLog() << "MIDI track #" << *reinterpret_cast<Id*>(&user_data)
                       << ": NoteOff(" << MidiKeyNumberFromPitch(pitch) << ") ";
        },
        reinterpret_cast<void*>(instrument_id));
    engine.SetInstrumentGain(instrument_id, kInstrumentGain);
    engine.SetInstrumentParameter(instrument_id,
                                  SynthInstrumentParameter::kEnvelopeAttack,
                                  kInstrumentEnvelopeAttack);
    engine.SetInstrumentParameter(instrument_id,
                                  SynthInstrumentParameter::kEnvelopeRelease,
                                  kInstrumentEnvelopeRelease);
    engine.SetInstrumentParameter(
        instrument_id, SynthInstrumentParameter::kOscillatorType,
        static_cast<float>(kInstrumentOscillatorType));
    engine.SetInstrumentParameter(instrument_id,
                                  SynthInstrumentParameter::kNumVoices,
                                  static_cast<float>(kNumInstrumentVoices));
    engine.SetPerformerInstrument(performer_id, instrument_id);
    instrument_ids.push_back(instrument_id);
  }
  ConsoleLog() << "Number of active MIDI tracks: " << instrument_ids.size();

  // Audio process callback.
  std::vector<float> temp_buffer(kNumChannels * kNumFrames);
  const auto process_callback = [&](float* output) {
    std::fill_n(output, kNumChannels * kNumFrames, 0.0f);
    for (const Id instrument_id : instrument_ids) {
      engine.ProcessInstrument(instrument_id, clock.GetTimestamp(),
                               temp_buffer.data(), kNumChannels, kNumFrames);
      std::transform(temp_buffer.cbegin(), temp_buffer.cend(), output, output,
                     std::plus<>());
    }
    clock.Update(kNumFrames);
  };
  audio_output.SetProcessCallback(process_callback);

  // Key down callback.
  bool quit = false;
  const auto key_down_callback = [&quit](const InputManager::Key& key) {
    if (static_cast<int>(key) == 27) {
      // ESC pressed, quit the app.
      quit = true;
      return;
    }
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start(kSampleRate, kNumChannels, kNumFrames);
  engine.StartPlayback();

  while (!quit) {
    input_manager.Update();
    engine.Update(clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  ConsoleLog() << "Stopping audio stream";
  engine.StopPlayback();
  audio_output.Stop();

  return 0;
}
