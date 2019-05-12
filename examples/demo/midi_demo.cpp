#include <algorithm>
#include <chrono>
#include <functional>
#include <memory>
#include <thread>
#include <vector>

#include "MidiFile.h"
#include "audio_output/pa_audio_output.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/ensemble/ensemble.h"
#include "barelymusician/ensemble/performer.h"
#include "barelymusician/sequencer/sequencer.h"
#include "composers/default_bar_composer.h"
#include "composers/default_section_composer.h"
#include "composers/midi_beat_composer.h"
#include "instruments/basic_synth_instrument.h"
#include "util/input_manager/win_console_input.h"

namespace {

using ::barelyapi::Ensemble;
using ::barelyapi::OscillatorType;
using ::barelyapi::Performer;
using ::barelyapi::Sequencer;
using ::barelyapi::examples::BasicSynthInstrument;
using ::barelyapi::examples::BasicSynthInstrumentParam;
using ::barelyapi::examples::DefaultBarComposer;
using ::barelyapi::examples::DefaultSectionComposer;
using ::barelyapi::examples::MidiBeatComposer;
using ::barelyapi::examples::PaAudioOutput;
using ::barelyapi::examples::WinConsoleInput;
using ::smf::MidiFile;

// System audio settings.
const int kSampleRate = 48000;
const int kNumChannels = 2;
const int kNumFrames = 512;

const float kSampleInterval = 1.0f / static_cast<float>(kSampleRate);

// Sequencer settings.
const float kTempo = 132.0f;

// Ensemble settings.
const int kNumInstrumentVoices = 12;

// Midi file name.
const char kMidiFileName[] = "data/midi/sample.mid";

}  // namespace

int main(int argc, char* argv[]) {
  PaAudioOutput audio_output;
  WinConsoleInput input_manager;

  MidiFile midi_file;
  CHECK(midi_file.read(kMidiFileName)) << "Failed to read " << kMidiFileName;
  midi_file.linkNotePairs();

  Sequencer sequencer(kSampleRate);
  sequencer.SetTempo(kTempo);

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
    instrument.SetFloatParam(BasicSynthInstrumentParam::kEnvelopeRelease, 0.2f);
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
