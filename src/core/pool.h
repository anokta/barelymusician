#ifndef BARELYMUSICIAN_CORE_POOL_H_
#define BARELYMUSICIAN_CORE_POOL_H_

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>

#include "core/arena.h"
#include "core/constants.h"

namespace barely {

// Memory pool template for a given item type.
template <typename ItemType>
class Pool {
 public:
  Pool(Arena& arena, uint32_t count) noexcept
      : items_(arena.AllocArray<ItemType>(count)),
        to_active_(arena.AllocArray<uint32_t>(count)),
        active_(arena.AllocArray<uint32_t>(count)),
        free_(arena.AllocArray<uint32_t>(count)) {
    if (arena.is_null()) {
      return;
    }
    assert(count > 0);
    assert(count != kInvalidIndex);
    count_ = count;
    std::fill_n(to_active_, count_, kInvalidIndex);
    for (uint32_t i = 0; i < count; ++i) {
      free_[i] = i;
    }
  }

  // Acquires a new item, or returns invalid index if maximum capacity was reached.
  [[nodiscard]] uint32_t Acquire() noexcept {
    if (active_count_ < count_) {
      const uint32_t index = free_[free_read_index_];
      free_read_index_ = (free_read_index_ + 1) % count_;

      assert(index < count_);
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
    free_write_index_ = (free_write_index_ + 1) % count_;
  }

  [[nodiscard]] uint32_t ActiveCount() const noexcept { return active_count_; }

  [[nodiscard]] bool CanAcquire() const noexcept { return active_count_ < count_; }

  [[nodiscard]] bool IsActive(uint32_t index) const noexcept {
    return index < count_ && to_active_[index] < count_;
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
  ItemType* items_ = nullptr;
  uint32_t* to_active_ = nullptr;  // maps item index to active index.

  uint32_t* active_ = nullptr;
  uint32_t* free_ = nullptr;

  uint32_t count_ = 0;
  uint32_t active_count_ = 0;

  uint32_t free_read_index_ = 0;
  uint32_t free_write_index_ = 0;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_CORE_POOL_H_
