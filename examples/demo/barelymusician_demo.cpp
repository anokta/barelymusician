#include <conio.h>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "barelymusician/base/sequencer.h"
#include "barelymusician/instrument/envelope.h"
#include "barelymusician/instrument/oscillator.h"
#include "utils/pa_wrapper.h"

using barelyapi::Envelope;
using barelyapi::Oscillator;
using barelyapi::Sequencer;

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

  sequencer.SetBpm(kBpm);
  sequencer.SetTimeSignature(kBeatsPerBar, kBeatLength);

  Oscillator oscillator(kSampleInterval);
  Envelope envelope(kSampleInterval);

  oscillator.SetType(kOscillatorType);
  oscillator.SetFrequency(kBarFrequency);
  envelope.SetRelease(kRelease);

  const auto process = [&sequencer, &oscillator, &envelope](float* output) {
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
      oscillator.SetFrequency(kBarFrequency);
      impulse_sample = sequencer.sample_offset();
    } else if (current_beat != sequencer.current_beat()) {
      oscillator.SetFrequency(kBeatFrequency);
      impulse_sample = sequencer.sample_offset();
    }
    for (int frame = 0; frame < kFramesPerBuffer; ++frame) {
      if (frame == impulse_sample) {
        oscillator.Reset();
        envelope.Start();
      }
      const float sample = envelope.Next() * oscillator.Next();
      if (frame == impulse_sample) {
        envelope.Stop();
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
