#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "barelymusician/instrument/oscillator.h"
#include "utils/pa_wrapper.h"

namespace {

// System audio settings.
const int kSampleRate = 48000;
const int kNumChannels = 2;
const int kFramesPerBuffer = 512;

const float kSampleInterval = 1.0f / static_cast<float>(kSampleRate);

}  // namespace

int main() {
  PaWrapper audio_io;

  // Set the audio process callback.
  barelyapi::Oscillator osc(kSampleInterval);
  const auto process = [&osc](float* output) {
    for (int frame = 0; frame < kFramesPerBuffer; ++frame) {
      for (int channel = 0; channel < kNumChannels; ++channel) {
        output[kNumChannels * frame + channel] = osc.Next();
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
