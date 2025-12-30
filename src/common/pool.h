#ifndef BARELYMUSICIAN_COMMON_POOL_H_
#define BARELYMUSICIAN_COMMON_POOL_H_

#include <array>
#include <cassert>
#include <cstdint>

namespace barely {

/// Memory pool template for a given item type.
template <typename ItemType, uint32_t kCount>
class Pool {
 public:
  Pool() noexcept {
    to_active_.fill(kCount);
    for (uint32_t i = 0; i < kCount; ++i) {
      free_[i] = kCount - i;
    }
  }

  /// Acquires a new item.
  ///
  /// @return 0 if capacity is reached, valid item index otherwise.
  [[nodiscard]] uint32_t Acquire() noexcept {
    if (free_count_ > 0) {
      const uint32_t index = free_[--free_count_];
      assert(index > 0);
      assert(index <= kCount);

      assert(to_active_[index] == kCount);
      assert(active_count_ < kCount);
      to_active_[index] = active_count_;
      active_[active_count_++] = index;

      return index;
    }
    return 0;
  }

  /// Releases an item.
  ///
  /// @param index Item index.
  void Release(uint32_t index) noexcept {
    assert(index > 0);
    assert(index <= kCount);

    assert(to_active_[index] < kCount);
    assert(active_count_ > 0);
    const uint32_t removed_active_index = to_active_[index];
    const uint32_t last_index = active_[--active_count_];
    active_[removed_active_index] = last_index;
    to_active_[last_index] = removed_active_index;
    to_active_[index] = kCount;

    free_[free_count_++] = index;
  }

  [[nodiscard]] constexpr uint32_t Count() const noexcept { return kCount; }

  [[nodiscard]] ItemType& Get(uint32_t index) noexcept {
    assert(index > 0);
    assert(index <= kCount);
    assert(to_active_[index] < kCount);
    return items_[index];
  }

  [[nodiscard]] const ItemType& Get(uint32_t index) const noexcept {
    assert(index > 0);
    assert(index <= kCount);
    assert(to_active_[index] < kCount);
    return items_[index];
  }

  [[nodiscard]] uint32_t GetIndex(ItemType& item) const noexcept {
    const uint32_t index = static_cast<uint32_t>(&item - &items_[0]);
    assert(index > 0);
    assert(index <= kCount);
    assert(to_active_[index] < kCount);
    return index;
  }

  [[nodiscard]] ItemType& GetActive(uint32_t active_index) noexcept {
    assert(active_index < active_count_);
    return Get(active_[active_index]);
  }

  [[nodiscard]] uint32_t GetActiveCount() const noexcept { return active_count_; }

 private:
  // Index 0 is reserved for zero-initialized nil reference to keep invalid access in-bounds.
  std::array<ItemType, kCount + 1> items_;
  std::array<uint32_t, kCount + 1> to_active_;  // maps item index to active index.

  std::array<uint32_t, kCount> active_;
  uint32_t active_count_ = 0;

  std::array<uint32_t, kCount> free_;
  uint32_t free_count_ = kCount;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_COMMON_POOL_H_
