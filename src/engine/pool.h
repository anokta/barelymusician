#ifndef BARELYMUSICIAN_ENGINE_POOL_H_
#define BARELYMUSICIAN_ENGINE_POOL_H_

#include <cassert>
#include <cstddef>
#include <memory>
#include <new>
#include <vector>

namespace barely::internal {

/// Memory pool template.
template <typename Type>
class Pool {
 public:
  explicit Pool(int capacity) noexcept {
    assert(capacity > 0 && "Pool capacity must be greater than 0");
    raw_items_ = static_cast<std::byte*>(
        ::operator new(sizeof(Type) * capacity, std::align_val_t(alignof(Type))));
    items_.resize(capacity);
    for (int i = 0; i < capacity; ++i) {
      items_[i] = reinterpret_cast<Type*>(raw_items_) + i;
    }
  }

  ~Pool() noexcept {
    for (int i = 0; i < free_index_; ++i) {
      std::destroy_at(items_[i]);
    }
    ::operator delete(raw_items_, std::align_val_t(alignof(Type)));
  }

  /// Non-copyable and non-movable.
  Pool(const Pool& other) noexcept = delete;
  Pool& operator=(const Pool& other) noexcept = delete;
  Pool(Pool&& other) noexcept = delete;
  Pool& operator=(Pool&& other) noexcept = delete;

  template <typename... Args>
  Type* Construct(Args&&... args) noexcept {
    assert(free_index_ < static_cast<int>(items_.size()) && "Pool capacity exceeded");
    Type* item = new (items_[free_index_]) Type(args...);
    ++free_index_;
    return item;
  }

  void Destruct(Type* item) noexcept {
    assert(free_index_ > 0 && "Pool underflow");
    assert(item - reinterpret_cast<Type*>(raw_items_) < static_cast<int>(items_.size()) &&
           "Invalid item");
    items_[--free_index_] = item;
    std::destroy_at(item);
  }

 private:
  // Array of pointers to pool items.
  std::vector<Type*> items_;

  // Free item index.
  int free_index_ = 0;

  // Raw allocation of pool items.
  std::byte* raw_items_ = nullptr;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_ENGINE_POOL_H_
