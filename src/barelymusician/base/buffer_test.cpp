#include "barelymusician/base/buffer.h"

#include <iterator>

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Buffer properties.
const int kNumChannels = 4;
const int kNumFrames = 16;
const int kNumSamples = kNumChannels * kNumFrames;

// Tests that read/write operations work as expected when iterating the buffer.
TEST(BufferTest, Iterate) {
  Buffer buffer(kNumChannels, kNumFrames);

  EXPECT_EQ(kNumChannels, buffer.num_channels());
  EXPECT_EQ(kNumFrames, buffer.num_frames());
  EXPECT_EQ(kNumSamples, buffer.num_samples());

  EXPECT_NE(buffer.begin(), buffer.end());
  EXPECT_EQ(kNumSamples, std::distance(buffer.begin(), buffer.end()));

  for (int i = 0; i < kNumSamples; ++i) {
    const float sample = static_cast<float>(i + 1);
    buffer[i] = sample;
    EXPECT_FLOAT_EQ(sample, buffer[i]);
  }
  EXPECT_FLOAT_EQ(1.0f, *buffer.begin());
}

// Tests that clearing the buffer resets all the samples to zero as expected.
TEST(BufferTest, Clear) {
  const float kSample = -5.0f;

  Buffer buffer(kNumChannels, kNumFrames);

  for (auto& sample : buffer) {
    sample = kSample;
  }
  for (const auto& sample : buffer) {
    EXPECT_FLOAT_EQ(kSample, sample);
  }

  buffer.Clear();
  for (const auto& sample : buffer) {
    EXPECT_FLOAT_EQ(0.0f, sample);
  }
}

}  // namespace
}  // namespace barelyapi
