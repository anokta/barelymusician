#ifndef BARELYMUSICIAN_DSP_SAMPLE_DATA_H_
#define BARELYMUSICIAN_DSP_SAMPLE_DATA_H_

#include <span>
#include <utility>
#include <vector>

#include "barelymusician.h"
#include "private/random_impl.h"

namespace barely {

/// Class that wraps sample data.
class SampleData {
 public:
  /// Default constructor.
  SampleData() noexcept = default;

  /// Constructs a new `SampleData`.
  ///
  /// @param slices Span of sample data slices.
  SampleData(std::span<const SampleDataSlice> slices) noexcept;

  [[nodiscard]] bool empty() const noexcept { return slices_.empty(); }

  /// Selects the sample data slice for a given pitch.
  ///
  /// @param pitch Note pitch.
  /// @return Pointer to sample data slice.
  [[nodiscard]] const SampleDataSlice* Select(float pitch) const noexcept;

  /// Swaps the sample data.
  ///
  /// @param other Other sample data.
  void Swap(SampleData& other) noexcept { slices_.swap(other.slices_); }

 private:
  // Array of sample data slices.
  std::vector<std::pair<SampleDataSlice, std::vector<float>>> slices_;

  // Random number generator for picking sample data slices.
  inline static RandomImpl random_ = RandomImpl();
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_SAMPLE_DATA_H_
