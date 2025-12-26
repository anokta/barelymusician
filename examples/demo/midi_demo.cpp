#include <barelymusician.h>

#include <cassert>
#include <chrono>
#include <cstddef>
#include <string>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

#include "MidiEventList.h"
#include "MidiFile.h"
#include "common/audio_clock.h"
#include "common/audio_output.h"
#include "common/console_log.h"
#include "common/input_manager.h"
#include "data/data.h"

namespace {

using ::barely::Engine;
using ::barely::Instrument;
using ::barely::InstrumentControlType;
using ::barely::NoteEventType;
using ::barely::PerformerRef;
using ::barely::TaskEventType;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::GetDataFilePath;
using ::barely::examples::InputManager;
using ::smf::MidiFile;

// System audio settings.
constexpr int kSampleRate = 48000;
constexpr int kChannelCount = 2;
constexpr int kFrameCount = 512;

constexpr double kLookahead = 0.1;

// Instrument settings.
constexpr float kInstrumentOscShape = 0.5f;
constexpr float kInstrumentEnvelopeAttack = 0.0f;
constexpr float kInstrumentEnvelopeRelease = 0.2f;
constexpr int kInstrumentVoiceCount = 16;
constexpr float kInstrumentGain = 0.1f;

// Midi file name.
constexpr char kMidiFileName[] = "midi/sample.mid";

constexpr double kTempo = 132.0;

// Builds the score for the given `midi_events`.
bool BuildScore(const smf::MidiEventList& midi_events, int ticks_per_beat, Engine& engine,
                Instrument& instrument, PerformerRef& performer) {
  const auto get_position_fn = [ticks_per_beat](int tick) -> double {
    return static_cast<double>(tick) / static_cast<double>(ticks_per_beat);
  };
  bool has_notes = false;
  for (int i = 0; i < midi_events.size(); ++i) {
    const auto& midi_event = midi_events[i];
    if (midi_event.isNoteOn()) {
      const double position = get_position_fn(midi_event.tick);
      const double duration = get_position_fn(midi_event.getTickDuration());
      const float pitch = static_cast<float>(midi_event.getKeyNumber() - 60) / 12.0f;
      const float gain = static_cast<float>(midi_event.getVelocity()) / 127.0f;
      engine.CreateTask(performer, position, duration, 0,
                        [&, pitch, gain](TaskEventType type) noexcept {
                          if (type == TaskEventType::kBegin) {
                            instrument.SetNoteOn(pitch, gain);
                          } else if (type == TaskEventType::kEnd) {
                            instrument.SetNoteOff(pitch);
                          }
                        });
      has_notes = true;
    }
  }
  return has_notes;
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int /*argc*/, char* argv[]) {
  InputManager input_manager;

  MidiFile midi_file;
  const std::string midi_file_path = GetDataFilePath(kMidiFileName, argv);
  [[maybe_unused]] const bool success = midi_file.read(midi_file_path);
  assert(success && midi_file.isAbsoluteTicks());
  midi_file.linkNotePairs();

  const int track_count = midi_file.getTrackCount();
  const int ticks_per_quarter = midi_file.getTPQ();
  ConsoleLog() << "Initializing " << kMidiFileName << " for MIDI playback (" << track_count
               << " tracks, " << ticks_per_quarter << " TPQ)";

  AudioClock audio_clock(kSampleRate);
  AudioOutput audio_output(kSampleRate, kChannelCount, kFrameCount);

  Engine engine(kSampleRate, kFrameCount);
  engine.SetTempo(kTempo);

  std::vector<std::tuple<Instrument, PerformerRef, size_t>> tracks;
  tracks.reserve(track_count);
  for (int i = 0; i < track_count; ++i) {
    tracks.emplace_back(engine.CreateInstrument(), engine.CreatePerformer(), tracks.size() + 1);
    auto& [instrument, performer, track_index] = tracks.back();
    // Build the score to perform.
    if (!BuildScore(midi_file[i], ticks_per_quarter, engine, instrument, performer)) {
      ConsoleLog() << "Empty MIDI track: " << i;
      tracks.pop_back();
      continue;
    }
    // Set the instrument settings.
    instrument.SetNoteEventCallback([track_index](NoteEventType type, float pitch) {
      ConsoleLog() << "MIDI track #" << track_index << ": Note"
                   << (type == NoteEventType::kBegin ? "On" : "Off") << "(" << pitch << ")";
    });
    instrument.SetControl(InstrumentControlType::kGain, kInstrumentGain);
    instrument.SetControl(InstrumentControlType::kOscMix, 1.0f);
    instrument.SetControl(InstrumentControlType::kOscShape, kInstrumentOscShape);
    instrument.SetControl(InstrumentControlType::kAttack, kInstrumentEnvelopeAttack);
    instrument.SetControl(InstrumentControlType::kRelease, kInstrumentEnvelopeRelease);
    instrument.SetControl(InstrumentControlType::kVoiceCount, kInstrumentVoiceCount);
  }
  ConsoleLog() << "Number of active MIDI tracks: " << tracks.size();

  // Audio process callback.
  audio_output.SetProcessCallback(
      [&](float* output_samples, int output_channel_count, int output_frame_count) {
        engine.Process(output_samples, output_channel_count, output_frame_count,
                       audio_clock.GetTimestamp());
        audio_clock.Update(output_frame_count);
      });

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
  audio_output.Start();
  engine.Update(kLookahead);
  for (auto& [instrument, performer, _] : tracks) {
    performer.Start();
  }

  while (!quit) {
    input_manager.Update();
    engine.Update(audio_clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  return 0;
}
