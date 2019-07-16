#include <cctype>
#include <chrono>
#include <thread>

#include "audio_output/pa_audio_output.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/base/sequencer.h"
#include "barelymusician/base/task_runner.h"
#include "barelymusician/base/transport.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/instrument/instrument.h"
#include "barelymusician/instrument/instrument_utils.h"
#include "instruments/basic_enveloped_voice.h"
#include "util/input_manager/win_console_input.h"

namespace {

using ::barelyapi::OscillatorType;
using ::barelyapi::Sequencer;
using ::barelyapi::TaskRunner;
using ::barelyapi::Transport;
using ::barelyapi::examples::BasicEnvelopedVoice;
using ::barelyapi::examples::PaAudioOutput;
using ::barelyapi::examples::WinConsoleInput;

// System audio settings.
const int kSampleRate = 48000;
const int kNumChannels = 2;
const int kNumFrames = 2048;

const float kSampleInterval = 1.0f / static_cast<float>(kSampleRate);

const int kNumMaxTasks = 100;

// Sequencer settings.
const float kTempo = 120.0f;
const int kNumBars = 4;
const int kNumBeats = 4;

const float kTempoIncrement = 10.0f;

// Metronome settings.
const float kGain = 0.5f;
const float kBeatNoteIndex = barelyapi::kNoteIndexA3;
const float kBarNoteIndex = barelyapi::kNoteIndexA4;
const float kSectionNoteIndex = barelyapi::kNoteIndexA5;
const OscillatorType kOscillatorType = OscillatorType::kSquare;
const float kRelease = 0.025f;

// Metronome instrument.
class MetronomeInstrument : public barelyapi::Instrument {
 public:
  MetronomeInstrument() : voice_(kSampleInterval) {
    voice_.generator().SetType(kOscillatorType);
    voice_.envelope().SetRelease(kRelease);
  }

  // Implements |Instrument|.
  void AllNotesOff() override { voice_.Stop(); }
  void NoteOff(float index) override { voice_.Stop(); }
  void NoteOn(float index, float intensity) override {
    voice_.generator().SetFrequency(barelyapi::FrequencyFromNoteIndex(index));
    voice_.set_gain(intensity);
    voice_.Start();
  }
  void Process(float* output, int num_channels, int num_frames) override {
    for (int frame = 0; frame < num_frames; ++frame) {
      const float mono_sample = voice_.Next(0);
      for (int channel = 0; channel < num_channels; ++channel) {
        output[frame * num_channels + channel] = mono_sample;
      }
    }
  }

 private:
  BasicEnvelopedVoice<barelyapi::Oscillator> voice_;
};

}  // namespace

int main(int argc, char* argv[]) {
  PaAudioOutput audio_output;
  WinConsoleInput input_manager;

  TaskRunner task_runner(kNumMaxTasks);

  Sequencer sequencer(kSampleRate);
  sequencer.SetTempo(kTempo);
  sequencer.SetNumBars(kNumBars);
  sequencer.SetNumBeats(kNumBeats);

  MetronomeInstrument metronome;

  // Beat callback.
  const auto beat_callback = [&metronome](const Transport& transport,
                                          int start_sample) {
    LOG(INFO) << "Tick " << transport.section << "." << transport.bar << "."
              << transport.beat;

    float note_index = kBeatNoteIndex;
    if (transport.beat == 0) {
      note_index = (transport.bar == 0) ? kSectionNoteIndex : kBarNoteIndex;
    }
    metronome.StartNote(note_index, kGain, start_sample);
    metronome.StopNote(note_index, start_sample + 1);
  };
  sequencer.SetBeatCallback(beat_callback);

  // Audio process callback.
  const auto process_callback = [&task_runner, &sequencer,
                                 &metronome](float* output) {
    task_runner.Run();
    sequencer.Update(kNumFrames);
    metronome.ProcessBuffer(output, kNumChannels, kNumFrames, 0);
  };
  audio_output.SetProcessCallback(process_callback);

  // Key down callback.
  bool quit = false;
  const auto key_down_callback = [&quit, &task_runner,
                                  &sequencer](const WinConsoleInput::Key& key) {
    if (static_cast<int>(key) == 27) {
      // ESC pressed, quit the app.
      quit = true;
      return;
    }
    // Adjust tempo.
    float tempo = sequencer.GetTransport().tempo;
    switch (std::toupper(key)) {
      case '-':
        tempo -= kTempoIncrement;
        break;
      case '+':
        tempo += kTempoIncrement;
        break;
      case '1':
        tempo *= 0.5f;
        break;
      case '2':
        tempo *= 2.0f;
        break;
      case 'R':
        tempo = kTempo;
        break;
      default:
        return;
    }
    task_runner.Add([&sequencer, tempo]() { sequencer.SetTempo(tempo); });
    LOG(INFO) << "Tempo set to " << tempo;
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
