#include "engine/slice_pool.h"

#include <barelymusician.h>

#include <array>
#include <cstdint>

#include "core/rng.h"
#include "engine/slice_state.h"
#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

using ::testing::Field;
using ::testing::Pointee;

TEST(SlicePoolTest, Select) {
  constexpr int kSampleRate = 1;
  constexpr std::array<float, 1> kSamples = {1.0f};
  const std::array<BarelySlice, 3> kSlices = {
      BarelySlice{kSamples.data(), 1, kSampleRate, 5.0f},
      BarelySlice{kSamples.data(), 1, kSampleRate, 15.0f},
      BarelySlice{kSamples.data(), 1, kSampleRate, 35.0f},
  };

  AudioRng rng;
  SlicePool slice_pool;

  const uint32_t first_slice_index =
      slice_pool.Acquire(kSlices.data(), static_cast<uint32_t>(kSlices.size()));

  for (int i = 0; i <= 40; ++i) {
    const uint32_t slice_index = slice_pool.Select(first_slice_index, static_cast<float>(i), rng);
    ASSERT_NE(slice_index, UINT32_MAX);
    EXPECT_THAT(
        slice_pool.Get(slice_index),
        Pointee(Field(&SliceState::root_pitch, ((i <= 10) ? 5.0f : (i <= 25.0f ? 15.0f : 35.0f)))))
        << i;
  }
}

}  // namespace
}  // namespace barely
