#include <cctype>
#include <chrono>
#include <thread>

#include "audio_output/pa_audio_output.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/base/sequencer.h"
#include "barelymusician/base/task_runner.h"
#include "barelymusician/base/transport.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/message/message_buffer.h"
#include "barelymusician/message/message_utils.h"
#include "instruments/basic_enveloped_voice.h"
#include "util/input_manager/win_console_input.h"

namespace {

using ::barelyapi::MessageBuffer;
using ::barelyapi::Oscillator;
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
const float kBeatTickFrequency = 220.0f;
const float kBarTickFrequency = 2.0f * kBeatTickFrequency;
const float kSectionTickFrequency = 4.0f * kBeatTickFrequency;
const OscillatorType kOscillatorType = OscillatorType::kSquare;
const float kRelease = 0.025f;

class Metronome {
 public:
  Metronome() : voice_(kSampleInterval) {
    voice_.generator().SetType(kOscillatorType);
    voice_.envelope().SetRelease(kRelease);
    voice_.set_gain(kGain);
  }

  void OnBeat(const Transport& transport, int start_sample) {
    float tick_frequency = kBeatTickFrequency;
    if (transport.beat == 0) {
      tick_frequency =
          (transport.bar == 0) ? kSectionTickFrequency : kBarTickFrequency;
    }
    message_buffer_.Push(
        barelyapi::BuildMessage<float>(0, tick_frequency, start_sample));
  }

  void Process(float* output, int num_channels, int num_frames) {
    int frame = 0;
    // Process messages.
    const auto messages = message_buffer_.GetIterator(0, num_frames);
    for (auto it = messages.begin; it != messages.end; ++it) {
      while (frame <= it->timestamp) {
        if (frame == it->timestamp) {
          // Tick.
          voice_.generator().SetFrequency(
              barelyapi::ReadMessageData<float>(it->data));
          voice_.Start();
        }
        const float mono_sample = voice_.Next(0);
        for (int channel = 0; channel < num_channels; ++channel) {
          output[frame * num_channels + channel] = mono_sample;
        }
        ++frame;
      }
      voice_.Stop();
    }
    message_buffer_.Clear(messages);
    // Process the rest of the buffer.
    while (frame < num_frames) {
      const float mono_sample = voice_.Next(0);
      for (int channel = 0; channel < num_channels; ++channel) {
        output[frame * num_channels + channel] = mono_sample;
      }
      ++frame;
    }
  }

 private:
  BasicEnvelopedVoice<Oscillator> voice_;
  MessageBuffer message_buffer_;
};

}  // namespace

int main(int argc, char* argv[]) {
  PaAudioOutput audio_output;
  WinConsoleInput input_manager;

  TaskRunner task_runner(kNumMaxTasks);

  Metronome metronome;

  Sequencer sequencer(kSampleRate);
  sequencer.SetTempo(kTempo);
  sequencer.SetNumBars(kNumBars);
  sequencer.SetNumBeats(kNumBeats);

  // Beat callback.
  const auto beat_callback = [&metronome](const Transport& transport,
                                          int start_sample,
                                          int num_samples_per_beat) {
    LOG(INFO) << "Tick " << transport.section << "." << transport.bar << "."
              << transport.beat;
    metronome.OnBeat(transport, start_sample);
  };
  sequencer.RegisterBeatCallback(beat_callback);

  // Audio process callback.
  const auto process_callback = [&task_runner, &sequencer,
                                 &metronome](float* output) {
    task_runner.Run();
    sequencer.Update(kNumFrames);
    metronome.Process(output, kNumChannels, kNumFrames);
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
