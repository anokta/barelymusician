#include <algorithm>
#include <cassert>
#include <chrono>
#include <memory>
#include <thread>
#include <utility>
#include <vector>

#include "MidiFile.h"
#include "examples/composition/note_pitch.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"
#include "examples/instruments/synth_instrument.h"
#include "platforms/api/barelymusician.h"
#include "tools/cpp/runfiles/runfiles.h"

namespace {

using ::barely::Instrument;
using ::barely::Musician;
using ::barely::Sequence;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;
using ::barely::examples::SynthInstrument;
using ::barely::examples::SynthInstrumentParameter;
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
              Sequence* sequence) {
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
      sequence->AddNote(get_position(midi_event.tick), note);
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

  Musician musician;
  musician.SetTempo(kTempo);

  std::vector<std::pair<Instrument, Sequence>> tracks;
  for (int i = 0; i < num_tracks; ++i) {
    // Build score.
    Sequence sequence = musician.CreateSequence();
    AddScore(midi_file[i], ticks_per_quarter, &sequence);
    if (sequence.IsEmpty()) {
      ConsoleLog() << "Empty MIDI track: " << i;
      continue;
    }
    // Add instrument.
    Instrument instrument = musician.CreateInstrument(
        SynthInstrument::GetDefinition(), kSampleRate);
    const auto track_index = tracks.size();
    instrument.SetNoteOnCallback([track_index](float pitch, float intensity,
                                               double /*timestamp*/) {
      ConsoleLog() << "MIDI track #" << track_index << ": NoteOn("
                   << MidiKeyNumberFromPitch(pitch) << ", " << intensity << ")";
    });
    instrument.SetNoteOffCallback(
        [track_index](float pitch, double /*timestamp*/) {
          ConsoleLog() << "MIDI track #" << track_index << ": NoteOff("
                       << MidiKeyNumberFromPitch(pitch) << ") ";
        });
    instrument.SetGain(kInstrumentGain);
    instrument.SetParameter(SynthInstrumentParameter::kEnvelopeAttack,
                            kInstrumentEnvelopeAttack);
    instrument.SetParameter(SynthInstrumentParameter::kEnvelopeRelease,
                            kInstrumentEnvelopeRelease);
    instrument.SetParameter(SynthInstrumentParameter::kOscillatorType,
                            static_cast<float>(kInstrumentOscillatorType));
    instrument.SetParameter(SynthInstrumentParameter::kNumVoices,
                            static_cast<float>(kNumInstrumentVoices));
    tracks.emplace_back(std::move(instrument), std::move(sequence));
    tracks.back().second.SetInstrument(&tracks.back().first);
  }
  ConsoleLog() << "Number of active MIDI tracks: " << tracks.size();

  // Audio process callback.
  std::vector<float> temp_buffer(kNumChannels * kNumFrames);
  const auto process_callback = [&](float* output) {
    std::fill_n(output, kNumChannels * kNumFrames, 0.0f);
    for (auto& [instrument, sequence] : tracks) {
      instrument.Process(clock.GetTimestamp(), temp_buffer.data(), kNumChannels,
                         kNumFrames);
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
  musician.Start();

  while (!quit) {
    input_manager.Update();
    musician.Update(clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  ConsoleLog() << "Stopping audio stream";
  musician.Stop();
  audio_output.Stop();

  return 0;
}
