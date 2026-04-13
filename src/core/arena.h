#ifndef BARELYMUSICIAN_CORE_ARENA_H_
#define BARELYMUSICIAN_CORE_ARENA_H_

#include <cstddef>
#include <cstdint>

namespace barely {

inline constexpr size_t AlignUp(size_t value, size_t alignment) noexcept {
  return (value + alignment - 1) & ~(alignment - 1);
}

class Arena {
 public:
  Arena() noexcept = default;
  Arena(void* data, size_t size) noexcept {
    const size_t unaligned_address = reinterpret_cast<size_t>(data);
    const size_t aligned_address = AlignUp(unaligned_address, alignof(std::max_align_t));
    const size_t adjustment = aligned_address - unaligned_address;
    capacity_ = (size > adjustment) ? (size - adjustment) : 0;
    head_ = (capacity_ > 0) ? reinterpret_cast<std::byte*>(aligned_address) : nullptr;
  }

  void* Alloc(size_t size, size_t alignment) noexcept {
    if (head_ == nullptr) {
      return nullptr;
    }
    const size_t aligned_offset = AlignUp(offset_, alignment);
    const size_t next_offset = aligned_offset + size;
    if (next_offset > capacity_) {
      return nullptr;
    }
    offset_ = next_offset;
    return head_ + aligned_offset;
  }

  template <typename T>
  T* Alloc() noexcept {
    return static_cast<T*>(Alloc(sizeof(T), alignof(T)));
  }

  template <typename T>
  T* AllocArray(size_t count) noexcept {
    return static_cast<T*>(Alloc(sizeof(T) * count, alignof(T)));
  }

 private:
  std::byte* head_ = nullptr;
  size_t capacity_ = 0;
  size_t offset_ = 0;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_CORE_ARENA_H_
