#include "barelymusician/dsp/gain_processor.h"

#include <vector>

#include "gtest/gtest.h"

namespace barely {
namespace {

TEST(GainProcessorTest, ProcessConstantGain) {
  constexpr int kFrameRate = 100;
  constexpr int kChannelCount = 3;
  constexpr double kGain = 0.75;

  GainProcessor gain_processor(kFrameRate);
  gain_processor.SetGain(kGain);

  std::vector<double> data(kChannelCount * kFrameRate);
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      const int index = frame * kChannelCount + channel;
      data[index] = static_cast<double>(index + 1);
    }
  }

  gain_processor.Process(data.data(), kChannelCount, kFrameRate);
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      const int index = frame * kChannelCount + channel;
      EXPECT_DOUBLE_EQ(data[index], kGain * static_cast<double>(index + 1));
    }
  }
}

TEST(GainProcessorTest, ProcessSetGain) {
  constexpr int kFrameRate = 200;
  constexpr int kChannelCount = 2;
  constexpr double kEpsilon = 1e-12;

  GainProcessor gain_processor(kFrameRate);

  std::vector<double> data(kChannelCount * kFrameRate);
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      const int index = frame * kChannelCount + channel;
      data[index] = static_cast<double>(index + 1);
    }
  }

  // No gain is set yet.
  gain_processor.Process(data.data(), kChannelCount, kFrameRate);
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      const int index = frame * kChannelCount + channel;
      EXPECT_DOUBLE_EQ(data[index], static_cast<double>(index + 1));
    }
  }

  // Set gain to 2.0.
  gain_processor.SetGain(2.0);
  gain_processor.Process(data.data(), kChannelCount, kFrameRate);
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      const int index = frame * kChannelCount + channel;
      // Gain should be ramping from 1.0 to 2.0 in the first 10 frames.
      const double gain =
          (frame < 10) ? 1.0 + static_cast<double>(frame + 1) / 10.0 : 2.0;
      EXPECT_NEAR(data[index], gain * static_cast<double>(index + 1), kEpsilon);
    }
  }

  // Reset values.
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      const int index = frame * kChannelCount + channel;
      data[index] = static_cast<double>(index + 1);
    }
  }

  // Set gain to -2.0, but process 20 frames only ramping half the way to 0.0.
  gain_processor.SetGain(-2.0);
  gain_processor.Process(data.data(), kChannelCount, 20);
  for (int frame = 0; frame < 20; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      const int index = frame * kChannelCount + channel;
      // Gain should be be ramping from 2.0 to 0.0 in the first 40 frames.
      const double gain = 2.0 - static_cast<double>(frame + 1) / 10.0;
      EXPECT_NEAR(data[index], gain * static_cast<double>(index + 1), kEpsilon);
    }
  }

  // Reset values.
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      const int index = frame * kChannelCount + channel;
      data[index] = static_cast<double>(index + 1);
    }
  }

  // Set gain back to 1.0.
  gain_processor.SetGain(1.0);
  gain_processor.Process(data.data(), kChannelCount, kFrameRate);
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      const int index = frame * kChannelCount + channel;
      // Gain should be ramping from 0.0 to 1.0 in the first 10 frames.
      const double gain =
          (frame < 10) ? static_cast<double>(frame + 1) / 10.0 : 1.0;
      EXPECT_NEAR(data[index], gain * static_cast<double>(index + 1), kEpsilon);
    }
  }

  // Reset values.
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      const int index = frame * kChannelCount + channel;
      data[index] = static_cast<double>(index + 1);
    }
  }

  // Set gain to 0.0.
  gain_processor.SetGain(0.0);
  gain_processor.Process(data.data(), kChannelCount, kFrameRate);
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      const int index = frame * kChannelCount + channel;
      // Gain should be ramping from 1.0 to 0.0 in the first 10 frames.
      const double gain =
          (frame < 10) ? 1.0 - static_cast<double>(frame + 1) / 10.0 : 0.0;
      EXPECT_NEAR(data[index], gain * static_cast<double>(index + 1), kEpsilon);
    }
  }
}

}  // namespace
}  // namespace barely
