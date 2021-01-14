#include <algorithm>
#include <atomic>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <memory>
#include <thread>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/base/random.h"
#include "barelymusician/dsp/dsp_utils.h"
#include "barelymusician/engine/engine.h"
#include "barelymusician/engine/note.h"
#include "barelymusician/engine/note_utils.h"
#include "examples/instruments/basic_drumkit_instrument.h"
#include "examples/instruments/basic_synth_instrument.h"
#include "examples/util/audio_output.h"
#include "examples/util/input_manager.h"
#include "examples/util/wav_file.h"
#include "tools/cpp/runfiles/runfiles.h"

namespace {

using ::barelyapi::Engine;
using ::barelyapi::GetPitch;
using ::barelyapi::Note;
using ::barelyapi::OscillatorType;
using ::barelyapi::examples::AudioOutput;
using ::barelyapi::examples::BasicDrumkitInstrument;
using ::barelyapi::examples::BasicSynthInstrument;
using ::barelyapi::examples::BasicSynthInstrumentParam;
using ::barelyapi::examples::InputManager;
using ::barelyapi::examples::WavFile;
using ::barelyapi::random::Uniform;
using ::bazel::tools::cpp::runfiles::Runfiles;

// Beat composer callback signature.
using BeatComposerCallback = std::function<void(
    int bar, int beat, int num_beats, int harmonic, std::vector<Note>* notes)>;

// System audio settings.
constexpr int kSampleRate = 48000;
constexpr int kNumChannels = 2;
constexpr int kNumFrames = 1024;

constexpr std::int64_t kLookahead = 4 * kNumFrames;

// Sequencer settings.
constexpr double kTempo = 124.0;
constexpr int kNumBeats = 3;

// Ensemble settings.
constexpr float kRootNote = barelyapi::kPitchD3;
constexpr int kNumInstrumentVoices = 8;

constexpr char kDrumsBaseFilename[] =
    "barelymusician/examples/data/audio/drums/";

int BuildSynthInstrument(Engine* engine, OscillatorType type, float gain,
                         float attack, float release) {
  return engine->CreateInstrument(
      BasicSynthInstrument::GetDefinition(kSampleRate),
      {{BasicSynthInstrumentParam::kNumVoices,
        static_cast<float>(kNumInstrumentVoices)},
       {BasicSynthInstrumentParam::kOscillatorType, static_cast<float>(type)},
       {BasicSynthInstrumentParam::kGain, gain},
       {BasicSynthInstrumentParam::kEnvelopeAttack, attack},
       {BasicSynthInstrumentParam::kEnvelopeRelease, release}});
}

void ComposeChord(float root_note_index, const std::vector<float>& scale,
                  float intensity, int harmonic, std::vector<Note>* notes) {
  const auto add_chord_note = [&](int index) {
    notes->push_back(Note{
        0.0, 1.0, root_note_index + GetPitch(scale, static_cast<float>(index)),
        intensity});
  };
  add_chord_note(harmonic);
  add_chord_note(harmonic + 2);
  add_chord_note(harmonic + 4);
  add_chord_note(harmonic + 7);
}

void ComposeLine(float root_note_index, const std::vector<float>& scale,
                 float intensity, int bar, int beat, int num_beats,
                 int harmonic, std::vector<Note>* notes) {
  const int note_offset = beat;
  const auto add_note = [&](double position, double duration, int index) {
    notes->push_back(
        Note{position, duration,
             root_note_index + GetPitch(scale, static_cast<float>(index)),
             intensity});
  };
  if (beat % 2 == 1) {
    add_note(0.0, 0.25, harmonic);
    add_note(0.33, 0.33, harmonic - note_offset);
    add_note(0.66, 0.33, harmonic);
  } else {
    add_note(0.0, 0.25, harmonic + note_offset);
  }
  if (beat % 2 == 0) {
    add_note(0.0, 0.05, harmonic - note_offset);
    add_note(0.5, 0.05, harmonic - 2 * note_offset);
  }
  if (beat + 1 == num_beats && bar % 2 == 1) {
    add_note(0.25, 0.125, harmonic + 2 * note_offset);
    add_note(0.75, 0.125, harmonic - 2 * note_offset);
    add_note(0.5, 0.25, harmonic + 2 * note_offset);
  }
}

void ComposeDrums(int bar, int beat, int num_beats, std::vector<Note>* notes) {
  const auto get_beat = [](int step) {
    return barelyapi::GetPosition(step, barelyapi::kNumSixteenthNotesPerBeat);
  };
  // Kick.
  if (beat % 2 == 0) {
    notes->emplace_back(
        Note{get_beat(0), get_beat(2), barelyapi::kPitchKick, 1.0f});
    if (bar % 2 == 1 && beat == 0) {
      notes->emplace_back(
          Note{get_beat(2), get_beat(2), barelyapi::kPitchKick, 1.0f});
    }
  }
  // Snare.
  if (beat % 2 == 1) {
    notes->emplace_back(
        Note{get_beat(0), get_beat(2), barelyapi::kPitchSnare, 1.0f});
  }
  if (beat + 1 == num_beats) {
    notes->emplace_back(
        Note{get_beat(2), get_beat(2), barelyapi::kPitchSnare, 0.75f});
    if (bar % 4 == 3) {
      notes->emplace_back(
          Note{get_beat(1), get_beat(1), barelyapi::kPitchSnare, 1.0f});
      notes->emplace_back(
          Note{get_beat(3), get_beat(1), barelyapi::kPitchSnare, 0.75f});
    }
  }
  // Hihat Closed.
  notes->emplace_back(Note{get_beat(0), get_beat(2),
                           barelyapi::kPitchHihatClosed, Uniform(0.5f, 0.75f)});
  notes->emplace_back(Note{get_beat(2), get_beat(2),
                           barelyapi::kPitchHihatClosed,
                           Uniform(0.25f, 0.75f)});
  // Hihat Open.
  if (beat + 1 == num_beats) {
    if (bar % 4 == 3) {
      notes->emplace_back(
          Note{get_beat(1), get_beat(1), barelyapi::kPitchHihatOpen, 0.5f});
    } else if (bar % 2 == 0) {
      notes->emplace_back(
          Note{get_beat(3), get_beat(1), barelyapi::kPitchHihatOpen, 0.5f});
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

  Engine engine;
  engine.SetTempo(kTempo);

  const std::vector<int> progression = {0, 3, 4, 0};
  const std::vector<float> scale(std::cbegin(barelyapi::kMajorScale),
                                 std::cend(barelyapi::kMajorScale));

  const auto bar_composer_callback = [&progression](int bar) -> int {
    return progression[bar % progression.size()];
  };

  std::unordered_map<int, BeatComposerCallback> performers;

  // Beat callback.
  int harmonic = 0;
  std::vector<Note> temp_notes;
  const auto beat_callback = [&](std::int64_t, int beat) {
    // Update transport.
    const int current_bar = beat / kNumBeats;
    const int current_beat = beat % kNumBeats;

    if (current_beat == 0) {
      // Compose next bar.
      harmonic = bar_composer_callback(current_bar);
    }
    // Update members.
    for (auto& [id, callback] : performers) {
      // Compose next beat notes.
      temp_notes.clear();
      if (callback) {
        callback(current_bar, current_beat, kNumBeats, harmonic, &temp_notes);
      }
      for (const Note& note : temp_notes) {
        const double position = static_cast<double>(beat) + note.position;
        engine.ScheduleNote(id, position, note.duration, note.pitch,
                            note.intensity);
      }
    }
  };
  engine.SetBeatCallback(beat_callback);

  // Note on callback.
  const auto note_on_callback = [](int performer_id, std::int64_t, float pitch,
                                   float intensity) {
    LOG(INFO) << "Performer #" << performer_id << ": NoteOn(" << pitch << ", "
              << intensity << ")";
  };
  engine.SetNoteOnCallback(note_on_callback);

  // Note off callback.
  const auto note_off_callback = [](int performer_id, std::int64_t,
                                    float pitch) {
    LOG(INFO) << "Performer #" << performer_id << ": NoteOff(" << pitch << ")";
  };
  engine.SetNoteOffCallback(note_off_callback);

  // Add synth instruments.
  auto chords_instrument_id = BuildSynthInstrument(
      &engine, OscillatorType::kSine, 0.1f, 0.125f, 0.125f);
  auto chords_2_instrument_id = BuildSynthInstrument(
      &engine, OscillatorType::kNoise, 0.025f, 0.5f, 0.025f);

  const auto chords_beat_composer_callback =
      std::bind(ComposeChord, kRootNote, scale, 0.5f, std::placeholders::_4,
                std::placeholders::_5);

  performers.emplace(chords_instrument_id, chords_beat_composer_callback);
  performers.emplace(chords_2_instrument_id, chords_beat_composer_callback);

  auto line_instrument_id = BuildSynthInstrument(&engine, OscillatorType::kSaw,
                                                 0.1f, 0.0025f, 0.125f);
  auto line_2_instrument_id = BuildSynthInstrument(
      &engine, OscillatorType::kSquare, 0.125f, 0.05f, 0.05f);

  const auto line_beat_composer_callback = std::bind(
      ComposeLine, kRootNote - barelyapi::kNumSemitones, scale, 1.0f,
      std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
      std::placeholders::_4, std::placeholders::_5);
  const auto line_2_beat_composer_callback =
      std::bind(ComposeLine, kRootNote, scale, 1.0f, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3,
                std::placeholders::_4, std::placeholders::_5);

  performers.emplace(line_instrument_id, line_beat_composer_callback);
  performers.emplace(line_2_instrument_id, line_2_beat_composer_callback);

  // Add drumkit instrument.
  const auto drumkit_instrument_id = engine.CreateInstrument(
      BasicDrumkitInstrument::GetDefinition(kSampleRate));
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
  engine.SetCustomData(drumkit_instrument_id,
                       reinterpret_cast<void*>(&drumkit_files));
  const auto drumkit_beat_composer_callback =
      std::bind(ComposeDrums, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3, std::placeholders::_5);
  performers.emplace(drumkit_instrument_id, drumkit_beat_composer_callback);

  // Audio process callback.
  std::vector<float> temp_buffer(kNumChannels * kNumFrames);
  std::atomic<std::int64_t> timestamp = 0;
  const auto process_callback = [&](float* output) {
    std::fill_n(output, kNumChannels * kNumFrames, 0.0f);
    for (const auto& [id, callback] : performers) {
      engine.Process(id, timestamp, temp_buffer.data(), kNumChannels,
                     kNumFrames);
      std::transform(temp_buffer.cbegin(), temp_buffer.cend(), output, output,
                     std::plus<float>());
    }
    timestamp += kNumFrames;
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
        if (engine.IsPlaying()) {
          engine.Stop();
          LOG(INFO) << "Stopped playback";
        } else {
          engine.Start(timestamp + kLookahead);
          LOG(INFO) << "Started playback";
        }
        break;
      case '1':
        engine.SetTempo(Uniform(0.5, 0.75) * engine.GetTempo());
        LOG(INFO) << "Tempo changed to " << engine.GetTempo();
        break;
      case '2':
        engine.SetTempo(Uniform(1.5, 2.0) * engine.GetTempo());
        LOG(INFO) << "Tempo changed to " << engine.GetTempo();
        break;
      case 'R':
        engine.SetTempo(kTempo);
        LOG(INFO) << "Tempo reset to " << kTempo;
        break;
    }
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  LOG(INFO) << "Starting audio stream";
  audio_output.Start(kSampleRate, kNumChannels, kNumFrames);
  engine.Start(timestamp + kLookahead);

  while (!quit) {
    input_manager.Update();
    engine.Update(kSampleRate, timestamp + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  LOG(INFO) << "Stopping audio stream";
  engine.Stop();
  audio_output.Stop();

  return 0;
}
