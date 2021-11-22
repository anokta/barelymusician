#include <algorithm>
#include <any>
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
#include "barelymusician/common/status.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/composition/note_duration.h"
#include "barelymusician/composition/note_pitch.h"
#include "barelymusician/composition/note_sequence.h"
#include "barelymusician/composition/sequencer.h"
#include "barelymusician/engine/instrument_manager.h"
#include "barelymusician/engine/transport.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/input_manager.h"
#include "examples/common/wav_file.h"
#include "examples/instruments/drumkit_instrument.h"
#include "examples/instruments/synth_instrument.h"
#include "tools/cpp/runfiles/runfiles.h"

namespace {

using ::barelyapi::GetPitch;
using ::barelyapi::GetStatusOrValue;
using ::barelyapi::Id;
using ::barelyapi::InstrumentManager;
using ::barelyapi::Note;
using ::barelyapi::NoteSequence;
using ::barelyapi::OscillatorType;
using ::barelyapi::Random;
using ::barelyapi::Sequencer;
using ::barelyapi::Transport;
using ::barelyapi::examples::AudioClock;
using ::barelyapi::examples::AudioOutput;
using ::barelyapi::examples::DrumkitInstrument;
using ::barelyapi::examples::InputManager;
using ::barelyapi::examples::SynthInstrument;
using ::barelyapi::examples::SynthInstrumentParam;
using ::barelyapi::examples::WavFile;
using ::bazel::tools::cpp::runfiles::Runfiles;

// Beat composer callback signature.
//
// @param bar Current bar.
// @param beat Current beat.
// @param num_beats Number of beats in a bar.
// @param harmonic Harmonic index.
// @param notes Pointer to mutable notes.
using BeatComposerCallback =
    std::function<void(int bar, int beat, int num_beats, int harmonic,
                       double offset, NoteSequence* notes)>;

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

Id note_id_count = 0;

void ComposeChord(float root_note, const std::vector<float>& scale,
                  float intensity, int harmonic, double offset,
                  NoteSequence* sequence) {
  const auto add_chord_note = [&](int index) {
    sequence->Add(++note_id_count, offset,
                  Note{root_note + GetPitch(scale, index), intensity, 1.0});
  };
  add_chord_note(harmonic);
  add_chord_note(harmonic + 2);
  add_chord_note(harmonic + 4);
  add_chord_note(harmonic + 7);
}

void ComposeLine(float root_note, const std::vector<float>& scale,
                 float intensity, int bar, int beat, int num_beats,
                 int harmonic, double offset, NoteSequence* sequence) {
  const int note_offset = beat;
  const auto add_note = [&](double begin_position, double end_position,
                            int index) {
    sequence->Add(++note_id_count, begin_position + offset,
                  Note{root_note + GetPitch(scale, index), intensity,
                       end_position - begin_position});
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
                  double offset, NoteSequence* sequence) {
  const auto get_beat = [](int step) {
    return barelyapi::GetPosition(step, barelyapi::kNumSixteenthNotesPerBeat);
  };
  const auto add_note = [&](double begin_position, double end_position,
                            float pitch, float intensity) {
    sequence->Add(++note_id_count, begin_position + offset,
                  Note{pitch, intensity, end_position - begin_position});
  };

  // Kick.
  if (beat % 2 == 0) {
    add_note(get_beat(0), get_beat(2), barelyapi::kPitchKick, 1.0f);
    if (bar % 2 == 1 && beat == 0) {
      add_note(get_beat(2), get_beat(4), barelyapi::kPitchKick, 1.0f);
    }
  }
  // Snare.
  if (beat % 2 == 1) {
    add_note(get_beat(0), get_beat(2), barelyapi::kPitchSnare, 1.0f);
  }
  if (beat + 1 == num_beats) {
    add_note(get_beat(2), get_beat(4), barelyapi::kPitchSnare, 0.75f);
    if (bar % 4 == 3) {
      add_note(get_beat(1), get_beat(2), barelyapi::kPitchSnare, 1.0f);
      add_note(get_beat(3), get_beat(4), barelyapi::kPitchSnare, 0.75f);
    }
  }
  // Hihat Closed.
  add_note(get_beat(0), get_beat(2), barelyapi::kPitchHihatClosed,
           random->DrawUniform(0.5f, 0.75f));
  add_note(get_beat(2), get_beat(4), barelyapi::kPitchHihatClosed,
           random->DrawUniform(0.25f, 0.75f));
  // Hihat Open.
  if (beat + 1 == num_beats) {
    if (bar % 4 == 3) {
      add_note(get_beat(1), get_beat(2), barelyapi::kPitchHihatOpen, 0.5f);
    } else if (bar % 2 == 0) {
      add_note(get_beat(3), get_beat(4), barelyapi::kPitchHihatOpen, 0.5f);
    }
  }
  if (beat == 0 && bar % 4 == 0) {
    add_note(get_beat(0), get_beat(2), barelyapi::kPitchHihatOpen, 0.75f);
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

  Transport transport;
  transport.SetTempo(kTempo);

  Sequencer sequencer;

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

  Id num_instruments = 0;
  const auto build_synth_instrument_fn = [&](OscillatorType type, float gain,
                                             float attack, float release) {
    instrument_manager.Create(
        ++num_instruments, 0.0, SynthInstrument::GetDefinition(),
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
                std::placeholders::_5, std::placeholders::_6);

  build_synth_instrument_fn(OscillatorType::kSine, 0.1f, 0.125f, 0.125f);
  sequencer.CreateSequence(static_cast<Id>(performers.size()));
  sequencer.AddInstrument(static_cast<Id>(performers.size()), num_instruments);

  build_synth_instrument_fn(OscillatorType::kNoise, 0.025f, 0.5f, 0.025f);
  sequencer.AddInstrument(static_cast<Id>(performers.size()), num_instruments);

  performers.push_back(chords_beat_composer_callback);

  const auto line_beat_composer_callback = std::bind(
      ComposeLine, kRootNote - 1.0f, scale, 1.0f, std::placeholders::_1,
      std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
      std::placeholders::_5, std::placeholders::_6);
  build_synth_instrument_fn(OscillatorType::kSaw, 0.1f, 0.0025f, 0.125f);
  sequencer.CreateSequence(static_cast<Id>(performers.size()));
  sequencer.AddInstrument(static_cast<Id>(performers.size()), num_instruments);
  performers.push_back(line_beat_composer_callback);

  const auto line_2_beat_composer_callback = std::bind(
      ComposeLine, kRootNote, scale, 1.0f, std::placeholders::_1,
      std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
      std::placeholders::_5, std::placeholders::_6);
  build_synth_instrument_fn(OscillatorType::kSquare, 0.125f, 0.05f, 0.05f);
  sequencer.CreateSequence(static_cast<Id>(performers.size()));
  sequencer.AddInstrument(static_cast<Id>(performers.size()), num_instruments);
  performers.push_back(line_2_beat_composer_callback);

  // Add drumkit instrument.
  instrument_manager.Create(++num_instruments, 0.0,
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
  instrument_manager.SetCustomData(num_instruments, 0.0,
                                   std::any(&drumkit_files));
  const auto drumkit_beat_composer_callback =
      std::bind(ComposeDrums, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3, &random, std::placeholders::_5,
                std::placeholders::_6);
  sequencer.CreateSequence(static_cast<Id>(performers.size()));
  sequencer.AddInstrument(static_cast<Id>(performers.size()), num_instruments);
  performers.push_back(drumkit_beat_composer_callback);

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
    for (int i = 0; i < static_cast<int>(performers.size()); ++i) {
      // Compose next beat notes.
      if (performers[i]) {
        performers[i](
            current_bar, current_beat, kNumBeats, harmonic, beat,
            GetStatusOrValue(sequencer.GetSequence(static_cast<Id>(i))));
      }
    }
  };
  transport.SetBeatCallback(beat_callback);

  // Transport update callback.
  const auto update_callback =
      [&](double begin_position, double end_position,
          const Transport::GetTimestampFn& get_timestamp_fn) {
        instrument_manager.ProcessEvents(
            sequencer.Process(begin_position, end_position, get_timestamp_fn));
      };
  transport.SetUpdateCallback(update_callback);

  // Audio process callback.
  std::vector<float> temp_buffer(kNumChannels * kNumFrames);
  const auto process_callback = [&](float* output) {
    std::fill_n(output, kNumChannels * kNumFrames, 0.0f);
    for (Id id = 1; id <= num_instruments; ++id) {
      instrument_manager.Process(id, clock.GetTimestamp(), temp_buffer.data(),
                                 kNumChannels, kNumFrames);
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
        if (transport.IsPlaying()) {
          transport.Stop();
          sequencer.Stop();
          instrument_manager.SetAllNotesOff(transport.GetTimestamp());
          LOG(INFO) << "Stopped playback";
        } else {
          transport.Start();
          LOG(INFO) << "Started playback";
        }
        break;
      case '1':
        transport.SetTempo(random.DrawUniform(0.5, 0.75) *
                           transport.GetTempo());
        LOG(INFO) << "Tempo changed to " << transport.GetTempo();
        break;
      case '2':
        transport.SetTempo(random.DrawUniform(1.5, 2.0) * transport.GetTempo());
        LOG(INFO) << "Tempo changed to " << transport.GetTempo();
        break;
      case 'R':
        transport.SetTempo(kTempo);
        LOG(INFO) << "Tempo reset to " << kTempo;
        break;
    }
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  LOG(INFO) << "Starting audio stream";
  audio_output.Start(kSampleRate, kNumChannels, kNumFrames);
  transport.Start();

  while (!quit) {
    input_manager.Update();
    transport.Update(clock.GetTimestamp() + kLookahead);
    instrument_manager.Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  LOG(INFO) << "Stopping audio stream";
  transport.Stop();
  audio_output.Stop();

  return 0;
}
