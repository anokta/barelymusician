#include <algorithm>
#include <chrono>
#include <functional>
#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>

#include "audio_output/pa_audio_output.h"
#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/base/random.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/composition/note_utils.h"
#include "barelymusician/ensemble/ensemble.h"
#include "barelymusician/ensemble/performer.h"
#include "barelymusician/sequencer/sequencer.h"
#include "barelymusician/sequencer/transport.h"
#include "instruments/basic_drumkit_instrument.h"
#include "instruments/basic_synth_instrument.h"
#include "util/input_manager/win_console_input.h"
#include "util/wav_file.h"

namespace {

using ::barelyapi::Ensemble;
using ::barelyapi::Note;
using ::barelyapi::OscillatorType;
using ::barelyapi::Performer;
using ::barelyapi::Random;
using ::barelyapi::Sequencer;
using ::barelyapi::Transport;
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
const float kMajorScale[] = {0.0f, 2.0f, 4.0f, 5.0f, 7.0f, 9.0f, 11.0f};
const float kMinorScale[] = {0.0f, 2.0f, 3.0f, 5.0f, 7.0f, 8.0f, 10.0f};
const int kNumInstrumentVoices = 8;

Note BuildNote(const std::vector<float>& scale, float root_note, float index,
               float intensity, float start_beat, float duration_beats) {
  Note note;
  note.index = root_note + barelyapi::GetScaledNoteIndex(index, scale);
  note.intensity = intensity;
  note.start_beat = start_beat;
  note.duration_beats = duration_beats;
  return note;
}

BasicSynthInstrument BuildSynthInstrument(OscillatorType type, float gain,
                                          float attack, float release) {
  BasicSynthInstrument synth_instrument(kSampleInterval, kNumInstrumentVoices);
  synth_instrument.SetFloatParam(BasicSynthInstrumentParam::kOscillatorType,
                                 static_cast<float>(type));
  synth_instrument.SetFloatParam(BasicSynthInstrumentParam::kGain, gain);
  synth_instrument.SetFloatParam(BasicSynthInstrumentParam::kEnvelopeAttack,
                                 attack);
  synth_instrument.SetFloatParam(BasicSynthInstrumentParam::kEnvelopeRelease,
                                 release);
  return synth_instrument;
}

std::vector<Note> ComposeChord(const std::vector<float>& scale, float root_note,
                               float intensity, int harmonic) {
  std::vector<Note> notes;
  const float start_note = static_cast<float>(harmonic);
  notes.push_back(
      BuildNote(scale, root_note, start_note, intensity, 0.0f, 1.0f));
  notes.push_back(
      BuildNote(scale, root_note, start_note + 2.0f, intensity, 0.0f, 1.0f));
  notes.push_back(
      BuildNote(scale, root_note, start_note + 4.0f, intensity, 0.0f, 1.0f));
  notes.push_back(
      BuildNote(scale, root_note, start_note + 7.0f, intensity, 0.0f, 1.0f));
  return notes;
}

std::vector<Note> ComposeLine(const std::vector<float>& scale, float root_note,
                              float intensity, const Transport& transport,
                              int harmonic) {
  std::vector<Note> notes;
  const float start_note = static_cast<float>(harmonic);
  const float beat = static_cast<float>(transport.beat);
  const auto add_note = [&](float index, float start_beat,
                            float duration_beats) {
    notes.push_back(BuildNote(scale, root_note, index, intensity, start_beat,
                              duration_beats));
  };
  if (transport.beat % 2 == 1) {
    add_note(start_note, 0.0f, 0.25f);
    add_note(start_note - beat, 0.33f, 0.25f);
    add_note(start_note, 0.66f, 0.25f);
  } else {
    add_note(start_note + beat, 0.0f, 0.25f);
  }
  if (transport.beat % 2 == 0) {
    add_note(start_note - beat, 0.0f, 0.05f);
    add_note(start_note - 2.0f * beat, 0.5f, 0.05f);
  }
  if (transport.beat + 1 == transport.num_beats && transport.bar % 2 == 1) {
    add_note(start_note + 2.0f * beat, 0.25f, 0.125f);
    add_note(start_note - 2.0f * beat, 0.75f, 0.125f);
    add_note(start_note + 2.0f * beat, 0.5f, 0.25f);
  }
  return notes;
}

std::vector<Note> ComposeDrums(const Transport& transport) {
  std::vector<Note> notes;
  // Kick.
  if (transport.beat % 2 == 0) {
    notes.push_back({barelyapi::kNoteIndexKick, 1.0f, 0.0f, 1.0f});
    if (transport.bar % 2 == 1 && transport.beat == 0) {
      notes.push_back({barelyapi::kNoteIndexKick, 1.0f, 0.5f, 0.5f});
    }
  }
  // Snare.
  if (transport.beat % 2 == 1) {
    notes.push_back({barelyapi::kNoteIndexSnare, 1.0f, 0.0f, 1.0f});
  }
  if (transport.beat + 1 == transport.num_beats) {
    notes.push_back({barelyapi::kNoteIndexSnare, 0.75f, 0.5f, 0.5f});
    if (transport.bar + 1 == transport.num_bars) {
      notes.push_back({barelyapi::kNoteIndexSnare, 1.0f, 0.25f, 0.25f});
      notes.push_back({barelyapi::kNoteIndexSnare, 0.75f, 0.75f, 0.25f});
    }
  }
  // Hihat Closed.
  notes.push_back({barelyapi::kNoteIndexHihatClosed,
                   Random::Uniform(0.5f, 0.75f), 0.0f, 0.5f});
  notes.push_back({barelyapi::kNoteIndexHihatClosed,
                   Random::Uniform(0.25f, 0.75f), 0.5f, 0.5f});
  // Hihat Open.
  if (transport.beat + 1 == transport.num_beats) {
    if (transport.bar + 1 == transport.num_bars) {
      notes.push_back({barelyapi::kNoteIndexHihatOpen, 0.75f, 0.25f, 0.25f});
    } else if (transport.bar % 2 == 0) {
      notes.push_back({barelyapi::kNoteIndexHihatOpen, 0.75f, 0.75f, 0.25f});
    }
  }
  if (transport.beat == 0 && transport.bar == 0) {
    notes.push_back({barelyapi::kNoteIndexHihatOpen, 1.0f, 0.0f, 0.5f});
  }
  return notes;
}

}  // namespace

int main(int argc, char* argv[]) {
  PaAudioOutput audio_output;
  WinConsoleInput input_manager;

  Sequencer sequencer(kSampleRate);
  sequencer.SetTempo(kTempo);
  sequencer.SetNumBars(kNumBars);
  sequencer.SetNumBeats(kNumBeats);

  const std::vector<int> progression = {0, 3, 4, 0};
  const std::vector<float> scale(std::begin(kMajorScale),
                                 std::end(kMajorScale));

  std::vector<Performer> performers;

  // Synth instruments.

  BasicSynthInstrument chords_instrument =
      BuildSynthInstrument(OscillatorType::kSine, 0.125f, 0.125f, 0.125f);
  BasicSynthInstrument chords_2_instrument =
      BuildSynthInstrument(OscillatorType::kNoise, 0.05f, 0.5f, 0.025f);

  const float chords_root_note = kRootNote - barelyapi::kNumSemitones;
  const auto chords_beat_composer_callback =
      [&chords_root_note, &scale](const Transport& transport, int section_type,
                                  int harmonic) -> std::vector<Note> {
    return ComposeChord(scale, chords_root_note, 0.5f, harmonic);
  };

  performers.emplace_back(&chords_instrument, chords_beat_composer_callback);
  performers.emplace_back(&chords_2_instrument, chords_beat_composer_callback);

  BasicSynthInstrument line_instrument =
      BuildSynthInstrument(OscillatorType::kSaw, 0.125f, 0.0025f, 0.125f);
  BasicSynthInstrument line_2_instrument =
      BuildSynthInstrument(OscillatorType::kSquare, 0.15f, 0.05f, 0.05f);

  const float line_root_note = kRootNote - barelyapi::kNumSemitones;
  const auto line_beat_composer_callback =
      [&line_root_note, &scale](const Transport& transport, int section_type,
                                int harmonic) -> std::vector<Note> {
    return ComposeLine(scale, line_root_note, 1.0f, transport, harmonic);
  };
  const float line_2_root_note = kRootNote;
  const auto line_2_beat_composer_callback =
      [&line_2_root_note, &scale](const Transport& transport, int section_type,
                                  int harmonic) -> std::vector<Note> {
    return ComposeLine(scale, line_2_root_note, 1.0f, transport, harmonic);
  };

  performers.emplace_back(&line_instrument, line_beat_composer_callback);
  performers.emplace_back(&line_2_instrument, line_2_beat_composer_callback);

  // Drumkit instrument.
  std::unordered_map<float, std::string> drumkit_map;
  drumkit_map[barelyapi::kNoteIndexKick] = "data/audio/drums/basic_kick.wav";
  drumkit_map[barelyapi::kNoteIndexSnare] = "data/audio/drums/basic_snare.wav";
  drumkit_map[barelyapi::kNoteIndexHihatClosed] =
      "data/audio/drums/basic_hihat_closed.wav";
  drumkit_map[barelyapi::kNoteIndexHihatOpen] =
      "data/audio/drums/basic_hihat_open.wav";
  BasicDrumkitInstrument drumkit_instrument(kSampleInterval);
  std::vector<WavFile> drumkit_files;
  for (const auto& it : drumkit_map) {
    drumkit_files.emplace_back();
    auto& drumkit_file = drumkit_files.back();
    CHECK(drumkit_file.Load(it.second));
    drumkit_instrument.Add(it.first, drumkit_file);
  }

  const auto drumkit_beat_composer_callback =
      [](const Transport& transport, int section_type,
         int harmonic) -> std::vector<Note> { return ComposeDrums(transport); };

  performers.emplace_back(&drumkit_instrument, drumkit_beat_composer_callback);

  // Ensemble.
  const auto section_composer_callback = [](const Transport& transport) -> int {
    return transport.section;
  };
  const auto bar_composer_callback = [&progression](const Transport& transport,
                                                    int section_type) -> int {
    const int index = section_type * transport.num_bars + transport.bar;
    return progression[index % progression.size()];
  };
  Ensemble ensemble(&sequencer);
  ensemble.SetSectionComposerCallback(section_composer_callback);
  ensemble.SetBarComposerCallback(bar_composer_callback);
  for (auto& performer : performers) {
    ensemble.AddPerformer(&performer);
  }

  // Audio process callback.
  std::vector<float> temp_buffer(kNumChannels * kNumFrames);
  const auto process_callback = [&sequencer, &performers,
                                 &temp_buffer](float* output) {
    sequencer.Update(kNumFrames);

    std::fill_n(output, kNumChannels * kNumFrames, 0.0f);
    for (auto& performer : performers) {
      performer.Process(temp_buffer.data(), kNumChannels, kNumFrames);
      std::transform(temp_buffer.begin(), temp_buffer.end(), output, output,
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
  input_manager.RegisterKeyDownCallback(key_down_callback);

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
