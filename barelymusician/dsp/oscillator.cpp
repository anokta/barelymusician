#include "barelymusician/dsp/oscillator.h"

#include <cmath>

#include "barelymusician/dsp/dsp_utils.h"

namespace barely {

Oscillator::Oscillator(int sample_rate) noexcept
    : sample_interval_(sample_rate > 0 ? 1.0 / static_cast<double>(sample_rate)
                                       : 0.0) {}

double Oscillator::Next() noexcept {
  double output = 0.0;
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

void Oscillator::SetFrequency(double frequency) noexcept {
  increment_ = frequency * sample_interval_;
}

void Oscillator::SetType(OscillatorType type) noexcept { type_ = type; }

}  // namespace barely
