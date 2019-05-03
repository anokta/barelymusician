#include "barelymusician/dsp/mixer.h"

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Mixer properties.
const int kNumChannels = 2;
const int kNumFrames = 8;

// Tests that accumulating multiple inputs returns the expected output.
TEST(MixerTest, AddMultipleInputs) {
  const int kNumInputs = 5;

  std::vector<Buffer> inputs;
  for (int i = 0; i < kNumInputs; ++i) {
    inputs.emplace_back(kNumChannels, kNumFrames);
    for (int frame = 0; frame < kNumFrames; ++frame) {
      auto& input_frame = inputs[i][frame];
      for (int channel = 0; channel < kNumChannels; ++channel) {
        input_frame[channel] = static_cast<float>((channel + 1) * (i + 1));
      }
    }
  }

  Mixer mixer(kNumChannels, kNumFrames);
  for (const auto& input : inputs) {
    mixer.AddInput(input);
  }

  const auto& output = mixer.GetOutput();
  for (int frame = 0; frame < kNumFrames; ++frame) {
    auto& output_frame = output[frame];
    for (int channel = 0; channel < kNumChannels; ++channel) {
      // Partial sum = (n * (n + 1)) / 2.
      const float expected_sample =
          static_cast<float>((channel + 1) * kNumInputs * (kNumInputs + 1) / 2);
      EXPECT_FLOAT_EQ(expected_sample, output_frame[channel]);
    }
  }
}

// Tests that resetting the mixer clears out its output buffer as expected.
TEST(MixerTest, Reset) {
  const float kInputValue = -0.5f;

  Buffer input(kNumChannels, kNumFrames);
  for (auto& frame : input) {
    for (auto& sample : frame) {
      sample = kInputValue;
    }
  }

  Mixer mixer(kNumChannels, kNumFrames);
  for (const auto& frame : mixer.GetOutput()) {
    for (const auto& sample : frame) {
      EXPECT_FLOAT_EQ(0.0f, sample);
    }
  }

  mixer.AddInput(input);
  for (const auto& frame : mixer.GetOutput()) {
    for (const auto& sample : frame) {
      EXPECT_FLOAT_EQ(kInputValue, sample);
    }
  }

  mixer.Reset();
  for (const auto& frame : mixer.GetOutput()) {
    for (const auto& sample : frame) {
      EXPECT_FLOAT_EQ(0.0f, sample);
    }
  }
}

}  // namespace
}  // namespace barelyapi
