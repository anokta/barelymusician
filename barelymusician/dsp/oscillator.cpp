#include "barelymusician/dsp/oscillator.h"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace barely {

Oscillator::Oscillator(int frame_rate) noexcept
    : frame_interval_(frame_rate > 0 ? 1.0 / static_cast<double>(frame_rate) : 0.0),
      max_frequency_(static_cast<double>(frame_rate / 2)) {}  // NOLINT(bugprone-integer-division)

double Oscillator::GetFrequency() const noexcept { return frequency_; }

double Oscillator::Next() noexcept {
  double output = 0.0;
  // Generate the next sample.
  switch (type_) {
    case OscillatorType::kSine:
      output = std::sin(phase_ * 2.0 * std::numbers::pi_v<double>);
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
  frequency_ = std::min(std::max(frequency, 0.0), max_frequency_);
  increment_ = frequency_ * frame_interval_;
}

void Oscillator::SetType(OscillatorType type) noexcept { type_ = type; }

}  // namespace barely
