#include <cassert>
#include <cctype>
#include <chrono>
#include <cmath>
#include <memory>
#include <thread>
#include <utility>
#include <variant>
#include <vector>

#include "barelymusician/api/conductor.h"
#include "barelymusician/api/instrument.h"
#include "barelymusician/api/performer.h"
#include "barelymusician/api/presets/instruments.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"
#include "examples/composition/note_pitch.h"

namespace {

using ::barely::Conductor;
using ::barely::Instrument;
using ::barely::IsOk;
using ::barely::Performer;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;
using ::barely::presets::CreateInstrument;
using ::barely::presets::InstrumentType;
using ::barely::presets::OscillatorType;
using ::barely::presets::SynthParameter;

// System audio settings.
constexpr int kFrameRate = 48000;
constexpr int kNumChannels = 2;
constexpr int kNumFrames = 1024;

constexpr double kLookahead = 0.1;

// Instrument settings.
constexpr double kGain = 0.2;
constexpr OscillatorType kOscillatorType = OscillatorType::kSaw;
constexpr double kAttack = 0.0;
constexpr double kRelease = 0.1;

constexpr double kInitialTempo = 120.0;
constexpr double kTempoIncrement = 10.0;

// Returns the MIDI key number for the given `pitch`.
int MidiKeyNumberFromPitch(double pitch) {
  return static_cast<int>(barelyapi::kNumSemitones * pitch) + 69;
}

}  // namespace

int main(int /*argc*/, char* /*argv*/[]) {
  AudioOutput audio_output;
  InputManager input_manager;

  AudioClock audio_clock(kFrameRate);

  // Create conductor.
  Conductor conductor;
  conductor.SetTempo(kInitialTempo);

  Instrument instrument = CreateInstrument(InstrumentType::kSynth, kFrameRate);
  instrument.SetParameter(SynthParameter::kAttack, kAttack);
  instrument.SetParameter(SynthParameter::kRelease, kRelease);
  instrument.SetParameter(SynthParameter::kOscillatorType,
                          static_cast<double>(kOscillatorType));
  instrument.SetNoteOnCallback(
      [](double pitch, double intensity, double /*timestamp*/) {
        ConsoleLog() << "Note{" << MidiKeyNumberFromPitch(pitch) << ", "
                     << intensity << "}";
      });

  Instrument metronome = CreateInstrument(InstrumentType::kSynth, kFrameRate);
  metronome.SetParameter(SynthParameter::kAttack, kAttack);
  metronome.SetParameter(SynthParameter::kRelease, 0.025);
  metronome.SetParameter(SynthParameter::kOscillatorType,
                         static_cast<double>(OscillatorType::kSquare));

  const auto build_note = [](double pitch, double duration,
                             double intensity = 0.25) {
    return BarelyNoteDefinition{duration, intensity, {.absolute_pitch = pitch}};
  };
  std::vector<std::pair<double, BarelyNoteDefinition>> notes;
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

  Performer performer;
  performer.SetInstrument(&instrument);
  performer.SetBeginPosition(2.0);
  performer.SetEndPosition(19.5);
  performer.SetBeginOffset(-1.0);
  performer.SetLooping(true);
  performer.SetLoopBeginOffset(3.0);
  performer.SetLoopLength(5.0);
  std::vector<BarelyNoteHandle> note_references;
  note_references.reserve(notes.size());
  for (const auto& [position, note] : notes) {
    note_references.push_back(performer.AddNote(position, note));
  }

  bool use_conductor = false;

  bool reset_position = false;
  const auto beat_callback = [&](double /*position*/, double timestamp) {
    metronome.StartNote(barelyapi::kPitchC3, 1.0, timestamp);
    metronome.StopNote(barelyapi::kPitchC3, timestamp);
    if (reset_position) {
      reset_position = false;
      conductor.SetPosition(0.0);
    }
    ConsoleLog() << "Beat: " << conductor.GetPosition();
  };
  conductor.SetBeatCallback(beat_callback);

  const auto update_callback = [&](double begin_position, double end_position,
                                   double /*begin_timestamp*/,
                                   double /*end_timestamp*/) {
    performer.Perform(conductor, begin_position, end_position);
  };
  conductor.SetUpdateCallback(update_callback);

  // Audio process callback.
  std::vector<double> gains = {kGain, 0.5 * kGain};
  std::vector<double> temp_buffer(kNumChannels * kNumFrames);
  const auto process_callback = [&](double* output) {
    std::fill_n(output, kNumChannels * kNumFrames, 0.0);
    int i = 0;
    for (auto* instr : {&instrument, &metronome}) {
      const double gain = gains[i++];
      instr->Process(temp_buffer.data(), kNumChannels, kNumFrames,
                     audio_clock.GetTimestamp());
      std::transform(temp_buffer.cbegin(), temp_buffer.cend(), output, output,
                     [&](double sample, double output_sample) {
                       return gain * sample + output_sample;
                     });
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
      if (IsOk(performer.RemoveNote(note_references[index - 1]))) {
        ConsoleLog() << "Removed note " << index;
      } else {
        note_references[index - 1] =
            performer.AddNote(notes[index - 1].first, notes[index - 1].second);
        ConsoleLog() << "Added note " << index;
      }
      return;
    }
    // Adjust tempo.
    double tempo = conductor.GetTempo();
    switch (std::toupper(key)) {
      case ' ':
        if (conductor.IsPlaying()) {
          conductor.Stop();
          ConsoleLog() << "Stopped playback";
        } else {
          conductor.Start();
          ConsoleLog() << "Started playback";
        }
        return;
      case 'L':
        if (performer.IsLooping()) {
          performer.SetLooping(false);
          ConsoleLog() << "Loop turned off";
        } else {
          performer.SetLooping(true);
          ConsoleLog() << "Loop turned on";
        }
        return;
      case 'C':
        use_conductor = !use_conductor;
        // TODO: Add back conductor adjustment.
        ConsoleLog() << "Conductor turned " << (use_conductor ? "on" : "off");
        return;
      case 'P':
        reset_position = true;
        return;
      case 'O':
        conductor.SetPosition(0.0);
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
    conductor.SetTempo(tempo);
    ConsoleLog() << "Tempo set to " << conductor.GetTempo() << " bpm";
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start(kFrameRate, kNumChannels, kNumFrames);
  conductor.Start();

  while (!quit) {
    input_manager.Update();
    conductor.Update(audio_clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  ConsoleLog() << "Stopping audio stream";
  conductor.Stop();
  audio_output.Stop();

  return 0;
}
