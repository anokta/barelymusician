#include "barelymusician/internal/sample_data.h"

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
  constexpr std::array<double, 1> kSamples = {1.0};
  const std::array<SampleDataSlice, 3> kSlices = {
      SampleDataSlice(5.0, kSampleRate, kSamples),
      SampleDataSlice(15.0, kSampleRate, kSamples),
      SampleDataSlice(35.0, kSampleRate, kSamples),
  };

  const SampleData sample_data(kSlices);
  for (int i = 0; i <= 40; ++i) {
    EXPECT_THAT(
        sample_data.Select(static_cast<double>(i)),
        Pointee(Field(&SampleDataSlice::root_pitch, ((i <= 10) ? 5.0 : (i <= 25.0 ? 15.0 : 35.0)))))
        << i;
  }
}

}  // namespace
}  // namespace barely::internal
