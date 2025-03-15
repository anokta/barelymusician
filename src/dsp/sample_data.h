#ifndef BARELYMUSICIAN_DSP_SAMPLE_DATA_H_
#define BARELYMUSICIAN_DSP_SAMPLE_DATA_H_

#include <span>
#include <utility>
#include <vector>

#include "barelymusician.h"
#include "common/rng.h"

namespace barely {

/// Class that wraps sample data.
class SampleData {
 public:
  /// Default constructor.
  SampleData() noexcept = default;

  /// Constructs a new `SampleData`.
  ///
  /// @param slices Span of slices.
  SampleData(std::span<const Slice> slices) noexcept;

  [[nodiscard]] bool empty() const noexcept { return slices_.empty(); }

  /// Selects the slice for a given pitch.
  ///
  /// @param pitch Note pitch.
  /// @return Pointer to slice.
  [[nodiscard]] const Slice* Select(float pitch) const noexcept;

  /// Swaps the sample data.
  ///
  /// @param other Other sample data.
  void Swap(SampleData& other) noexcept { slices_.swap(other.slices_); }

 private:
  // Array of slices.
  std::vector<std::pair<Slice, std::vector<float>>> slices_;

  // Random number generator for picking slices.
  inline static AudioRng rng_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_SAMPLE_DATA_H_
