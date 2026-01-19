#ifndef BARELYMUSICIAN_CORE_POOL_H_
#define BARELYMUSICIAN_CORE_POOL_H_

#include <array>
#include <cassert>
#include <cstdint>

#include "core/constants.h"

namespace barely {

// Memory pool template for a given item type.
template <typename ItemType, uint32_t kCount>
class Pool {
 public:
  Pool() noexcept {
    to_active_.fill(kInvalidIndex);
    for (uint32_t i = 0; i < kCount; ++i) {
      free_[i] = i;
    }
  }

  // Acquires a new item, or returns invalid index if maximum capacity was reached.
  [[nodiscard]] uint32_t Acquire() noexcept {
    if (active_count_ < kCount) {
      const uint32_t index = free_[free_read_index_];
      free_read_index_ = (free_read_index_ + 1) % kCount;

      assert(index < kCount);
      assert(to_active_[index] == kInvalidIndex);
      to_active_[index] = active_count_;
      active_[active_count_++] = index;

      return index;
    }
    return kInvalidIndex;
  }

  void Release(uint32_t index) noexcept {
    assert(IsActive(index));
    assert(active_count_ > 0);

    const uint32_t removed_active_index = to_active_[index];
    const uint32_t last_index = active_[--active_count_];
    active_[removed_active_index] = last_index;
    to_active_[last_index] = removed_active_index;
    to_active_[index] = kInvalidIndex;

    free_[free_write_index_] = index;
    free_write_index_ = (free_write_index_ + 1) % kCount;
  }

  [[nodiscard]] uint32_t ActiveCount() const noexcept { return active_count_; }

  [[nodiscard]] bool CanAcquire() const noexcept { return active_count_ < kCount; }

  [[nodiscard]] bool IsActive(uint32_t index) const noexcept {
    return index < kCount && to_active_[index] < kCount;
  }

  [[nodiscard]] ItemType& Get(uint32_t index) noexcept {
    assert(IsActive(index));
    return items_[index];
  }

  [[nodiscard]] const ItemType& Get(uint32_t index) const noexcept {
    assert(IsActive(index));
    return items_[index];
  }

  [[nodiscard]] uint32_t GetActive(uint32_t active_index) const noexcept {
    assert(active_index < active_count_);
    return active_[active_index];
  }

 private:
  static_assert(kCount != kInvalidIndex);

  std::array<ItemType, kCount> items_;
  std::array<uint32_t, kCount> to_active_;  // maps item index to active index.

  std::array<uint32_t, kCount> active_;
  uint32_t active_count_ = 0;

  std::array<uint32_t, kCount> free_;
  uint32_t free_read_index_ = 0;
  uint32_t free_write_index_ = 0;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_CORE_POOL_H_
