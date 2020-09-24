#include <algorithm>
#include <chrono>
#include <memory>
#include <thread>
#include <utility>
#include <vector>

#include "MidiFile.h"
#include "audio_output/pa_audio_output.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/engine/engine.h"
#include "barelymusician/engine/note.h"
#include "instruments/basic_synth_instrument.h"
#include "util/input_manager/win_console_input.h"

namespace {

using ::barelyapi::Engine;
using ::barelyapi::Note;
using ::barelyapi::OscillatorType;
using ::barelyapi::examples::BasicSynthInstrument;
using ::barelyapi::examples::BasicSynthInstrumentParam;
using ::barelyapi::examples::PaAudioOutput;
using ::barelyapi::examples::WinConsoleInput;
using ::smf::MidiFile;

// System audio settings.
const int kSampleRate = 48000;
const int kNumChannels = 2;
const int kNumFrames = 512;

// Sequencer settings.
const double kTempo = 132.0;

// Performer settings.
const int kNumInstrumentVoices = 16;
const float kInstrumentGain = 1.0f / static_cast<float>(kNumInstrumentVoices);
const float kInstrumentEnvelopeAttack = 0.0f;
const float kInstrumentEnvelopeRelease = 0.2f;
const OscillatorType kInstrumentOscillatorType = OscillatorType::kSquare;

const float kMaxVelocity = 127.0f;

// Midi file name.
const char kMidiFileName[] = "data/midi/sample.mid";

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
      note.position = get_position(midi_event.tick);
      note.duration = get_position(midi_event.getTickDuration());
      note.index = static_cast<float>(midi_event.getKeyNumber());
      note.intensity =
          static_cast<float>(midi_event.getVelocity()) / kMaxVelocity;
      score.push_back(std::move(note));
    }
  }
  return score;
}

}  // namespace

int main(int argc, char* argv[]) {
  PaAudioOutput audio_output;
  WinConsoleInput input_manager;

  MidiFile midi_file;
  CHECK(midi_file.read(kMidiFileName)) << "Failed to read " << kMidiFileName;
  CHECK(midi_file.isAbsoluteTicks()) << "Events should be in absolute ticks";
  midi_file.linkNotePairs();

  const int num_tracks = midi_file.getTrackCount();
  const int ticks_per_quarter = midi_file.getTPQ();
  LOG(INFO) << "Initializing " << kMidiFileName << " for MIDI playback ("
            << num_tracks << " tracks, " << ticks_per_quarter << " TPQ)";

  Engine engine(kSampleRate);
  engine.SetTempo(kTempo);
  engine.SetNoteOnCallback([](int instrument_id, float index, float intensity) {
    LOG(INFO) << "MIDI track #" << instrument_id << ": NoteOn(" << index << ", "
              << intensity << ")";
  });
  engine.SetNoteOffCallback([](int instrument_id, float index) {
    LOG(INFO) << "MIDI track #" << instrument_id << ": NoteOff(" << index
              << ") ";
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
    auto instrument = std::make_unique<BasicSynthInstrument>(
        kSampleRate, kNumInstrumentVoices);
    instrument->Control(BasicSynthInstrumentParam::kOscillatorType,
                        static_cast<float>(kInstrumentOscillatorType));
    instrument->Control(BasicSynthInstrumentParam::kEnvelopeAttack,
                        kInstrumentEnvelopeAttack);
    instrument->Control(BasicSynthInstrumentParam::kEnvelopeRelease,
                        kInstrumentEnvelopeRelease);
    instrument->Control(BasicSynthInstrumentParam::kGain, kInstrumentGain);
    engine.Create(i, std::move(instrument));
    for (const Note& note : score) {
      engine.ScheduleNote(i, note.position, note.duration, note.index,
                          note.intensity);
    }
    instrument_ids.push_back(i);
  }
  LOG(INFO) << "Number of active MIDI tracks: " << instrument_ids.size();

  // Audio process callback.
  std::vector<float> temp_buffer(kNumChannels * kNumFrames);
  const auto process_callback = [&](float* output) {
    engine.Update(kNumFrames);
    std::fill_n(output, kNumChannels * kNumFrames, 0.0f);
    for (const int id : instrument_ids) {
      engine.Process(id, temp_buffer.data(), kNumChannels, kNumFrames);
      std::transform(temp_buffer.cbegin(), temp_buffer.cend(), output, output,
                     std::plus<float>());
    }
  };
  audio_output.SetProcessCallback(process_callback);

  // Key down callback.
  bool quit = false;
  const auto key_down_callback = [&quit](const WinConsoleInput::Key& key) {
    if (static_cast<int>(key) == 27) {
      // ESC pressed, quit the app.
      quit = true;
      return;
    }
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  LOG(INFO) << "Starting audio stream";

  input_manager.Initialize();
  audio_output.Start(kSampleRate, kNumChannels, kNumFrames);
  engine.Start();

  while (!quit) {
    input_manager.Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  LOG(INFO) << "Stopping audio stream";

  engine.Stop();

  audio_output.Stop();
  input_manager.Shutdown();

  return 0;
}
