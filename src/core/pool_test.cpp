#include "core/pool.h"

#include <cstdint>

#include "gtest/gtest.h"

namespace barely {
namespace {

TEST(PoolTest, AcquireMax) {
  constexpr uint32_t kCount = 10;

  struct TestData {};
  Pool<TestData, kCount> pool;

  // Acquire up to maximum capacity.
  for (uint32_t i = 0; i < kCount; ++i) {
    EXPECT_LT(pool.Acquire(), kCount);
    EXPECT_EQ(pool.GetActiveCount(), i + 1);
  }

  // Exceeded maximum capacity.
  for (uint32_t i = 0; i < kCount; ++i) {
    EXPECT_EQ(pool.Acquire(), UINT32_MAX);
    EXPECT_EQ(pool.GetActiveCount(), kCount);
  }

  // Capacity should recover after freeing an item.
  pool.Release(1);
  EXPECT_LT(pool.Acquire(), kCount);
}

}  // namespace
}  // namespace barely
