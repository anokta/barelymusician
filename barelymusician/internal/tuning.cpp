
#include "barelymusician/internal/tuning.h"

#include <cassert>

#include "barelymusician/barelymusician.h"

namespace barely::internal {

Tuning::Tuning(const TuningDefinition& definition) noexcept
    : definition_(definition),
      pitch_ratios_(definition.pitch_ratios,
                    definition.pitch_ratios + definition.pitch_ratio_count) {
  assert(definition.pitch_ratios != nullptr);
  assert(definition.pitch_ratio_count > 0);
  definition_.pitch_ratios = pitch_ratios_.data();
  definition_.pitch_ratio_count = static_cast<int32_t>(pitch_ratios_.size());
}

}  // namespace barely::internal
