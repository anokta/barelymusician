#include "dsp/gain_processor.h"

#include <vector>

#include "gtest/gtest.h"

namespace barely {
namespace {

TEST(GainProcessorTest, ProcessConstantGain) {
  constexpr int kSampleRate = 100;
  constexpr float kGain = 0.75f;

  GainProcessor gain_processor(kSampleRate);
  gain_processor.SetGain(kGain);

  std::vector<float> data(kSampleRate);
  // Flush the gain state.
  gain_processor.Process(data.data(), kSampleRate);

  for (int i = 0; i < kSampleRate; ++i) {
    data[i] = static_cast<float>(i + 1);
  }
  gain_processor.Process(data.data(), kSampleRate);
  for (int i = 0; i < kSampleRate; ++i) {
    EXPECT_FLOAT_EQ(data[i], kGain * static_cast<float>(i + 1));
  }
}

TEST(GainProcessorTest, ProcessSetGain) {
  constexpr int kSampleRate = 200;
  constexpr float kEpsilon = 1e-5f;

  GainProcessor gain_processor(kSampleRate);

  std::vector<float> data(kSampleRate);
  for (int i = 0; i < kSampleRate; ++i) {
    data[i] = static_cast<float>(i + 1);
  }

  // No gain is set yet.
  gain_processor.Process(data.data(), kSampleRate);
  for (int i = 0; i < kSampleRate; ++i) {
    // Gain should be ramping from 0.0f to 1.0f in the first 10 samples.
    const float gain = (i < 10) ? static_cast<float>(i + 1) / 10.0f : 1.0f;
    EXPECT_NEAR(data[i], gain * static_cast<float>(i + 1), kEpsilon);
  }

  // Reset values.
  for (int i = 0; i < kSampleRate; ++i) {
    data[i] = static_cast<float>(i + 1);
  }

  // Set gain to 2.0.
  gain_processor.SetGain(2.0f);
  gain_processor.Process(data.data(), kSampleRate);
  for (int i = 0; i < kSampleRate; ++i) {
    // Gain should be ramping from 1.0f to 2.0f in the first 10 samples.
    const float gain = (i < 10) ? 1.0f + static_cast<float>(i + 1) / 10.0f : 2.0f;
    EXPECT_NEAR(data[i], gain * static_cast<float>(i + 1), kEpsilon);
  }

  // Reset values.
  for (int i = 0; i < kSampleRate; ++i) {
    data[i] = static_cast<float>(i + 1);
  }

  // Set gain to -2.0f, but process 20 samples only ramping half the way to 0.0f.
  gain_processor.SetGain(-2.0f);
  gain_processor.Process(data.data(), 20);
  for (int i = 0; i < 20; ++i) {
    // Gain should be be ramping from 2.0f to 0.0f in the first 40 samples.
    const float gain = 2.0f - static_cast<float>(i + 1) / 10.0f;
    EXPECT_NEAR(data[i], gain * static_cast<float>(i + 1), kEpsilon);
  }

  // Reset values.
  for (int i = 0; i < kSampleRate; ++i) {
    data[i] = static_cast<float>(i + 1);
  }

  // Set gain back to 1.0.
  gain_processor.SetGain(1.0f);
  gain_processor.Process(data.data(), kSampleRate);
  for (int i = 0; i < kSampleRate; ++i) {
    // Gain should be ramping from 0.0f to 1.0f in the first 10 samples.
    const float gain = (i < 10) ? static_cast<float>(i + 1) / 10.0f : 1.0f;
    EXPECT_NEAR(data[i], gain * static_cast<float>(i + 1), kEpsilon);
  }

  // Reset values.
  for (int i = 0; i < kSampleRate; ++i) {
    data[i] = static_cast<float>(i + 1);
  }

  // Set gain to 0.0.
  gain_processor.SetGain(0.0f);
  gain_processor.Process(data.data(), kSampleRate);
  for (int i = 0; i < kSampleRate; ++i) {
    // Gain should be ramping from 1.0f to 0.0f in the first 10 samples.
    const float gain = (i < 10) ? 1.0f - static_cast<float>(i + 1) / 10.0f : 0.0f;
    EXPECT_NEAR(data[i], gain * static_cast<float>(i + 1), kEpsilon);
  }
}

}  // namespace
}  // namespace barely
