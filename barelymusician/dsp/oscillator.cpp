#include "barelymusician/dsp/oscillator.h"

#include <cmath>

#include "barelymusician/barelymusician.h"
#include "barelymusician/dsp/dsp_utils.h"

namespace barely {

Oscillator::Oscillator(Integer frame_rate) noexcept
    : frame_interval_(frame_rate > 0 ? 1.0 / static_cast<Real>(frame_rate)
                                     : 0.0) {}

Real Oscillator::Next() noexcept {
  Real output = 0.0;
  // Generate the next sample.
  switch (type_) {
    case OscillatorType::kSine:
      output = std::sin(phase_ * kTwoPi);
      break;
    case OscillatorType::kSaw:
      output = 2.0 * phase_ - 1.0;
      break;
    case OscillatorType::kSquare:
      output = (phase_ < 0.5) ? -1.0 : 1.0;
      break;
    case OscillatorType::kNoise:
    default:
      output = random_.DrawUniform(-1.0, 1.0);
      break;
  }
  // Update the phasor.
  phase_ += increment_;
  if (phase_ >= 1.0) {
    phase_ -= 1.0;
  }
  return output;
}

void Oscillator::Reset() noexcept { phase_ = 0.0; }

void Oscillator::SetFrequency(Real frequency) noexcept {
  increment_ = frequency * frame_interval_;
}

void Oscillator::SetType(OscillatorType type) noexcept { type_ = type; }

}  // namespace barely
