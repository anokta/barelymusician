#include <algorithm>
#include <array>
#include <cassert>
#include <cctype>
#include <chrono>
#include <cstddef>
#include <cstring>
#include <functional>
#include <span>
#include <string>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

#include "barelymusician.h"
#include "common/audio_clock.h"
#include "common/audio_output.h"
#include "common/console_log.h"
#include "common/input_manager.h"
#include "common/wav_file.h"
#include "data/data.h"

namespace {

using ::barely::ControlType;
using ::barely::Engine;
using ::barely::Instrument;
using ::barely::Performer;
using ::barely::Random;
using ::barely::Scale;
using ::barely::Slice;
using ::barely::SliceMode;
using ::barely::Task;
using ::barely::TaskState;
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
// @param instrument Instrument handle.
// @param performer Performer handle.
using BeatComposerCallback =
    std::function<void(int bar, int beat, int beat_count, int harmonic, Instrument& instrument,
                       Performer& performer, std::vector<Task>& tasks)>;

// System audio settings.
constexpr int kSampleRate = 48000;
constexpr int kSampleCount = 1024;

constexpr double kLookahead = 0.1;

// Performer settings.
constexpr double kTempo = 124.0;
constexpr int kBeatCount = 3;

// Number of semitones in an octave.
constexpr int kSemitoneCount = 12;

// Number of pitches in a heptatonic scale.
constexpr int kHeptatonicScaleCount = 7;

/// Common musical scale ratios.
constexpr std::array<float, kSemitoneCount> kSemitones = {
    0.0f / static_cast<float>(kSemitoneCount),  1.0f / static_cast<float>(kSemitoneCount),
    2.0f / static_cast<float>(kSemitoneCount),  3.0f / static_cast<float>(kSemitoneCount),
    4.0f / static_cast<float>(kSemitoneCount),  5.0f / static_cast<float>(kSemitoneCount),
    6.0f / static_cast<float>(kSemitoneCount),  7.0f / static_cast<float>(kSemitoneCount),
    8.0f / static_cast<float>(kSemitoneCount),  9.0f / static_cast<float>(kSemitoneCount),
    10.0f / static_cast<float>(kSemitoneCount), 11.0f / static_cast<float>(kSemitoneCount),
};
constexpr std::array<float, kHeptatonicScaleCount> kDiatonicPitches = {
    kSemitones[0], kSemitones[2], kSemitones[4],  kSemitones[5],
    kSemitones[7], kSemitones[9], kSemitones[11],
};

/// Number of sixteenth notes in a quarter note beat duration.
constexpr double kSixteenthNotesPerBeat = 4.0;

// Ensemble settings.
constexpr float kRootPitch = kSemitones[2];

constexpr float kPitchKick = 0.0f;
constexpr float kPitchSnare = 1.0f;
constexpr float kPitchHihatClosed = 2.0f;
constexpr float kPitchHihatOpen = 3.0f;

constexpr char kDrumsDir[] = "audio/drums/";

// Inserts pad data to a given `data` from a given `file_path`.
void InsertPadData(float pitch, const std::string& file_path, std::vector<float>& samples,
                   std::vector<Slice>& slices) {
  WavFile sample_file;
  [[maybe_unused]] const bool success = sample_file.Load(file_path);
  assert(success);

  samples = sample_file.GetData();
  slices.emplace_back(pitch, sample_file.GetSampleRate(), samples);
}

// Schedules performer to play an instrument note.
void ScheduleNote(double position, double duration, float pitch, float intensity,
                  Instrument& instrument, Performer& performer, std::vector<Task>& tasks) {
  tasks.emplace_back(
      performer.CreateTask(performer.GetPosition() + position, duration,
                           [pitch, intensity, &instrument](TaskState state) noexcept {
                             if (state == TaskState::kBegin) {
                               instrument.SetNoteOn(pitch, intensity);
                             } else if (state == TaskState::kEnd) {
                               instrument.SetNoteOff(pitch);
                             }
                           }));
}

void ComposeChord(float intensity, int harmonic, const Scale& scale, Instrument& instrument,
                  Performer& performer, std::vector<Task>& tasks) {
  const auto add_chord_note = [&](int degree) {
    ScheduleNote(0.0, 1.0, scale.GetPitch(degree), intensity, instrument, performer, tasks);
  };
  add_chord_note(harmonic);
  add_chord_note(harmonic + 2);
  add_chord_note(harmonic + 4);
}

void ComposeLine(int octave_offset, float intensity, int bar, int beat, int beat_count,
                 int harmonic, const Scale& scale, Instrument& instrument, Performer& performer,
                 std::vector<Task>& tasks) {
  const int note_offset = beat;
  const auto add_note = [&](double begin_position, double end_position, int degree) {
    ScheduleNote(begin_position, end_position - begin_position,
                 scale.GetPitch(octave_offset * scale.GetPitchCount() + degree), intensity,
                 instrument, performer, tasks);
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
    add_note(0.5, 0.625, harmonic - 2 * note_offset);
  }
  if (beat + 1 == beat_count && bar % 2 == 1) {
    add_note(0.25, 0.375, harmonic + 2 * note_offset);
    add_note(0.75, 0.875, harmonic - 2 * note_offset);
    add_note(0.5, 0.75, harmonic + 2 * note_offset);
  }
}

void ComposeDrums(int bar, int beat, int beat_count, Random& random, Instrument& instrument,
                  Performer& performer, std::vector<Task>& tasks) {
  const auto get_beat = [](int step) { return static_cast<double>(step) / kSixteenthNotesPerBeat; };
  const auto add_note = [&](double begin_position, double end_position, float pitch,
                            float intensity) {
    ScheduleNote(begin_position, end_position - begin_position, pitch, intensity, instrument,
                 performer, tasks);
  };

  // Kick.
  if (beat % 2 == 0) {
    add_note(get_beat(0), get_beat(2), kPitchKick, 1.0f);
    if (bar % 2 == 1 && beat == 0) {
      add_note(get_beat(2), get_beat(4), kPitchKick, 1.0f);
    }
  }
  // Snare.
  if (beat % 2 == 1) {
    add_note(get_beat(0), get_beat(2), kPitchSnare, 1.0f);
  }
  if (beat + 1 == beat_count) {
    add_note(get_beat(2), get_beat(4), kPitchSnare, 0.75f);
    if (bar % 4 == 3) {
      add_note(get_beat(1), get_beat(2), kPitchSnare, 1.0f);
      add_note(get_beat(3), get_beat(4), kPitchSnare, 0.75f);
    }
  }
  // Hihat Closed.
  add_note(get_beat(0), get_beat(2), kPitchHihatClosed, random.DrawUniform(0.5f, 0.75f));
  add_note(get_beat(2), get_beat(4), kPitchHihatClosed, random.DrawUniform(0.25f, 0.75f));
  // Hihat Open.
  if (beat + 1 == beat_count) {
    if (bar % 4 == 3) {
      add_note(get_beat(1), get_beat(2), kPitchHihatOpen, 0.5);
    } else if (bar % 2 == 0) {
      add_note(get_beat(3), get_beat(4), kPitchHihatOpen, 0.5);
    }
  }
  if (beat == 0 && bar % 4 == 0) {
    add_note(get_beat(0), get_beat(2), kPitchHihatOpen, 0.75);
  }
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int /*argc*/, char* argv[]) {
  InputManager input_manager;

  Random random;

  AudioClock clock(kSampleRate);
  AudioOutput audio_output(kSampleRate, kSampleCount);

  Engine engine(kSampleRate);
  engine.SetTempo(kTempo);

  // Note on callback.
  const auto set_note_callbacks_fn = [&](size_t index, Instrument& instrument) {
    instrument.SetNoteOffCallback([index](float pitch) {
      ConsoleLog() << "Instrument #" << index << ": NoteOff(" << pitch << ")";
    });
    instrument.SetNoteOnCallback([index](float pitch, float intensity) {
      ConsoleLog() << "Instrument #" << index << ": NoteOn(" << pitch << ", " << intensity << ")";
    });
  };

  const std::vector<int> progression = {0, 3, 4, 0};

  // Initialize performers.
  std::vector<std::tuple<Performer, std::vector<Task>, BeatComposerCallback, size_t>> performers;
  std::vector<Instrument> instruments;

  const auto build_instrument_fn = [&](float shape, float gain, float attack, float release) {
    instruments.emplace_back(engine.CreateInstrument());
    auto& instrument = instruments.back();
    instrument.SetControl(ControlType::kGain, gain);
    instrument.SetControl(ControlType::kOscMix, 1.0f);
    if (shape < 0.0f) {
      instrument.SetControl(ControlType::kOscNoiseMix, 1.0f);
    } else {
      instrument.SetControl(ControlType::kOscShape, shape);
    }
    instrument.SetControl(ControlType::kAttack, attack);
    instrument.SetControl(ControlType::kRelease, release);
    set_note_callbacks_fn(instruments.size(), instrument);
  };

  Scale scale = {kDiatonicPitches, kRootPitch};

  // Add synth instruments.
  const auto chords_beat_composer_callback = [&](int /*bar*/, int /*beat*/, int /*beat_count*/,
                                                 int harmonic, Instrument& instrument,
                                                 Performer& performer, std::vector<Task>& tasks) {
    ComposeChord(0.5, harmonic, scale, instrument, performer, tasks);
  };

  build_instrument_fn(0.0f, -25.0f, 0.125f, 0.125f);
  performers.emplace_back(engine.CreatePerformer(), std::vector<Task>{},
                          chords_beat_composer_callback, instruments.size() - 1);

  build_instrument_fn(-1.0f, -40.0f, 0.5f, 0.025f);
  performers.emplace_back(engine.CreatePerformer(), std::vector<Task>{},
                          chords_beat_composer_callback, instruments.size() - 1);

  const auto line_beat_composer_callback = [&](int bar, int beat, int beat_count, int harmonic,
                                               Instrument& instrument, Performer& performer,
                                               std::vector<Task>& tasks) {
    ComposeLine(-1, 1.0f, bar, beat, beat_count, harmonic, scale, instrument, performer, tasks);
  };

  build_instrument_fn(1.0f, -24.0f, 0.0025f, 0.125f);
  performers.emplace_back(engine.CreatePerformer(), std::vector<Task>{},
                          line_beat_composer_callback, instruments.size() - 1);

  const auto line_2_beat_composer_callback = [&](int bar, int beat, int beat_count, int harmonic,
                                                 Instrument& instrument, Performer& performer,
                                                 std::vector<Task>& tasks) {
    ComposeLine(0, 1.0f, bar, beat, beat_count, harmonic, scale, instrument, performer, tasks);
  };

  build_instrument_fn(0.5f, -24.0f, 0.05f, 0.05f);
  performers.emplace_back(engine.CreatePerformer(), std::vector<Task>{},
                          line_2_beat_composer_callback, instruments.size() - 1);

  // Add percussion instrument.
  instruments.emplace_back(engine.CreateInstrument());
  auto& percussion = instruments.back();
  percussion.SetControl(ControlType::kGain, -18.0f);
  percussion.SetControl(ControlType::kAttack, 0.0f);
  percussion.SetControl(ControlType::kRetrigger, true);
  percussion.SetControl(ControlType::kSliceMode, SliceMode::kOnce);
  set_note_callbacks_fn(instruments.size(), percussion);
  const auto set_percussion_pad_map_fn =
      [&](const std::vector<std::pair<float, std::string>>& percussion_map) {
        std::vector<Slice> slices;
        std::vector<std::vector<float>> samples;
        slices.reserve(percussion_map.size());
        samples.reserve(percussion_map.size());
        for (const auto& [pitch, file_path] : percussion_map) {
          samples.emplace_back();
          InsertPadData(pitch, GetDataFilePath(kDrumsDir + file_path, argv), samples.back(),
                        slices);
        }
        percussion.SetSampleData(slices);
      };
  set_percussion_pad_map_fn({
      {kPitchKick, "basic_kick.wav"},
      {kPitchSnare, "basic_snare.wav"},
      {kPitchHihatClosed, "basic_hihat_closed.wav"},
      {kPitchHihatOpen, "basic_hihat_open.wav"},
  });
  const auto percussion_beat_composer_callback =
      [&](int bar, int beat, int beat_count, int /*harmonic*/, Instrument& instrument,
          Performer& performer, std::vector<Task>& tasks) {
        ComposeDrums(bar, beat, beat_count, random, instrument, performer, tasks);
      };

  performers.emplace_back(engine.CreatePerformer(), std::vector<Task>{},
                          percussion_beat_composer_callback, instruments.size() - 1);

  // Bar callback.
  const auto bar_composer_callback = [&progression](int bar) -> int {
    return progression[bar % progression.size()];
  };

  // Beat callback.
  auto metronome = engine.CreatePerformer();
  int harmonic = 0;
  metronome.SetBeatCallback([&]() {
    // Update transport.
    int beat = static_cast<int>(metronome.GetPosition());
    const int bar = beat / kBeatCount;
    beat %= kBeatCount;

    if (beat == 0) {
      // Compose next bar.
      harmonic = bar_composer_callback(bar);
    }
    // Update members.
    for (auto& [performer, tasks, beat_composer_callback, index] : performers) {
      // Compose next beat notes.
      if (beat_composer_callback) {
        beat_composer_callback(bar, beat, kBeatCount, harmonic, instruments[index], performer,
                               tasks);
      }
    }
  });

  // Audio process callback.
  std::vector<float> temp_buffer(kSampleCount);
  const auto process_callback = [&](std::span<float> output_samples) {
    std::fill_n(output_samples.begin(), kSampleCount, 0.0f);
    for (auto& instrument : instruments) {
      instrument.Process(temp_buffer, clock.GetTimestamp());
      std::transform(temp_buffer.begin(), temp_buffer.end(), output_samples.begin(),
                     output_samples.begin(), std::plus());
    }
    clock.Update(static_cast<int>(output_samples.size()));
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
          for (auto& [performer, tasks, beat_composer_callback, index] : performers) {
            performer.Stop();
          }
          for (auto& instrument : instruments) {
            instrument.SetAllNotesOff();
          }
          ConsoleLog() << "Stopped playback";
        } else {
          for (auto& [performer, tasks, beat_composer_callback, index] : performers) {
            performer.Start();
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
        set_percussion_pad_map_fn({
            {kPitchKick, "basic_kick.wav"},
            {kPitchSnare, "basic_snare.wav"},
            {kPitchHihatClosed, "basic_hihat_closed.wav"},
            {kPitchHihatOpen, "basic_hihat_open.wav"},
        });
        break;
      case 'H':
        set_percussion_pad_map_fn({
            {kPitchKick, "basic_hihat_closed.wav"},
            {kPitchSnare, "basic_hihat_open.wav"},
            {kPitchHihatClosed, "basic_hihat_closed.wav"},
            {kPitchHihatOpen, "basic_hihat_open.wav"},
        });
        break;
      case 'Q':
        scale.mode = (scale.mode - 1 + scale.GetPitchCount()) % scale.GetPitchCount();
        ConsoleLog() << "Scale mode set to " << scale.mode;
        break;
      case 'W':
        scale.mode = 0;
        ConsoleLog() << "Scale mode reset to " << scale.mode;
        break;
      case 'E':
        scale.mode = (scale.mode + 1) % scale.GetPitchCount();
        ConsoleLog() << "Scale mode set to " << scale.mode;
        break;
      default:
        break;
    }
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  ConsoleLog() << "Starting audio stream";
  audio_output.Start();
  engine.Update(kLookahead);
  for (auto& [performer, tasks, beat_composer_callback, index] : performers) {
    performer.Start();
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
  for (auto& [performer, tasks, beat_composer_callback, index] : performers) {
    performer.Stop();
    tasks.clear();
  }
  audio_output.Stop();

  return 0;
}
