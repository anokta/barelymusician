#include <cctype>
#include <chrono>
#include <cmath>
#include <map>
#include <memory>
#include <thread>
#include <utility>
#include <vector>

#include "barelymusician/common/id.h"
#include "barelymusician/common/logging.h"
#include "barelymusician/common/status.h"
#include "barelymusician/composition/note_pitch.h"
#include "barelymusician/composition/note_sequence.h"
#include "barelymusician/engine/instrument_manager.h"
#include "barelymusician/engine/transport.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/input_manager.h"
#include "examples/instruments/synth_instrument.h"

namespace {

using ::barelyapi::Id;
using ::barelyapi::InstrumentManager;
using ::barelyapi::IsOk;
using ::barelyapi::Note;
using ::barelyapi::NoteSequence;
using ::barelyapi::OscillatorType;
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
constexpr float kGain = 0.25f;
constexpr OscillatorType kOscillatorType = OscillatorType::kSaw;
constexpr float kAttack = 0.0f;
constexpr float kRelease = 0.1f;

constexpr int kNumBeats = 4;
constexpr double kInitialTempo = 2.0;
constexpr double kTempoIncrement = 10.0;

}  // namespace

int main(int /*argc*/, char* /*argv*/[]) {
  AudioOutput audio_output;
  InputManager input_manager;

  AudioClock audio_clock(kSampleRate);

  InstrumentManager instrument_manager(kSampleRate);
  Transport transport;
  transport.SetTempo(kInitialTempo);

  const auto build_note = [](float pitch, double duration,
                             float intensity = 0.25f) {
    return Note{.pitch = pitch, .intensity = intensity, .duration = duration};
  };
  int note_index = 0;
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

  NoteSequence note_sequence;
  note_sequence.SetLooping(true);
  for (const auto& [position, note] : notes) {
    note_sequence.Add(++note_index, position, note);
  }

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
      2, 0.0, SynthInstrument::GetDefinition(),
      {{SynthInstrumentParam::kNumVoices, static_cast<float>(kNumVoices)},
       {SynthInstrumentParam::kGain, 0.5f * kGain},
       {SynthInstrumentParam::kOscillatorType,
        static_cast<float>(OscillatorType::kSquare)},
       {SynthInstrumentParam::kEnvelopeAttack, kAttack},
       {SynthInstrumentParam::kEnvelopeRelease, 0.025f}});

  // Transport update callback.
  std::multimap<double, Note> active_notes;
  const auto update_callback =
      [&](double begin_position, double end_position,
          const Transport::GetTimestampFn& get_timestamp_fn) {
        barelyapi::InstrumentControllerEvents events;
        for (auto it = active_notes.begin(); it != active_notes.end();) {
          const double note_begin_position = it->first;
          const double note_end_position = it->first + it->second.duration;
          if (note_end_position < end_position ||
              begin_position < note_begin_position) {
            const double off_position = (note_end_position < end_position)
                                            ? note_end_position
                                            : begin_position;
            events.emplace(get_timestamp_fn(off_position),
                           std::pair{kInstrumentId, barelyapi::SetNoteOffEvent{
                                                        it->second.pitch}});
            it = active_notes.erase(it);
          } else {
            ++it;
          }
        }
        const auto note_callback = [&](double position, const Note& note) {
          events.emplace(
              get_timestamp_fn(position),
              std::pair{kInstrumentId,
                        barelyapi::SetNoteOnEvent{note.pitch, note.intensity}});
          if (position + note.duration < end_position) {
            events.emplace(get_timestamp_fn(position + note.duration),
                           std::pair{kInstrumentId,
                                     barelyapi::SetNoteOffEvent{note.pitch}});
          } else {
            active_notes.emplace(position, note);
          }
        };
        note_sequence.SetNoteCallback(note_callback);
        note_sequence.Process(begin_position, end_position);
        instrument_manager.ProcessEvents(std::move(events));
      };
  transport.SetUpdateCallback(update_callback);

  transport.SetBeatCallback([&](double position) {
    instrument_manager.SetNoteOn(2, transport.GetTimestamp(),
                                 barelyapi::kPitchC3, 1.0);
    instrument_manager.SetNoteOff(2, transport.GetTimestamp(),
                                  barelyapi::kPitchC3);
    if (note_sequence.IsLooping() && position >= 8.0) {
      transport.SetPosition(std::fmod(position, 8.0));
    }
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

  // Key down callback.
  bool quit = false;
  const auto key_down_callback = [&](const InputManager::Key& key) {
    if (static_cast<int>(key) == 27) {
      // ESC pressed, quit the app.
      quit = true;
      return;
    }
    if (const Id id = static_cast<int>(key - '0'); id > 0 && id < 10) {
      // Toggle notes.
      if (IsOk(note_sequence.Remove(id))) {
        LOG(INFO) << "Removed note " << id;
      } else {
        note_sequence.Add(id, notes[id - 1].first, notes[id - 1].second);
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
          instrument_manager.SetAllNotesOff(transport.GetTimestamp());
          LOG(INFO) << "Stopped playback";
        } else {
          transport.Start();
          LOG(INFO) << "Started playback";
        }
        return;
      case 'L':
        if (note_sequence.IsLooping()) {
          note_sequence.SetLooping(false);
          LOG(INFO) << "Looping turned off";
        } else {
          note_sequence.SetLooping(true);
          LOG(INFO) << "Looping turned on";
        }
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
    LOG(INFO) << "Tempo set to " << tempo << " BPM";
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
