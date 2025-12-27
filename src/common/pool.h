#ifndef BARELYMUSICIAN_COMMON_POOL_H_
#define BARELYMUSICIAN_COMMON_POOL_H_

#include <array>
#include <bitset>
#include <cassert>
#include <cstdint>

namespace barely {

/// Memory pool template for a given item type.
template <typename ItemType, uint32_t kCount>
class Pool {
 public:
  Pool() noexcept {
    for (uint32_t i = 0; i < kCount; ++i) {
      free_indices_[i] = kCount - i;
    }
  }

  /// Acquires a new item.
  ///
  /// @return 0 if capacity is reached, valid item index otherwise.
  [[nodiscard]] uint32_t Acquire() noexcept {
    if (free_count_ > 0) {
      const uint32_t index = free_indices_[--free_count_];
      assert(!in_use_.test(index));
      in_use_.set(index);
      return index;
    }
    return 0;
  }

  /// Releases an item.
  ///
  /// @param index Item index.
  void Release(uint32_t index) noexcept {
    assert(in_use_.test(index));
    in_use_.reset(index);
    free_indices_[free_count_++] = index;
  }

  [[nodiscard]] ItemType* Begin() noexcept { return &items_[1]; }

  [[nodiscard]] ItemType& Get(uint32_t index) noexcept {
    assert(index <= kCount);
    assert(in_use_.test(index));
    return items_[index];
  }

  [[nodiscard]] bool InUse(uint32_t index) const noexcept { return in_use_.test(index); }

 private:
  // 0 is reserved for nil.
  std::array<ItemType, kCount + 1> items_;

  // Free indices are cached for easier edits.
  std::array<uint32_t, kCount> free_indices_;
  int free_count_ = kCount;
  std::bitset<kCount + 1> in_use_ = {};
};

}  // namespace barely

#endif  // BARELYMUSICIAN_COMMON_POOL_H_
