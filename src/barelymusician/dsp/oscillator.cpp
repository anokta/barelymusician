#include "barelymusician/dsp/oscillator.h"

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/base/random.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <unordered_map>

namespace barelyapi {

namespace {

// Type-agnostic oscillator sample generator signutare.
using OscillatorFn = std::function<float(float)>;

// Returns a sine wave sample for the given |phase|.
//
// @param phase Phase offset in range [0, 1].
// @return Sample in range [-1, 1].
float GenerateSineSample(float phase) { return std::sin(phase * kTwoPi); }

// Returns a sawtooth wave sample for the given |phase|.
//
// @param phase Phase offset in range [0, 1].
// @return Sample in range [-1, 1].
float GenerateSawSample(float phase) { return 2.0f * phase - 1.0f; }

// Returns a square wave sample for the given |phase|.
//
// @param phase Phase offset in range [0, 1].
// @return Sample in range [-1, 1].
float GenerateSquareSample(float phase) {
  return (phase < 0.5f) ? -1.0f : 1.0f;
}

// Returns a white noise sample.
//
// @return Sample in range [-1, 1].
float GenerateNoiseSample(float) { return random::Uniform(-1.0f, 1.0f); }

// Oscillator type - sample generation method map.
// TODO(#27): Discourage global initialization of non-primitive types.
static const std::unordered_map<OscillatorType, OscillatorFn> kOscillatorMap = {
    {OscillatorType::kSine, &GenerateSineSample},
    {OscillatorType::kSaw, &GenerateSawSample},
    {OscillatorType::kSquare, &GenerateSquareSample},
    {OscillatorType::kNoise, &GenerateNoiseSample}};

}  // namespace

Oscillator::Oscillator(float sample_interval)
    : sample_interval_(sample_interval),
      type_(OscillatorType::kNoise),
      phase_(0.0f),
      increment_(0.0f) {
  DCHECK_GE(sample_interval_, 0.0f);
}

float Oscillator::Next() {
  // Generate the next sample.
  const float output = kOscillatorMap.at(type_)(phase_);
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

void Oscillator::SetType(OscillatorType type) {
  DCHECK(kOscillatorMap.find(type) != kOscillatorMap.end());
  type_ = type;
}

}  // namespace barelyapi
