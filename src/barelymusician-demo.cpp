#define _USE_MATH_DEFINES

#include <cmath>
#include <iostream>

#include "portaudio.h"

namespace {

// System audio settings.
const int kSampleRate = 48000;
const int kNumChannels = 2;
const int kFramesPerBuffer = 512;

// Produces a test sine wave output.
void ProcessTestOutput(float* output) {
  static const float kSampleInterval = 1.0f / static_cast<float>(kSampleRate);
  static const float kFrequency = 220.0f;
  static float phase = 0.0f;
  for (int frame = 0; frame < kFramesPerBuffer; ++frame) {
    float sample = std::sin(2.0f * static_cast<float>(M_PI) * phase);
    phase += kSampleInterval * kFrequency;
    if (phase >= 1.0f) {
      phase -= 1.0f;
    }
    for (int channel = 0; channel < kNumChannels; ++channel) {
      output[kNumChannels * frame + channel] = sample;
    }
  }
}

// Audio callback function.
int AudioProcess(const void* inputBuffer, void* outputBuffer,
                 unsigned long framesPerBuffer,
                 const PaStreamCallbackTimeInfo* timeInfo,
                 PaStreamCallbackFlags statusFlags, void* d) {
  float* output = reinterpret_cast<float*>(outputBuffer);
  ProcessTestOutput(output);

  return paContinue;
}

}  // namespace

int main() {
  // Initialize PortAudio.
  Pa_Initialize();

  PaStreamParameters outputParameters;
  outputParameters.device = Pa_GetDefaultOutputDevice();
  outputParameters.channelCount = kNumChannels;
  outputParameters.sampleFormat = paFloat32;
  outputParameters.suggestedLatency =
      Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
  outputParameters.hostApiSpecificStreamInfo = NULL;

  // Create a stream and start the playback.
  PaStream* stream;
  Pa_OpenStream(&stream, NULL, &outputParameters, kSampleRate, kFramesPerBuffer,
                paClipOff, AudioProcess, NULL);

  Pa_StartStream(stream);
  while (getchar() == 0) {
    Pa_Sleep(200);
  }

  // Stop the stream and shutdown the system.
  Pa_StopStream(stream);
  Pa_CloseStream(stream);

  Pa_Terminate();

  return 0;
}
