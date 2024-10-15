#include "barelymusician/internal/sample_data.h"

#include <span>

#include "barelymusician/barelymusician.h"

namespace barely {

SampleData::SampleData(std::span<const SampleDataDefinition> definitions) noexcept {
  data_.reserve(definitions.size());
  for (int i = 0; i < static_cast<int>(definitions.size()); ++i) {
    data_.emplace_back(definitions[i],
                       std::vector<double>(definitions[i].samples,
                                           definitions[i].samples + definitions[i].sample_count));
    data_[i].first.samples = data_[i].second.data();
  }
}

const SampleDataDefinition* SampleData::Select(double pitch) const noexcept {
  if (data_.empty()) {
    return nullptr;
  }
  // TODO(#139): `std::upper_bound` turned out to be slow here, but this may be optimized further.
  for (int i = 0; i < static_cast<int>(data_.size()); ++i) {
    if (const auto* current = &data_[i].first; pitch <= current->root_pitch) {
      return (i == 0 || pitch - data_[i - 1].first.root_pitch > current->root_pitch - pitch)
                 ? current
                 : &data_[i - 1].first;
    }
  }
  return &data_.back().first;
}

void SampleData::Swap(SampleData& other) noexcept { data_.swap(other.data_); }

}  // namespace barely
