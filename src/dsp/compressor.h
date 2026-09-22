#ifndef BARELYMUSICIAN_DSP_COMPRESSOR_H_
#define BARELYMUSICIAN_DSP_COMPRESSOR_H_

#include <algorithm>
#include <cmath>

#include "core/constants.h"
#include "core/control.h"
#include "core/decibels.h"

namespace barely {

struct CompressorParams {
  float mix = 1.0f;
  float threshold_db = 0.0f;
  float ratio = 1.0f;

  void Approach(const CompressorParams& params, float coeff) noexcept {
    ApproachValue(mix, params.mix, coeff);
    ApproachValue(threshold_db, params.threshold_db, coeff);
    ApproachValue(ratio, params.ratio, coeff);
  }

  void SetRatio(float normalized_ratio) noexcept {
    static constexpr float kCompRatioRange = 31.0f;  // [1.0f, 32.0f]
    ratio = 1.0f + normalized_ratio * kCompRatioRange;
  }
};

class Compressor {
 public:
  void Process(float frame[kStereoChannelCount], const CompressorParams& params) noexcept {
    const float input_peak_db =
        AmplitudeToDecibels(std::max(std::abs(frame[0]), std::abs(frame[1])));

    const float coeff = (input_peak_db > peak_db_) ? attack_coeff_ : release_coeff_;
    peak_db_ = input_peak_db + coeff * (peak_db_ - input_peak_db);

    if (peak_db_ > params.threshold_db) {
      const float gain = DecibelsToAmplitude(
          params.threshold_db + (peak_db_ - params.threshold_db) / params.ratio - peak_db_);
      for (int channel = 0; channel < kStereoChannelCount; ++channel) {
        frame[channel] = std::lerp(frame[channel], frame[channel] * gain, params.mix);
      }
    }
  }

  void SetAttack(float attack, float sample_rate) noexcept {
    attack_coeff_ = GetCoefficient(sample_rate, attack);
  }

  void SetRelease(float release, float sample_rate) noexcept {
    release_coeff_ = GetCoefficient(sample_rate, release);
  }

 private:
  float attack_coeff_ = 0.0f;
  float release_coeff_ = 0.0f;

  float peak_db_ = kMinDecibels;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_COMPRESSOR_H_
