#include "barelymusician/instrument/oscillator.h"

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace barelyapi {

Oscillator::Oscillator(float sample_interval)
    : sample_interval_(sample_interval),
      type_(Type::kNoise),
      phase_(0.0f),
      increment_(0.0f) {
  DCHECK_GE(sample_interval_, 0.0f);
}

float Oscillator::Next() {
  float output = 0.0f;
  // Generate the next sample.
  switch (type_) {
    case Type::kSine:
      output = std::sin(phase_ * kTwoPi);
      break;
    case Type::kSaw:
      output = 2.0f * phase_ - 1.0f;
      break;
    case Type::kSquare:
      output = (phase_ < 0.5f) ? -1.0f : 1.0f;
      break;
    case Type::kNoise:
    default:
      // TODO(#3): Consider switching to STL random number generators.
      output = 2.0f * (static_cast<float>(std::rand()) / RAND_MAX) - 1.0f;
      break;
  }
  // Update the phasor.
  phase_ += increment_;
  if (phase_ >= 1.0f) {
    phase_ -= 1.0f;
  }
  return output;
}

void Oscillator::Reset() { phase_ = 0.0f; }

void Oscillator::SetFrequency(float frequency) {
  increment_ = std::max(frequency, 0.0f) * sample_interval_;
}

void Oscillator::SetType(Type type) { type_ = type; }

}  // namespace barelyapi
