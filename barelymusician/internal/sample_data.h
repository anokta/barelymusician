#ifndef BARELYMUSICIAN_INTERNAL_SAMPLE_DATA_H_
#define BARELYMUSICIAN_INTERNAL_SAMPLE_DATA_H_

#include <span>
#include <utility>
#include <vector>

#include "barelymusician/barelymusician.h"

namespace barely {

/// Class that wraps sample data.
class SampleData {
 public:
  /// Default constructor.
  SampleData() noexcept = default;

  /// Constructs a new `SampleData`.
  ///
  /// @param definitions Span of sample data definitions.
  SampleData(std::span<const SampleDataDefinition> definitions) noexcept;

  /// Selects the sample data for a given pitch.
  ///
  /// @param pitch Note pitch.
  const SampleDataDefinition* Select(double pitch) const noexcept;

  /// Swaps the sample data.
  ///
  /// @param other Other sample data.
  void Swap(SampleData& other) noexcept;

 private:
  // Array of sample data.
  std::vector<std::pair<SampleDataDefinition, std::vector<double>>> data_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_INTERNAL_SAMPLE_DATA_H_
