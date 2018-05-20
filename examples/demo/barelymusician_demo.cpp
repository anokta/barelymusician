#include <conio.h>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "barelymusician/base/sequencer.h"
#include "barelymusician/instrument/oscillator.h"
#include "barelymusician/instrument/voice.h"
#include "utils/pa_wrapper.h"

using barelyapi::Envelope;
using barelyapi::Oscillator;
using barelyapi::Sequencer;
using barelyapi::Voice;

namespace {

// System audio settings.
constexpr int kSampleRate = 48000;
constexpr int kNumChannels = 2;
constexpr int kFramesPerBuffer = 512;

constexpr float kSampleInterval = 1.0f / static_cast<float>(kSampleRate);

// Sequencer settings.
constexpr float kBpm = 120.0f;
constexpr int kBeatsPerBar = 4;
constexpr Sequencer::NoteValue kBeatLength = Sequencer::NoteValue::kQuarterNote;

// Synth settings.
constexpr float kBarFrequency = 440.0f;
constexpr float kBeatFrequency = 220.0f;
constexpr Oscillator::Type kOscillatorType = Oscillator::Type::kSquare;
constexpr float kRelease = 0.025f;

}  // namespace

int main(int argc, char* argv[]) {
  // Set the audio process callback.
  PaWrapper audio_io;

  Sequencer sequencer(kSampleRate);
  Voice voice(kSampleInterval);

  sequencer.SetBpm(kBpm);
  sequencer.SetTimeSignature(kBeatsPerBar, kBeatLength);

  voice.envelope().SetRelease(kRelease);
  voice.oscillator().SetType(kOscillatorType);
  voice.oscillator().SetFrequency(kBarFrequency);

  const auto process = [&sequencer, &voice](float* output) {
    const int current_bar = sequencer.current_bar();
    const int current_beat = sequencer.current_beat();
    int impulse_sample = -1;
    if (current_bar == 0 && current_beat == 0 &&
        sequencer.sample_offset() == 0) {
      // First tick.
      impulse_sample = 0;
    }
    sequencer.Update(kFramesPerBuffer);
    if (current_bar != sequencer.current_bar()) {
      voice.oscillator().SetFrequency(kBarFrequency);
      impulse_sample = sequencer.sample_offset();
    } else if (current_beat != sequencer.current_beat()) {
      voice.oscillator().SetFrequency(kBeatFrequency);
      impulse_sample = sequencer.sample_offset();
    }
    for (int frame = 0; frame < kFramesPerBuffer; ++frame) {
      if (frame == impulse_sample) {
        voice.Start();
      }
      const float sample = voice.Next();
      if (frame == impulse_sample) {
        voice.Stop();
      }
      for (int channel = 0; channel < kNumChannels; ++channel) {
        output[kNumChannels * frame + channel] = sample;
      }
    }
  };
  audio_io.SetAudioProcessCallback(process);

  // Initialize the audio routine.
  audio_io.Initialize(kSampleRate, kNumChannels, kFramesPerBuffer);

  // TODO(#2): This is obviously hacky, may consider add a proper I/O lib.
  int input;
  while (true) {
    input = _getch();
    if (input == 27) {
      // ESC pressed, quit the app.
      break;
    }
    // Test things.
    switch (static_cast<char>(input)) {
      case 't':
        sequencer.SetBpm(2.0f * kBpm);
        break;
      case 'r':
        sequencer.SetBpm(kBpm);
        break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  // Shutdown the audio routine.
  audio_io.Shutdown();

  return 0;
}
