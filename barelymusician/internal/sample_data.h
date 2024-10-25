#ifndef BARELYMUSICIAN_INTERNAL_SAMPLE_DATA_H_
#define BARELYMUSICIAN_INTERNAL_SAMPLE_DATA_H_

#include <span>
#include <utility>
#include <vector>

#include "barelymusician/barelymusician.h"

namespace barely::internal {

/// Class that wraps sample data.
class SampleData {
 public:
  /// Default constructor.
  SampleData() noexcept = default;

  /// Constructs a new `SampleData`.
  ///
  /// @param slices Span of sample data slices.
  SampleData(std::span<const SampleDataSlice> slices) noexcept;

  /// Selects the sample data slice for a given pitch.
  ///
  /// @param pitch Note pitch.
  /// @return Pointer to sample data slice.
  const SampleDataSlice* Select(double pitch) const noexcept;

  /// Swaps the sample data.
  ///
  /// @param other Other sample data.
  void Swap(SampleData& other) noexcept;

 private:
  // Array of sample data slices.
  std::vector<std::pair<SampleDataSlice, std::vector<double>>> slices_;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_SAMPLE_DATA_H_
