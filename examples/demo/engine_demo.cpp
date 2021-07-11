#include <algorithm>
#include <any>
#include <atomic>
#include <cctype>
#include <chrono>
#include <memory>
#include <thread>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "barelymusician/common/id.h"
#include "barelymusician/common/logging.h"
#include "barelymusician/common/random.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/composition/note_utils.h"
#include "barelymusician/engine/instrument_manager.h"
#include "barelymusician/engine/sequencer.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/input_manager.h"
#include "examples/common/wav_file.h"
#include "examples/instruments/drumkit_instrument.h"
#include "examples/instruments/synth_instrument.h"
#include "tools/cpp/runfiles/runfiles.h"

namespace {

using ::barelyapi::GetPitch;
using ::barelyapi::Id;
using ::barelyapi::InstrumentManager;
using ::barelyapi::Note;
using ::barelyapi::OscillatorType;
using ::barelyapi::Random;
using ::barelyapi::Sequencer;
using ::barelyapi::examples::AudioClock;
using ::barelyapi::examples::AudioOutput;
using ::barelyapi::examples::DrumkitInstrument;
using ::barelyapi::examples::InputManager;
using ::barelyapi::examples::SynthInstrument;
using ::barelyapi::examples::SynthInstrumentParam;
using ::barelyapi::examples::WavFile;
using ::bazel::tools::cpp::runfiles::Runfiles;

// Beat composer callback signature.
using BeatComposerCallback = std::function<void(
    int bar, int beat, int num_beats, int harmonic, std::vector<Note>* notes)>;

// System audio settings.
constexpr int kSampleRate = 48000;
constexpr int kNumChannels = 2;
constexpr int kNumFrames = 1024;

constexpr double kLookahead = 0.1;

// Sequencer settings.
constexpr double kTempo = 124.0 / 60.0;
constexpr int kNumBeats = 3;

// Ensemble settings.
constexpr float kRootNote = barelyapi::kPitchD3;
constexpr int kNumInstrumentVoices = 8;

constexpr char kDrumsBaseFilename[] =
    "barelymusician/examples/data/audio/drums/";

void ComposeChord(float root_note, const std::vector<float>& scale,
                  float intensity, int harmonic, std::vector<Note>* notes) {
  const auto add_chord_note = [&](int index) {
    notes->push_back(
        Note{0.0, 1.0, root_note + GetPitch(scale, index), intensity});
  };
  add_chord_note(harmonic);
  add_chord_note(harmonic + 2);
  add_chord_note(harmonic + 4);
  add_chord_note(harmonic + 7);
}

void ComposeLine(float root_note, const std::vector<float>& scale,
                 float intensity, int bar, int beat, int num_beats,
                 int harmonic, std::vector<Note>* notes) {
  const int note_offset = beat;
  const auto add_note = [&](double begin_position, double end_position,
                            int index) {
    notes->push_back(Note{begin_position, end_position,
                          root_note + GetPitch(scale, index), intensity});
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

void ComposeDrums(int bar, int beat, int num_beats, std::vector<Note>* notes,
                  Random* random) {
  const auto get_beat = [](int step) {
    return barelyapi::GetPosition(step, barelyapi::kNumSixteenthNotesPerBeat);
  };
  // Kick.
  if (beat % 2 == 0) {
    notes->emplace_back(
        Note{get_beat(0), get_beat(2), barelyapi::kPitchKick, 1.0f});
    if (bar % 2 == 1 && beat == 0) {
      notes->emplace_back(
          Note{get_beat(2), get_beat(4), barelyapi::kPitchKick, 1.0f});
    }
  }
  // Snare.
  if (beat % 2 == 1) {
    notes->emplace_back(
        Note{get_beat(0), get_beat(2), barelyapi::kPitchSnare, 1.0f});
  }
  if (beat + 1 == num_beats) {
    notes->emplace_back(
        Note{get_beat(2), get_beat(4), barelyapi::kPitchSnare, 0.75f});
    if (bar % 4 == 3) {
      notes->emplace_back(
          Note{get_beat(1), get_beat(2), barelyapi::kPitchSnare, 1.0f});
      notes->emplace_back(
          Note{get_beat(3), get_beat(4), barelyapi::kPitchSnare, 0.75f});
    }
  }
  // Hihat Closed.
  notes->emplace_back(Note{get_beat(0), get_beat(2),
                           barelyapi::kPitchHihatClosed,
                           random->DrawUniform(0.5f, 0.75f)});
  notes->emplace_back(Note{get_beat(2), get_beat(4),
                           barelyapi::kPitchHihatClosed,
                           random->DrawUniform(0.25f, 0.75f)});
  // Hihat Open.
  if (beat + 1 == num_beats) {
    if (bar % 4 == 3) {
      notes->emplace_back(
          Note{get_beat(1), get_beat(2), barelyapi::kPitchHihatOpen, 0.5f});
    } else if (bar % 2 == 0) {
      notes->emplace_back(
          Note{get_beat(3), get_beat(4), barelyapi::kPitchHihatOpen, 0.5f});
    }
  }
  if (beat == 0 && bar % 4 == 0) {
    notes->emplace_back(
        Note{get_beat(0), get_beat(2), barelyapi::kPitchHihatOpen, 0.75f});
  }
}

}  // namespace

int main(int /*argc*/, char* argv[]) {
  std::string error;
  std::unique_ptr<Runfiles> runfiles(Runfiles::Create(argv[0], &error));
  CHECK(runfiles);

  AudioOutput audio_output;
  InputManager input_manager;

  Random random;

  AudioClock clock(kSampleRate);
  InstrumentManager instrument_manager(kSampleRate);
  Sequencer sequencer(&instrument_manager);
  sequencer.SetPlaybackTempo(kTempo);

  // Note on callback.
  const auto note_on_callback = [](Id performer_id, double /*timestamp*/,
                                   float pitch, float intensity) {
    LOG(INFO) << "Performer #" << performer_id << ": NoteOn(" << pitch << ", "
              << intensity << ")";
  };
  instrument_manager.SetNoteOnCallback(note_on_callback);

  // Note off callback.
  const auto note_off_callback = [](Id performer_id, double /*timestamp*/,
                                    float pitch) {
    LOG(INFO) << "Performer #" << performer_id << ": NoteOff(" << pitch << ")";
  };
  instrument_manager.SetNoteOffCallback(note_off_callback);

  const std::vector<int> progression = {0, 3, 4, 0};
  const std::vector<float> scale(std::cbegin(barelyapi::kPitchMajorScale),
                                 std::cend(barelyapi::kPitchMajorScale));

  // Initialize performers
  std::vector<BeatComposerCallback> performers;

  const auto build_synth_instrument_fn = [&](OscillatorType type, float gain,
                                             float attack, float release) {
    instrument_manager.Create(
        static_cast<Id>(performers.size()), 0.0,
        SynthInstrument::GetDefinition(),
        {{SynthInstrumentParam::kNumVoices,
          static_cast<float>(kNumInstrumentVoices)},
         {SynthInstrumentParam::kOscillatorType, static_cast<float>(type)},
         {SynthInstrumentParam::kGain, gain},
         {SynthInstrumentParam::kEnvelopeAttack, attack},
         {SynthInstrumentParam::kEnvelopeRelease, release}});
  };

  // Add synth instruments.
  const auto chords_beat_composer_callback =
      std::bind(ComposeChord, kRootNote, scale, 0.5f, std::placeholders::_4,
                std::placeholders::_5);

  build_synth_instrument_fn(OscillatorType::kSine, 0.1f, 0.125f, 0.125f);
  performers.push_back(chords_beat_composer_callback);

  build_synth_instrument_fn(OscillatorType::kNoise, 0.025f, 0.5f, 0.025f);
  performers.push_back(chords_beat_composer_callback);

  const auto line_beat_composer_callback = std::bind(
      ComposeLine, kRootNote - 1.0f, scale, 1.0f, std::placeholders::_1,
      std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
      std::placeholders::_5);
  build_synth_instrument_fn(OscillatorType::kSaw, 0.1f, 0.0025f, 0.125f);
  performers.push_back(line_beat_composer_callback);

  const auto line_2_beat_composer_callback =
      std::bind(ComposeLine, kRootNote, scale, 1.0f, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3,
                std::placeholders::_4, std::placeholders::_5);
  build_synth_instrument_fn(OscillatorType::kSquare, 0.125f, 0.05f, 0.05f);
  performers.push_back(line_2_beat_composer_callback);

  // Add drumkit instrument.
  const Id drumkit_instrument_id = static_cast<Id>(performers.size());
  instrument_manager.Create(drumkit_instrument_id, 0.0,
                            DrumkitInstrument::GetDefinition(), {});
  std::unordered_map<float, std::string> drumkit_map = {
      {barelyapi::kPitchKick, "basic_kick.wav"},
      {barelyapi::kPitchSnare, "basic_snare.wav"},
      {barelyapi::kPitchHihatClosed, "basic_hihat_closed.wav"},
      {barelyapi::kPitchHihatOpen, "basic_hihat_open.wav"}};
  std::unordered_map<float, WavFile> drumkit_files;
  for (const auto& [index, name] : drumkit_map) {
    auto it = drumkit_files.emplace(index, WavFile{});
    const std::string path = runfiles->Rlocation(kDrumsBaseFilename + name);
    CHECK(it.first->second.Load(path)) << path;
  }
  instrument_manager.SetCustomData(drumkit_instrument_id, 0.0,
                                   std::any(&drumkit_files));
  const auto drumkit_beat_composer_callback =
      std::bind(ComposeDrums, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3, std::placeholders::_5, &random);
  performers.push_back(drumkit_beat_composer_callback);

  // Bar callback.
  const auto bar_composer_callback = [&progression](int bar) -> int {
    return progression[bar % progression.size()];
  };

  // Beat callback.
  int harmonic = 0;
  std::vector<Note> temp_notes;
  const auto beat_callback = [&](int beat) {
    // Update transport.
    const int current_bar = beat / kNumBeats;
    const int current_beat = beat % kNumBeats;

    if (current_beat == 0) {
      // Compose next bar.
      harmonic = bar_composer_callback(current_bar);
    }
    // Update members.
    for (int i = 0; i < static_cast<int>(performers.size()); ++i) {
      // Compose next beat notes.
      temp_notes.clear();
      if (performers[i]) {
        performers[i](current_bar, current_beat, kNumBeats, harmonic,
                      &temp_notes);
      }
      for (const Note& note : temp_notes) {
        const double begin_position =
            static_cast<double>(beat) + note.begin_position;
        const double end_position =
            static_cast<double>(beat) + note.end_position;
        sequencer.ScheduleInstrumentNote(static_cast<Id>(i), begin_position,
                                         end_position, note.pitch,
                                         note.intensity);
      }
    }
  };
  sequencer.SetBeatCallback(beat_callback);

  // Audio process callback.
  std::vector<float> temp_buffer(kNumChannels * kNumFrames);
  const auto process_callback = [&](float* output) {
    std::fill_n(output, kNumChannels * kNumFrames, 0.0f);
    for (int i = 0; i < static_cast<int>(performers.size()); ++i) {
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
  const auto key_down_callback = [&](const InputManager::Key& key) {
    if (static_cast<int>(key) == 27) {
      // ESC pressed, quit the app.
      quit = true;
      return;
    }
    switch (std::toupper(key)) {
      case ' ':
        if (sequencer.IsPlaying()) {
          sequencer.StopPlayback();
          LOG(INFO) << "Stopped playback";
        } else {
          sequencer.StartPlayback();
          LOG(INFO) << "Started playback";
        }
        break;
      case '1':
        sequencer.SetPlaybackTempo(random.DrawUniform(0.5, 0.75) *
                                   sequencer.GetPlaybackTempo());
        LOG(INFO) << "Tempo changed to " << sequencer.GetPlaybackTempo();
        break;
      case '2':
        sequencer.SetPlaybackTempo(random.DrawUniform(1.5, 2.0) *
                                   sequencer.GetPlaybackTempo());
        LOG(INFO) << "Tempo changed to " << sequencer.GetPlaybackTempo();
        break;
      case 'R':
        sequencer.SetPlaybackTempo(kTempo);
        LOG(INFO) << "Tempo reset to " << kTempo;
        break;
    }
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  LOG(INFO) << "Starting audio stream";
  audio_output.Start(kSampleRate, kNumChannels, kNumFrames);
  sequencer.StartPlayback();

  while (!quit) {
    input_manager.Update();
    instrument_manager.Update();
    sequencer.Update(clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  LOG(INFO) << "Stopping audio stream";
  sequencer.StopPlayback();
  audio_output.Stop();

  return 0;
}
