#include "barelymusician/dsp/oscillator.h"

#include <cmath>

#include "barelymusician/dsp/dsp_utils.h"

namespace barely {

Oscillator::Oscillator(int sample_rate)
    : sample_interval_(sample_rate > 0 ? 1.0f / static_cast<float>(sample_rate)
                                       : 0.0f),
      type_(OscillatorType::kNoise),
      phase_(0.0f),
      increment_(0.0f) {}

float Oscillator::Next() {
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

void Oscillator::Reset() { phase_ = 0.0f; }

void Oscillator::SetFrequency(float frequency) {
  increment_ = frequency * sample_interval_;
}

void Oscillator::SetType(OscillatorType type) { type_ = type; }

}  // namespace barely
