#ifndef BARELYMUSICIAN_DSP_COMPRESSOR_H_
#define BARELYMUSICIAN_DSP_COMPRESSOR_H_

#include <algorithm>
#include <cmath>

#include "core/constants.h"
#include "core/control.h"
#include "core/decibels.h"

namespace barely {

struct CompressorParams {
  double mix = 1.0;
  double threshold_db = 0.0;
  double ratio = 1.0;

  void Approach(const CompressorParams& params, double coeff) noexcept {
    ApproachValue(mix, params.mix, coeff);
    ApproachValue(threshold_db, params.threshold_db, coeff);
    ApproachValue(ratio, params.ratio, coeff);
  }

  void SetRatio(double normalized_ratio) noexcept {
    static constexpr double kCompRatioRange = 31.0;  // [1.0, 32.0]
    ratio = 1.0 + normalized_ratio * kCompRatioRange;
  }
};

class Compressor {
 public:
  void Process(double frame[kStereoChannelCount], const CompressorParams& params) noexcept {
    const double input_peak_db =
        AmplitudeToDecibels(std::max(std::abs(frame[0]), std::abs(frame[1])));

    const double coeff = (input_peak_db > peak_db_) ? attack_coeff_ : release_coeff_;
    peak_db_ = input_peak_db + coeff * (peak_db_ - input_peak_db);

    if (peak_db_ > params.threshold_db) {
      const double gain = DecibelsToAmplitude(
          params.threshold_db + (peak_db_ - params.threshold_db) / params.ratio - peak_db_);
      for (int channel = 0; channel < kStereoChannelCount; ++channel) {
        frame[channel] = std::lerp(frame[channel], frame[channel] * gain, params.mix);
      }
    }
  }

  void SetAttack(double attack, double sample_rate) noexcept {
    attack_coeff_ = GetCoefficient(sample_rate, attack);
  }

  void SetRelease(double release, double sample_rate) noexcept {
    release_coeff_ = GetCoefficient(sample_rate, release);
  }

 private:
  double attack_coeff_ = 0.0;
  double release_coeff_ = 0.0;

  double peak_db_ = kMinDecibels;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_COMPRESSOR_H_
