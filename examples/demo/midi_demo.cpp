#include <chrono>
#include <iomanip>
#include <iterator>
#include <thread>
#include <vector>

#include "MidiFile.h"
#include "barelymusician/base/buffer.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/dsp/mixer.h"
#include "barelymusician/ensemble/ensemble.h"
#include "barelymusician/sequencer/sequencer.h"
#include "composers/default_bar_composer.h"
#include "composers/default_section_composer.h"
#include "composers/midi_beat_composer.h"
#include "instruments/basic_synth_instrument.h"
#include "util/audio_io/pa_wrapper.h"
#include "util/input_manager/win_console_input.h"

namespace {

using ::barelyapi::Buffer;
using ::barelyapi::Ensemble;
using ::barelyapi::Instrument;
using ::barelyapi::Mixer;
using ::barelyapi::OscillatorType;
using ::barelyapi::Sequencer;
using ::barelyapi::Transport;
using ::barelyapi::examples::BasicSynthInstrument;
using ::barelyapi::examples::BasicSynthInstrumentFloatParam;
using ::barelyapi::examples::DefaultBarComposer;
using ::barelyapi::examples::DefaultSectionComposer;
using ::barelyapi::examples::MidiBeatComposer;
using ::barelyapi::examples::PaWrapper;
using ::barelyapi::examples::WinConsoleInput;
using ::smf::MidiFile;

// System audio settings.
const int kSampleRate = 48000;
const int kNumChannels = 2;
const int kNumFrames = 512;

const float kSampleInterval = 1.0f / static_cast<float>(kSampleRate);

// Sequencer settings.
const float kTempo = 240.0f;
const int kNumBars = 4;
const int kNumBeats = 4;

// Ensemble settings.
const int kNumInstrumentVoices = 10;

// Midi file name.
const char kMidiFileName[] = "sample.mid";

}  // namespace

int main(int argc, char* argv[]) {
  PaWrapper audio_io;
  WinConsoleInput input_manager;

  MidiFile midi_file;
  midi_file.read(kMidiFileName);
  midi_file.linkNotePairs();

  Sequencer sequencer(kSampleRate);
  sequencer.SetTempo(kTempo);
  sequencer.SetNumBars(kNumBars);
  sequencer.SetNumBeats(kNumBeats);

  DefaultSectionComposer section_composer;
  DefaultBarComposer bar_composer;

  Ensemble ensemble(&sequencer, &section_composer, &bar_composer);

  std::vector<BasicSynthInstrument> instruments;
  std::vector<MidiBeatComposer> composers;

  const int num_tracks = midi_file.getTrackCount();
  const int ticks_per_quarter = midi_file.getTPQ();

  instruments.reserve(num_tracks);
  composers.reserve(num_tracks);
  for (int i = 0; i < num_tracks; ++i) {
    BasicSynthInstrument instrument(kSampleInterval, kNumInstrumentVoices);
    instrument.SetFloatParam(BasicSynthInstrumentFloatParam::kOscillatorType,
                             static_cast<float>(OscillatorType::kSquare));
    instrument.SetFloatParam(BasicSynthInstrumentFloatParam::kEnvelopeAttack,
                             0.001f);
    instrument.SetFloatParam(BasicSynthInstrumentFloatParam::kEnvelopeRelease,
                             0.5f);
    instrument.SetFloatParam(BasicSynthInstrumentFloatParam::kGain, 0.05f);
    instruments.push_back(instrument);

    MidiBeatComposer composer(midi_file[i], ticks_per_quarter);
    composers.push_back(composer);

    ensemble.AddPerformer(&instruments[i], &composers[i]);
  }
  // Audio process callback.
  Buffer mono_buffer(barelyapi::kNumMonoChannels, kNumFrames);
  Mixer mono_mixer(barelyapi::kNumMonoChannels, kNumFrames);
  const auto audio_process_callback = [&sequencer, &instruments, &mono_buffer,
                                       &mono_mixer](float* output) {
    sequencer.Update(kNumFrames);

    mono_mixer.Reset();
    for (auto& instrument : instruments) {
      instrument.Process(&mono_buffer);
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

    LOG(INFO) << "Pressed " << key;
  };
  input_manager.RegisterKeyDownCallback(key_down_callback);

  // Key up callback.
  const auto key_up_callback = [](const WinConsoleInput::Key& key) {
    LOG(INFO) << "Released " << key;
  };
  input_manager.RegisterKeyUpCallback(key_up_callback);

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
