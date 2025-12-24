#ifndef BARELYMUSICIAN_COMMON_POOL_H_
#define BARELYMUSICIAN_COMMON_POOL_H_

#include <array>
#include <cstdint>

namespace barely {

/// Memory pool template for a given item type.
template <typename ItemType, uint32_t kCount>
class Pool {
 public:
  Pool() noexcept {
    for (uint32_t i = 0; i < kCount; ++i) {
      free_items_[i] = kCount - i;
    }
  }

  /// Acquires a new item.
  ///
  /// @return 0 if capacity is reached, valid item index otherwise.
  [[nodiscard]] uint32_t Acquire() noexcept {
    return (free_item_count_ > 0) ? free_items_[--free_item_count_] : 0;
  }

  /// Releases an item.
  ///
  /// @param index Item index.
  // TODO(#126): Must be acquired already, might be useful to keep "in-use" field to assert.
  void Release(uint32_t index) noexcept { free_items_[free_item_count_++] = index; }

  [[nodiscard]] ItemType& Get(uint32_t index) noexcept {
    assert(index <= kCount);
    return items_[index];
  }

 private:
  std::array<ItemType, kCount + 1> items_;  // 0 is reserved for nil.

  // Cached array of free items for easier edits.
  std::array<uint32_t, kCount> free_items_;
  int free_item_count_ = kCount;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_COMMON_POOL_H_
