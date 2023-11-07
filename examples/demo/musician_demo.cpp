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
#include "barelymusician/components/metronome.h"
#include "barelymusician/composition/duration.h"
#include "barelymusician/composition/pitch.h"
#include "barelymusician/instruments/percussion_instrument.h"
#include "barelymusician/instruments/synth_instrument.h"
#include "examples/common/audio_clock.h"
#include "examples/common/audio_output.h"
#include "examples/common/console_log.h"
#include "examples/common/input_manager.h"
#include "examples/common/wav_file.h"
#include "examples/data/data.h"

namespace {

using ::barely::Instrument;
using ::barely::Metronome;
using ::barely::Musician;
using ::barely::OscillatorType;
using ::barely::PercussionInstrument;
using ::barely::Performer;
using ::barely::Random;
using ::barely::SynthInstrument;
using ::barely::examples::AudioClock;
using ::barely::examples::AudioOutput;
using ::barely::examples::ConsoleLog;
using ::barely::examples::GetDataFilePath;
using ::barely::examples::InputManager;
using ::barely::examples::WavFile;

// Beat composer callback signature.
//
// @param bar Current bar.
// @param beat Current beat.
// @param beat_count Number of beats in a bar.
// @param harmonic Harmonic index.
// @param musician Pointer to musician.
// @param instrument Instrument.
// @param performer Performer.
using BeatComposerCallback = std::function<void(int bar, int beat, int beat_count, int harmonic,
                                                Instrument& instrument, Performer& performer)>;

// System audio settings.
constexpr int kFrameRate = 48000;
constexpr int kChannelCount = 2;
constexpr int kFrameCount = 1024;

constexpr double kLookahead = 0.1;

// Performer settings.
constexpr double kTempo = 124.0;
constexpr int kBeatCount = 3;

// Ensemble settings.
constexpr double kRootNote = barely::kPitchD3;

constexpr char kDrumsDir[] = "audio/drums/";

// Inserts pad data to a given `data` from a given `file_path`.
void InsertPadData(double pitch, const std::string& file_path, std::vector<double>& data) {
  WavFile sample_file;
  [[maybe_unused]] const bool success = sample_file.Load(file_path);
  assert(success);

  const double frame_rate = static_cast<double>(sample_file.GetFrameRate());
  const auto& sample_data = sample_file.GetData();
  const double length = static_cast<double>(sample_data.size());
  data.reserve(data.size() + sample_data.size() + 3);
  data.push_back(pitch);
  data.push_back(frame_rate);
  data.push_back(length);
  data.insert(data.end(), sample_data.begin(), sample_data.end());
}

// Schedules performer to play an instrument note.
void ScheduleNote(double position, double duration, double pitch, double intensity,
                  Instrument& instrument, Performer& performer) {
  performer
      .CreateTask([pitch, intensity, &instrument]() { instrument.SetNoteOn(pitch, intensity); },
                  /*is_one_off=*/true, position)
      .Release();
  performer
      .CreateTask([pitch, &instrument]() { instrument.SetNoteOff(pitch); },
                  /*is_one_off=*/true, position + duration,
                  /*process_order=*/-1)
      .Release();
}

void ComposeChord(double intensity, int harmonic, Instrument& instrument, Performer& performer) {
  const auto add_chord_note = [&](int index) {
    ScheduleNote(0.0, 1.0, kRootNote + barely::PitchFromScale(barely::kPitchMajorScale, index),
                 intensity, instrument, performer);
  };
  add_chord_note(harmonic);
  add_chord_note(harmonic + 2);
  add_chord_note(harmonic + 4);
}

void ComposeLine(double octave_offset, double intensity, int bar, int beat, int beat_count,
                 int harmonic, Instrument& instrument, Performer& performer) {
  const int note_offset = beat;
  const auto add_note = [&](double begin_position, double end_position, int index) {
    ScheduleNote(
        begin_position, end_position - begin_position,
        kRootNote + octave_offset + barely::PitchFromScale(barely::kPitchMajorScale, index),
        intensity, instrument, performer);
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

void ComposeDrums(int bar, int beat, int beat_count, Random& random, Instrument& instrument,
                  Performer& performer) {
  const auto get_beat = [](int step) {
    return barely::GetPosition(step, barely::kSixteenthNotesPerBeat);
  };
  const auto add_note = [&](double begin_position, double end_position, double pitch,
                            double intensity) {
    ScheduleNote(begin_position, end_position - begin_position, pitch, intensity, instrument,
                 performer);
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
  add_note(get_beat(0), get_beat(2), barely::kPitchHihatClosed, random.DrawUniform(0.5, 0.75));
  add_note(get_beat(2), get_beat(4), barely::kPitchHihatClosed, random.DrawUniform(0.25, 0.75));
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
  AudioOutput audio_output;
  InputManager input_manager;

  Random random;

  AudioClock clock(kFrameRate);

  Musician musician;
  musician.SetTempo(kTempo);

  // Note on callback.
  const auto set_note_callbacks_fn = [&](auto index, Instrument& instrument) {
    instrument.SetNoteOffEvent([index](double pitch) {
      ConsoleLog() << "Instrument #" << index << ": NoteOff(" << pitch << ")";
    });
    instrument.SetNoteOnEvent([index](double pitch, double intensity) {
      ConsoleLog() << "Instrument #" << index << ": NoteOn(" << pitch << ", " << intensity << ")";
    });
  };

  const std::vector<int> progression = {0, 3, 4, 0};

  // Initialize performers.
  std::vector<std::tuple<Performer, BeatComposerCallback, size_t>> performers;
  std::vector<Instrument> instruments;

  const auto build_synth_instrument_fn = [&](OscillatorType type, double gain, double attack,
                                             double release) {
    instruments.push_back(musician.CreateInstrument<SynthInstrument>(kFrameRate));
    auto& instrument = instruments.back();
    instrument.SetControl(SynthInstrument::Control::kGain, gain);
    instrument.SetControl(SynthInstrument::Control::kOscillatorType, type);
    instrument.SetControl(SynthInstrument::Control::kAttack, attack);
    instrument.SetControl(SynthInstrument::Control::kRelease, release);
    set_note_callbacks_fn(instruments.size(), instrument);
  };

  // Add synth instruments.
  const auto chords_beat_composer_callback =
      [&](int /*bar*/, int /*beat*/, int /*beat_count*/, int harmonic, Instrument& instrument,
          Performer& performer) { return ComposeChord(0.5, harmonic, instrument, performer); };

  build_synth_instrument_fn(OscillatorType::kSine, 0.075, 0.125, 0.125);
  performers.emplace_back(musician.CreatePerformer(), chords_beat_composer_callback,
                          instruments.size() - 1);

  build_synth_instrument_fn(OscillatorType::kNoise, 0.0125, 0.5, 0.025);
  performers.emplace_back(musician.CreatePerformer(), chords_beat_composer_callback,
                          instruments.size() - 1);

  const auto line_beat_composer_callback = [&](int bar, int beat, int beat_count, int harmonic,
                                               Instrument& instrument, Performer& performer) {
    return ComposeLine(-1.0, 1.0, bar, beat, beat_count, harmonic, instrument, performer);
  };

  build_synth_instrument_fn(OscillatorType::kSaw, 0.1, 0.0025, 0.125);
  performers.emplace_back(musician.CreatePerformer(), line_beat_composer_callback,
                          instruments.size() - 1);

  const auto line_2_beat_composer_callback = [&](int bar, int beat, int beat_count, int harmonic,
                                                 Instrument& instrument, Performer& performer) {
    return ComposeLine(0, 1.0, bar, beat, beat_count, harmonic, instrument, performer);
  };

  build_synth_instrument_fn(OscillatorType::kSquare, 0.1, 0.05, 0.05);
  performers.emplace_back(musician.CreatePerformer(), line_2_beat_composer_callback,
                          instruments.size() - 1);

  // Add percussion instrument.
  instruments.push_back(musician.CreateInstrument<PercussionInstrument>(kFrameRate));
  auto& percussion = instruments.back();
  percussion.SetControl(PercussionInstrument::Control::kGain, 0.25);
  set_note_callbacks_fn(instruments.size(), percussion);
  const auto set_percussion_pad_map_fn =
      [&](const std::unordered_map<double, std::string>& percussion_map) {
        std::vector<double> data;
        data.push_back(static_cast<double>(percussion_map.size()));
        for (const auto& [pitch, file_path] : percussion_map) {
          InsertPadData(pitch, GetDataFilePath(kDrumsDir + file_path, argv), data);
        }
        percussion.SetData(data);
      };
  set_percussion_pad_map_fn({
      {barely::kPitchKick, "basic_kick.wav"},
      {barely::kPitchSnare, "basic_snare.wav"},
      {barely::kPitchHihatClosed, "basic_hihat_closed.wav"},
      {barely::kPitchHihatOpen, "basic_hihat_open.wav"},
  });
  const auto percussion_beat_composer_callback = [&](int bar, int beat, int beat_count,
                                                     int /*harmonic*/, Instrument& instrument,
                                                     Performer& performer) {
    return ComposeDrums(bar, beat, beat_count, random, instrument, performer);
  };

  performers.emplace_back(musician.CreatePerformer(), percussion_beat_composer_callback,
                          instruments.size() - 1);

  for (auto& [performer, beat_composer_callback, index] : performers) {
    performer.SetLooping(true);
  }

  // Bar callback.
  const auto bar_composer_callback = [&progression](int bar) -> int {
    return progression[bar % progression.size()];
  };

  // Beat callback.
  int harmonic = 0;
  const auto beat_callback = [&](int beat) {
    // Update transport.
    const int bar = beat / kBeatCount;
    beat = beat % kBeatCount;

    if (beat == 0) {
      // Compose next bar.
      harmonic = bar_composer_callback(bar);
    }
    // Update members.
    for (auto& [performer, beat_composer_callback, index] : performers) {
      // Compose next beat notes.
      if (beat_composer_callback) {
        beat_composer_callback(bar, beat, kBeatCount, harmonic, instruments[index], performer);
      }
    }
  };

  auto metronome = musician.CreateComponent<Metronome>(-10);
  metronome.SetBeatCallback(beat_callback);

  // Audio process callback.
  std::vector<double> temp_buffer(kChannelCount * kFrameCount);
  const auto process_callback = [&](double* output) {
    std::fill_n(output, kChannelCount * kFrameCount, 0.0);
    for (auto& instrument : instruments) {
      instrument.Process(temp_buffer.data(), kChannelCount, kFrameCount, clock.GetTimestamp());
      std::transform(temp_buffer.begin(), temp_buffer.end(), output, output, std::plus());
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
          for (auto& [performer, beat_composer_callback, index] : performers) {
            performer.Stop();
          }
          for (auto& instrument : instruments) {
            instrument.SetAllNotesOff();
          }
          ConsoleLog() << "Stopped playback";
        } else {
          for (auto& [performer, beat_composer_callback, index] : performers) {
            performer.Start();
          }
          metronome.Start();
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
        set_percussion_pad_map_fn({
            {barely::kPitchKick, "basic_kick.wav"},
            {barely::kPitchSnare, "basic_snare.wav"},
            {barely::kPitchHihatClosed, "basic_hihat_closed.wav"},
            {barely::kPitchHihatOpen, "basic_hihat_open.wav"},
        });
        break;
      case 'H':
        set_percussion_pad_map_fn({
            {barely::kPitchKick, "basic_hihat_closed.wav"},
            {barely::kPitchSnare, "basic_hihat_open.wav"},
            {barely::kPitchHihatClosed, "basic_hihat_closed.wav"},
            {barely::kPitchHihatOpen, "basic_hihat_open.wav"},
        });
        break;
    }
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start(kFrameRate, kChannelCount, kFrameCount);
  for (auto& [performer, beat_composer_callback, index] : performers) {
    performer.Start();
  }
  metronome.Start();

  while (!quit) {
    input_manager.Update();
    musician.Update(clock.GetTimestamp() + kLookahead);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  ConsoleLog() << "Stopping audio stream";
  metronome.Stop();
  audio_output.Stop();

  return 0;
}
