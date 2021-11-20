#include <algorithm>
#include <chrono>
#include <memory>
#include <thread>
#include <utility>
#include <vector>

#include "MidiFile.h"
#include "barelymusician/common/id.h"
#include "barelymusician/common/logging.h"
#include "barelymusician/common/status.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/composition/note_pitch.h"
#include "barelymusician/composition/note_sequence.h"
#include "barelymusician/composition/sequencer.h"
#include "barelymusician/engine/instrument_manager.h"
#include "barelymusician/engine/transport.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/input_manager.h"
#include "examples/instruments/synth_instrument.h"
#include "tools/cpp/runfiles/runfiles.h"

namespace {

using ::barelyapi::GetStatusOrValue;
using ::barelyapi::Id;
using ::barelyapi::InstrumentManager;
using ::barelyapi::Note;
using ::barelyapi::NoteSequence;
using ::barelyapi::OscillatorType;
using ::barelyapi::Sequencer;
using ::barelyapi::Transport;
using ::barelyapi::examples::AudioClock;
using ::barelyapi::examples::AudioOutput;
using ::barelyapi::examples::InputManager;
using ::barelyapi::examples::SynthInstrument;
using ::barelyapi::examples::SynthInstrumentParam;
using ::bazel::tools::cpp::runfiles::Runfiles;
using ::smf::MidiFile;

// System audio settings.
constexpr int kSampleRate = 48000;
constexpr int kNumChannels = 2;
constexpr int kNumFrames = 512;

constexpr double kLookahead = 0.1;

// Sequencer settings.
constexpr double kTempo = 132.0 / 60.0;

// Performer settings.
constexpr int kNumInstrumentVoices = 16;
constexpr float kInstrumentGain =
    1.0f / static_cast<float>(kNumInstrumentVoices);
constexpr float kInstrumentEnvelopeAttack = 0.0f;
constexpr float kInstrumentEnvelopeRelease = 0.2f;
constexpr OscillatorType kInstrumentOscillatorType = OscillatorType::kSquare;

constexpr float kMaxVelocity = 127.0f;

// Midi file name.
constexpr char kMidiFileName[] = "barelymusician/examples/data/midi/sample.mid";

// Returns the pitch for the given |midi_key_number|.
float PitchFromMidiKeyNumber(int midi_key_number) {
  return static_cast<float>(midi_key_number - 69) / barelyapi::kNumSemitones;
}

// Returns the MIDI key number for the given |pitch|.
int MidiKeyNumberFromPitch(float pitch) {
  return static_cast<int>(barelyapi::kNumSemitones * pitch) + 69;
}

// Builds the score from the given |midi_events|.
bool BuildScore(const smf::MidiEventList& midi_events, int ticks_per_beat,
                NoteSequence* sequence) {
  const auto get_position = [ticks_per_beat](int tick) -> double {
    return static_cast<double>(tick) / static_cast<double>(ticks_per_beat);
  };
  for (int i = 0; i < midi_events.size(); ++i) {
    const auto& midi_event = midi_events[i];
    if (midi_event.isNoteOn()) {
      Note note;
      note.pitch = PitchFromMidiKeyNumber(midi_event.getKeyNumber());
      note.intensity =
          static_cast<float>(midi_event.getVelocity()) / kMaxVelocity;
      note.duration = get_position(midi_event.getTickDuration());
      sequence->Add(static_cast<Id>(i), get_position(midi_event.tick),
                    std::move(note));
    }
  }
  return !sequence->IsEmpty();
}

}  // namespace

int main(int /*argc*/, char* argv[]) {
  std::string error;
  std::unique_ptr<Runfiles> runfiles(Runfiles::Create(argv[0], &error));
  CHECK(runfiles);

  AudioOutput audio_output;
  InputManager input_manager;

  MidiFile midi_file;
  const std::string midi_file_path = runfiles->Rlocation(kMidiFileName);
  CHECK(midi_file.read(midi_file_path)) << "Failed to read " << kMidiFileName;
  CHECK(midi_file.isAbsoluteTicks()) << "Events should be in absolute ticks";
  midi_file.linkNotePairs();

  const int num_tracks = midi_file.getTrackCount();
  const int ticks_per_quarter = midi_file.getTPQ();
  LOG(INFO) << "Initializing " << kMidiFileName << " for MIDI playback ("
            << num_tracks << " tracks, " << ticks_per_quarter << " TPQ)";

  AudioClock clock(kSampleRate);
  InstrumentManager instrument_manager(kSampleRate);
  instrument_manager.SetNoteOnCallback(
      [](Id id, double /*timestamp*/, float pitch, float intensity) {
        LOG(INFO) << "MIDI track #" << id << ": NoteOn("
                  << MidiKeyNumberFromPitch(pitch) << ", " << intensity << ")";
      });
  instrument_manager.SetNoteOffCallback(
      [](Id id, double /*timestamp*/, float pitch) {
        LOG(INFO) << "MIDI track #" << id << ": NoteOff("
                  << MidiKeyNumberFromPitch(pitch) << ") ";
      });

  Transport transport;
  transport.SetTempo(kTempo);

  Sequencer sequencer;
  int num_instruments = 0;
  for (int i = 0; i < num_tracks; ++i) {
    // Build score.
    const Id sequence_id = static_cast<Id>(i);
    sequencer.CreateSequence(sequence_id);
    if (!BuildScore(midi_file[i], ticks_per_quarter,
                    GetStatusOrValue(sequencer.GetSequence(sequence_id)))) {
      LOG(WARNING) << "Empty MIDI track: " << i;
      sequencer.DestroySequence(sequence_id);
      continue;
    }
    // Create instrument.
    const Id instrument_id = static_cast<Id>(num_instruments++);
    instrument_manager.Create(
        instrument_id, 0.0, SynthInstrument::GetDefinition(),
        {{SynthInstrumentParam::kNumVoices,
          static_cast<float>(kNumInstrumentVoices)},
         {SynthInstrumentParam::kOscillatorType,
          static_cast<float>(kInstrumentOscillatorType)},
         {SynthInstrumentParam::kEnvelopeAttack, kInstrumentEnvelopeAttack},
         {SynthInstrumentParam::kEnvelopeRelease, kInstrumentEnvelopeRelease},
         {SynthInstrumentParam::kGain, kInstrumentGain}});
    sequencer.SetInstrument(sequence_id, instrument_id);
  }
  LOG(INFO) << "Number of active MIDI tracks: " << num_instruments;

  // Transport update callback.
  const auto update_callback =
      [&](double begin_position, double end_position,
          const Transport::GetTimestampFn& get_timestamp_fn) {
        instrument_manager.ProcessEvents(
            sequencer.Process(begin_position, end_position, get_timestamp_fn));
      };
  transport.SetUpdateCallback(update_callback);

  // Audio process callback.
  std::vector<float> temp_buffer(kNumChannels * kNumFrames);
  const auto process_callback = [&](float* output) {
    std::fill_n(output, kNumChannels * kNumFrames, 0.0f);
    for (int i = 0; i < num_instruments; ++i) {
      instrument_manager.Process(static_cast<Id>(i), clock.GetTimestamp(),
                                 temp_buffer.data(), kNumChannels, kNumFrames);
      std::transform(temp_buffer.cbegin(), temp_buffer.cend(), output, output,
                     std::plus<float>());
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
  LOG(INFO) << "Starting audio stream";
  audio_output.Start(kSampleRate, kNumChannels, kNumFrames);
  transport.Start();

  while (!quit) {
    input_manager.Update();
    transport.Update(clock.GetTimestamp() + kLookahead);
    instrument_manager.Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  LOG(INFO) << "Stopping audio stream";
  audio_output.Stop();

  return 0;
}
