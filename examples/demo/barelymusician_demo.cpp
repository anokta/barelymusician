#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "barelymusician/base/sequencer.h"
#include "barelymusician/instrument/envelope.h"
#include "barelymusician/instrument/oscillator.h"
#include "utils/pa_wrapper.h"

namespace {

using barelyapi::Envelope;
using barelyapi::Oscillator;
using barelyapi::Sequencer;

// System audio settings.
const int kSampleRate = 48000;
const int kNumChannels = 2;
const int kFramesPerBuffer = 512;

const float kSampleInterval = 1.0f / static_cast<float>(kSampleRate);

// Sequencer settings.
const float kBpm = 120.0f;
const int kBeatsPerBar = 4;
const Sequencer::NoteValue kBeatLength = Sequencer::NoteValue::kQuarterNote;

// Synth settings.
const float kBarFrequency = 440.0f;
const float kBeatFrequency = 220.0f;
const Oscillator::Type kOscillatorType = Oscillator::Type::kSquare;
const float kRelease = 0.05f;

}  // namespace

int main() {
  PaWrapper audio_io;

  // Set the audio process callback.
  Sequencer sequencer(kSampleRate);
  Oscillator oscillator(kSampleInterval);
  Envelope envelope(kSampleInterval);

  sequencer.SetBpm(kBpm);
  sequencer.SetNumBeatsPerBar(kBeatsPerBar);
  sequencer.SetBeatLength(kBeatLength);

  envelope.SetRelease(kRelease);
  oscillator.SetType(kOscillatorType);
  oscillator.SetFrequency(kBarFrequency);

  const auto process = [&sequencer, &oscillator, &envelope](float* output) {
    const int current_bar = sequencer.GetCurrentBar();
    const int current_beat = sequencer.GetCurrentBeat();
    int impulse_sample = -1;
    if (current_bar == 0 && current_beat == 0 &&
        sequencer.GetCurrentSampleOffset() == 0) {
      // First tick.
      impulse_sample = 0;
    }
    sequencer.Update(kFramesPerBuffer);
    if (current_bar != sequencer.GetCurrentBar()) {
      oscillator.SetFrequency(kBarFrequency);
      impulse_sample = sequencer.GetCurrentSampleOffset();
    } else if (current_beat != sequencer.GetCurrentBeat()) {
      oscillator.SetFrequency(kBeatFrequency);
      impulse_sample = sequencer.GetCurrentSampleOffset();
    }
    for (int frame = 0; frame < kFramesPerBuffer; ++frame) {
      if (frame == impulse_sample) {
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

  // Execute the audio routine.
  audio_io.Initialize(kSampleRate, kNumChannels, kFramesPerBuffer);
  while (getchar() == 0) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  audio_io.Shutdown();

  return 0;
}
