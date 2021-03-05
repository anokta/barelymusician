#include <algorithm>
#include <chrono>
#include <memory>
#include <thread>
#include <utility>
#include <vector>

#include "MidiFile.h"
#include "barelymusician/common/logging.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/composition/note_utils.h"
#include "barelymusician/engine/engine.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/input_manager.h"
#include "examples/instruments/synth_instrument.h"
#include "tools/cpp/runfiles/runfiles.h"

namespace {

using ::barelyapi::Engine;
using ::barelyapi::Note;
using ::barelyapi::OscillatorType;
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
std::vector<Note> BuildScore(const smf::MidiEventList& midi_events,
                             int ticks_per_beat) {
  const auto get_position = [ticks_per_beat](int tick) -> double {
    return static_cast<double>(tick) / static_cast<double>(ticks_per_beat);
  };
  std::vector<Note> score;
  for (int i = 0; i < midi_events.size(); ++i) {
    const auto& midi_event = midi_events[i];
    if (midi_event.isNoteOn()) {
      Note note;
      note.begin_position = get_position(midi_event.tick);
      note.end_position =
          note.begin_position + get_position(midi_event.getTickDuration());
      note.pitch = PitchFromMidiKeyNumber(midi_event.getKeyNumber());
      note.intensity =
          static_cast<float>(midi_event.getVelocity()) / kMaxVelocity;
      score.push_back(std::move(note));
    }
  }
  return score;
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
  Engine engine(kSampleRate);
  engine.SetPlaybackTempo(kTempo);
  engine.SetNoteOnCallback([](int id, double, float pitch, float intensity) {
    LOG(INFO) << "MIDI track #" << id << ": NoteOn("
              << MidiKeyNumberFromPitch(pitch) << ", " << intensity << ")";
  });
  engine.SetNoteOffCallback([](int id, double, float pitch) {
    LOG(INFO) << "MIDI track #" << id << ": NoteOff("
              << MidiKeyNumberFromPitch(pitch) << ") ";
  });

  std::vector<int> instrument_ids;
  for (int i = 0; i < num_tracks; ++i) {
    // Build score.
    const auto score = BuildScore(midi_file[i], ticks_per_quarter);
    if (score.empty()) {
      LOG(WARNING) << "Empty MIDI track: " << i;
      continue;
    }
    // Create instrument.
    const auto instrument_id = engine.CreateInstrument(
        SynthInstrument::GetDefinition(),
        {{SynthInstrumentParam::kNumVoices,
          static_cast<float>(kNumInstrumentVoices)},
         {SynthInstrumentParam::kOscillatorType,
          static_cast<float>(kInstrumentOscillatorType)},
         {SynthInstrumentParam::kEnvelopeAttack, kInstrumentEnvelopeAttack},
         {SynthInstrumentParam::kEnvelopeRelease, kInstrumentEnvelopeRelease},
         {SynthInstrumentParam::kGain, kInstrumentGain}});
    for (const Note& note : score) {
      engine.ScheduleInstrumentNote(instrument_id, note.begin_position,
                                    note.end_position, note.pitch,
                                    note.intensity);
    }
    instrument_ids.push_back(instrument_id);
  }
  LOG(INFO) << "Number of active MIDI tracks: " << instrument_ids.size();

  // Audio process callback.
  std::vector<float> temp_buffer(kNumChannels * kNumFrames);
  const auto process_callback = [&](float* output) {
    std::fill_n(output, kNumChannels * kNumFrames, 0.0f);
    for (const auto& id : instrument_ids) {
      engine.ProcessInstrument(id, clock.GetTimestamp(), temp_buffer.data(),
                               kNumChannels, kNumFrames);
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
  engine.Update(clock.GetTimestamp() + kLookahead);
  engine.StartPlayback();

  while (!quit) {
    input_manager.Update();
    engine.Update(clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  LOG(INFO) << "Stopping audio stream";
  engine.StopPlayback();
  audio_output.Stop();

  return 0;
}
