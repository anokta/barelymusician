#include "barelymusician/dsp/gain_processor.h"

#include <vector>

#include "gtest/gtest.h"

namespace barely {
namespace {

TEST(GainProcessorTest, ProcessConstantGain) {
  constexpr int kFrameRate = 100;
  constexpr int kChannelCount = 3;
  constexpr float kGain = 0.75f;

  GainProcessor gain_processor(kFrameRate);
  gain_processor.SetGain(kGain);

  std::vector<float> data(kChannelCount * kFrameRate);
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      const int index = frame * kChannelCount + channel;
      data[index] = static_cast<float>(index + 1);
    }
  }

  gain_processor.Process(data.data(), kChannelCount, kFrameRate);
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      const int index = frame * kChannelCount + channel;
      EXPECT_FLOAT_EQ(data[index], kGain * static_cast<float>(index + 1));
    }
  }
}

TEST(GainProcessorTest, ProcessSetGain) {
  constexpr int kFrameRate = 200;
  constexpr int kChannelCount = 2;
  constexpr float kEpsilon = 2e-5f;

  GainProcessor gain_processor(kFrameRate);

  std::vector<float> data(kChannelCount * kFrameRate);
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      const int index = frame * kChannelCount + channel;
      data[index] = static_cast<float>(index + 1);
    }
  }

  // No gain is set yet.
  gain_processor.Process(data.data(), kChannelCount, kFrameRate);
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      const int index = frame * kChannelCount + channel;
      EXPECT_FLOAT_EQ(data[index], static_cast<float>(index + 1));
    }
  }

  // Set gain to 2.0f.
  gain_processor.SetGain(2.0f);
  gain_processor.Process(data.data(), kChannelCount, kFrameRate);
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      const int index = frame * kChannelCount + channel;
      // Gain should be ramping from 1.0f to 2.0f in the first 10 frames.
      const float gain = (frame < 10) ? 1.0f + static_cast<float>(frame + 1) / 10.0f : 2.0f;
      EXPECT_NEAR(data[index], gain * static_cast<float>(index + 1), kEpsilon);
    }
  }

  // Reset values.
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      const int index = frame * kChannelCount + channel;
      data[index] = static_cast<float>(index + 1);
    }
  }

  // Set gain to -2.0f, but process 20 frames only ramping half the way to 0.0f.
  gain_processor.SetGain(-2.0f);
  gain_processor.Process(data.data(), kChannelCount, 20);
  for (int frame = 0; frame < 20; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      const int index = frame * kChannelCount + channel;
      // Gain should be be ramping from 2.0 to 0.0 in the first 40 frames.
      const float gain = 2.0f - static_cast<float>(frame + 1) / 10.0f;
      EXPECT_NEAR(data[index], gain * static_cast<float>(index + 1), kEpsilon);
    }
  }

  // Reset values.
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      const int index = frame * kChannelCount + channel;
      data[index] = static_cast<float>(index + 1);
    }
  }

  // Set gain back to 1.0f.
  gain_processor.SetGain(1.0f);
  gain_processor.Process(data.data(), kChannelCount, kFrameRate);
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      const int index = frame * kChannelCount + channel;
      // Gain should be ramping from 0.0 to 1.0 in the first 10 frames.
      const float gain = (frame < 10) ? static_cast<float>(frame + 1) / 10.0f : 1.0f;
      EXPECT_NEAR(data[index], gain * static_cast<float>(index + 1), kEpsilon);
    }
  }

  // Reset values.
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      const int index = frame * kChannelCount + channel;
      data[index] = static_cast<float>(index + 1);
    }
  }

  // Set gain to 0.0f.
  gain_processor.SetGain(0.0f);
  gain_processor.Process(data.data(), kChannelCount, kFrameRate);
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      const int index = frame * kChannelCount + channel;
      // Gain should be ramping from 1.0 to 0.0 in the first 10 frames.
      const float gain = (frame < 10) ? 1.0f - static_cast<float>(frame + 1) / 10.0f : 0.0f;
      EXPECT_NEAR(data[index], gain * static_cast<float>(index + 1), kEpsilon);
    }
  }
}

}  // namespace
}  // namespace barely
