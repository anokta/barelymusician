#include "dsp/sample_data.h"

#include <barelymusician.h>

#include <span>
#include <vector>

#include "core/rng.h"

namespace barely {

// NOLINTNEXTLINE(bugprone-exception-escape)
SampleData::SampleData(std::span<const BarelySlice> slices) noexcept {
  slices_.reserve(slices.size());
  for (int i = 0; i < static_cast<int>(slices.size()); ++i) {
    slices_.emplace_back(slices[i]);
  }
}

const Slice* SampleData::Select(float pitch, AudioRng& rng) const noexcept {
  if (slices_.empty()) {
    return nullptr;
  }
  const int slice_count = static_cast<int>(slices_.size());
  // TODO(#139): `std::upper_bound` turned out to be slow here, but this may be optimized further.
  float current_pitch = slices_.front().root_pitch;
  int current_start_index = 0;
  for (int i = 0; i < slice_count; ++i) {
    const auto* current = &slices_[i];
    if (current_pitch != current->root_pitch) {
      if (pitch <= current->root_pitch) {
        if (pitch - current_pitch > current->root_pitch - pitch) {
          current_start_index = i;
          while (i < slice_count && slices_[i].root_pitch == current->root_pitch) {
            ++i;
          }
        }
        return &slices_[(current_start_index + 1 == i) ? current_start_index
                                                       : rng.Generate(current_start_index, i)];
      }
      current_pitch = current->root_pitch;
      current_start_index = i;
    }
  }
  return &slices_[(current_start_index + 1 == slice_count)
                      ? current_start_index
                      : rng.Generate(current_start_index, slice_count)];
}

}  // namespace barely
