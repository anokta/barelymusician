#include <algorithm>
#include <chrono>
#include <functional>
#include <memory>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include "audio_output/pa_audio_output.h"
#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/base/random.h"
#include "barelymusician/base/sequencer.h"
#include "barelymusician/base/transport.h"
#include "barelymusician/composition/ensemble.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/composition/note_utils.h"
#include "barelymusician/dsp/dsp_utils.h"
#include "barelymusician/instrument/instrument.h"
#include "barelymusician/instrument/instrument_utils.h"
#include "barelymusician/message/message_queue.h"
#include "instruments/basic_drumkit_instrument.h"
#include "instruments/basic_synth_instrument.h"
#include "util/input_manager/win_console_input.h"
#include "util/wav_file.h"

namespace {

using ::barelyapi::Ensemble;
using ::barelyapi::Instrument;
using ::barelyapi::MessageQueue;
using ::barelyapi::Note;
using ::barelyapi::OscillatorType;
using ::barelyapi::Process;
using ::barelyapi::PushNoteOffMessage;
using ::barelyapi::PushNoteOnMessage;
using ::barelyapi::Random;
using ::barelyapi::SamplesFromBeats;
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

// Note BuildNote(float root_note_index, const Scale& scale, float index, float
// intensity, float )

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
                 float intensity, const Transport& transport, int harmonic,
                 std::vector<Note>* notes) {
  const float start_note = static_cast<float>(harmonic);
  const float beat = static_cast<float>(transport.beat);
  const auto add_note = [&](float index, float start_beat,
                            float duration_beats) {
    notes->push_back({root_note_index + barelyapi::GetNoteIndex(scale, index),
                      intensity, start_beat, duration_beats});
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
}

void ComposeDrums(const Transport& transport, std::vector<Note>* notes) {
  const auto get_beat = [](int num_notes) {
    return barelyapi::GetBeatDuration(num_notes,
                                      barelyapi::kNumSixteenthNotesPerBeat);
  };
  // Kick.
  if (transport.beat % 2 == 0) {
    notes->push_back(
        {barelyapi::kNoteIndexKick, 1.0f, get_beat(0), get_beat(4)});
    if (transport.bar % 2 == 1 && transport.beat == 0) {
      notes->push_back(
          {barelyapi::kNoteIndexKick, 1.0f, get_beat(2), get_beat(2)});
    }
  }
  // Snare.
  if (transport.beat % 2 == 1) {
    notes->push_back(
        {barelyapi::kNoteIndexSnare, 1.0f, get_beat(0), get_beat(4)});
  }
  if (transport.beat + 1 == transport.num_beats) {
    notes->push_back(
        {barelyapi::kNoteIndexSnare, 0.75f, get_beat(2), get_beat(2)});
    if (transport.bar + 1 == transport.num_bars) {
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
  if (transport.beat + 1 == transport.num_beats) {
    if (transport.bar + 1 == transport.num_bars) {
      notes->push_back(
          {barelyapi::kNoteIndexHihatOpen, 0.75f, get_beat(1), get_beat(1)});
    } else if (transport.bar % 2 == 0) {
      notes->push_back(
          {barelyapi::kNoteIndexHihatOpen, 0.75f, get_beat(3), get_beat(1)});
    }
  }
  if (transport.beat == 0 && transport.bar == 0) {
    notes->push_back(
        {barelyapi::kNoteIndexHihatOpen, 1.0f, get_beat(0), get_beat(2)});
  }
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
  const std::vector<float> scale(std::begin(barelyapi::kMajorScale),
                                 std::end(barelyapi::kMajorScale));

  std::vector<std::unique_ptr<Instrument>> instruments;

  // Ensemble.
  Ensemble ensemble;
  ensemble.section_composer_callback = [](const Transport& transport) -> int {
    return transport.section;
  };
  ensemble.bar_composer_callback = [&progression](const Transport& transport,
                                                  int section_type) -> int {
    const int index = section_type * transport.num_bars + transport.bar;
    return progression[index % progression.size()];
  };

  // Synth instruments.
  auto chords_instrument =
      BuildSynthInstrument(OscillatorType::kSine, 0.125f, 0.125f, 0.125f);
  auto chords_2_instrument =
      BuildSynthInstrument(OscillatorType::kNoise, 0.05f, 0.5f, 0.025f);

  const auto chords_beat_composer_callback =
      std::bind(ComposeChord, kRootNote, scale, 0.5f, std::placeholders::_3,
                std::placeholders::_4);

  ensemble.performers.insert(
      std::make_pair(chords_instrument.get(),
                     Ensemble::Performer(chords_beat_composer_callback)));
  ensemble.performers.insert(
      std::make_pair(chords_2_instrument.get(),
                     Ensemble::Performer(chords_beat_composer_callback)));
  instruments.push_back(std::move(chords_instrument));
  instruments.push_back(std::move(chords_2_instrument));

  auto line_instrument =
      BuildSynthInstrument(OscillatorType::kSaw, 0.125f, 0.0025f, 0.125f);
  auto line_2_instrument =
      BuildSynthInstrument(OscillatorType::kSquare, 0.15f, 0.05f, 0.05f);

  const auto line_beat_composer_callback = std::bind(
      ComposeLine, kRootNote - barelyapi::kNumSemitones, scale, 1.0f,
      std::placeholders::_1, std::placeholders::_3, std::placeholders::_4);
  const auto line_2_beat_composer_callback =
      std::bind(ComposeLine, kRootNote, scale, 1.0f, std::placeholders::_1,
                std::placeholders::_3, std::placeholders::_4);

  ensemble.performers.insert(std::make_pair(
      line_instrument.get(), Ensemble::Performer(line_beat_composer_callback)));
  ensemble.performers.insert(
      std::make_pair(line_2_instrument.get(),
                     Ensemble::Performer(line_2_beat_composer_callback)));
  instruments.push_back(std::move(line_instrument));
  instruments.push_back(std::move(line_2_instrument));

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
      std::bind(ComposeDrums, std::placeholders::_1, std::placeholders::_4);

  ensemble.performers.insert(
      std::make_pair(drumkit_instrument.get(),
                     Ensemble::Performer(drumkit_beat_composer_callback)));
  instruments.push_back(std::move(drumkit_instrument));

  // Beat callback.
  int section_type = 0;
  int harmonic = 0;
  std::vector<Note> temp_notes;
  const auto beat_callback = [&ensemble, &section_type, &harmonic, &temp_notes](
                                 const Transport& transport, int start_sample) {
    if (transport.beat == 0) {
      // New bar.
      if (transport.bar == 0) {
        // New section.
        section_type = ensemble.section_composer_callback(transport);
      }
      harmonic = ensemble.bar_composer_callback(transport, section_type);
    }
    for (auto& it : ensemble.performers) {
      temp_notes.clear();
      auto& performer = it.second;
      performer.beat_composer_callback(transport, section_type, harmonic,
                                       &temp_notes);
      for (const Note& note : temp_notes) {
        const int note_on_timestamp =
            start_sample +
            SamplesFromBeats(note.start_beat, transport.num_samples_per_beat);
        PushNoteOnMessage(note.index, note.intensity, note_on_timestamp,
                          &performer.messages);
        const int note_off_timestamp =
            note_on_timestamp +
            SamplesFromBeats(note.duration_beats,
                             transport.num_samples_per_beat);
        PushNoteOffMessage(note.index, note_off_timestamp, &performer.messages);
      }
    }
  };
  sequencer.RegisterBeatCallback(beat_callback);

  // Audio process callback.
  std::vector<float> temp_buffer(kNumChannels * kNumFrames);
  const auto process_callback = [&sequencer, &ensemble,
                                 &temp_buffer](float* output) {
    sequencer.Update(kNumFrames);

    std::fill_n(output, kNumChannels * kNumFrames, 0.0f);
    for (auto& performer : ensemble.performers) {
      Instrument* instrument = performer.first;
      MessageQueue* messages = &performer.second.messages;
      Process(instrument, messages, temp_buffer.data(), kNumChannels,
              kNumFrames);
      std::transform(temp_buffer.begin(), temp_buffer.end(), output, output,
                     std::plus<float>());
      messages->Update(kNumFrames);
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

  sequencer.Start();
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
