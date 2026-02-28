#ifndef BARELYMUSICIAN_DSP_COMPRESSOR_H_
#define BARELYMUSICIAN_DSP_COMPRESSOR_H_

#include <algorithm>
#include <array>
#include <cmath>

#include "core/constants.h"
#include "core/control.h"
#include "core/decibels.h"

namespace barely {

struct CompressorParams {
  float mix = 1.0f;
  float threshold_db = 0.0f;
  float ratio = 1.0f;

  void Approach(const CompressorParams& params) noexcept {
    ApproachValue(mix, params.mix);
    ApproachValue(threshold_db, params.threshold_db);
    ApproachValue(ratio, params.ratio);
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

    // TODO(#174): Merge this with the envelope implementation.
    const float coeff = (input_peak_db > peak_db_) ? attack_coeff_ : release_coeff_;
    peak_db_ = std::lerp(input_peak_db, peak_db_, coeff);

    if (peak_db_ > params.threshold_db) {
      const float gain = DecibelsToAmplitude(
          params.threshold_db + (peak_db_ - params.threshold_db) / params.ratio - peak_db_);
      for (int channel = 0; channel < kStereoChannelCount; ++channel) {
        frame[channel] = std::lerp(frame[channel], frame[channel] * gain, params.mix);
      }
    }
  }

  void SetAttack(float attack, float sample_rate) noexcept {
    attack_coeff_ = (attack > 0.0f) ? std::exp(-1.0f / (attack * sample_rate)) : 0.0f;
  }

  void SetRelease(float release, float sample_rate) noexcept {
    release_coeff_ = (release > 0.0f) ? std::exp(-1.0f / (release * sample_rate)) : 0.0f;
  }

 private:
  float attack_coeff_ = 0.0f;
  float release_coeff_ = 0.0f;

  float peak_db_ = kMinDecibels;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_COMPRESSOR_H_
