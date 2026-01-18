#include "dsp/sample_generators.h"

#include <cstdint>

#include "gtest/gtest.h"

namespace barely {
namespace {

TEST(SampleGeneratorsTest, GenerateOscSample) {
  static constexpr float kShape = 1.0f;  // sawtooth

  for (int i = 0; i < 4; ++i) {
    EXPECT_FLOAT_EQ(GenerateOscSample(0.25f * i, kShape),
                    (i < 2) ? static_cast<float>(i) * 0.5f : static_cast<float>(i - 4) * 0.5f);
  }
}

TEST(SampleGeneratorsTest, GenerateSliceSample) {
  static constexpr uint32_t kDataLength = 5;
  static constexpr float kData[kDataLength] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};

  for (uint32_t i = 0; i < kDataLength; ++i) {
    EXPECT_FLOAT_EQ(GenerateSliceSample(kData, kDataLength, static_cast<float>(i)), kData[i]);
    if (i < kDataLength - 1) {
      EXPECT_FLOAT_EQ(GenerateSliceSample(kData, kDataLength, static_cast<float>(i) + 0.5f),
                      0.5f * (kData[i] + kData[i + 1]));
    }
  }
  EXPECT_FLOAT_EQ(GenerateSliceSample(kData, kDataLength, static_cast<float>(kDataLength)), 0.0f);
}

}  // namespace
}  // namespace barely
