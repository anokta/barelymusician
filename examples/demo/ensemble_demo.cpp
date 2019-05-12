#include <chrono>
#include <iomanip>
#include <iterator>
#include <thread>
#include <unordered_map>
#include <vector>

#include "barelymusician/base/buffer.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/dsp/mixer.h"
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
#include "util/audio_io/pa_wrapper.h"
#include "util/input_manager/win_console_input.h"
#include "util/wav_file.h"

namespace {

using ::barelyapi::Buffer;
using ::barelyapi::Ensemble;
using ::barelyapi::Mixer;
using ::barelyapi::OscillatorType;
using ::barelyapi::Performer;
using ::barelyapi::Sequencer;
using ::barelyapi::Transport;
using ::barelyapi::examples::BasicDrumkitInstrument;
using ::barelyapi::examples::BasicSynthInstrument;
using ::barelyapi::examples::BasicSynthInstrumentParam;
using ::barelyapi::examples::DefaultBarComposer;
using ::barelyapi::examples::DefaultSectionComposer;
using ::barelyapi::examples::DrumkitIndices;
using ::barelyapi::examples::PaWrapper;
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
const float kRootNote = 64.0f;
const float kMajorScale[] = {0.0f, 4.0f, 5.0f, 7.0f, 9.0f, 11.0f};
const float kMinorScale[] = {0.0f, 2.0f, 3.0f, 5.0f, 7.0f, 8.0f, 10.0f};
const int kNumInstrumentVoices = 8;

}  // namespace

int main(int argc, char* argv[]) {
  PaWrapper audio_io;
  WinConsoleInput input_manager;

  Sequencer sequencer(kSampleRate);
  sequencer.SetTempo(kTempo);
  sequencer.SetNumBars(kNumBars);
  sequencer.SetNumBeats(kNumBeats);

  const std::vector<float> scale(std::begin(kMajorScale),
                                 std::end(kMajorScale));

  std::vector<Performer> performers;

  BasicSynthInstrument chords_instrument(kSampleInterval, kNumInstrumentVoices);
  chords_instrument.SetFloatParam(BasicSynthInstrumentParam::kOscillatorType,
                                  static_cast<float>(OscillatorType::kSine));
  chords_instrument.SetFloatParam(BasicSynthInstrumentParam::kGain, 0.125f);
  chords_instrument.SetFloatParam(BasicSynthInstrumentParam::kEnvelopeAttack,
                                  0.125f);
  chords_instrument.SetFloatParam(BasicSynthInstrumentParam::kEnvelopeRelease,
                                  0.125f);

  BasicSynthInstrument chords_2_instrument(kSampleInterval,
                                           kNumInstrumentVoices);
  chords_2_instrument.SetFloatParam(BasicSynthInstrumentParam::kOscillatorType,
                                    static_cast<float>(OscillatorType::kNoise));
  chords_2_instrument.SetFloatParam(BasicSynthInstrumentParam::kGain, 0.05f);
  chords_2_instrument.SetFloatParam(BasicSynthInstrumentParam::kEnvelopeAttack,
                                    0.5f);
  chords_2_instrument.SetFloatParam(BasicSynthInstrumentParam::kEnvelopeRelease,
                                    0.025f);

  SimpleChordsBeatComposer chords_composer(kRootNote - barelyapi::kNumSemitones,
                                           scale);

  performers.emplace_back(&chords_instrument, &chords_composer);
  performers.emplace_back(&chords_2_instrument, &chords_composer);

  BasicSynthInstrument line_instrument(kSampleInterval, kNumInstrumentVoices);
  line_instrument.SetFloatParam(BasicSynthInstrumentParam::kOscillatorType,
                                static_cast<float>(OscillatorType::kSaw));
  line_instrument.SetFloatParam(BasicSynthInstrumentParam::kGain, 0.125f);
  line_instrument.SetFloatParam(BasicSynthInstrumentParam::kEnvelopeAttack,
                                0.0025f);
  line_instrument.SetFloatParam(BasicSynthInstrumentParam::kEnvelopeRelease,
                                0.125f);

  SimpleLineBeatComposer line_composer(kRootNote, scale);

  performers.emplace_back(&line_instrument, &line_composer);

  BasicSynthInstrument line_2_instrument(kSampleInterval, kNumInstrumentVoices);
  line_2_instrument.SetFloatParam(BasicSynthInstrumentParam::kOscillatorType,
                                  static_cast<float>(OscillatorType::kSquare));
  line_2_instrument.SetFloatParam(BasicSynthInstrumentParam::kGain, 0.15f);
  line_2_instrument.SetFloatParam(BasicSynthInstrumentParam::kEnvelopeRelease,
                                  0.05f);
  line_2_instrument.SetFloatParam(BasicSynthInstrumentParam::kEnvelopeRelease,
                                  0.05f);

  SimpleLineBeatComposer line_2_composer(kRootNote - barelyapi::kNumSemitones,
                                         scale);

  performers.emplace_back(&line_2_instrument, &line_2_composer);

  std::unordered_map<DrumkitIndices, std::string> drumkit_map;
  drumkit_map[DrumkitIndices::kKick] = "data/audio/drums/basic_kick.wav";
  drumkit_map[DrumkitIndices::kSnare] = "data/audio/drums/basic_snare.wav";
  drumkit_map[DrumkitIndices::kHihatClosed] =
      "data/audio/drums/basic_hihat_closed.wav";
  drumkit_map[DrumkitIndices::kHihatOpen] =
      "data/audio/drums/basic_hihat_open.wav";

  BasicDrumkitInstrument drumkit_instrument(kSampleInterval);
  std::vector<WavFile> drumkit_files;
  for (const auto& it : drumkit_map) {
    drumkit_files.emplace_back();
    auto& drumkit_file = drumkit_files.back();
    CHECK(drumkit_file.Load(it.second));
    drumkit_instrument.Add(static_cast<float>(it.first), drumkit_file);
  }

  SimpleDrumkitBeatComposer drumkit_composer;

  performers.emplace_back(&drumkit_instrument, &drumkit_composer);

  DefaultSectionComposer section_composer;
  DefaultBarComposer bar_composer;
  Ensemble ensemble(&sequencer, &section_composer, &bar_composer);
  for (auto& performer : performers) {
    ensemble.AddPerformer(&performer);
  }

  // Audio process callback.
  Buffer mono_buffer(barelyapi::kNumMonoChannels, kNumFrames);
  Mixer mono_mixer(barelyapi::kNumMonoChannels, kNumFrames);
  const auto audio_process_callback = [&sequencer, &performers, &mono_buffer,
                                       &mono_mixer](float* output) {
    sequencer.Update(kNumFrames);

    mono_mixer.Reset();
    for (auto& performer : performers) {
      performer.Process(&mono_buffer);
      mono_mixer.AddInput(mono_buffer);
    }

    const auto& mono_output = mono_mixer.GetOutput();
    for (int frame = 0; frame < kNumFrames; ++frame) {
      const float sample = mono_output[frame][0];
      for (int channel = 0; channel < kNumChannels; ++channel) {
        output[kNumChannels * frame + channel] = sample;
      }
    }
  };
  audio_io.SetAudioProcessCallback(audio_process_callback);

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
  audio_io.Initialize(kSampleRate, kNumChannels, kNumFrames);

  while (!quit) {
    input_manager.Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  // Stop the demo.
  LOG(INFO) << "Stopping audio stream";

  audio_io.Shutdown();
  input_manager.Shutdown();

  return 0;
}
