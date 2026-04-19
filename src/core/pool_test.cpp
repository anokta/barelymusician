#include "core/pool.h"

#include <cstdint>

#include "core/arena.h"
#include "core/constants.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

TEST(PoolTest, AcquireMax) {
  constexpr uint32_t kCount = 10;

  struct TestData {};

  const size_t size = GetAllocSize<Pool<TestData>>(kCount);
  auto data = std::make_unique<std::byte[]>(size);
  Arena arena(data.get(), size);

  Pool<TestData> pool(arena, kCount);

  // Acquire up to maximum capacity.
  for (uint32_t i = 0; i < kCount; ++i) {
    EXPECT_LT(pool.Acquire(), kCount);
    EXPECT_EQ(pool.ActiveCount(), i + 1);
  }

  // Exceeded maximum capacity.
  for (uint32_t i = 0; i < kCount; ++i) {
    EXPECT_EQ(pool.Acquire(), kInvalidIndex);
    EXPECT_EQ(pool.ActiveCount(), kCount);
  }

  // Capacity should recover after freeing an item.
  pool.Release(1);
  EXPECT_LT(pool.Acquire(), kCount);
}

}  // namespace
}  // namespace barely
