#include "dsp/sample_data.h"

#include <array>

#include "barelymusician.h"
#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

using ::testing::Field;
using ::testing::Pointee;

TEST(SampleDataTest, Select) {
  constexpr int kSampleRate = 1;
  constexpr std::array<float, 1> kSamples = {1.0f};
  const std::array<SampleDataSlice, 3> kSlices = {
      SampleDataSlice(5.0f, kSampleRate, kSamples),
      SampleDataSlice(15.0f, kSampleRate, kSamples),
      SampleDataSlice(35.0f, kSampleRate, kSamples),
  };

  const SampleData sample_data(kSlices);
  for (int i = 0; i <= 40; ++i) {
    EXPECT_THAT(sample_data.Select(static_cast<float>(i)),
                Pointee(Field(&SampleDataSlice::root_pitch,
                              ((i <= 10) ? 5.0f : (i <= 25.0f ? 15.0f : 35.0f)))))
        << i;
  }
}

}  // namespace
}  // namespace barely::internal