#include "core/pool.h"

#include <cstdint>

#include "core/arena.h"
#include "core/constants.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

struct TestData {};

[[nodiscard]] size_t GetPoolSize(int count) noexcept {
  Arena arena;  // sizing arena
  Pool<TestData>().Init(arena, count);
  return AlignUp(arena.offset(), alignof(std::max_align_t)) + alignof(std::max_align_t);
}

TEST(PoolTest, AcquireMax) {
  constexpr uint32_t kCount = 10;

  const size_t size = GetPoolSize(kCount);
  auto data = std::make_unique<std::byte[]>(size);
  Arena arena(data.get(), size);

  Pool<TestData> pool;
  pool.Init(arena, kCount);

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
