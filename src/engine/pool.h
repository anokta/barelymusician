#ifndef BARELYMUSICIAN_ENGINE_POOL_H_
#define BARELYMUSICIAN_ENGINE_POOL_H_

#include <array>
#include <cassert>

namespace barely::internal {

/// Memory pool template.
template <typename Type, int kCapacity>
class Pool {
 public:
  Pool() noexcept {
    static_assert(kCapacity > 0, "Pool capacity must be greater than 0");
    for (int i = 0; i < kCapacity; ++i) {
      free_items_[i] = &items_[i];
    }
  }

  Type* Allocate() noexcept {
    assert(free_index_ < kCapacity && "Pool capacity exceeded");
    return free_items_[free_index_++];
  }

  void Deallocate(Type* item) noexcept {
    assert(free_index_ > 0 && "Pool underflow");
    assert(item - items_.data() < kCapacity && "Invalid item");
    free_items_[free_index_--] = item;
  }

 private:
  // Array of free items available to allocate.
  std::array<Type*, kCapacity> free_items_;

  // Free item index.
  int free_index_ = 0;

  // Array of statically allocated pool items.
  std::array<Type, kCapacity> items_;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_ENGINE_POOL_H_
