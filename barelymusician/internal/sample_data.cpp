#include "barelymusician/internal/sample_data.h"

#include <span>
#include <vector>

#include "barelymusician.h"

namespace barely::internal {

// NOLINTNEXTLINE(bugprone-exception-escape)
SampleData::SampleData(std::span<const SampleDataSlice> slices) noexcept {
  slices_.reserve(slices.size());
  for (int i = 0; i < static_cast<int>(slices.size()); ++i) {
    slices_.emplace_back(
        slices[i],
        std::vector<double>(slices[i].samples, slices[i].samples + slices[i].sample_count));
    slices_[i].first.samples = slices_[i].second.data();
  }
}

const SampleDataSlice* SampleData::Select(double pitch) const noexcept {
  if (slices_.empty()) {
    return nullptr;
  }
  // TODO(#139): `std::upper_bound` turned out to be slow here, but this may be optimized further.
  for (int i = 0; i < static_cast<int>(slices_.size()); ++i) {
    if (const auto* current = &slices_[i].first; pitch <= current->root_pitch) {
      return (i == 0 || pitch - slices_[i - 1].first.root_pitch > current->root_pitch - pitch)
                 ? current
                 : &slices_[i - 1].first;
    }
  }
  return &slices_.back().first;
}

void SampleData::Swap(SampleData& other) noexcept { slices_.swap(other.slices_); }

}  // namespace barely::internal
