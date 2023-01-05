#include <algorithm>
#include <cassert>
#include <cctype>
#include <chrono>
#include <cstddef>
#include <cstring>
#include <functional>
#include <memory>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/random.h"
#include "barelymusician/composition/note_duration.h"
#include "barelymusician/composition/note_pitch.h"
#include "barelymusician/instruments/percussion_instrument.h"
#include "barelymusician/instruments/synth_instrument.h"
#include "barelymusician/sequencers/metronome.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"
#include "examples/common/wav_file.h"
#include "tools/cpp/runfiles/runfiles.h"

namespace {

using ::barely::Engine;
using ::barely::Instrument;
using ::barely::Metronome;
using ::barely::OscillatorType;
using ::barely::PercussionInstrument;
using ::barely::Random;
using ::barely::Sequencer;
using ::barely::SynthInstrument;
using ::barely::SynthParameter;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::InputManager;
using ::barely::examples::WavFile;
using ::bazel::tools::cpp::runfiles::Runfiles;

// Beat composer callback signature.
//
// @param bar Current bar.
// @param beat Current beat.
// @param beat_count Number of beats in a bar.
// @param harmonic Harmonic index.
// @param offset Position offset in beats.
// @param engine Pointer to engine.
// @param instrument Instrument.
// @param sequencer Sequencer.
using BeatComposerCallback = std::function<void(
    int bar, int beat, int beat_count, int harmonic, double offset,
    Instrument& instrument, Sequencer& sequencer)>;

// System audio settings.
constexpr int kFrameRate = 48000;
constexpr int kChannelCount = 2;
constexpr int kFrameCount = 1024;

constexpr double kLookahead = 0.1;

// Sequencer settings.
constexpr double kTempo = 124.0;
constexpr int kBeatCount = 3;

// Ensemble settings.
constexpr double kRootNote = barely::kPitchD3;

constexpr char kDrumsBaseFilename[] =
    "barelymusician/examples/data/audio/drums/";

// Schedules sequencer to play an instrument note.
void ScheduleNote(double position, double duration, double pitch,
                  double intensity, Instrument& instrument,
                  Sequencer& sequencer) {
  sequencer.ScheduleOneOffTask(position, [pitch, intensity, &instrument]() {
    instrument.StartNote(pitch, intensity);
  });
  sequencer.ScheduleOneOffTask(position + duration, [pitch, &instrument]() {
    instrument.StopNote(pitch);
  });
}

void ComposeChord(double intensity, int harmonic, double offset,
                  Instrument& instrument, Sequencer& sequencer) {
  const auto add_chord_note = [&](int index) {
    ScheduleNote(offset, 1.0,
                 kRootNote + barely::GetPitch(barely::kPitchMajorScale, index),
                 intensity, instrument, sequencer);
  };
  add_chord_note(harmonic);
  add_chord_note(harmonic + 2);
  add_chord_note(harmonic + 4);
}

void ComposeLine(double octave_offset, double intensity, int bar, int beat,
                 int beat_count, int harmonic, double offset,
                 Instrument& instrument, Sequencer& sequencer) {
  const int note_offset = beat;
  const auto add_note = [&](double begin_position, double end_position,
                            int index) {
    ScheduleNote(begin_position + offset, end_position - begin_position,
                 kRootNote + octave_offset +
                     barely::GetPitch(barely::kPitchMajorScale, index),
                 intensity, instrument, sequencer);
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
  if (beat + 1 == beat_count && bar % 2 == 1) {
    add_note(0.25, 0.375, harmonic + 2 * note_offset);
    add_note(0.75, 0.875, harmonic - 2 * note_offset);
    add_note(0.5, 0.75, harmonic + 2 * note_offset);
  }
}

void ComposeDrums(int bar, int beat, int beat_count, Random& random,
                  double offset, Instrument& instrument, Sequencer& sequencer) {
  const auto get_beat = [](int step) {
    return barely::GetPosition(step, barely::kSixteenthNotesPerBeat);
  };
  const auto add_note = [&](double begin_position, double end_position,
                            double pitch, double intensity) {
    ScheduleNote(begin_position + offset, end_position - begin_position, pitch,
                 intensity, instrument, sequencer);
  };

  // Kick.
  if (beat % 2 == 0) {
    add_note(get_beat(0), get_beat(2), barely::kPitchKick, 1.0);
    if (bar % 2 == 1 && beat == 0) {
      add_note(get_beat(2), get_beat(4), barely::kPitchKick, 1.0);
    }
  }
  // Snare.
  if (beat % 2 == 1) {
    add_note(get_beat(0), get_beat(2), barely::kPitchSnare, 1.0);
  }
  if (beat + 1 == beat_count) {
    add_note(get_beat(2), get_beat(4), barely::kPitchSnare, 0.75);
    if (bar % 4 == 3) {
      add_note(get_beat(1), get_beat(2), barely::kPitchSnare, 1.0);
      add_note(get_beat(3), get_beat(4), barely::kPitchSnare, 0.75);
    }
  }
  // Hihat Closed.
  add_note(get_beat(0), get_beat(2), barely::kPitchHihatClosed,
           random.DrawUniform(0.5, 0.75));
  add_note(get_beat(2), get_beat(4), barely::kPitchHihatClosed,
           random.DrawUniform(0.25, 0.75));
  // Hihat Open.
  if (beat + 1 == beat_count) {
    if (bar % 4 == 3) {
      add_note(get_beat(1), get_beat(2), barely::kPitchHihatOpen, 0.5);
    } else if (bar % 2 == 0) {
      add_note(get_beat(3), get_beat(4), barely::kPitchHihatOpen, 0.5);
    }
  }
  if (beat == 0 && bar % 4 == 0) {
    add_note(get_beat(0), get_beat(2), barely::kPitchHihatOpen, 0.75);
  }
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int /*argc*/, char* argv[]) {
  std::unique_ptr<Runfiles> runfiles(Runfiles::Create(argv[0]));
  assert(runfiles);

  AudioOutput audio_output;
  InputManager input_manager;

  Random random;

  AudioClock clock(kFrameRate);

  Engine engine;
  engine.SetTempo(kTempo);

  // Note on callback.
  const auto set_note_callbacks_fn = [&](auto index, Instrument* instrument) {
    instrument->SetNoteOffCallback([index](double pitch) {
      ConsoleLog() << "Instrument #" << index << ": NoteOff(" << pitch << ")";
    });
    instrument->SetNoteOnCallback([index](double pitch, double intensity) {
      ConsoleLog() << "Instrument #" << index << ": NoteOn(" << pitch << ", "
                   << intensity << ")";
    });
  };

  const std::vector<int> progression = {0, 3, 4, 0};

  // Initialize performers.
  std::vector<std::tuple<Sequencer, BeatComposerCallback, size_t>> performers;
  std::vector<Instrument> instruments;
  std::vector<double> gains;

  const auto build_synth_instrument_fn = [&](OscillatorType type, double gain,
                                             double attack, double release) {
    instruments.push_back(
        engine.CreateInstrument(SynthInstrument::GetDefinition(), kFrameRate));
    auto& instrument = instruments.back();
    gains.push_back(gain);
    instrument.SetParameter(SynthParameter::kOscillatorType, type);
    instrument.SetParameter(SynthParameter::kAttack, attack);
    instrument.SetParameter(SynthParameter::kRelease, release);
    set_note_callbacks_fn(instruments.size(), &instrument);
  };

  // Add synth instruments.
  const auto chords_beat_composer_callback =
      [&](int /*bar*/, int /*beat*/, int /*beat_count*/, int harmonic,
          double offset, Instrument& instrument, Sequencer& sequencer) {
        return ComposeChord(0.5, harmonic, offset, instrument, sequencer);
      };

  build_synth_instrument_fn(OscillatorType::kSine, 0.075, 0.125, 0.125);
  performers.emplace_back(engine.CreateSequencer(),
                          chords_beat_composer_callback,
                          instruments.size() - 1);

  build_synth_instrument_fn(OscillatorType::kNoise, 0.0125, 0.5, 0.025);
  performers.emplace_back(engine.CreateSequencer(),
                          chords_beat_composer_callback,
                          instruments.size() - 1);

  const auto line_beat_composer_callback =
      [&](int bar, int beat, int beat_count, int harmonic, double offset,
          Instrument& instrument, Sequencer& sequencer) {
        return ComposeLine(-1.0, 1.0, bar, beat, beat_count, harmonic, offset,
                           instrument, sequencer);
      };

  build_synth_instrument_fn(OscillatorType::kSaw, 0.1, 0.0025, 0.125);
  performers.emplace_back(engine.CreateSequencer(), line_beat_composer_callback,
                          instruments.size() - 1);

  const auto line_2_beat_composer_callback =
      [&](int bar, int beat, int beat_count, int harmonic, double offset,
          Instrument& instrument, Sequencer& sequencer) {
        return ComposeLine(0, 1.0, bar, beat, beat_count, harmonic, offset,
                           instrument, sequencer);
      };

  build_synth_instrument_fn(OscillatorType::kSquare, 0.1, 0.05, 0.05);
  performers.emplace_back(engine.CreateSequencer(),
                          line_2_beat_composer_callback,
                          instruments.size() - 1);

  // Add percussion instrument.
  instruments.push_back(engine.CreateInstrument(
      PercussionInstrument::GetDefinition(), kFrameRate));
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
        std::vector<std::byte> data;
        for (const auto& [pitch, file] : percussion_files) {
          const int size = static_cast<int>(data.size());
          const int frequency = file.GetFrameRate();
          const int length = static_cast<int>(file.GetData().size());
          const double* voice_data = file.GetData().data();
          data.resize(size + sizeof(double) + sizeof(int) + sizeof(int) +
                      sizeof(double) * length);
          std::byte* back = &data[size];
          std::memcpy(back, reinterpret_cast<const void*>(&pitch),
                      sizeof(double));
          back += sizeof(double);
          std::memcpy(back, reinterpret_cast<const void*>(&frequency),
                      sizeof(int));
          back += sizeof(int);
          std::memcpy(back, reinterpret_cast<const void*>(&length),
                      sizeof(int));
          back += sizeof(int);
          std::memcpy(back, reinterpret_cast<const void*>(voice_data),
                      sizeof(double) * length);
        }
        percussion.SetData(data.data(), static_cast<int>(data.size()));
      };
  set_percussion_pad_map_fn(
      {{barely::kPitchKick, "basic_kick.wav"},
       {barely::kPitchSnare, "basic_snare.wav"},
       {barely::kPitchHihatClosed, "basic_hihat_closed.wav"},
       {barely::kPitchHihatOpen, "basic_hihat_open.wav"}});
  const auto percussion_beat_composer_callback =
      [&](int bar, int beat, int beat_count, int /*harmonic*/, double offset,
          Instrument& instrument, Sequencer& sequencer) {
        return ComposeDrums(bar, beat, beat_count, random, offset, instrument,
                            sequencer);
      };

  performers.emplace_back(engine.CreateSequencer(),
                          percussion_beat_composer_callback,
                          instruments.size() - 1);

  // Bar callback.
  const auto bar_composer_callback = [&progression](int bar) -> int {
    return progression[bar % progression.size()];
  };

  // Beat callback.
  int harmonic = 0;
  const auto beat_callback = [&](int beat) {
    const double position = static_cast<double>(beat);
    // Update transport.
    const int bar = beat / kBeatCount;
    beat = beat % kBeatCount;

    if (beat == 0) {
      // Compose next bar.
      harmonic = bar_composer_callback(bar);
    }
    // Update members.
    for (auto& [sequencer, beat_composer_callback, index] : performers) {
      // Compose next beat notes.
      if (beat_composer_callback) {
        beat_composer_callback(bar, beat, kBeatCount, harmonic, position,
                               instruments[index], sequencer);
      }
    }
  };

  Metronome metronome(engine);
  metronome.SetBeatCallback(beat_callback);

  // Audio process callback.
  std::vector<double> temp_buffer(kChannelCount * kFrameCount);
  const auto process_callback = [&](double* output) {
    std::fill_n(output, kChannelCount * kFrameCount, 0.0);
    int i = 0;
    for (auto& instrument : instruments) {
      const double gain = gains[i++];
      instrument.Process(temp_buffer.data(), kChannelCount, kFrameCount,
                         clock.GetTimestamp());
      std::transform(temp_buffer.cbegin(), temp_buffer.cend(), output, output,
                     [&](double sample, double output_sample) {
                       return gain * sample + output_sample;
                     });
    }
    clock.Update(kFrameCount);
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
        if (metronome.IsPlaying()) {
          metronome.Stop();
          for (auto& [sequencer, beat_composer_callback, index] : performers) {
            sequencer.Stop();
          }
          for (auto& instrument : instruments) {
            instrument.StopAllNotes();
          }
          ConsoleLog() << "Stopped playback";
        } else {
          for (auto& [sequencer, beat_composer_callback, index] : performers) {
            sequencer.Start();
          }
          metronome.Start();
          ConsoleLog() << "Started playback";
        }
        break;
      case '1':
        engine.SetTempo(random.DrawUniform(0.5, 0.75) * engine.GetTempo());
        ConsoleLog() << "Tempo changed to " << engine.GetTempo();
        break;
      case '2':
        engine.SetTempo(random.DrawUniform(1.5, 2.0) * engine.GetTempo());
        ConsoleLog() << "Tempo changed to " << engine.GetTempo();
        break;
      case 'R':
        engine.SetTempo(kTempo);
        ConsoleLog() << "Tempo reset to " << kTempo;
        break;
      case 'D':
        set_percussion_pad_map_fn(
            {{barely::kPitchKick, "basic_kick.wav"},
             {barely::kPitchSnare, "basic_snare.wav"},
             {barely::kPitchHihatClosed, "basic_hihat_closed.wav"},
             {barely::kPitchHihatOpen, "basic_hihat_open.wav"}});
        break;
      case 'H':
        set_percussion_pad_map_fn(
            {{barely::kPitchKick, "basic_hihat_closed.wav"},
             {barely::kPitchSnare, "basic_hihat_open.wav"},
             {barely::kPitchHihatClosed, "basic_hihat_closed.wav"},
             {barely::kPitchHihatOpen, "basic_hihat_open.wav"}});
        break;
    }
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start(kFrameRate, kChannelCount, kFrameCount);
  for (auto& [sequencer, beat_composer_callback, index] : performers) {
    sequencer.Start();
  }
  metronome.Start();

  while (!quit) {
    input_manager.Update();
    engine.Update(clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  ConsoleLog() << "Stopping audio stream";
  metronome.Stop();
  audio_output.Stop();

  return 0;
}
