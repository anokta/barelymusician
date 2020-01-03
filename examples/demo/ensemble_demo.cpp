#include <algorithm>
#include <chrono>
#include <functional>
#include <memory>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include "audio_output/pa_audio_output.h"
#include "barelymusician/base/clock.h"
#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/base/random.h"
#include "barelymusician/dsp/dsp_utils.h"
#include "barelymusician/instrument/instrument.h"
#include "barelymusician/message/message_buffer.h"
#include "barelymusician/musician/ensemble.h"
#include "barelymusician/musician/note.h"
#include "barelymusician/musician/note_utils.h"
#include "instruments/basic_drumkit_instrument.h"
#include "instruments/basic_synth_instrument.h"
#include "util/input_manager/win_console_input.h"
#include "util/wav_file.h"

namespace {

using ::barelyapi::Clock;
using ::barelyapi::Ensemble;
using ::barelyapi::Instrument;
using ::barelyapi::MessageBuffer;
using ::barelyapi::Note;
using ::barelyapi::OscillatorType;
using ::barelyapi::Random;
using ::barelyapi::SamplesFromBeats;
using ::barelyapi::examples::BasicDrumkitInstrument;
using ::barelyapi::examples::BasicSynthInstrument;
using ::barelyapi::examples::BasicSynthInstrumentParam;
using ::barelyapi::examples::PaAudioOutput;
using ::barelyapi::examples::WavFile;
using ::barelyapi::examples::WinConsoleInput;

// System audio settings.
const int kSampleRate = 48000;
const int kNumChannels = 2;
const int kNumFrames = 512;

const float kSampleInterval = 1.0f / static_cast<float>(kSampleRate);

// Sequencer settings.
const float kTempo = 124.0f;
const int kNumBars = 4;
const int kNumBeats = 3;

// Ensemble settings.
const float kRootNote = barelyapi::kNoteIndexD3;
const int kNumInstrumentVoices = 8;

std::unique_ptr<BasicSynthInstrument> BuildSynthInstrument(OscillatorType type,
                                                           float gain,
                                                           float attack,
                                                           float release) {
  auto synth_instrument = std::make_unique<BasicSynthInstrument>(
      kSampleInterval, kNumInstrumentVoices);
  synth_instrument->SetFloatParam(BasicSynthInstrumentParam::kOscillatorType,
                                  static_cast<float>(type));
  synth_instrument->SetFloatParam(BasicSynthInstrumentParam::kGain, gain);
  synth_instrument->SetFloatParam(BasicSynthInstrumentParam::kEnvelopeAttack,
                                  attack);
  synth_instrument->SetFloatParam(BasicSynthInstrumentParam::kEnvelopeRelease,
                                  release);
  return std::move(synth_instrument);
}

void ComposeChord(float root_note_index, const std::vector<float>& scale,
                  float intensity, int harmonic, std::vector<Note>* notes) {
  const auto add_chord_note = [&](float index) {
    const float note_index =
        root_note_index + barelyapi::GetNoteIndex(scale, index);
    notes->push_back({note_index, intensity, 0.0f, 1.0f});
  };
  const float start_note = static_cast<float>(harmonic);
  add_chord_note(start_note);
  add_chord_note(start_note + 2.0f);
  add_chord_note(start_note + 4.0f);
  add_chord_note(start_note + 7.0f);
}

void ComposeLine(float root_note_index, const std::vector<float>& scale,
                 float intensity, int bar, int beat, int harmonic,
                 std::vector<Note>* notes) {
  const float start_note = static_cast<float>(harmonic);
  const float note_offset = static_cast<float>(beat);
  const auto add_note = [&](float index, float start_beat, float end_beat) {
    notes->push_back({root_note_index + barelyapi::GetNoteIndex(scale, index),
                      intensity, start_beat, end_beat});
  };
  if (beat % 2 == 1) {
    add_note(start_note, 0.0f, 0.25f);
    add_note(start_note - note_offset, 0.33f, 0.33f);
    add_note(start_note, 0.66f, 0.33f);
  } else {
    add_note(start_note + note_offset, 0.0f, 0.25f);
  }
  if (beat % 2 == 0) {
    add_note(start_note - note_offset, 0.0f, 0.05f);
    add_note(start_note - 2.0f * note_offset, 0.5f, 0.05f);
  }
  if (beat + 1 == kNumBeats && bar % 2 == 1) {
    add_note(start_note + 2.0f * note_offset, 0.25f, 0.125f);
    add_note(start_note - 2.0f * note_offset, 0.75f, 0.125f);
    add_note(start_note + 2.0f * note_offset, 0.5f, 0.25f);
  }
}

void ComposeDrums(int bar, int beat, std::vector<Note>* notes) {
  const auto get_beat = [](int step) {
    return barelyapi::GetBeat(step, barelyapi::kNumSixteenthNotesPerBeat);
  };
  // Kick.
  if (beat % 2 == 0) {
    notes->push_back(
        {barelyapi::kNoteIndexKick, 1.0f, get_beat(0), get_beat(2)});
    if (bar % 2 == 1 && beat == 0) {
      notes->push_back(
          {barelyapi::kNoteIndexKick, 1.0f, get_beat(2), get_beat(2)});
    }
  }
  // Snare.
  if (beat % 2 == 1) {
    notes->push_back(
        {barelyapi::kNoteIndexSnare, 1.0f, get_beat(0), get_beat(2)});
  }
  if (beat + 1 == kNumBeats) {
    notes->push_back(
        {barelyapi::kNoteIndexSnare, 0.75f, get_beat(2), get_beat(2)});
    if (bar + 1 == kNumBars) {
      notes->push_back(
          {barelyapi::kNoteIndexSnare, 1.0f, get_beat(1), get_beat(1)});
      notes->push_back(
          {barelyapi::kNoteIndexSnare, 0.75f, get_beat(3), get_beat(1)});
    }
  }
  // Hihat Closed.
  notes->push_back({barelyapi::kNoteIndexHihatClosed,
                    Random::Uniform(0.5f, 0.75f), get_beat(0), get_beat(2)});
  notes->push_back({barelyapi::kNoteIndexHihatClosed,
                    Random::Uniform(0.25f, 0.75f), get_beat(2), get_beat(2)});
  // Hihat Open.
  if (beat + 1 == kNumBeats) {
    if (bar + 1 == kNumBars) {
      notes->push_back(
          {barelyapi::kNoteIndexHihatOpen, 0.75f, get_beat(1), get_beat(1)});
    } else if (bar % 2 == 0) {
      notes->push_back(
          {barelyapi::kNoteIndexHihatOpen, 0.75f, get_beat(3), get_beat(1)});
    }
  }
  if (beat == 0 && bar == 0) {
    notes->push_back(
        {barelyapi::kNoteIndexHihatOpen, 1.0f, get_beat(0), get_beat(2)});
  }
}

}  // namespace

int main(int argc, char* argv[]) {
  PaAudioOutput audio_output;
  WinConsoleInput input_manager;

  Clock clock(kSampleRate);
  clock.SetTempo(kTempo);

  const std::vector<int> progression = {0, 3, 4, 0};
  const std::vector<float> scale(std::begin(barelyapi::kMajorScale),
                                 std::end(barelyapi::kMajorScale));

  // Ensemble.
  Ensemble ensemble;
  ensemble.section_composer_callback = [](int section) -> int {
    return section;
  };
  ensemble.bar_composer_callback = [&progression](int bar, int, int) -> int {
    return progression[bar % progression.size()];
  };

  // Synth instruments.
  auto chords_instrument =
      BuildSynthInstrument(OscillatorType::kSine, 0.125f, 0.125f, 0.125f);
  auto chords_2_instrument =
      BuildSynthInstrument(OscillatorType::kNoise, 0.05f, 0.5f, 0.025f);

  const auto chords_beat_composer_callback =
      std::bind(ComposeChord, kRootNote, scale, 0.5f, std::placeholders::_4,
                std::placeholders::_5);

  ensemble.performers.emplace_back(
      std::make_pair(chords_instrument.get(), chords_beat_composer_callback));
  ensemble.performers.emplace_back(
      std::make_pair(chords_2_instrument.get(), chords_beat_composer_callback));

  auto line_instrument =
      BuildSynthInstrument(OscillatorType::kSaw, 0.125f, 0.0025f, 0.125f);
  auto line_2_instrument =
      BuildSynthInstrument(OscillatorType::kSquare, 0.15f, 0.05f, 0.05f);

  const auto line_beat_composer_callback =
      std::bind(ComposeLine, kRootNote - barelyapi::kNumSemitones, scale, 1.0f,
                std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_4, std::placeholders::_5);
  const auto line_2_beat_composer_callback = std::bind(
      ComposeLine, kRootNote, scale, 1.0f, std::placeholders::_1,
      std::placeholders::_2, std::placeholders::_4, std::placeholders::_5);

  ensemble.performers.emplace_back(
      std::make_pair(line_instrument.get(), line_beat_composer_callback));
  ensemble.performers.emplace_back(
      std::make_pair(line_2_instrument.get(), line_2_beat_composer_callback));

  // Drumkit instrument.
  std::unordered_map<float, std::string> drumkit_map;
  drumkit_map[barelyapi::kNoteIndexKick] = "data/audio/drums/basic_kick.wav";
  drumkit_map[barelyapi::kNoteIndexSnare] = "data/audio/drums/basic_snare.wav";
  drumkit_map[barelyapi::kNoteIndexHihatClosed] =
      "data/audio/drums/basic_hihat_closed.wav";
  drumkit_map[barelyapi::kNoteIndexHihatOpen] =
      "data/audio/drums/basic_hihat_open.wav";
  auto drumkit_instrument =
      std::make_unique<BasicDrumkitInstrument>(kSampleInterval);
  std::vector<WavFile> drumkit_files;
  for (const auto& it : drumkit_map) {
    drumkit_files.emplace_back();
    auto& drumkit_file = drumkit_files.back();
    CHECK(drumkit_file.Load(it.second));
    drumkit_instrument->Add(it.first, drumkit_file);
  }

  const auto drumkit_beat_composer_callback =
      std::bind(ComposeDrums, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_5);

  ensemble.performers.emplace_back(
      std::make_pair(drumkit_instrument.get(), drumkit_beat_composer_callback));

  // Beat callback.
  const int num_samples_per_beat = clock.GetNumSamplesPerBeat();
  int section = 0;
  int bar = 0;
  int section_type = 0;
  int harmonic = 0;
  std::vector<Note> temp_notes;
  int timestamp = 0;
  const auto beat_callback = [&num_samples_per_beat, &ensemble, &section, &bar,
                              &section_type, &harmonic, &temp_notes,
                              &timestamp](int beat, int leftover_samples) {
    bar = beat / kNumBeats;
    beat %= kNumBeats;
    section = bar / kNumBars;
    bar %= kNumBars;
    if (beat == 0) {
      // New bar.
      if (bar == 0) {
        // New section.
        section_type = ensemble.section_composer_callback(section);
      }
      harmonic = ensemble.bar_composer_callback(bar, kNumBars, section_type);
    }
    for (const auto& it : ensemble.performers) {
      temp_notes.clear();
      it.second(bar, beat, section_type, harmonic, &temp_notes);
      const int beat_timestamp = timestamp + kNumFrames - leftover_samples;
      for (const Note& note : temp_notes) {
        const int note_on_timestamp =
            beat_timestamp +
            SamplesFromBeats(note.start_beat, num_samples_per_beat);
        it.first->NoteOnScheduled(note.index, note.intensity,
                                  note_on_timestamp);
        const int note_off_timestamp =
            beat_timestamp +
            SamplesFromBeats(note.start_beat + note.duration_beats,
                             num_samples_per_beat);
        it.first->NoteOffScheduled(note.index, note_off_timestamp);
      }
    }
  };
  clock.SetBeatCallback(beat_callback);

  // Audio process callback.
  std::vector<float> temp_buffer(kNumChannels * kNumFrames);
  const auto process_callback = [&clock, &ensemble, &temp_buffer,
                                 &timestamp](float* output) {
    clock.Update(kNumFrames);

    std::fill_n(output, kNumChannels * kNumFrames, 0.0f);
    for (const auto& it : ensemble.performers) {
      it.first->ProcessScheduled(temp_buffer.data(), kNumChannels, kNumFrames,
                                 timestamp);
      std::transform(temp_buffer.begin(), temp_buffer.end(), output, output,
                     std::plus<float>());
    }
    timestamp += kNumFrames;
  };
  audio_output.SetProcessCallback(process_callback);

  // Key down callback.
  bool quit = false;
  const auto key_down_callback = [&quit](const WinConsoleInput::Key& key) {
    if (static_cast<int>(key) == 27) {
      // ESC pressed, quit the app.
      quit = true;
      return;
    }
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  LOG(INFO) << "Starting audio stream";

  input_manager.Initialize();
  audio_output.Start(kSampleRate, kNumChannels, kNumFrames);

  while (!quit) {
    input_manager.Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  // Stop the demo.
  LOG(INFO) << "Stopping audio stream";

  audio_output.Stop();
  input_manager.Shutdown();

  return 0;
}
