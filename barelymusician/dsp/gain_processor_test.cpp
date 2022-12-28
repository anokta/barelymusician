#include "barelymusician/dsp/gain_processor.h"

#include <vector>

#include "gtest/gtest.h"

namespace barely {
namespace {

TEST(GainProcessorTest, ProcessConstantGain) {
  const int kFrameRate = 100;
  const int kNumChannels = 3;
  const double kGain = 0.75;

  GainProcessor gain_processor(kFrameRate);
  gain_processor.SetGain(kGain);

  std::vector<double> data(kNumChannels * kFrameRate);
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      const int index = frame * kNumChannels + channel;
      data[index] = static_cast<double>(index + 1);
    }
  }

  gain_processor.Process(data.data(), kNumChannels, kFrameRate);
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      const int index = frame * kNumChannels + channel;
      EXPECT_DOUBLE_EQ(data[index], kGain * static_cast<double>(index + 1));
    }
  }
}

TEST(GainProcessorTest, ProcessSetGain) {
  const int kFrameRate = 200;
  const int kNumChannels = 2;
  const double kEpsilon = 1e-12;

  GainProcessor gain_processor(kFrameRate);

  std::vector<double> data(kNumChannels * kFrameRate);
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      const int index = frame * kNumChannels + channel;
      data[index] = static_cast<double>(index + 1);
    }
  }

  // No gain is set yet.
  gain_processor.Process(data.data(), kNumChannels, kFrameRate);
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      const int index = frame * kNumChannels + channel;
      EXPECT_DOUBLE_EQ(data[index], static_cast<double>(index + 1));
    }
  }

  // Set gain to 2.0.
  gain_processor.SetGain(2.0);
  gain_processor.Process(data.data(), kNumChannels, kFrameRate);
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      const int index = frame * kNumChannels + channel;
      // Gain should be ramping from 1.0 to 2.0 in the first 10 frames.
      const double gain =
          (frame < 10) ? 1.0 + static_cast<double>(frame + 1) / 10.0 : 2.0;
      EXPECT_NEAR(data[index], gain * static_cast<double>(index + 1), kEpsilon);
    }
  }

  // Reset values.
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      const int index = frame * kNumChannels + channel;
      data[index] = static_cast<double>(index + 1);
    }
  }

  // Set gain to -2.0, but process 20 frames only ramping half the way to 0.0.
  gain_processor.SetGain(-2.0);
  gain_processor.Process(data.data(), kNumChannels, 20);
  for (int frame = 0; frame < 20; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      const int index = frame * kNumChannels + channel;
      // Gain should be be ramping from 2.0 to 0.0 in the first 40 frames.
      const double gain = 2.0 - static_cast<double>(frame + 1) / 10.0;
      EXPECT_NEAR(data[index], gain * static_cast<double>(index + 1), kEpsilon);
    }
  }

  // Reset values.
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      const int index = frame * kNumChannels + channel;
      data[index] = static_cast<double>(index + 1);
    }
  }

  // Set gain back to 1.0.
  gain_processor.SetGain(1.0);
  gain_processor.Process(data.data(), kNumChannels, kFrameRate);
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      const int index = frame * kNumChannels + channel;
      // Gain should be ramping from 0.0 to 1.0 in the first 10 frames.
      const double gain =
          (frame < 10) ? static_cast<double>(frame + 1) / 10.0 : 1.0;
      EXPECT_NEAR(data[index], gain * static_cast<double>(index + 1), kEpsilon);
    }
  }

  // Reset values.
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      const int index = frame * kNumChannels + channel;
      data[index] = static_cast<double>(index + 1);
    }
  }

  // Set gain to 0.0.
  gain_processor.SetGain(0.0);
  gain_processor.Process(data.data(), kNumChannels, kFrameRate);
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      const int index = frame * kNumChannels + channel;
      // Gain should be ramping from 1.0 to 0.0 in the first 10 frames.
      const double gain =
          (frame < 10) ? 1.0 - static_cast<double>(frame + 1) / 10.0 : 0.0;
      EXPECT_NEAR(data[index], gain * static_cast<double>(index + 1), kEpsilon);
    }
  }
}

}  // namespace
}  // namespace barely