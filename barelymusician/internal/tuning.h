#ifndef BARELYMUSICIAN_INTERNAL_TUNING_H_
#define BARELYMUSICIAN_INTERNAL_TUNING_H_

#include <vector>

#include "barelymusician/barelymusician.h"

namespace barely::internal {

/// Class that wraps a tuning system.
class Tuning {
 public:
  /// Constructs a new `Tuning`.
  ///
  /// @param definition Tuning definition.
  explicit Tuning(const TuningDefinition& definition) noexcept;

  /// Returns the definition.
  ///
  /// @return Tuning definition.
  const TuningDefinition& GetDefinition() const noexcept { return definition_; }

 private:
  // Definition.
  TuningDefinition definition_;

  // Array of pitch ratios.
  std::vector<double> pitch_ratios_;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_TUNING_H_
