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
#include "barelymusician/ensemble/performer.h"
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
using ::barelyapi::Performer;
using ::barelyapi::Sequencer;
using ::barelyapi::Transport;
using ::barelyapi::examples::BasicSynthInstrument;
using ::barelyapi::examples::BasicSynthInstrumentParam;
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

  std::vector<BasicSynthInstrument> instruments;
  std::vector<MidiBeatComposer> composers;
  std::vector<Performer> performers;

  const int num_tracks = midi_file.getTrackCount();
  const int ticks_per_quarter = midi_file.getTPQ();
  LOG(INFO) << "Initializing " << kMidiFileName << " for MIDI playback ("
            << num_tracks << " tracks, " << ticks_per_quarter << " TPQ)";

  for (int i = 0; i < num_tracks; ++i) {
    BasicSynthInstrument instrument(kSampleInterval, kNumInstrumentVoices);
    instrument.SetFloatParam(BasicSynthInstrumentParam::kOscillatorType,
                             static_cast<float>(OscillatorType::kSquare));
    instrument.SetFloatParam(BasicSynthInstrumentParam::kEnvelopeAttack, 0.0f);
    instrument.SetFloatParam(BasicSynthInstrumentParam::kEnvelopeRelease, 0.5f);
    instrument.SetFloatParam(BasicSynthInstrumentParam::kGain, 0.1f);
    instruments.push_back(instrument);

    MidiBeatComposer composer(midi_file[i], ticks_per_quarter);
    composers.push_back(composer);
  }

  DefaultSectionComposer section_composer;
  DefaultBarComposer bar_composer;
  Ensemble ensemble(&sequencer, &section_composer, &bar_composer);
  performers.reserve(num_tracks);
  for (int i = 0; i < num_tracks; ++i) {
    performers.emplace_back(&instruments[i], &composers[i]);
    ensemble.AddPerformer(&performers[i]);
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
