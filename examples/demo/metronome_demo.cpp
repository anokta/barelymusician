#include <cctype>
#include <chrono>
#include <cmath>
#include <thread>

#include "audio_output/pa_audio_output.h"
#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/dsp/dsp_utils.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/engine/clock.h"
#include "barelymusician/util/task_runner.h"
#include "instruments/basic_enveloped_voice.h"
#include "util/input_manager/win_console_input.h"

namespace {

using ::barelyapi::Clock;
using ::barelyapi::FrequencyFromNoteIndex;
using ::barelyapi::OscillatorType;
using ::barelyapi::TaskRunner;
using ::barelyapi::examples::BasicEnvelopedVoice;
using ::barelyapi::examples::PaAudioOutput;
using ::barelyapi::examples::WinConsoleInput;

// System audio settings.
const int kSampleRate = 48000;
const int kNumChannels = 2;
const int kNumFrames = 2048;

const int kNumMaxTasks = 100;

// Metronome settings.
const float kGain = 0.5f;
const float kBarFrequency = FrequencyFromNoteIndex(barelyapi::kNoteIndexA4);
const float kBeatFrequency = FrequencyFromNoteIndex(barelyapi::kNoteIndexA3);
const OscillatorType kOscillatorType = OscillatorType::kSquare;
const float kRelease = 0.025f;

const int kNumBeats = 4;
const double kInitialTempo = 120.0;
const double kTempoIncrement = 10.0;

// Metronome processor.
class Metronome {
 public:
  Metronome() : voice_(kSampleRate) {
    voice_.generator().SetType(kOscillatorType);
    voice_.envelope().SetRelease(kRelease);
    voice_.set_gain(kGain);
  }

  // Processes the next |output| buffer.
  void Process(float* output, int num_channels, int num_frames) {
    for (int frame = 0; frame < num_frames; ++frame) {
      const float mono_sample = voice_.Next(0);
      for (int channel = 0; channel < num_channels; ++channel) {
        output[frame * num_channels + channel] = mono_sample;
      }
    }
  }

  // Ticks the metronome with the given |beat|.
  void Tick(int beat) {
    const float frequency = (beat == 0) ? kBarFrequency : kBeatFrequency;
    voice_.generator().SetFrequency(frequency);
    voice_.Start();
    voice_.Next(0);
    voice_.Stop();
  }

 private:
  BasicEnvelopedVoice<barelyapi::Oscillator> voice_;
};

}  // namespace

int main(int argc, char* argv[]) {
  PaAudioOutput audio_output;
  WinConsoleInput input_manager;

  TaskRunner task_runner(kNumMaxTasks);

  Clock clock(kSampleRate);
  clock.SetTempo(kInitialTempo);

  Metronome metronome;

  // Audio process callback.
  const auto process_callback = [&task_runner, &clock,
                                 &metronome](float* output) {
    task_runner.Run();

    // Update clock.
    const double start_position = clock.GetPosition();
    clock.UpdatePosition(kNumFrames);
    const double end_position = clock.GetPosition();

    const double frames_per_beat =
        static_cast<double>(kNumFrames) / (end_position - start_position);

    int frame = 0;
    for (double beat = std::ceil(start_position); beat < end_position; ++beat) {
      const int beat_frame =
          static_cast<int>(frames_per_beat * (beat - start_position));
      if (frame < beat_frame) {
        metronome.Process(&output[kNumChannels * frame], kNumChannels,
                          beat_frame - frame);
        frame = beat_frame;
      }
      // Tick.
      const int current_bar = static_cast<int>(beat) / kNumBeats;
      const int current_beat = static_cast<int>(beat) % kNumBeats;
      metronome.Tick(current_beat);
      LOG(INFO) << "Tick " << current_bar << "." << current_beat;
    }
    if (frame < kNumFrames) {
      metronome.Process(&output[kNumChannels * frame], kNumChannels,
                        kNumFrames - frame);
    }
  };
  audio_output.SetProcessCallback(process_callback);

  // Key down callback.
  bool quit = false;
  const auto key_down_callback = [&quit, &task_runner,
                                  &clock](const WinConsoleInput::Key& key) {
    if (static_cast<int>(key) == 27) {
      // ESC pressed, quit the app.
      quit = true;
      return;
    }
    // Adjust tempo.
    double tempo = clock.GetTempo();
    switch (std::toupper(key)) {
      case '-':
        tempo -= kTempoIncrement;
        break;
      case '+':
        tempo += kTempoIncrement;
        break;
      case '1':
        tempo *= 0.5;
        break;
      case '2':
        tempo *= 2.0;
        break;
      case 'R':
        tempo = kInitialTempo;
        break;
      default:
        return;
    }
    task_runner.Add([&clock, tempo]() { clock.SetTempo(tempo); });
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
