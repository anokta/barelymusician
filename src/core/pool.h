#ifndef BARELYMUSICIAN_CORE_POOL_H_
#define BARELYMUSICIAN_CORE_POOL_H_

#include <array>
#include <cassert>
#include <cstdint>

namespace barely {

/// Memory pool template for a given item type.
template <typename ItemType, uint32_t kCount>
class Pool {
 public:
  Pool() noexcept {
    to_active_.fill(UINT32_MAX);
    for (uint32_t i = 0; i < kCount; ++i) {
      free_[i] = i;
    }
  }

  /// Acquires a new item.
  ///
  /// @return Invalid index if capacity is reached, valid item index otherwise.
  [[nodiscard]] uint32_t Acquire() noexcept {
    if (active_count_ < kCount) {
      const uint32_t index = free_[free_read_index_];
      free_read_index_ = (free_read_index_ + 1) % kCount;

      assert(index < kCount);
      assert(to_active_[index] == UINT32_MAX);
      assert(active_count_ < kCount);
      to_active_[index] = active_count_;
      active_[active_count_++] = index;

      return index;
    }
    return UINT32_MAX;
  }

  /// Releases an item.
  ///
  /// @param index Item index.
  void Release(uint32_t index) noexcept {
    assert(IsActive(index));
    assert(active_count_ > 0);

    const uint32_t removed_active_index = to_active_[index];
    const uint32_t last_index = active_[--active_count_];
    active_[removed_active_index] = last_index;
    to_active_[last_index] = removed_active_index;
    to_active_[index] = UINT32_MAX;

    free_[free_write_index_] = index;
    free_write_index_ = (free_write_index_ + 1) % kCount;
  }

  [[nodiscard]] constexpr uint32_t Count() const noexcept { return kCount; }

  [[nodiscard]] ItemType& Get(uint32_t index) noexcept {
    assert(IsActive(index));
    return items_[index];
  }

  [[nodiscard]] const ItemType& Get(uint32_t index) const noexcept {
    assert(IsActive(index));
    return items_[index];
  }

  [[nodiscard]] uint32_t GetIndex(const ItemType& item) const noexcept {
    const uint32_t index = static_cast<uint32_t>(&item - &items_[0]);
    assert(IsActive(index));
    return index;
  }

  [[nodiscard]] bool IsActive(uint32_t index) const noexcept {
    return index < kCount && to_active_[index] < kCount;
  }

  [[nodiscard]] ItemType& GetActive(uint32_t active_index) noexcept {
    assert(active_index < active_count_);
    return Get(active_[active_index]);
  }

  [[nodiscard]] const ItemType& GetActive(uint32_t active_index) const noexcept {
    assert(active_index < active_count_);
    return Get(active_[active_index]);
  }

  // TODO(#126): Clean up redundant getters.
  [[nodiscard]] uint32_t GetActiveIndex(uint32_t active_index) noexcept {
    assert(active_index < active_count_);
    return active_[active_index];
  }

  [[nodiscard]] uint32_t GetActiveCount() const noexcept { return active_count_; }

 private:
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
