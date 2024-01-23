#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <functional>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "MidiEventList.h"
#include "MidiFile.h"
#include "barelymusician/barelymusician.h"
#include "barelymusician/composition/intensity.h"
#include "barelymusician/composition/pitch.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/instruments/synth_instrument.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"
#include "examples/data/data.h"

namespace {

using ::barely::Instrument;
using ::barely::IntensityFromMidi;
using ::barely::MidiFromIntensity;
using ::barely::MidiFromPitch;
using ::barely::Musician;
using ::barely::OscillatorType;
using ::barely::Performer;
using ::barely::PitchFromMidi;
using ::barely::Rational;
using ::barely::SynthInstrument;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::GetDataFilePath;
using ::barely::examples::InputManager;
using ::smf::MidiFile;

// System audio settings.
constexpr int kFrameRate = 48000;
constexpr int kChannelCount = 2;
constexpr int kFrameCount = 512;

constexpr int64_t kLookahead = kFrameRate / 10;

// Instrument settings.
constexpr OscillatorType kInstrumentOscillatorType = OscillatorType::kSquare;
constexpr Rational kInstrumentEnvelopeAttack = 0;
constexpr Rational kInstrumentEnvelopeRelease = Rational(1, 5);
constexpr int kInstrumentVoiceCount = 16;
constexpr Rational kInstrumentGain = Rational(1, kInstrumentVoiceCount);

// Midi file name.
constexpr char kMidiFileName[] = "midi/sample.mid";

constexpr int kTempo = 132;

// Builds the score for the given `midi_events`.
bool BuildScore(const smf::MidiEventList& midi_events, int ticks_per_beat, Instrument& instrument,
                Performer& performer) {
  bool has_notes = false;
  for (int i = 0; i < midi_events.size(); ++i) {
    const auto& midi_event = midi_events[i];
    if (midi_event.isNoteOn()) {
      const Rational position = Rational(midi_event.tick, ticks_per_beat);
      const Rational duration = Rational(midi_event.getTickDuration(), ticks_per_beat);
      const Rational pitch = PitchFromMidi(midi_event.getKeyNumber());
      const float intensity = IntensityFromMidi(midi_event.getVelocity());
      performer.ScheduleOneOffTask(
          [&instrument, pitch, intensity]() mutable { instrument.SetNoteOn(pitch, intensity); },
          position);
      performer.ScheduleOneOffTask([&instrument, pitch]() mutable { instrument.SetNoteOff(pitch); },
                                   position + duration);
      has_notes = true;
    }
  }
  return has_notes;
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int /*argc*/, char* argv[]) {
  AudioOutput audio_output;
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

  AudioClock clock(kFrameRate);

  Musician musician(kFrameRate);
  musician.SetTempo(kTempo);

  std::vector<std::pair<Instrument, Performer>> tracks;
  tracks.reserve(track_count);
  for (int i = 0; i < track_count; ++i) {
    tracks.emplace_back(musician.CreateInstrument<SynthInstrument>(), musician.CreatePerformer());
    auto& [instrument, performer] = tracks.back();
    // Build the score to perform.
    if (!BuildScore(midi_file[i], ticks_per_quarter, instrument, performer)) {
      ConsoleLog() << "Empty MIDI track: " << i;
      tracks.pop_back();
      continue;
    }
    // Set the instrument settings.
    const auto track_index = tracks.size() + 1;
    instrument.SetNoteOnEvent([track_index](Rational pitch, float intensity) {
      ConsoleLog() << "MIDI track #" << track_index << ": NoteOn(key: " << MidiFromPitch(pitch)
                   << ", velocity: " << MidiFromIntensity(intensity) << ")";
    });
    instrument.SetNoteOffEvent([track_index](Rational pitch) {
      ConsoleLog() << "MIDI track #" << track_index << ": NoteOff(key: " << MidiFromPitch(pitch)
                   << ")";
    });
    instrument.SetControl(SynthInstrument::Control::kGain, kInstrumentGain);
    instrument.SetControl(SynthInstrument::Control::kOscillatorType,
                          static_cast<int>(kInstrumentOscillatorType));
    instrument.SetControl(SynthInstrument::Control::kAttack, kInstrumentEnvelopeAttack);
    instrument.SetControl(SynthInstrument::Control::kRelease, kInstrumentEnvelopeRelease);
    instrument.SetControl(SynthInstrument::Control::kVoiceCount, kInstrumentVoiceCount);
  }
  ConsoleLog() << "Number of active MIDI tracks: " << tracks.size();

  // Audio process callback.
  std::vector<float> mix_buffer(kChannelCount * kFrameCount);
  const auto process_callback = [&](float* output) {
    std::fill_n(output, kChannelCount * kFrameCount, 0.0f);
    for (auto& [instrument, performer] : tracks) {
      instrument.Process(mix_buffer.data(), kChannelCount, kFrameCount, clock.GetTimestamp());
      std::transform(mix_buffer.begin(), mix_buffer.end(), output, output, std::plus<>());
    }
    clock.Update(kFrameCount);
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
  audio_output.Start(kFrameRate, kChannelCount, kFrameCount);
  for (auto& [instrument, performer] : tracks) {
    performer.Start();
  }

  while (!quit) {
    input_manager.Update();
    musician.Update(clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  ConsoleLog() << "Stopping audio stream";
  for (auto& [instrument, performer] : tracks) {
    performer.Stop();
    instrument.SetAllNotesOff();
  }
  audio_output.Stop();

  return 0;
}
