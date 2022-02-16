#include "examples/dsp/oscillator.h"

#include <cmath>

#include "examples/dsp/dsp_utils.h"

namespace barelyapi {

Oscillator::Oscillator(int sample_rate) noexcept
    : sample_interval_(sample_rate > 0 ? 1.0f / static_cast<float>(sample_rate)
                                       : 0.0f),
      type_(OscillatorType::kNoise),
      increment_(0.0f),
      phase_(0.0f),
      white_noise_distribution_(-1.0f, 1.0f) {}

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
      output = white_noise_distribution_(white_noise_engine_);
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
  increment_ = frequency * sample_interval_;
}

void Oscillator::SetType(OscillatorType type) noexcept { type_ = type; }

}  // namespace barelyapi
