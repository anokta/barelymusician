#include "dsp/gain_processor.h"

#include <vector>

#include "gtest/gtest.h"

namespace barely::internal {
namespace {

TEST(GainProcessorTest, ProcessConstantGain) {
  constexpr int kSampleRate = 100;
  constexpr double kGain = 0.75;

  GainProcessor gain_processor(kSampleRate);
  gain_processor.SetGain(kGain);

  std::vector<double> data(kSampleRate);
  // Flush the gain state.
  gain_processor.Process(data.data(), kSampleRate);

  for (int i = 0; i < kSampleRate; ++i) {
    data[i] = static_cast<double>(i + 1);
  }
  gain_processor.Process(data.data(), kSampleRate);
  for (int i = 0; i < kSampleRate; ++i) {
    EXPECT_DOUBLE_EQ(data[i], kGain * static_cast<double>(i + 1));
  }
}

TEST(GainProcessorTest, ProcessSetGain) {
  constexpr int kSampleRate = 200;
  constexpr double kEpsilon = 1e-12;

  GainProcessor gain_processor(kSampleRate);

  std::vector<double> data(kSampleRate);
  for (int i = 0; i < kSampleRate; ++i) {
    data[i] = static_cast<double>(i + 1);
  }

  // No gain is set yet.
  gain_processor.Process(data.data(), kSampleRate);
  for (int i = 0; i < kSampleRate; ++i) {
    // Gain should be ramping from 0.0 to 1.0 in the first 10 samples.
    const double gain = (i < 10) ? static_cast<double>(i + 1) / 10.0 : 1.0;
    EXPECT_NEAR(data[i], gain * static_cast<double>(i + 1), kEpsilon);
  }

  // Reset values.
  for (int i = 0; i < kSampleRate; ++i) {
    data[i] = static_cast<double>(i + 1);
  }

  // Set gain to 2.0.
  gain_processor.SetGain(2.0);
  gain_processor.Process(data.data(), kSampleRate);
  for (int i = 0; i < kSampleRate; ++i) {
    // Gain should be ramping from 1.0 to 2.0 in the first 10 samples.
    const double gain = (i < 10) ? 1.0 + static_cast<double>(i + 1) / 10.0 : 2.0;
    EXPECT_NEAR(data[i], gain * static_cast<double>(i + 1), kEpsilon);
  }

  // Reset values.
  for (int i = 0; i < kSampleRate; ++i) {
    data[i] = static_cast<double>(i + 1);
  }

  // Set gain to -2.0, but process 20 samples only ramping half the way to 0.0.
  gain_processor.SetGain(-2.0);
  gain_processor.Process(data.data(), 20);
  for (int i = 0; i < 20; ++i) {
    // Gain should be be ramping from 2.0 to 0.0 in the first 40 samples.
    const double gain = 2.0 - static_cast<double>(i + 1) / 10.0;
    EXPECT_NEAR(data[i], gain * static_cast<double>(i + 1), kEpsilon);
  }

  // Reset values.
  for (int i = 0; i < kSampleRate; ++i) {
    data[i] = static_cast<double>(i + 1);
  }

  // Set gain back to 1.0.
  gain_processor.SetGain(1.0);
  gain_processor.Process(data.data(), kSampleRate);
  for (int i = 0; i < kSampleRate; ++i) {
    // Gain should be ramping from 0.0 to 1.0 in the first 10 samples.
    const double gain = (i < 10) ? static_cast<double>(i + 1) / 10.0 : 1.0;
    EXPECT_NEAR(data[i], gain * static_cast<double>(i + 1), kEpsilon);
  }

  // Reset values.
  for (int i = 0; i < kSampleRate; ++i) {
    data[i] = static_cast<double>(i + 1);
  }

  // Set gain to 0.0.
  gain_processor.SetGain(0.0);
  gain_processor.Process(data.data(), kSampleRate);
  for (int i = 0; i < kSampleRate; ++i) {
    // Gain should be ramping from 1.0 to 0.0 in the first 10 samples.
    const double gain = (i < 10) ? 1.0 - static_cast<double>(i + 1) / 10.0 : 0.0;
    EXPECT_NEAR(data[i], gain * static_cast<double>(i + 1), kEpsilon);
  }
}

}  // namespace
}  // namespace barely::internal
