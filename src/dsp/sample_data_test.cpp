#include "dsp/sample_data.h"

#include <barelymusician.h>

#include <array>

#include "core/rng.h"
#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

using ::testing::Field;
using ::testing::Pointee;

TEST(SampleDataTest, Select) {
  constexpr int kSampleRate = 1;
  constexpr std::array<float, 1> kSamples = {1.0f};
  const std::array<BarelySlice, 3> kSlices = {
      BarelySlice{kSamples.data(), 1, kSampleRate, 5.0f},
      BarelySlice{kSamples.data(), 1, kSampleRate, 15.0f},
      BarelySlice{kSamples.data(), 1, kSampleRate, 35.0f},
  };

  AudioRng rng;
  const SampleData sample_data(kSlices);
  for (int i = 0; i <= 40; ++i) {
    EXPECT_THAT(
        sample_data.Select(static_cast<float>(i), rng),
        Pointee(Field(&Slice::root_pitch, ((i <= 10) ? 5.0f : (i <= 25.0f ? 15.0f : 35.0f)))))
        << i;
  }
}

}  // namespace
}  // namespace barely
