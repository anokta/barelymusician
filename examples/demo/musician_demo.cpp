#include <algorithm>
#include <cassert>
#include <cctype>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <string>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/random.h"
#include "barelymusician/common/rational.h"
#include "barelymusician/components/metronome.h"
#include "barelymusician/composition/pitch.h"
#include "barelymusician/dsp/oscillator.h"
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
using ::barely::Rational;
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

constexpr int64_t kLookahead = kFrameRate / 10;

// Performer settings.
constexpr int kTempo = 124;
constexpr int kBeatCount = 3;

// Ensemble settings.
constexpr Rational kRootNote = barely::kPitchD4;

constexpr char kDrumsDir[] = "audio/drums/";

// Inserts pad data to a given `data` from a given `file_path`.
void InsertPadData(Rational pitch, const std::string& file_path, std::vector<float>& data) {
  WavFile sample_file;
  [[maybe_unused]] const bool success = sample_file.Load(file_path);
  assert(success);

  const float frame_rate = static_cast<float>(sample_file.GetFrameRate());
  const auto& sample_data = sample_file.GetData();
  const float length = static_cast<float>(sample_data.size());
  data.reserve(data.size() + sample_data.size() + 4);
  data.push_back(static_cast<float>(pitch.numerator));
  data.push_back(static_cast<float>(pitch.denominator));
  data.push_back(frame_rate);
  data.push_back(length);
  data.insert(data.end(), sample_data.begin(), sample_data.end());
}

// Schedules performer to play an instrument note.
void ScheduleNote(Rational position, Rational duration, Rational pitch, Rational intensity,
                  Instrument& instrument, Performer& performer) {
  performer.ScheduleOneOffTask(
      [pitch, intensity, &instrument]() { instrument.SetNoteOn(pitch, intensity); }, position);
  performer.ScheduleOneOffTask([pitch, &instrument]() { instrument.SetNoteOff(pitch); },
                               position + duration,
                               /*process_order=*/-1);
}

void ComposeChord(Rational intensity, int harmonic, Instrument& instrument, Performer& performer) {
  const auto add_chord_note = [&](int index) {
    ScheduleNote(0, 1, kRootNote + barely::PitchFromScale(barely::kPitchMajorScale, index),
                 intensity, instrument, performer);
  };
  add_chord_note(harmonic);
  add_chord_note(harmonic + 2);
  add_chord_note(harmonic + 4);
}

void ComposeLine(Rational octave_offset, Rational intensity, int bar, int beat, int beat_count,
                 int harmonic, Instrument& instrument, Performer& performer) {
  const int note_offset = beat;
  const auto add_note = [&](Rational begin_position, Rational end_position, int index) {
    ScheduleNote(
        begin_position, end_position - begin_position,
        kRootNote + octave_offset + barely::PitchFromScale(barely::kPitchMajorScale, index),
        intensity, instrument, performer);
  };
  if (beat % 2 == 1) {
    add_note(0, Rational(1, 3), harmonic);
    add_note(Rational(1, 3), Rational(2, 3), harmonic - note_offset);
    add_note(Rational(2, 3), 1, harmonic);
  } else {
    add_note(0, Rational(1, 4), harmonic + note_offset);
  }
  if (beat % 2 == 0) {
    add_note(0, Rational(1, 8), harmonic - note_offset);
    add_note(Rational(1, 2), Rational(55, 100), harmonic - 2 * note_offset);
  }
  if (beat + 1 == beat_count && bar % 2 == 1) {
    add_note(Rational(1, 4), Rational(3, 8), harmonic + 2 * note_offset);
    add_note(Rational(3, 4), Rational(7, 8), harmonic - 2 * note_offset);
    add_note(Rational(1, 2), Rational(3, 4), harmonic + 2 * note_offset);
  }
}

void ComposeDrums(int bar, int beat, int beat_count, Random& random, Instrument& instrument,
                  Performer& performer) {
  const auto get_beat = [](int step) { return Rational(step, 4); };
  const auto add_note = [&](Rational begin_position, Rational end_position, Rational pitch,
                            Rational intensity) {
    ScheduleNote(begin_position, end_position - begin_position, pitch, intensity, instrument,
                 performer);
  };

  // Kick.
  if (beat % 2 == 0) {
    add_note(get_beat(0), get_beat(2), barely::kPitchKick, 1);
    if (bar % 2 == 1 && beat == 0) {
      add_note(get_beat(2), get_beat(4), barely::kPitchKick, 1);
    }
  }
  // Snare.
  if (beat % 2 == 1) {
    add_note(get_beat(0), get_beat(2), barely::kPitchSnare, 1);
  }
  if (beat + 1 == beat_count) {
    add_note(get_beat(2), get_beat(4), barely::kPitchSnare, Rational(3, 4));
    if (bar % 4 == 3) {
      add_note(get_beat(1), get_beat(2), barely::kPitchSnare, 1);
      add_note(get_beat(3), get_beat(4), barely::kPitchSnare, Rational(3, 4));
    }
  }
  // Hihat Closed.
  add_note(get_beat(0), get_beat(2), barely::kPitchHihatClosed,
           Rational(random.DrawUniform(2, 3), 4));
  add_note(get_beat(2), get_beat(4), barely::kPitchHihatClosed,
           Rational(random.DrawUniform(1, 3), 4));
  // Hihat Open.
  if (beat + 1 == beat_count) {
    if (bar % 4 == 3) {
      add_note(get_beat(1), get_beat(2), barely::kPitchHihatOpen, Rational(1, 2));
    } else if (bar % 2 == 0) {
      add_note(get_beat(3), get_beat(4), barely::kPitchHihatOpen, Rational(1, 2));
    }
  }
  if (beat == 0 && bar % 4 == 0) {
    add_note(get_beat(0), get_beat(2), barely::kPitchHihatOpen, Rational(3, 4));
  }
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int /*argc*/, char* argv[]) {
  AudioOutput audio_output;
  InputManager input_manager;

  Random random;

  AudioClock clock(kFrameRate);

  Musician musician(kFrameRate);
  musician.SetTempo(kTempo);

  // Note on callback.
  const auto set_note_callbacks_fn = [&](auto index, Instrument& instrument) {
    instrument.SetNoteOffEvent([index](Rational pitch) {
      ConsoleLog() << "Instrument #" << index << ": NoteOff(" << pitch << ")";
    });
    instrument.SetNoteOnEvent([index](Rational pitch, Rational intensity) {
      ConsoleLog() << "Instrument #" << index << ": NoteOn(" << pitch << ", " << intensity << ")";
    });
  };

  const std::vector<int> progression = {0, 3, 4, 0};

  // Initialize performers.
  std::vector<std::tuple<Performer, BeatComposerCallback, size_t>> performers;
  std::vector<Instrument> instruments;

  const auto build_synth_instrument_fn = [&](OscillatorType type, Rational gain, Rational attack,
                                             Rational release) {
    instruments.push_back(musician.CreateInstrument<SynthInstrument>());
    auto& instrument = instruments.back();
    instrument.SetControl(SynthInstrument::Control::kGain, gain);
    instrument.SetControl(SynthInstrument::Control::kOscillatorType, static_cast<int>(type));
    instrument.SetControl(SynthInstrument::Control::kAttack, attack);
    instrument.SetControl(SynthInstrument::Control::kRelease, release);
    set_note_callbacks_fn(instruments.size(), instrument);
  };

  // Add synth instruments.
  const auto chords_beat_composer_callback = [&](int /*bar*/, int /*beat*/, int /*beat_count*/,
                                                 int harmonic, Instrument& instrument,
                                                 Performer& performer) {
    return ComposeChord(Rational(1, 2), harmonic, instrument, performer);
  };

  build_synth_instrument_fn(OscillatorType::kSine, Rational(3, 40), Rational(1, 8), Rational(1, 8));
  performers.emplace_back(musician.CreatePerformer(), chords_beat_composer_callback,
                          instruments.size() - 1);

  build_synth_instrument_fn(OscillatorType::kNoise, Rational(1, 80), Rational(1, 2),
                            Rational(1, 40));
  performers.emplace_back(musician.CreatePerformer(), chords_beat_composer_callback,
                          instruments.size() - 1);

  const auto line_beat_composer_callback = [&](int bar, int beat, int beat_count, int harmonic,
                                               Instrument& instrument, Performer& performer) {
    return ComposeLine(-1, 1, bar, beat, beat_count, harmonic, instrument, performer);
  };

  build_synth_instrument_fn(OscillatorType::kSaw, Rational(1, 10), Rational(1, 400),
                            Rational(1, 8));
  performers.emplace_back(musician.CreatePerformer(), line_beat_composer_callback,
                          instruments.size() - 1);

  const auto line_2_beat_composer_callback = [&](int bar, int beat, int beat_count, int harmonic,
                                                 Instrument& instrument, Performer& performer) {
    return ComposeLine(0, 1, bar, beat, beat_count, harmonic, instrument, performer);
  };

  build_synth_instrument_fn(OscillatorType::kSquare, Rational(1, 10), Rational(1, 20),
                            Rational(1, 20));
  performers.emplace_back(musician.CreatePerformer(), line_2_beat_composer_callback,
                          instruments.size() - 1);

  // Add percussion instrument.
  instruments.push_back(musician.CreateInstrument<PercussionInstrument>());
  auto& percussion = instruments.back();
  percussion.SetControl(PercussionInstrument::Control::kGain, Rational(1, 4));
  set_note_callbacks_fn(instruments.size(), percussion);
  const auto set_percussion_pad_map_fn =
      [&](const std::unordered_map<Rational, std::string>& percussion_map) {
        std::vector<float> data;
        data.push_back(static_cast<float>(percussion_map.size()));
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
  std::vector<float> temp_buffer(kChannelCount * kFrameCount);
  const auto process_callback = [&](float* output) {
    std::fill_n(output, kChannelCount * kFrameCount, 0.0f);
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
        musician.SetTempo(static_cast<int>(random.DrawUniform(0.5f, 0.75f) * musician.GetTempo()));
        ConsoleLog() << "Tempo changed to " << musician.GetTempo();
        break;
      case '2':
        musician.SetTempo(std::min(kFrameRate / 2, static_cast<int>(random.DrawUniform(1.5f, 2.0f) *
                                                                    musician.GetTempo())));
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
      default:
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
