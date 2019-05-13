#include "barelymusician/instrument/instrument_utils.h"

#include <algorithm>
#include <cmath>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"

namespace barelyapi {

namespace {

// Middle A note (A4) frequency.
const float kFrequencyA4 = 440.0f;

}  // namespace

float FrequencyFromNoteIndex(float index) {
  // Middle A note (A4) is selected as the base note frequency, where:
  //  f = fA4 * 2 ^ ((i - iA4) / 12).
  return kFrequencyA4 * std::pow(2.0f, (index - kNoteIndexA4) / kNumSemitones);
}

}  // namespace barelyapi
