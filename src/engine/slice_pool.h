#ifndef BARELYMUSICIAN_ENGINE_SLICE_POOL_H_
#define BARELYMUSICIAN_ENGINE_SLICE_POOL_H_

#include <barelymusician.h>

#include <array>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <utility>

#include "core/rng.h"
#include "engine/slice_state.h"

namespace barely {

class SlicePool {
 public:
  SlicePool() noexcept {
    for (uint32_t i = 0; i < kCount; ++i) {
      free_[i] = i;
    }
  }

  [[nodiscard]] uint32_t Acquire(const BarelySlice* slices, uint32_t slice_count) noexcept {
    if (free_count_ < slice_count) {
      return UINT32_MAX;
    }

    const uint32_t first_slice_index = free_[free_read_index_];

    uint32_t slice_index = first_slice_index;
    for (uint32_t i = 0; i < slice_count; ++i) {
      free_read_index_ = (free_read_index_ + 1) % kCount;

      const BarelySlice& slice = slices[i];
      const uint32_t next_slice_index =
          (i + 1 < slice_count) ? free_[free_read_index_] : UINT32_MAX;
      slices_[slice_index] = {
          slice.samples, slice.sample_count, slice.sample_rate, slice.root_pitch, next_slice_index,
      };
      slice_index = next_slice_index;
    }

    free_count_ -= slice_count;

    return first_slice_index;
  }

  void ReleaseAt(uint32_t first_slice_index, int64_t frame) noexcept {
    to_release_[to_release_write_index_] = {first_slice_index, frame};
    to_release_write_index_ = (to_release_write_index_ + 1) % kCount;
  }

  void CleanUpSafeToRelease() noexcept {
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

  void MarkSafeToRelease(int64_t end_frame) noexcept {
    end_frame_.store(end_frame, std::memory_order_release);
  }

  [[nodiscard]] const SliceState* Get(uint32_t slice_index) const noexcept {
    if (slice_index < kCount) {
      return &slices_[slice_index];
    }
    return nullptr;
  }

  [[nodiscard]] uint32_t Select(uint32_t first_slice_index, float pitch,
                                AudioRng& rng) const noexcept {
    if (first_slice_index == UINT32_MAX) {
      return UINT32_MAX;
    }

    static constexpr uint32_t kMaxSelectedCount = 16;
    std::array<uint32_t, kMaxSelectedCount> selected_slices;
    uint32_t selected_slice_count = 0;

    uint32_t slice_index = first_slice_index;
    while (slice_index != UINT32_MAX) {
      if (selected_slice_count == 0 ||
          slices_[slice_index].root_pitch ==
              slices_[selected_slices[selected_slice_count - 1]].root_pitch) {
        if (selected_slice_count < kMaxSelectedCount) {
          selected_slices[selected_slice_count++] = slice_index;
        }
      } else {
        const SliceState& slice = slices_[slice_index];
        const float previous_root_pitch = slices_[selected_slices[0]].root_pitch;
        if (pitch <= slice.root_pitch) {
          if (pitch - previous_root_pitch > slice.root_pitch - pitch) {
            selected_slices[0] = slice_index;
            selected_slice_count = 1;
            while (slice_index != UINT32_MAX &&
                   slices_[slice_index].root_pitch == slice.root_pitch) {
              if (selected_slice_count < kMaxSelectedCount) {
                selected_slices[selected_slice_count++] = slice_index;
              }
              slice_index = slices_[slice_index].next_slice_index;
            }
          }
          return selected_slices[(selected_slice_count == 1)
                                     ? 0
                                     : rng.Generate(0U, selected_slice_count)];
        }
        selected_slices[0] = slice_index;
        selected_slice_count = 1;
      }
      slice_index = slices_[slice_index].next_slice_index;
    }

    assert(selected_slice_count > 0);
    return selected_slices[(selected_slice_count == 1) ? 0
                                                       : rng.Generate(0U, selected_slice_count)];
  }

 private:
  static constexpr uint32_t kCount = BARELY_MAX_SLICE_COUNT;

  std::array<SliceState, kCount> slices_;

  std::atomic<int64_t> end_frame_ = 0;

  // Free queue.
  std::array<uint32_t, kCount> free_;
  uint32_t free_read_index_ = 0;
  uint32_t free_write_index_ = 0;
  uint32_t free_count_ = kCount;

  // Release queue.
  std::array<std::pair<uint32_t, int64_t>, kCount> to_release_;
  uint32_t to_release_read_index_ = 0;
  uint32_t to_release_write_index_ = 0;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_SLICE_POOL_H_
