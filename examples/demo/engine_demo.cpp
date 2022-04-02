#include <algorithm>
#include <cassert>
#include <cctype>
#include <chrono>
#include <functional>
#include <memory>
#include <thread>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/random.h"
#include "barelymusician/instruments/percussion_instrument.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"
#include "examples/common/wav_file.h"
#include "examples/composition/note_duration.h"
#include "examples/composition/note_pitch.h"
#include "tools/cpp/runfiles/runfiles.h"

namespace {

using ::barely::Instrument;
using ::barely::InstrumentType;
using ::barely::Musician;
using ::barely::NoteDefinition;
using ::barely::NotePitchDefinition;
using ::barely::OscillatorType;
using ::barely::Sequence;
using ::barely::SynthParameter;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;
using ::barely::examples::WavFile;
using ::barelyapi::GetPitch;
using ::barelyapi::PercussionPad;
using ::barelyapi::PercussionPadMap;
using ::barelyapi::Random;
using ::bazel::tools::cpp::runfiles::Runfiles;

// Beat composer callback signature.
//
// @param bar Current bar.
// @param beat Current beat.
// @param num_beats Number of beats in a bar.
// @param harmonic Harmonic index.
// @param offset Position offset in beats.
// @param engine Pointer to engine.
// @param performer_id Sequence id.
using BeatComposerCallback =
    std::function<void(int bar, int beat, int num_beats, int harmonic,
                       double offset, Sequence* sequence)>;

// System audio settings.
constexpr int kFrameRate = 48000;
constexpr int kNumChannels = 2;
constexpr int kNumFrames = 1024;

constexpr double kLookahead = 0.1;

// Sequencer settings.
constexpr double kTempo = 124.0;
constexpr int kNumBeats = 3;

// Ensemble settings.
constexpr double kRootNote = barelyapi::kPitchD3;

constexpr char kDrumsBaseFilename[] =
    "barelymusician/examples/data/audio/drums/";

void ComposeChord(const std::vector<double>& scale, double intensity,
                  int harmonic, double offset, Sequence* sequence) {
  const auto add_chord_note = [&](int index) {
    sequence->AddNote(
        NoteDefinition(
            1.0, NotePitchDefinition::RelativePitch(GetPitch(scale, index)),
            intensity),
        offset);
  };
  add_chord_note(harmonic);
  add_chord_note(harmonic + 2);
  add_chord_note(harmonic + 4);
  add_chord_note(harmonic + 7);
}

void ComposeLine(double pitch_offset, const std::vector<double>& scale,
                 double intensity, int bar, int beat, int num_beats,
                 int harmonic, double offset, Sequence* sequence) {
  const int note_offset = beat;
  const auto add_note = [&](double begin_position, double end_position,
                            int index) {
    sequence->AddNote(NoteDefinition(end_position - begin_position,
                                     NotePitchDefinition::RelativePitch(
                                         pitch_offset + GetPitch(scale, index)),
                                     intensity),
                      begin_position + offset);
  };
  if (beat % 2 == 1) {
    add_note(0.0, 0.33, harmonic);
    add_note(0.33, 0.66, harmonic - note_offset);
    add_note(0.66, 1.0, harmonic);
  } else {
    add_note(0.0, 0.25, harmonic + note_offset);
  }
  if (beat % 2 == 0) {
    add_note(0.0, 0.125, harmonic - note_offset);
    add_note(0.5, 0.55, harmonic - 2 * note_offset);
  }
  if (beat + 1 == num_beats && bar % 2 == 1) {
    add_note(0.25, 0.375, harmonic + 2 * note_offset);
    add_note(0.75, 0.875, harmonic - 2 * note_offset);
    add_note(0.5, 0.75, harmonic + 2 * note_offset);
  }
}

void ComposeDrums(int bar, int beat, int num_beats, Random* random,
                  double offset, Sequence* sequence) {
  const auto get_beat = [](int step) {
    return barelyapi::GetPosition(step, barelyapi::kNumSixteenthNotesPerBeat);
  };
  const auto add_note = [&](double begin_position, double end_position,
                            double pitch, double intensity) {
    sequence->AddNote(
        NoteDefinition(end_position - begin_position,
                       NotePitchDefinition::AbsolutePitch(pitch), intensity),
        begin_position + offset);
  };

  // Kick.
  if (beat % 2 == 0) {
    add_note(get_beat(0), get_beat(2), barelyapi::kPitchKick, 1.0);
    if (bar % 2 == 1 && beat == 0) {
      add_note(get_beat(2), get_beat(4), barelyapi::kPitchKick, 1.0);
    }
  }
  // Snare.
  if (beat % 2 == 1) {
    add_note(get_beat(0), get_beat(2), barelyapi::kPitchSnare, 1.0);
  }
  if (beat + 1 == num_beats) {
    add_note(get_beat(2), get_beat(4), barelyapi::kPitchSnare, 0.75);
    if (bar % 4 == 3) {
      add_note(get_beat(1), get_beat(2), barelyapi::kPitchSnare, 1.0);
      add_note(get_beat(3), get_beat(4), barelyapi::kPitchSnare, 0.75);
    }
  }
  // Hihat Closed.
  add_note(get_beat(0), get_beat(2), barelyapi::kPitchHihatClosed,
           random->DrawUniform(0.5, 0.75));
  add_note(get_beat(2), get_beat(4), barelyapi::kPitchHihatClosed,
           random->DrawUniform(0.25, 0.75));
  // Hihat Open.
  if (beat + 1 == num_beats) {
    if (bar % 4 == 3) {
      add_note(get_beat(1), get_beat(2), barelyapi::kPitchHihatOpen, 0.5);
    } else if (bar % 2 == 0) {
      add_note(get_beat(3), get_beat(4), barelyapi::kPitchHihatOpen, 0.5);
    }
  }
  if (beat == 0 && bar % 4 == 0) {
    add_note(get_beat(0), get_beat(2), barelyapi::kPitchHihatOpen, 0.75);
  }
}

}  // namespace

int main(int /*argc*/, char* argv[]) {
  std::unique_ptr<Runfiles> runfiles(Runfiles::Create(argv[0]));
  assert(runfiles);

  AudioOutput audio_output;
  InputManager input_manager;

  Random random;

  AudioClock clock(kFrameRate);

  Musician musician;
  musician.SetTempo(kTempo);
  musician.SetRootNote(kRootNote);
  // musician.SetScale(
  //     std::vector<double>(std::cbegin(barelyapi::kPitchMajorScale),
  //                         std::cend(barelyapi::kPitchMajorScale)));
  musician.SetAdjustNoteCallback([&](NoteDefinition* definition) {
    definition->intensity *= random.DrawUniform(0.75, 1.0);
  });

  // Note on callback.
  const auto set_note_callbacks_fn = [&](auto index, Instrument* instrument) {
    instrument->SetNoteOffCallback([index](double pitch, double /*timestamp*/) {
      ConsoleLog() << "Instrument #" << index << ": NoteOff(" << pitch << ")";
    });
    instrument->SetNoteOnCallback(
        [index](double pitch, double intensity, double /*timestamp*/) {
          ConsoleLog() << "Instrument #" << index << ": NoteOn(" << pitch
                       << ", " << intensity << ")";
        });
  };

  const std::vector<int> progression = {0, 3, 4, 0};
  const std::vector<double> scale(std::cbegin(barelyapi::kPitchMajorScale),
                                  std::cend(barelyapi::kPitchMajorScale));

  // Initialize performers.
  std::vector<std::pair<Sequence, BeatComposerCallback>> performers;
  std::vector<Instrument> instruments;
  std::vector<double> gains;

  const auto build_synth_instrument_fn = [&](OscillatorType type, double gain,
                                             double attack, double release) {
    instruments.push_back(
        musician.CreateInstrument(InstrumentType::kSynth, kFrameRate));
    auto& instrument = instruments.back();
    gains.push_back(gain);
    instrument.SetParameter(SynthParameter::kOscillatorType, type);
    instrument.SetParameter(SynthParameter::kAttack, attack);
    instrument.SetParameter(SynthParameter::kRelease, release);
    set_note_callbacks_fn(instruments.size(), &instrument);
  };

  // Add synth instruments.
  const auto chords_beat_composer_callback =
      [&](int /*bar*/, int /*beat*/, int /*num_beats*/, int harmonic,
          double offset, Sequence* sequence) {
        ComposeChord(scale, 0.5, harmonic, offset, sequence);
      };

  build_synth_instrument_fn(OscillatorType::kSine, 0.075, 0.125, 0.125);
  performers.emplace_back(musician.CreateSequence(),
                          chords_beat_composer_callback);
  performers.back().first.SetInstrument(&instruments.back());

  build_synth_instrument_fn(OscillatorType::kNoise, 0.0125, 0.5, 0.025);
  performers.emplace_back(musician.CreateSequence(),
                          chords_beat_composer_callback);
  performers.back().first.SetInstrument(&instruments.back());

  const auto line_beat_composer_callback = [&](int bar, int beat, int num_beats,
                                               int harmonic, double offset,
                                               Sequence* sequence) {
    ComposeLine(-1.0, scale, 1.0, bar, beat, num_beats, harmonic, offset,
                sequence);
  };

  build_synth_instrument_fn(OscillatorType::kSaw, 0.1, 0.0025, 0.125);
  performers.emplace_back(musician.CreateSequence(),
                          line_beat_composer_callback);
  performers.back().first.SetInstrument(&instruments.back());

  const auto line_2_beat_composer_callback =
      [&](int bar, int beat, int num_beats, int harmonic, double offset,
          Sequence* sequence) {
        ComposeLine(0.0, scale, 1.0, bar, beat, num_beats, harmonic, offset,
                    sequence);
      };

  build_synth_instrument_fn(OscillatorType::kSquare, 0.1, 0.05, 0.05);
  performers.emplace_back(musician.CreateSequence(),
                          line_2_beat_composer_callback);
  performers.back().first.SetInstrument(&instruments.back());

  // Add percussion instrument.
  instruments.push_back(
      musician.CreateInstrument(InstrumentType::kPercussion, kFrameRate));
  gains.push_back(0.2);
  set_note_callbacks_fn(instruments.size(), &instruments.back());
  auto& percussion = instruments.back();
  const auto set_percussion_pad_map_fn =
      [&](const std::unordered_map<double, std::string>& percussion_map) {
        std::unordered_map<double, WavFile> percussion_files;
        for (const auto& [index, name] : percussion_map) {
          auto it = percussion_files.emplace(index, WavFile{});
          const std::string path =
              runfiles->Rlocation(kDrumsBaseFilename + name);
          const bool success = it.first->second.Load(path);
          assert(success);
        }
        static PercussionPadMap percussion_pads;
        for (const auto& [pitch, file] : percussion_files) {
          percussion_pads.insert_or_assign(
              pitch, PercussionPad{file.GetData(), file.GetSampleRate()});
        }
        percussion.SetData(&percussion_pads);
      };
  set_percussion_pad_map_fn(
      {{barelyapi::kPitchKick, "basic_kick.wav"},
       {barelyapi::kPitchSnare, "basic_snare.wav"},
       {barelyapi::kPitchHihatClosed, "basic_hihat_closed.wav"},
       {barelyapi::kPitchHihatOpen, "basic_hihat_open.wav"}});
  const auto percussion_beat_composer_callback =
      [&](int bar, int beat, int num_beats, int /*harmonic*/, double offset,
          Sequence* sequence) {
        ComposeDrums(bar, beat, num_beats, &random, offset, sequence);
      };

  performers.emplace_back(musician.CreateSequence(),
                          percussion_beat_composer_callback);
  performers.back().first.SetInstrument(&instruments.back());

  // Bar callback.
  const auto bar_composer_callback = [&progression](int bar) -> int {
    return progression[bar % progression.size()];
  };

  // Beat callback.
  int harmonic = 0;
  const auto beat_callback = [&](double position, double /*timestamp*/) {
    // Update transport.
    const int bar = static_cast<int>(position) / kNumBeats;
    const int beat = static_cast<int>(position) % kNumBeats;

    if (beat == 0) {
      // Compose next bar.
      harmonic = bar_composer_callback(bar);
    }
    // Update members.
    for (auto& [sequence, beat_composer_callback] : performers) {
      // Compose next beat notes.
      if (beat_composer_callback) {
        beat_composer_callback(bar, beat, kNumBeats, harmonic, position,
                               &sequence);
      }
    }
  };
  musician.SetBeatCallback(beat_callback);

  // Audio process callback.
  std::vector<double> temp_buffer(kNumChannels * kNumFrames);
  const auto process_callback = [&](double* output) {
    std::fill_n(output, kNumChannels * kNumFrames, 0.0);
    int i = 0;
    for (auto& instrument : instruments) {
      const double gain = gains[i++];
      instrument.Process(temp_buffer.data(), kNumChannels, kNumFrames,
                         clock.GetTimestamp());
      std::transform(temp_buffer.cbegin(), temp_buffer.cend(), output, output,
                     [&](double sample, double output_sample) {
                       return gain * sample + output_sample;
                     });
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
          musician.Stop();
          ConsoleLog() << "Stopped playback";
        } else {
          musician.Start();
          ConsoleLog() << "Started playback";
        }
        break;
      case '1':
        musician.SetTempo(random.DrawUniform(0.5, 0.75) * musician.GetTempo());
        ConsoleLog() << "Tempo changed to " << musician.GetTempo();
        break;
      case '2':
        musician.SetTempo(random.DrawUniform(1.5, 2.0) * musician.GetTempo());
        ConsoleLog() << "Tempo changed to " << musician.GetTempo();
        break;
      case 'R':
        musician.SetTempo(kTempo);
        ConsoleLog() << "Tempo reset to " << kTempo;
        break;
      case 'D':
        set_percussion_pad_map_fn(
            {{barelyapi::kPitchKick, "basic_kick.wav"},
             {barelyapi::kPitchSnare, "basic_snare.wav"},
             {barelyapi::kPitchHihatClosed, "basic_hihat_closed.wav"},
             {barelyapi::kPitchHihatOpen, "basic_hihat_open.wav"}});
        break;
      case 'H':
        set_percussion_pad_map_fn(
            {{barelyapi::kPitchKick, "basic_hihat_closed.wav"},
             {barelyapi::kPitchSnare, "basic_hihat_open.wav"},
             {barelyapi::kPitchHihatClosed, "basic_hihat_closed.wav"},
             {barelyapi::kPitchHihatOpen, "basic_hihat_open.wav"}});
        break;
    }
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start(kFrameRate, kNumChannels, kNumFrames);
  musician.Start();

  while (!quit) {
    input_manager.Update();
    musician.Update(clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  ConsoleLog() << "Stopping audio stream";
  musician.Stop();
  audio_output.Stop();

  return 0;
}
