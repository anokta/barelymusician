#include <algorithm>
#include <atomic>
#include <cctype>
#include <chrono>
#include <memory>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/engine/engine.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/engine/note.h"
#include "barelymusician/engine/note_utils.h"
#include "barelymusician/util/random.h"
#include "examples/audio_output/pa_audio_output.h"
#include "examples/input_manager/win_console_input.h"
#include "examples/instruments/basic_drumkit_instrument.h"
#include "examples/instruments/basic_synth_instrument.h"
#include "examples/util/wav_file.h"

namespace {

using ::barelyapi::Engine;
using ::barelyapi::Id;
using ::barelyapi::Instrument;
using ::barelyapi::Note;
using ::barelyapi::NoteIndex;
using ::barelyapi::OscillatorType;
using ::barelyapi::Position;
using ::barelyapi::examples::BasicDrumkitInstrument;
using ::barelyapi::examples::BasicSynthInstrument;
using ::barelyapi::examples::BasicSynthInstrumentParam;
using ::barelyapi::examples::PaAudioOutput;
using ::barelyapi::examples::WavFile;
using ::barelyapi::examples::WinConsoleInput;
using ::barelyapi::random::Uniform;

// Beat composer callback signature.
using BeatComposerCallback = std::function<void(
    int bar, int beat, int num_beats, int harmonic, std::vector<Note>* notes)>;

// System audio settings.
const int kSampleRate = 48000;
const int kNumChannels = 2;
const int kNumFrames = 1024;

const double kLookahead = 0.05;

// Sequencer settings.
const double kTempo = 124.0;
const int kNumBeats = 3;

// Ensemble settings.
const float kRootNote = barelyapi::kNoteIndexD3;
const int kNumInstrumentVoices = 8;

std::unique_ptr<Instrument> BuildSynthInstrument(OscillatorType type,
                                                 float gain, float attack,
                                                 float release) {
  auto synth_instrument = std::make_unique<BasicSynthInstrument>(kSampleRate);
  synth_instrument->Control(BasicSynthInstrumentParam::kNumVoices,
                            static_cast<float>(kNumInstrumentVoices));
  synth_instrument->Control(BasicSynthInstrumentParam::kOscillatorType,
                            static_cast<float>(type));
  synth_instrument->Control(BasicSynthInstrumentParam::kGain, gain);
  synth_instrument->Control(BasicSynthInstrumentParam::kEnvelopeAttack, attack);
  synth_instrument->Control(BasicSynthInstrumentParam::kEnvelopeRelease,
                            release);
  return std::move(synth_instrument);
}

void ComposeChord(float root_note_index, const std::vector<float>& scale,
                  float intensity, int harmonic, std::vector<Note>* notes) {
  const auto add_chord_note = [&](int index) {
    notes->emplace_back(
        Note{0.0, 1.0, barelyapi::QuantizedNoteIndex{root_note_index, index},
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
    notes->emplace_back(
        Note{position, duration,
             barelyapi::QuantizedNoteIndex{root_note_index, index}, intensity});
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
    return barelyapi::QuantizedPosition{step,
                                        barelyapi::kNumSixteenthNotesPerBeat};
  };
  // Kick.
  if (beat % 2 == 0) {
    notes->emplace_back(
        Note{get_beat(0), get_beat(2), barelyapi::kNoteIndexKick, 1.0f});
    if (bar % 2 == 1 && beat == 0) {
      notes->emplace_back(
          Note{get_beat(2), get_beat(2), barelyapi::kNoteIndexKick, 1.0f});
    }
  }
  // Snare.
  if (beat % 2 == 1) {
    notes->emplace_back(
        Note{get_beat(0), get_beat(2), barelyapi::kNoteIndexSnare, 1.0f});
  }
  if (beat + 1 == num_beats) {
    notes->emplace_back(
        Note{get_beat(2), get_beat(2), barelyapi::kNoteIndexSnare, 0.75f});
    if (bar % 4 == 3) {
      notes->emplace_back(
          Note{get_beat(1), get_beat(1), barelyapi::kNoteIndexSnare, 1.0f});
      notes->emplace_back(
          Note{get_beat(3), get_beat(1), barelyapi::kNoteIndexSnare, 0.75f});
    }
  }
  // Hihat Closed.
  notes->emplace_back(Note{get_beat(0), get_beat(2),
                           barelyapi::kNoteIndexHihatClosed,
                           Uniform(0.5f, 0.75f)});
  notes->emplace_back(Note{get_beat(2), get_beat(2),
                           barelyapi::kNoteIndexHihatClosed,
                           Uniform(0.25f, 0.75f)});
  // Hihat Open.
  if (beat + 1 == num_beats) {
    if (bar % 4 == 3) {
      notes->emplace_back(
          Note{get_beat(1), get_beat(1), barelyapi::kNoteIndexHihatOpen, 0.5f});
    } else if (bar % 2 == 0) {
      notes->emplace_back(
          Note{get_beat(3), get_beat(1), barelyapi::kNoteIndexHihatOpen, 0.5f});
    }
  }
  if (beat == 0 && bar % 4 == 0) {
    notes->emplace_back(
        Note{get_beat(0), get_beat(2), barelyapi::kNoteIndexHihatOpen, 0.75f});
  }
}

}  // namespace

int main(int argc, char* argv[]) {
  PaAudioOutput audio_output;
  WinConsoleInput input_manager;

  Engine engine;
  engine.SetTempo(kTempo);

  const std::vector<int> progression = {0, 3, 4, 0};
  const std::vector<float> scale(std::cbegin(barelyapi::kMajorScale),
                                 std::cend(barelyapi::kMajorScale));

  const auto bar_composer_callback = [&progression](int bar) -> int {
    return progression[bar % progression.size()];
  };

  std::unordered_map<Id, BeatComposerCallback> performers;
  const auto create_performer_fn =
      [&](std::unique_ptr<Instrument> instrument,
          BeatComposerCallback beat_composer_callback) {
        const Id id = GetValue(engine.Create(std::move(instrument), {}));
        performers.emplace(id, std::move(beat_composer_callback));
      };

  // Beat callback.
  int harmonic = 0;
  std::vector<Note> temp_notes;
  const auto beat_callback = [&](double, int beat) {
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
        const double position = barelyapi::GetRawPosition(note.position) +
                                static_cast<double>(beat);
        const double duration = barelyapi::GetRawPosition(note.duration);
        const float index = barelyapi::GetRawNoteIndex(scale, note.index);
        engine.ScheduleNote(id, position, duration, index, note.intensity);
      }
    }
  };
  engine.SetBeatCallback(beat_callback);

  // Note on callback.
  const auto note_on_callback = [](double, Id performer_id, float index,
                                   float intensity) {
    LOG(INFO) << "Performer #" << performer_id << ": NoteOn(" << index << ", "
              << intensity << ")";
  };
  engine.SetNoteOnCallback(note_on_callback);

  // Note off callback.
  const auto note_off_callback = [](double, Id performer_id, float index) {
    LOG(INFO) << "Performer #" << performer_id << ": NoteOff(" << index << ")";
  };
  engine.SetNoteOffCallback(note_off_callback);

  // Add synth instruments.
  auto chords_instrument =
      BuildSynthInstrument(OscillatorType::kSine, 0.1f, 0.125f, 0.125f);
  auto chords_2_instrument =
      BuildSynthInstrument(OscillatorType::kNoise, 0.025f, 0.5f, 0.025f);

  const auto chords_beat_composer_callback =
      std::bind(ComposeChord, kRootNote, scale, 0.5f, std::placeholders::_4,
                std::placeholders::_5);

  create_performer_fn(std::move(chords_instrument),
                      chords_beat_composer_callback);
  create_performer_fn(std::move(chords_2_instrument),
                      chords_beat_composer_callback);

  auto line_instrument =
      BuildSynthInstrument(OscillatorType::kSaw, 0.1f, 0.0025f, 0.125f);
  auto line_2_instrument =
      BuildSynthInstrument(OscillatorType::kSquare, 0.125f, 0.05f, 0.05f);

  const auto line_beat_composer_callback = std::bind(
      ComposeLine, kRootNote - barelyapi::kNumSemitones, scale, 1.0f,
      std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
      std::placeholders::_4, std::placeholders::_5);
  const auto line_2_beat_composer_callback =
      std::bind(ComposeLine, kRootNote, scale, 1.0f, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3,
                std::placeholders::_4, std::placeholders::_5);

  create_performer_fn(std::move(line_instrument), line_beat_composer_callback);
  create_performer_fn(std::move(line_2_instrument),
                      line_2_beat_composer_callback);

  // Add drumkit instrument.
  std::unordered_map<float, std::string> drumkit_map;
  drumkit_map[barelyapi::kNoteIndexKick] = "data/audio/drums/basic_kick.wav";
  drumkit_map[barelyapi::kNoteIndexSnare] = "data/audio/drums/basic_snare.wav";
  drumkit_map[barelyapi::kNoteIndexHihatClosed] =
      "data/audio/drums/basic_hihat_closed.wav";
  drumkit_map[barelyapi::kNoteIndexHihatOpen] =
      "data/audio/drums/basic_hihat_open.wav";
  auto drumkit_instrument =
      std::make_unique<BasicDrumkitInstrument>(kSampleRate);
  std::vector<WavFile> drumkit_files;
  for (const auto& it : drumkit_map) {
    drumkit_files.emplace_back();
    auto& drumkit_file = drumkit_files.back();
    CHECK(drumkit_file.Load(it.second));
    drumkit_instrument->Add(it.first, drumkit_file);
  }

  const auto drumkit_beat_composer_callback =
      std::bind(ComposeDrums, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3, std::placeholders::_5);

  create_performer_fn(std::move(drumkit_instrument),
                      drumkit_beat_composer_callback);

  // Audio process callback.
  std::vector<float> temp_buffer(kNumChannels * kNumFrames);
  std::atomic<double> timestamp = 0.0;
  const auto process_callback = [&](float* output) {
    const double end_timestamp =
        timestamp +
        static_cast<double>(kNumFrames) / static_cast<double>(kSampleRate);
    std::fill_n(output, kNumChannels * kNumFrames, 0.0f);
    for (const auto& [id, callback] : performers) {
      engine.Process(id, timestamp, end_timestamp, temp_buffer.data(),
                     kNumChannels, kNumFrames);
      std::transform(temp_buffer.cbegin(), temp_buffer.cend(), output, output,
                     std::plus<float>());
    }
    timestamp = end_timestamp;
  };
  audio_output.SetProcessCallback(process_callback);

  // Key down callback.
  bool quit = false;
  const auto key_down_callback = [&](const WinConsoleInput::Key& key) {
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

  input_manager.Initialize();
  audio_output.Start(kSampleRate, kNumChannels, kNumFrames);

  engine.Start(timestamp + kLookahead);

  while (!quit) {
    input_manager.Update();
    engine.Update(timestamp + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  LOG(INFO) << "Stopping audio stream";

  engine.Stop();

  audio_output.Stop();
  input_manager.Shutdown();

  return 0;
}
