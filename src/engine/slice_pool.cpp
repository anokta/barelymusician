#include "engine/slice_pool.h"

#include <barelymusician.h>

#include <cstdint>
#include <memory>

namespace barely {

uint32_t SlicePool::Acquire(const BarelySlice* slices, uint32_t slice_count) noexcept {
  if (free_count_ < slice_count) {
    return UINT32_MAX;
  }

  const uint32_t first_slice_index = free_[free_read_index_];

  uint32_t slice_index = first_slice_index;
  for (uint32_t i = 0; i < slice_count; ++i) {
    free_read_index_ = (free_read_index_ + 1) % kCount;

    const BarelySlice& slice = slices[i];
    const uint32_t next_slice_index = (i + 1 < slice_count) ? free_[free_read_index_] : UINT32_MAX;
    slices_[slice_index] = {
        slice.samples, slice.sample_count, slice.sample_rate, slice.root_pitch, next_slice_index,
    };
    slice_index = next_slice_index;
  }

  free_count_ -= slice_count;

  return first_slice_index;
}

void SlicePool::ReleaseAt(uint32_t first_slice_index, int64_t frame) noexcept {
  to_release_[to_release_write_index_] = {first_slice_index, frame};
  to_release_write_index_ = (to_release_write_index_ + 1) % kCount;
}

void SlicePool::CleanUpSafeToRelease() noexcept {
  const int64_t end_frame = end_frame_.load(std::memory_order_relaxed);
  while (to_release_read_index_ != to_release_write_index_) {
    auto& [first_slice_index, frame] = to_release_[to_release_read_index_];
    if (frame >= end_frame) {
      break;
    }
    uint32_t slice_index = first_slice_index;
    while (slice_index != UINT32_MAX) {
      free_[free_write_index_] = slice_index;
      free_write_index_ = (free_write_index_ + 1) % kCount;
      slice_index = slices_[slice_index].next_slice_index;
      ++free_count_;
    }
    to_release_read_index_ = (to_release_read_index_ + 1) % kCount;
  }
}

void SlicePool::MarkSafeToRelease(int64_t end_frame) noexcept {
  end_frame_.store(end_frame, std::memory_order_release);
}

}  // namespace barely
