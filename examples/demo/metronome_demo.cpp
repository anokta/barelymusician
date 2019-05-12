#include <cctype>
#include <chrono>
#include <thread>

#include "audio_output/pa_audio_output.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/sequencer/sequencer.h"
#include "instruments/basic_synth_voice.h"
#include "util/input_manager/win_console_input.h"

namespace {

using ::barelyapi::OscillatorType;
using ::barelyapi::Sequencer;
using ::barelyapi::Transport;
using ::barelyapi::examples::BasicSynthVoice;
using ::barelyapi::examples::PaAudioOutput;
using ::barelyapi::examples::WinConsoleInput;

// System audio settings.
const int kSampleRate = 48000;
const int kNumChannels = 2;
const int kNumFrames = 512;

const float kSampleInterval = 1.0f / static_cast<float>(kSampleRate);

// Sequencer settings.
const float kTempo = 120.0f;
const int kNumBars = 4;
const int kNumBeats = 4;

// Metronome settings.
const float kGain = 0.5f;
const float kSectionFrequency = 880.0f;
const float kBarFrequency = 440.0f;
const float kBeatFrequency = 220.0f;
const OscillatorType kOscillatorType = OscillatorType::kSquare;
const float kRelease = 0.025f;
const float kTempoIncrement = 10.0f;

}  // namespace

int main(int argc, char* argv[]) {
  PaAudioOutput audio_output;
  WinConsoleInput input_manager;

  BasicSynthVoice metronome_voice(kSampleInterval);
  metronome_voice.SetGain(kGain);
  metronome_voice.SetOscillatorType(kOscillatorType);
  metronome_voice.SetEnvelopeRelease(kRelease);

  Sequencer sequencer(kSampleRate);
  sequencer.SetTempo(kTempo);
  sequencer.SetNumBars(kNumBars);
  sequencer.SetNumBeats(kNumBeats);

  // Beat callback.
  int tick_sample = 0;
  const auto beat_callback = [&metronome_voice, &tick_sample](
                                 const Transport& transport, int start_sample,
                                 int num_samples_per_beat) {
    LOG(INFO) << "Tick " << transport.section << "." << transport.bar << "."
              << transport.beat;

    float frequency = kBeatFrequency;
    if (transport.beat == 0) {
      frequency = (transport.bar == 0) ? kSectionFrequency : kBarFrequency;
    }
    metronome_voice.SetOscillatorFrequency(frequency);
    tick_sample = start_sample;
  };
  sequencer.RegisterBeatCallback(beat_callback);

  // Audio process callback.
  const auto process_callback = [&sequencer, &metronome_voice,
                                 &tick_sample](float* output) {
    tick_sample = -1;
    sequencer.Update(kNumFrames);
    for (int frame = 0; frame < kNumFrames; ++frame) {
      if (frame == tick_sample) {
        metronome_voice.Start();
      }
      const float sample = metronome_voice.Next();
      if (frame == tick_sample) {
        metronome_voice.Stop();
      }
      for (int channel = 0; channel < kNumChannels; ++channel) {
        output[kNumChannels * frame + channel] = sample;
      }
    }
  };
  audio_output.SetProcessCallback(process_callback);

  // Key down callback.
  bool quit = false;
  const auto key_down_callback = [&quit,
                                  &sequencer](const WinConsoleInput::Key& key) {
    if (static_cast<int>(key) == 27) {
      // ESC pressed, quit the app.
      quit = true;
      return;
    }
    // Adjust tempo.
    switch (std::toupper(key)) {
      case '-':
        sequencer.SetTempo(sequencer.GetTransport().tempo - kTempoIncrement);
        break;
      case '+':
        sequencer.SetTempo(sequencer.GetTransport().tempo + kTempoIncrement);
        break;
      case '1':
        sequencer.SetTempo(0.5f * sequencer.GetTransport().tempo);
        break;
      case '2':
        sequencer.SetTempo(2.0f * sequencer.GetTransport().tempo);
        break;
      case 'R':
        sequencer.SetTempo(kTempo);
        break;
      default:
        return;
    }
    LOG(INFO) << "Tempo set to " << sequencer.GetTransport().tempo;
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
