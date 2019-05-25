#include "barelymusician/composition/quantizer.h"

#include <cmath>

#include "barelymusician/base/logging.h"

namespace barelyapi {

Quantizer::Quantizer(int num_notes_per_beat)
    : num_notes_per_beat_(num_notes_per_beat) {
  DCHECK_GT(num_notes_per_beat_, 0);
}

float Quantizer::GetDurationBeats(int num_notes) const {
  const float num_beats = static_cast<float>(num_notes / num_notes_per_beat_);
  return num_beats + static_cast<float>(num_notes % num_notes_per_beat_) /
                         static_cast<float>(num_notes_per_beat_);
}

}  // namespace barelyapi
