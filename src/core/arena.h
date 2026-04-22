#ifndef BARELYMUSICIAN_CORE_ARENA_H_
#define BARELYMUSICIAN_CORE_ARENA_H_

#include <cassert>
#include <cstddef>
#include <cstdint>

namespace barely {

constexpr size_t AlignUp(size_t value, size_t alignment) noexcept {
  return (value + alignment - 1) & ~(alignment - 1);
}

class Arena {
 public:
  Arena() noexcept = default;  // null arena for fetching size.
  Arena(void* data, size_t size) noexcept {
    const size_t unaligned_address = reinterpret_cast<size_t>(data);
    const size_t aligned_address = AlignUp(unaligned_address, alignof(std::max_align_t));
    const size_t adjustment = aligned_address - unaligned_address;

    assert(adjustment <= size);
    capacity_ = size - adjustment;
    head_ = reinterpret_cast<std::byte*>(aligned_address);  // NOLINT(performance-no-int-to-ptr)
    assert(head_ != nullptr);
  }

  [[nodiscard]] void* Alloc(size_t size, size_t alignment) noexcept {
    const size_t aligned_offset = AlignUp(offset_, alignment);
    const size_t next_offset = aligned_offset + size;
    assert(head_ == nullptr || next_offset <= capacity_);

    offset_ = next_offset;
    return (head_ != nullptr) ? head_ + aligned_offset : nullptr;
  }

  template <typename T>
  T* Alloc() noexcept {
    return static_cast<T*>(Alloc(sizeof(T), alignof(T)));
  }

  template <typename T>
  T* AllocArray(size_t count) noexcept {
    return static_cast<T*>(Alloc(sizeof(T) * count, alignof(T)));
  }

  [[nodiscard]] bool is_null() const noexcept { return head_ == nullptr; }
  [[nodiscard]] size_t offset() const noexcept { return offset_; }

 private:
  std::byte* head_ = nullptr;
  size_t capacity_ = 0;
  size_t offset_ = 0;
};

template <typename T, typename... Args>
[[nodiscard]] constexpr size_t GetAllocSize(Args&&... args) noexcept {
  Arena arena;  // sizing arena
  arena.Alloc<T>();
  T(arena, args...);
  return AlignUp(arena.offset(), alignof(std::max_align_t)) + alignof(std::max_align_t);
}

}  // namespace barely

#endif  // BARELYMUSICIAN_CORE_ARENA_H_
