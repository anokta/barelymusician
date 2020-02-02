#include <algorithm>
#include <chrono>
#include <memory>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include "audio_output/pa_audio_output.h"
#include "barelymusician/base/clock.h"
#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/dsp/dsp_utils.h"
#include "barelymusician/instrument/instrument.h"
#include "barelymusician/musician/musician.h"
#include "barelymusician/musician/note.h"
#include "barelymusician/musician/note_utils.h"
#include "barelymusician/util/random.h"
#include "instruments/basic_drumkit_instrument.h"
#include "instruments/basic_synth_instrument.h"
#include "util/input_manager/win_console_input.h"
#include "util/wav_file.h"

namespace {

using ::barelyapi::Clock;
using ::barelyapi::Instrument;
using ::barelyapi::Musician;
using ::barelyapi::Note;
using ::barelyapi::OscillatorType;
using ::barelyapi::Random;
using ::barelyapi::examples::BasicDrumkitInstrument;
using ::barelyapi::examples::BasicSynthInstrument;
using ::barelyapi::examples::BasicSynthInstrumentParam;
using ::barelyapi::examples::PaAudioOutput;
using ::barelyapi::examples::WavFile;
using ::barelyapi::examples::WinConsoleInput;

using Ensemble = Musician::Ensemble;

// System audio settings.
const int kSampleRate = 48000;
const int kNumChannels = 2;
const int kNumFrames = 512;

const float kSampleInterval = 1.0f / static_cast<float>(kSampleRate);

// Sequencer settings.
const double kTempo = 124.0;
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
    notes->push_back({note_index, intensity, 0.0, 1.0});
  };
  const float start_note = static_cast<float>(harmonic);
  add_chord_note(start_note);
  add_chord_note(start_note + 2.0f);
  add_chord_note(start_note + 4.0f);
  add_chord_note(start_note + 7.0f);
}

void ComposeLine(float root_note_index, const std::vector<float>& scale,
                 float intensity, int bar, int beat, int num_beats,
                 int harmonic, std::vector<Note>* notes) {
  const float start_note = static_cast<float>(harmonic);
  const float note_offset = static_cast<float>(beat);
  const auto add_note = [&](float index, double offset_beats, double end_beat) {
    notes->push_back({root_note_index + barelyapi::GetNoteIndex(scale, index),
                      intensity, offset_beats, end_beat});
  };
  if (beat % 2 == 1) {
    add_note(start_note, 0.0, 0.25);
    add_note(start_note - note_offset, 0.33, 0.33);
    add_note(start_note, 0.66, 0.33);
  } else {
    add_note(start_note + note_offset, 0.0, 0.25);
  }
  if (beat % 2 == 0) {
    add_note(start_note - note_offset, 0.0, 0.05);
    add_note(start_note - 2.0f * note_offset, 0.5, 0.05);
  }
  if (beat + 1 == num_beats && bar % 2 == 1) {
    add_note(start_note + 2.0f * note_offset, 0.25, 0.125);
    add_note(start_note - 2.0f * note_offset, 0.75, 0.125);
    add_note(start_note + 2.0f * note_offset, 0.5, 0.25);
  }
}

void ComposeDrums(int bar, int beat, int num_beats, std::vector<Note>* notes) {
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
  if (beat + 1 == num_beats) {
    notes->push_back(
        {barelyapi::kNoteIndexSnare, 0.75f, get_beat(2), get_beat(2)});
    if (bar % 4 == 3) {
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
  if (beat + 1 == num_beats) {
    if (bar % 4 == 3) {
      notes->push_back(
          {barelyapi::kNoteIndexHihatOpen, 0.75f, get_beat(1), get_beat(1)});
    } else if (bar % 2 == 0) {
      notes->push_back(
          {barelyapi::kNoteIndexHihatOpen, 0.75f, get_beat(3), get_beat(1)});
    }
  }
  if (beat == 0 && bar % 4 == 0) {
    notes->push_back(
        {barelyapi::kNoteIndexHihatOpen, 1.0f, get_beat(0), get_beat(2)});
  }
}

}  // namespace

int main(int argc, char* argv[]) {
  PaAudioOutput audio_output;
  WinConsoleInput input_manager;

  Musician musician(kSampleRate);
  musician.SetNumBeats(kNumBeats);
  musician.SetTempo(kTempo);

  const std::vector<int> progression = {0, 3, 4, 0};
  const std::vector<float> scale(std::cbegin(barelyapi::kMajorScale),
                                 std::cend(barelyapi::kMajorScale));

  // Ensemble.
  Ensemble& ensemble = musician.ensemble();
  ensemble.bar_composer_callback =
      [&progression](int bar, [[maybe_unused]] int num_beats) -> int {
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

  ensemble.performers.emplace_back(chords_instrument.get(),
                                   chords_beat_composer_callback);
  ensemble.performers.emplace_back(chords_2_instrument.get(),
                                   chords_beat_composer_callback);

  auto line_instrument =
      BuildSynthInstrument(OscillatorType::kSaw, 0.125f, 0.0025f, 0.125f);
  auto line_2_instrument =
      BuildSynthInstrument(OscillatorType::kSquare, 0.15f, 0.05f, 0.05f);

  const auto line_beat_composer_callback = std::bind(
      ComposeLine, kRootNote - barelyapi::kNumSemitones, scale, 1.0f,
      std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
      std::placeholders::_4, std::placeholders::_5);
  const auto line_2_beat_composer_callback =
      std::bind(ComposeLine, kRootNote, scale, 1.0f, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3,
                std::placeholders::_4, std::placeholders::_5);

  ensemble.performers.emplace_back(line_instrument.get(),
                                   line_beat_composer_callback);
  ensemble.performers.emplace_back(line_2_instrument.get(),
                                   line_2_beat_composer_callback);

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
                std::placeholders::_3, std::placeholders::_5);

  ensemble.performers.emplace_back(drumkit_instrument.get(),
                                   drumkit_beat_composer_callback);

  // Audio process callback.
  std::vector<float> temp_buffer(kNumChannels * kNumFrames);
  const auto process_callback = [&musician, &ensemble,
                                 &temp_buffer](float* output) {
    musician.Update(kNumFrames);

    std::fill_n(output, kNumChannels * kNumFrames, 0.0f);
    for (auto& it : ensemble.performers) {
      musician.Process(temp_buffer.data(), kNumChannels, kNumFrames, &it);
      std::transform(temp_buffer.cbegin(), temp_buffer.cend(), output, output,
                     std::plus<float>());
    }
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
