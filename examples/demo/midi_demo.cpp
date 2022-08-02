#include <algorithm>
#include <cassert>
#include <chrono>
#include <memory>
#include <thread>
#include <utility>
#include <vector>

#include "MidiFile.h"
#include "barelymusician/barelymusician.h"
#include "barelymusician/composition/note_pitch.h"
#include "barelymusician/instruments/synth_instrument.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"
#include "tools/cpp/runfiles/runfiles.h"

namespace {

using ::barely::Instrument;
using ::barely::Musician;
using ::barely::OscillatorType;
using ::barely::Sequencer;
using ::barely::SynthInstrument;
using ::barely::SynthParameter;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;
using ::bazel::tools::cpp::runfiles::Runfiles;
using ::smf::MidiFile;

// System audio settings.
constexpr int kSampleRate = 48000;
constexpr int kNumChannels = 2;
constexpr int kNumFrames = 512;

constexpr double kLookahead = 0.1;

// Instrument settings.
constexpr OscillatorType kInstrumentOscillatorType = OscillatorType::kSquare;
constexpr double kInstrumentEnvelopeAttack = 0.0;
constexpr double kInstrumentEnvelopeRelease = 0.2;
constexpr int kNumInstrumentVoices = 16;
constexpr double kInstrumentGain =
    1.0 / static_cast<double>(kNumInstrumentVoices);

constexpr double kMaxVelocity = 127.0;

// Midi file name.
constexpr char kMidiFileName[] = "barelymusician/examples/data/midi/sample.mid";

constexpr double kTempo = 132.0;

// Returns the pitch for the given `midi_key_number`.
double PitchFromMidiKeyNumber(int midi_key_number) {
  return static_cast<double>(midi_key_number - 69) / barely::kNumSemitones;
}

// Returns the MIDI key number for the given `pitch`.
int MidiKeyNumberFromPitch(double pitch) {
  return static_cast<int>(barely::kNumSemitones * pitch) + 69;
}

// Builds the score for the given `midi_events`.
bool BuildScore(const smf::MidiEventList& midi_events, int ticks_per_beat,
                Instrument& instrument, Sequencer& sequencer) {
  const auto get_position_fn = [ticks_per_beat](int tick) -> double {
    return static_cast<double>(tick) / static_cast<double>(ticks_per_beat);
  };
  const auto play_note_fn = [&](double duration, double pitch,
                                double intensity) -> Sequencer::EventCallback {
    return
        [&instrument, &sequencer, pitch, duration, intensity](double position) {
          instrument.StartNote(pitch, intensity);
          sequencer.ScheduleOneOffEvent(
              position + duration,
              [&instrument, pitch]([[maybe_unused]] double position) {
                instrument.StopNote(pitch);
              });
        };
  };
  bool has_notes = false;
  for (int i = 0; i < midi_events.size(); ++i) {
    const auto& midi_event = midi_events[i];
    if (midi_event.isNoteOn()) {
      const double position = get_position_fn(midi_event.tick);
      const double duration = get_position_fn(midi_event.getTickDuration());
      const double pitch = PitchFromMidiKeyNumber(midi_event.getKeyNumber());
      const double intensity =
          static_cast<double>(midi_event.getVelocity()) / kMaxVelocity;
      sequencer.AddEvent(position, play_note_fn(duration, pitch, intensity));
      has_notes = true;
    }
  }
  return has_notes;
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
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

  std::vector<std::pair<Instrument, Sequencer>> tracks;
  tracks.reserve(num_tracks);
  for (int i = 0; i < num_tracks; ++i) {
    tracks.emplace_back(musician.CreateInstrument(
                            SynthInstrument::GetDefinition(), kSampleRate),
                        musician.CreateSequencer());
    Instrument& instrument = tracks.back().first;
    Sequencer& sequencer = tracks.back().second;
    // Build score.
    if (!BuildScore(midi_file[i], ticks_per_quarter, instrument, sequencer)) {
      ConsoleLog() << "Empty MIDI track: " << i;
      tracks.pop_back();
      continue;
    }
    // Set instrument.
    const auto track_index = tracks.size();
    instrument.SetNoteOnCallback([track_index](double pitch, double intensity) {
      ConsoleLog() << "MIDI track #" << track_index << ": NoteOn("
                   << MidiKeyNumberFromPitch(pitch) << ", " << intensity << ")";
    });
    instrument.SetNoteOffCallback([track_index](double pitch) {
      ConsoleLog() << "MIDI track #" << track_index << ": NoteOff("
                   << MidiKeyNumberFromPitch(pitch) << ") ";
    });
    instrument.SetParameter(SynthParameter::kOscillatorType,
                            kInstrumentOscillatorType);
    instrument.SetParameter(SynthParameter::kAttack, kInstrumentEnvelopeAttack);
    instrument.SetParameter(SynthParameter::kRelease,
                            kInstrumentEnvelopeRelease);
    instrument.SetParameter(SynthParameter::kNumVoices, kNumInstrumentVoices);
  }
  ConsoleLog() << "Number of active MIDI tracks: " << tracks.size();

  // Audio process callback.
  std::vector<double> temp_buffer(kNumChannels * kNumFrames);
  const auto process_callback = [&](double* output) {
    std::fill_n(output, kNumChannels * kNumFrames, 0.0);
    for (auto& [instrument, sequencer] : tracks) {
      instrument.Process(temp_buffer.data(), kNumChannels, kNumFrames,
                         clock.GetTimestamp());
      std::transform(temp_buffer.cbegin(), temp_buffer.cend(), output, output,
                     [](double sample, double output_sample) {
                       return kInstrumentGain * sample + output_sample;
                     });
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
  for (auto& [instrument, sequencer] : tracks) {
    sequencer.Start();
  }

  while (!quit) {
    input_manager.Update();
    musician.Update(clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  ConsoleLog() << "Stopping audio stream";
  for (auto& [instrument, sequencer] : tracks) {
    sequencer.Stop();
    instrument.StopAllNotes();
  }
  audio_output.Stop();

  return 0;
}
