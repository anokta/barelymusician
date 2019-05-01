#include "barelymusician/base/buffer.h"

#include <iterator>

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Buffer properties.
const int kNumChannels = 4;
const int kNumFrames = 16;

// Tests that read/write operations work as expected when iterating the buffer.
TEST(BufferTest, Iterate) {
  Buffer buffer(kNumChannels, kNumFrames);

  EXPECT_EQ(kNumChannels, buffer.num_channels());
  EXPECT_EQ(kNumFrames, buffer.num_frames());
  EXPECT_EQ(kNumChannels * kNumFrames, buffer.size());

  EXPECT_NE(buffer.begin(), buffer.end());
  EXPECT_EQ(buffer.num_frames(), std::distance(buffer.begin(), buffer.end()));

  for (int i = 0; i < buffer.num_frames(); ++i) {
    const float sample = static_cast<float>(i + 1);
    auto& frame = buffer[i];
    for (int channel = 0; channel < frame.size(); ++channel) {
      frame[channel] = sample;
      EXPECT_FLOAT_EQ(sample, frame[channel]);
    }
  }
  EXPECT_FLOAT_EQ(1.0f, *buffer.begin()->begin());
}

// Tests that clearing the buffer resets all the samples to zero as expected.
TEST(BufferTest, Clear) {
  const float kSample = -5.0f;

  Buffer buffer(kNumChannels, kNumFrames);

  for (auto& frame : buffer) {
    for (auto& sample : frame) {
      sample = kSample;
    }
  }
  for (const auto& frame : buffer) {
    for (const auto& sample : frame) {
      EXPECT_FLOAT_EQ(kSample, sample);
    }
  }

  buffer.clear();
  for (const auto& frame : buffer) {
    for (const auto& sample : frame) {
      EXPECT_FLOAT_EQ(0.0f, sample);
    }
  }
}

}  // namespace
}  // namespace barelyapi
