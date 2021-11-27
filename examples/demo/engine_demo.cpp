#include <algorithm>
#include <any>
#include <cassert>
#include <cctype>
#include <chrono>
#include <memory>
#include <thread>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "barelymusician/common/id.h"
#include "barelymusician/common/random.h"
#include "barelymusician/common/status.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/composition/note_duration.h"
#include "barelymusician/composition/note_pitch.h"
#include "barelymusician/engine/musician.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"
#include "examples/common/wav_file.h"
#include "examples/instruments/drumkit_instrument.h"
#include "examples/instruments/synth_instrument.h"
#include "tools/cpp/runfiles/runfiles.h"

namespace {

using ::barely::GetPitch;
using ::barely::GetStatusOrValue;
using ::barely::Id;
using ::barely::Musician;
using ::barely::Note;
using ::barely::OscillatorType;
using ::barely::Random;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::DrumkitInstrument;
using ::barely::examples::InputManager;
using ::barely::examples::SynthInstrument;
using ::barely::examples::SynthInstrumentParam;
using ::barely::examples::WavFile;
using ::bazel::tools::cpp::runfiles::Runfiles;

// Beat composer callback signature.
//
// @param bar Current bar.
// @param beat Current beat.
// @param num_beats Number of beats in a bar.
// @param harmonic Harmonic index.
// @param offset Position offset in beats.
// @param musician Pointer to Musician.
// @param performer_id Performer id.
using BeatComposerCallback =
    std::function<void(int bar, int beat, int num_beats, int harmonic,
                       double offset, Musician* musician, Id performer_id)>;

// System audio settings.
constexpr int kSampleRate = 48000;
constexpr int kNumChannels = 2;
constexpr int kNumFrames = 1024;

constexpr double kLookahead = 0.1;

// Sequencer settings.
constexpr double kTempo = 124.0;
constexpr int kNumBeats = 3;

// Ensemble settings.
constexpr float kRootNote = barely::kPitchD3;
constexpr int kNumInstrumentVoices = 8;

constexpr char kDrumsBaseFilename[] =
    "barelymusician/examples/data/audio/drums/";

void ComposeChord(float root_note, const std::vector<float>& scale,
                  float intensity, int harmonic, double offset,
                  Musician* musician, Id performer_id) {
  const auto add_chord_note = [&](int index) {
    musician->AddPerformerNote(
        performer_id, offset,
        Note{root_note + GetPitch(scale, index), intensity, 1.0});
  };
  add_chord_note(harmonic);
  add_chord_note(harmonic + 2);
  add_chord_note(harmonic + 4);
  add_chord_note(harmonic + 7);
}

void ComposeLine(float root_note, const std::vector<float>& scale,
                 float intensity, int bar, int beat, int num_beats,
                 int harmonic, double offset, Musician* musician,
                 Id performer_id) {
  const int note_offset = beat;
  const auto add_note = [&](double begin_position, double end_position,
                            int index) {
    musician->AddPerformerNote(performer_id, begin_position + offset,
                               Note{root_note + GetPitch(scale, index),
                                    intensity, end_position - begin_position});
  };
  if (beat % 2 == 1) {
    add_note(0.0, 0.25, harmonic);
    add_note(0.33, 0.66, harmonic - note_offset);
    add_note(0.66, 1.0, harmonic);
  } else {
    add_note(0.0, 0.25, harmonic + note_offset);
  }
  if (beat % 2 == 0) {
    add_note(0.0, 0.05, harmonic - note_offset);
    add_note(0.5, 0.55, harmonic - 2 * note_offset);
  }
  if (beat + 1 == num_beats && bar % 2 == 1) {
    add_note(0.25, 0.375, harmonic + 2 * note_offset);
    add_note(0.75, 0.875, harmonic - 2 * note_offset);
    add_note(0.5, 0.75, harmonic + 2 * note_offset);
  }
}

void ComposeDrums(int bar, int beat, int num_beats, Random* random,
                  double offset, Musician* musician, Id performer_id) {
  const auto get_beat = [](int step) {
    return barely::GetPosition(step, barely::kNumSixteenthNotesPerBeat);
  };
  const auto add_note = [&](double begin_position, double end_position,
                            float pitch, float intensity) {
    musician->AddPerformerNote(
        performer_id, begin_position + offset,
        Note{pitch, intensity, end_position - begin_position});
  };

  // Kick.
  if (beat % 2 == 0) {
    add_note(get_beat(0), get_beat(2), barely::kPitchKick, 1.0f);
    if (bar % 2 == 1 && beat == 0) {
      add_note(get_beat(2), get_beat(4), barely::kPitchKick, 1.0f);
    }
  }
  // Snare.
  if (beat % 2 == 1) {
    add_note(get_beat(0), get_beat(2), barely::kPitchSnare, 1.0f);
  }
  if (beat + 1 == num_beats) {
    add_note(get_beat(2), get_beat(4), barely::kPitchSnare, 0.75f);
    if (bar % 4 == 3) {
      add_note(get_beat(1), get_beat(2), barely::kPitchSnare, 1.0f);
      add_note(get_beat(3), get_beat(4), barely::kPitchSnare, 0.75f);
    }
  }
  // Hihat Closed.
  add_note(get_beat(0), get_beat(2), barely::kPitchHihatClosed,
           random->DrawUniform(0.5f, 0.75f));
  add_note(get_beat(2), get_beat(4), barely::kPitchHihatClosed,
           random->DrawUniform(0.25f, 0.75f));
  // Hihat Open.
  if (beat + 1 == num_beats) {
    if (bar % 4 == 3) {
      add_note(get_beat(1), get_beat(2), barely::kPitchHihatOpen, 0.5f);
    } else if (bar % 2 == 0) {
      add_note(get_beat(3), get_beat(4), barely::kPitchHihatOpen, 0.5f);
    }
  }
  if (beat == 0 && bar % 4 == 0) {
    add_note(get_beat(0), get_beat(2), barely::kPitchHihatOpen, 0.75f);
  }
}

}  // namespace

int main(int /*argc*/, char* argv[]) {
  std::string error;
  std::unique_ptr<Runfiles> runfiles(Runfiles::Create(argv[0], &error));
  assert(runfiles);

  AudioOutput audio_output;
  InputManager input_manager;

  Random random;

  AudioClock clock(kSampleRate);

  Musician musician(kSampleRate);
  musician.SetPlaybackTempo(kTempo);

  // Note on callback.
  const auto note_on_callback = [](Id instrument_id, float pitch,
                                   float intensity) {
    ConsoleLog() << "Instrument #" << instrument_id << ": NoteOn(" << pitch
                 << ", " << intensity << ")";
  };
  musician.SetInstrumentNoteOnCallback(note_on_callback);

  // Note off callback.
  const auto note_off_callback = [](Id performer_id, float pitch) {
    ConsoleLog() << "Instrument #" << performer_id << ": NoteOff(" << pitch
                 << ")";
  };
  musician.SetInstrumentNoteOffCallback(note_off_callback);

  const std::vector<int> progression = {0, 3, 4, 0};
  const std::vector<float> scale(std::cbegin(barely::kPitchMajorScale),
                                 std::cend(barely::kPitchMajorScale));

  // Initialize performers.
  std::vector<std::pair<Id, BeatComposerCallback>> performers;
  std::vector<Id> instrument_ids;

  const auto build_synth_instrument_fn = [&](OscillatorType type, float gain,
                                             float attack, float release) {
    instrument_ids.push_back(musician.AddInstrument(
        SynthInstrument::GetDefinition(),
        {{SynthInstrumentParam::kNumVoices,
          static_cast<float>(kNumInstrumentVoices)},
         {SynthInstrumentParam::kOscillatorType, static_cast<float>(type)},
         {SynthInstrumentParam::kGain, gain},
         {SynthInstrumentParam::kEnvelopeAttack, attack},
         {SynthInstrumentParam::kEnvelopeRelease, release}}));
  };

  // Add synth instruments.
  const auto chords_beat_composer_callback = std::bind(
      ComposeChord, kRootNote, scale, 0.5f, std::placeholders::_4,
      std::placeholders::_5, std::placeholders::_6, std::placeholders::_7);
  performers.emplace_back(musician.AddPerformer(),
                          chords_beat_composer_callback);

  build_synth_instrument_fn(OscillatorType::kSine, 0.1f, 0.125f, 0.125f);
  musician.AddPerformerInstrument(performers.back().first,
                                  instrument_ids.back());

  build_synth_instrument_fn(OscillatorType::kNoise, 0.025f, 0.5f, 0.025f);
  musician.AddPerformerInstrument(performers.back().first,
                                  instrument_ids.back());

  const auto line_beat_composer_callback = std::bind(
      ComposeLine, kRootNote - 1.0f, scale, 1.0f, std::placeholders::_1,
      std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
      std::placeholders::_5, std::placeholders::_6, std::placeholders::_7);
  performers.emplace_back(musician.AddPerformer(), line_beat_composer_callback);

  build_synth_instrument_fn(OscillatorType::kSaw, 0.1f, 0.0025f, 0.125f);
  musician.AddPerformerInstrument(performers.back().first,
                                  instrument_ids.back());

  const auto line_2_beat_composer_callback = std::bind(
      ComposeLine, kRootNote, scale, 1.0f, std::placeholders::_1,
      std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
      std::placeholders::_5, std::placeholders::_6, std::placeholders::_7);
  performers.emplace_back(musician.AddPerformer(),
                          line_2_beat_composer_callback);

  build_synth_instrument_fn(OscillatorType::kSquare, 0.125f, 0.05f, 0.05f);
  musician.AddPerformerInstrument(performers.back().first,
                                  instrument_ids.back());

  // Add drumkit instrument.
  instrument_ids.push_back(
      musician.AddInstrument(DrumkitInstrument::GetDefinition(), {}));
  std::unordered_map<float, std::string> drumkit_map = {
      {barely::kPitchKick, "basic_kick.wav"},
      {barely::kPitchSnare, "basic_snare.wav"},
      {barely::kPitchHihatClosed, "basic_hihat_closed.wav"},
      {barely::kPitchHihatOpen, "basic_hihat_open.wav"}};
  std::unordered_map<float, WavFile> drumkit_files;
  for (const auto& [index, name] : drumkit_map) {
    auto it = drumkit_files.emplace(index, WavFile{});
    const std::string path = runfiles->Rlocation(kDrumsBaseFilename + name);
    assert(it.first->second.Load(path));
  }
  musician.SetCustomInstrumentData(instrument_ids.back(),
                                   std::any(&drumkit_files));
  const auto drumkit_beat_composer_callback =
      std::bind(ComposeDrums, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3, &random, std::placeholders::_5,
                std::placeholders::_6, std::placeholders::_7);
  performers.emplace_back(musician.AddPerformer(),
                          drumkit_beat_composer_callback);
  musician.AddPerformerInstrument(performers.back().first,
                                  instrument_ids.back());

  // Bar callback.
  const auto bar_composer_callback = [&progression](int bar) -> int {
    return progression[bar % progression.size()];
  };

  // Beat callback.
  int harmonic = 0;
  const auto beat_callback = [&](double beat) {
    // Update transport.
    const int current_bar = static_cast<int>(beat) / kNumBeats;
    const int current_beat = static_cast<int>(beat) % kNumBeats;

    if (current_beat == 0) {
      // Compose next bar.
      harmonic = bar_composer_callback(current_bar);
    }
    // Update members.
    for (const auto& [performer_id, beat_callback] : performers) {
      // Compose next beat notes.
      if (beat_callback) {
        beat_callback(current_bar, current_beat, kNumBeats, harmonic, beat,
                      &musician, performer_id);
      }
    }
  };
  musician.SetPlaybackBeatCallback(beat_callback);

  // Audio process callback.
  std::vector<float> temp_buffer(kNumChannels * kNumFrames);
  const auto process_callback = [&](float* output) {
    std::fill_n(output, kNumChannels * kNumFrames, 0.0f);
    for (const Id instrument_id : instrument_ids) {
      musician.ProcessInstrument(instrument_id, clock.GetTimestamp(),
                                 temp_buffer.data(), kNumChannels, kNumFrames);
      std::transform(temp_buffer.cbegin(), temp_buffer.cend(), output, output,
                     std::plus<float>());
    }
    clock.Update(kNumFrames);
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
    switch (std::toupper(key)) {
      case ' ':
        if (musician.IsPlaying()) {
          musician.StopPlayback();
          ConsoleLog() << "Stopped playback";
        } else {
          musician.StartPlayback();
          ConsoleLog() << "Started playback";
        }
        break;
      case '1':
        musician.SetPlaybackTempo(random.DrawUniform(0.5, 0.75) *
                                  musician.GetPlaybackTempo());
        ConsoleLog() << "Tempo changed to " << musician.GetPlaybackTempo();
        break;
      case '2':
        musician.SetPlaybackTempo(random.DrawUniform(1.5, 2.0) *
                                  musician.GetPlaybackTempo());
        ConsoleLog() << "Tempo changed to " << musician.GetPlaybackTempo();
        break;
      case 'R':
        musician.SetPlaybackTempo(kTempo);
        ConsoleLog() << "Tempo reset to " << kTempo;
        break;
    }
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start(kSampleRate, kNumChannels, kNumFrames);
  musician.StartPlayback();

  while (!quit) {
    input_manager.Update();
    musician.Update(clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  ConsoleLog() << "Stopping audio stream";
  musician.StopPlayback();
  audio_output.Stop();

  return 0;
}
