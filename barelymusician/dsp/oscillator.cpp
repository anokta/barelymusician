#include "barelymusician/dsp/oscillator.h"

#include <algorithm>
#include <cmath>

#include "barelymusician/dsp/dsp_utils.h"

namespace barely {

Oscillator::Oscillator(int frame_rate) noexcept
    : frame_interval_(frame_rate > 0 ? 1.0f / static_cast<float>(frame_rate) : 0.0f),
      max_frequency_(static_cast<float>(frame_rate / 2)) {}  // NOLINT(bugprone-integer-division)

float Oscillator::Next() noexcept {
  float output = 0.0f;
  // Generate the next sample.
  switch (type_) {
    case OscillatorType::kSine:
      output = std::sin(phase_ * kTwoPi);
      break;
    case OscillatorType::kSaw:
      output = 2.0f * phase_ - 1.0f;
      break;
    case OscillatorType::kSquare:
      output = (phase_ < 0.5f) ? -1.0f : 1.0f;
      break;
    case OscillatorType::kNoise:
    default:
      output = random_.DrawUniform(-1.0f, 1.0f);
      break;
  }
  // Update the phasor.
  phase_ += increment_;
  if (phase_ >= 1.0f) {
    phase_ -= 1.0f;
  }
  return output;
}

void Oscillator::Reset() noexcept { phase_ = 0.0f; }

void Oscillator::SetFrequency(float frequency) noexcept {
  increment_ = std::min(std::max(frequency, 0.0f), max_frequency_) * frame_interval_;
}

void Oscillator::SetType(OscillatorType type) noexcept { type_ = type; }

}  // namespace barely
