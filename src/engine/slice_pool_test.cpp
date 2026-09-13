#include "engine/slice_pool.h"

#include <barelymusician.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>

#include "core/arena.h"
#include "core/constants.h"
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
  constexpr std::array<double, 1> kSamples = {1.0};
  const std::array<BarelySlice, 3> kSlices = {
      BarelySlice{5.0, kSamples.data(), 1, kSampleRate},
      BarelySlice{15.0, kSamples.data(), 1, kSampleRate},
      BarelySlice{35.0, kSamples.data(), 1, kSampleRate},
  };
  constexpr uint32_t kCount = 100;

  const auto size = GetAllocSize<SlicePool>(kCount);
  auto data = std::make_unique<std::byte[]>(size);
  Arena arena(data.get(), size);

  AudioRng rng;
  SlicePool slice_pool(arena, kCount);

  const uint32_t first_slice_index =
      slice_pool.Acquire(kSlices.data(), static_cast<uint32_t>(kSlices.size()));

  for (int i = 0; i <= 40; ++i) {
    const uint32_t slice_index = slice_pool.Select(first_slice_index, static_cast<double>(i), rng);
    ASSERT_NE(slice_index, kInvalidIndex);
    EXPECT_THAT(
        slice_pool.Get(slice_index),
        Pointee(Field(&SliceState::root_pitch, ((i <= 10) ? 5.0 : (i <= 25.0 ? 15.0 : 35.0)))))
        << i;
  }
}

}  // namespace
}  // namespace barely
