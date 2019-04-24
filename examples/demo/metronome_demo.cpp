#include <conio.h>

#include <cctype>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/base/sequencer.h"
#include "barelymusician/dsp/envelope.h"
#include "barelymusician/dsp/oscillator.h"
#include "util/audio_io/pa_wrapper.h"
#include "util/input_manager/win_console_input.h"

using barelyapi::Envelope;
using barelyapi::Oscillator;
using barelyapi::OscillatorType;
using barelyapi::Sequencer;
using barelyapi::examples::PaWrapper;
using barelyapi::examples::WinConsoleInput;

namespace {

// System audio settings.
const int kSampleRate = 48000;
const int kNumChannels = 2;
const int kFramesPerBuffer = 512;

const float kSampleInterval = 1.0f / static_cast<float>(kSampleRate);

// Sequencer settings.
const float kTempo = 120.0f;
const int kNumBars = 4;
const int kNumBeats = 4;

// Metronome settings.
const float kBarFrequency = 440.0f;
const float kBeatFrequency = 220.0f;
const OscillatorType kOscillatorType = OscillatorType::kSquare;
const float kRelease = 0.025f;

const float kMetronomeTempoIncrement = 10.0f;

}  // namespace

int main(int argc, char* argv[]) {
  Sequencer sequencer(kSampleRate);
  sequencer.SetTempo(kTempo);
  sequencer.SetNumBars(kNumBars);
  sequencer.SetNumBeats(kNumBeats);

  Oscillator oscillator(kSampleInterval);
  oscillator.SetType(kOscillatorType);
  oscillator.SetFrequency(kBarFrequency);
  Envelope envelope(kSampleInterval);
  envelope.SetRelease(kRelease);

  WinConsoleInput input_manager;
  PaWrapper audio_io;

  const auto process = [&sequencer, &oscillator, &envelope](float* output) {
    const auto previous_transport = sequencer.GetTransport();
    int impulse_sample = -1;
    if (previous_transport.offset_beats == 0) {
      impulse_sample = 0;
    }
    sequencer.Update(kFramesPerBuffer);
    const auto current_transport = sequencer.GetTransport();
    const float num_samples_per_beat =
        (current_transport.tempo > 0.0f)
            ? barelyapi::kSecondsFromMinutes * static_cast<float>(kSampleRate) /
                  current_transport.tempo
            : 0.0f;
    if (current_transport.bar != previous_transport.bar) {
      oscillator.SetFrequency(kBarFrequency);
      impulse_sample =
          kFramesPerBuffer - static_cast<int>(current_transport.offset_beats *
                                              num_samples_per_beat);
    } else if (current_transport.beat != previous_transport.beat) {
      oscillator.SetFrequency(kBeatFrequency);
      impulse_sample =
          kFramesPerBuffer - static_cast<int>(current_transport.offset_beats *
                                              num_samples_per_beat);
    }
    for (int frame = 0; frame < kFramesPerBuffer; ++frame) {
      if (frame == impulse_sample) {
        oscillator.Reset();
        envelope.Start();
      }

      const float sample = envelope.Next() * oscillator.Next();
      if (frame == impulse_sample) {
        LOG(INFO) << "Transport " << current_transport.section << "."
                  << current_transport.bar << "." << current_transport.beat;
        envelope.Stop();
      }

      for (int channel = 0; channel < kNumChannels; ++channel) {
        output[kNumChannels * frame + channel] = sample;
      }
    }
  };
  audio_io.SetAudioProcessCallback(process);

  bool quit = false;
  const auto on_key_down = [&quit,
                            &sequencer](const WinConsoleInput::Key& key) {
    if (static_cast<int>(key) == 27) {
      // ESC pressed, quit the app.
      quit = true;
      return;
    }

    switch (std::toupper(key)) {
      case '-':
        sequencer.SetTempo(sequencer.GetTransport().tempo -
                           kMetronomeTempoIncrement);
        LOG(INFO) << "Tempo decreased to " << sequencer.GetTransport().tempo;
        break;
      case '+':
        sequencer.SetTempo(sequencer.GetTransport().tempo +
                           kMetronomeTempoIncrement);
        LOG(INFO) << "Tempo increased to " << sequencer.GetTransport().tempo;
        break;
      case '1':
        sequencer.SetTempo(0.5f * sequencer.GetTransport().tempo);
        LOG(INFO) << "Tempo halved to " << sequencer.GetTransport().tempo;
        break;
      case '2':
        sequencer.SetTempo(2.0f * sequencer.GetTransport().tempo);
        LOG(INFO) << "Tempo doubled to " << sequencer.GetTransport().tempo;
        break;
      case 'R':
        sequencer.SetTempo(kTempo);
        LOG(INFO) << "Tempo reset to " << sequencer.GetTransport().tempo;
        break;
    }
  };
  input_manager.SetOnKeyDownCallback(on_key_down);

  // Start the demo.
  input_manager.Initialize();
  audio_io.Initialize(kSampleRate, kNumChannels, kFramesPerBuffer);

  while (!quit) {
    input_manager.Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  audio_io.Shutdown();
  input_manager.Shutdown();

  return 0;
}
