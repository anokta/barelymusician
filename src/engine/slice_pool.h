#ifndef BARELYMUSICIAN_ENGINE_SLICE_POOL_H_
#define BARELYMUSICIAN_ENGINE_SLICE_POOL_H_

#include <barelymusician.h>

#include <array>
#include <cassert>
#include <cstdint>

#include "core/arena.h"
#include "core/constants.h"
#include "core/rng.h"
#include "engine/slice_state.h"

namespace barely {

class SlicePool {
 public:
  SlicePool(Arena& arena, uint32_t count) noexcept
      : slices_(arena.AllocArray<SliceState>(count)), free_(arena.AllocArray<uint32_t>(count)) {
    if (arena.is_null()) {
      return;
    }
    assert(count > 0);
    assert(count != kInvalidIndex);
    count_ = count;
    free_count_ = count;
    for (uint32_t i = 0; i < count_; ++i) {
      free_[i] = i;
    }
  }

  [[nodiscard]] uint32_t Acquire(const BarelySlice* slices, uint32_t slice_count) noexcept {
    if (slice_count == 0 || free_count_ < slice_count) {
      return kInvalidIndex;
    }
    assert(slices != nullptr);

    const uint32_t first_slice_index = free_[free_read_index_];

    uint32_t slice_index = first_slice_index;
    for (uint32_t i = 0; i < slice_count; ++i) {
      free_read_index_ = (free_read_index_ + 1) % count_;

      const BarelySlice& slice = slices[i];
      const uint32_t next_slice_index =
          (i + 1 < slice_count) ? free_[free_read_index_] : kInvalidIndex;
      slices_[slice_index] = {
          slice.samples,    slice.sample_count, static_cast<float>(slice.sample_rate),
          slice.root_pitch, next_slice_index,
      };
      slice_index = next_slice_index;
    }

    free_count_ -= slice_count;

    return first_slice_index;
  }

  void Release(uint32_t first_slice_index) noexcept {
    uint32_t slice_index = first_slice_index;
    while (slice_index != kInvalidIndex) {
      free_[free_write_index_] = slice_index;
      free_write_index_ = (free_write_index_ + 1) % count_;
      slice_index = slices_[slice_index].next_slice_index;
      ++free_count_;
    }
  }

  [[nodiscard]] const SliceState* Get(uint32_t slice_index) const noexcept {
    if (slice_index < count_) {
      return &slices_[slice_index];
    }
    return nullptr;
  }

  [[nodiscard]] uint32_t Select(uint32_t first_slice_index, float pitch,
                                AudioRng& rng) const noexcept {
    if (first_slice_index == kInvalidIndex) {
      return kInvalidIndex;
    }

    static constexpr uint32_t kMaxSelectedCount = 16;
    std::array<uint32_t, kMaxSelectedCount> selected_slices;
    uint32_t selected_slice_count = 0;

    uint32_t slice_index = first_slice_index;
    while (slice_index != kInvalidIndex) {
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
            while (slice_index != kInvalidIndex &&
                   slices_[slice_index].root_pitch == slice.root_pitch) {
              if (selected_slice_count < kMaxSelectedCount) {
                selected_slices[selected_slice_count++] = slice_index;
              }
              slice_index = slices_[slice_index].next_slice_index;
            }
          }
          return selected_slices[(selected_slice_count == 1)
                                     ? 0
                                     : rng.Generate(0, selected_slice_count)];
        }
        selected_slices[0] = slice_index;
        selected_slice_count = 1;
      }
      slice_index = slices_[slice_index].next_slice_index;
    }

    assert(selected_slice_count > 0);
    return selected_slices[(selected_slice_count == 1) ? 0 : rng.Generate(0, selected_slice_count)];
  }

 private:
  SliceState* slices_ = nullptr;
  uint32_t* free_ = nullptr;

  uint32_t count_ = 0;
  uint32_t free_read_index_ = 0;
  uint32_t free_write_index_ = 0;
  uint32_t free_count_ = 0;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_SLICE_POOL_H_
