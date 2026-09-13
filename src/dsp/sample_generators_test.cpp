#include "dsp/sample_generators.h"

#include <cmath>
#include <cstdint>
#include <numbers>

#include "gtest/gtest.h"

namespace barely {
namespace {

TEST(SampleGeneratorsTest, GenerateOscSample) {
  static constexpr double kShape = 0.0;  // sine

  for (int i = 0; i < 4; ++i) {
    EXPECT_DOUBLE_EQ(GenerateOscSample(kShape, 0.25 * i, 0.25),
                     std::sin(std::numbers::pi_v<double> * 0.5 * i));
  }
}

TEST(SampleGeneratorsTest, GenerateSliceSample) {
  static constexpr uint32_t kDataLength = 5;
  static constexpr double kData[kDataLength] = {1.0, 2.0, 3.0, 4.0, 5.0};

  for (uint32_t i = 0; i < kDataLength; ++i) {
    EXPECT_DOUBLE_EQ(
        GenerateSliceSample(kData, kDataLength, static_cast<double>(i), /*is_looping=*/false),
        kData[i]);
    if (i < kDataLength - 1) {
      EXPECT_DOUBLE_EQ(GenerateSliceSample(kData, kDataLength, static_cast<double>(i) + 0.5,
                                           /*is_looping=*/false),
                       0.5 * (kData[i] + kData[i + 1]));
    } else {
      EXPECT_DOUBLE_EQ(GenerateSliceSample(kData, kDataLength, static_cast<double>(i) + 0.5,
                                           /*is_looping=*/true),
                       0.5 * (kData[i] + kData[0]));
    }
  }
  EXPECT_DOUBLE_EQ(GenerateSliceSample(kData, kDataLength, static_cast<double>(kDataLength),
                                       /*is_looping=*/false),
                   0.0);
}

}  // namespace
}  // namespace barely
