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
#include "barelymusician/ensemble/ensemble.h"
#include "barelymusician/ensemble/performer.h"
#include "barelymusician/sequencer/sequencer.h"
#include "composers/default_bar_composer.h"
#include "composers/default_section_composer.h"
#include "composers/simple_chords_beat_composer.h"
#include "composers/simple_drumkit_beat_composer.h"
#include "composers/simple_line_beat_composer.h"
#include "instruments/basic_drumkit_instrument.h"
#include "instruments/basic_synth_instrument.h"
#include "util/input_manager/win_console_input.h"
#include "util/wav_file.h"

namespace {

using ::barelyapi::Ensemble;
using ::barelyapi::OscillatorType;
using ::barelyapi::Performer;
using ::barelyapi::Sequencer;
using ::barelyapi::examples::BasicDrumkitInstrument;
using ::barelyapi::examples::BasicSynthInstrument;
using ::barelyapi::examples::BasicSynthInstrumentParam;
using ::barelyapi::examples::DefaultBarComposer;
using ::barelyapi::examples::DefaultSectionComposer;
using ::barelyapi::examples::PaAudioOutput;
using ::barelyapi::examples::SimpleChordsBeatComposer;
using ::barelyapi::examples::SimpleDrumkitBeatComposer;
using ::barelyapi::examples::SimpleLineBeatComposer;
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
const float kMajorScale[] = {0.0f, 4.0f, 5.0f, 7.0f, 9.0f, 11.0f};
const float kMinorScale[] = {0.0f, 2.0f, 3.0f, 5.0f, 7.0f, 8.0f, 10.0f};
const int kNumInstrumentVoices = 8;

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

}  // namespace

int main(int argc, char* argv[]) {
  PaAudioOutput audio_output;
  WinConsoleInput input_manager;

  Sequencer sequencer(kSampleRate);
  sequencer.SetTempo(kTempo);
  sequencer.SetNumBars(kNumBars);
  sequencer.SetNumBeats(kNumBeats);

  const std::vector<float> scale(std::begin(kMajorScale),
                                 std::end(kMajorScale));

  std::vector<Performer> performers;

  // Synth instruments.
  BasicSynthInstrument chords_instrument =
      BuildSynthInstrument(OscillatorType::kSine, 0.125f, 0.125f, 0.125f);
  BasicSynthInstrument chords_2_instrument =
      BuildSynthInstrument(OscillatorType::kNoise, 0.05f, 0.5f, 0.025f);
  SimpleChordsBeatComposer chords_composer(kRootNote - barelyapi::kNumSemitones,
                                           scale);
  performers.emplace_back(&chords_instrument, &chords_composer);
  performers.emplace_back(&chords_2_instrument, &chords_composer);

  BasicSynthInstrument line_instrument =
      BuildSynthInstrument(OscillatorType::kSaw, 0.125f, 0.0025f, 0.125f);
  SimpleLineBeatComposer line_composer(kRootNote, scale);
  performers.emplace_back(&line_instrument, &line_composer);

  BasicSynthInstrument line_2_instrument =
      BuildSynthInstrument(OscillatorType::kSquare, 0.15f, 0.05f, 0.05f);
  SimpleLineBeatComposer line_2_composer(kRootNote - barelyapi::kNumSemitones,
                                         scale);
  performers.emplace_back(&line_2_instrument, &line_2_composer);

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
  SimpleDrumkitBeatComposer drumkit_composer;
  performers.emplace_back(&drumkit_instrument, &drumkit_composer);

  // Ensemble.
  DefaultSectionComposer section_composer;
  DefaultBarComposer bar_composer;
  Ensemble ensemble(&sequencer, &section_composer, &bar_composer);
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
