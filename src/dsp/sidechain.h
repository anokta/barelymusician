#ifndef BARELYMUSICIAN_DSP_SIDECHAIN_H_
#define BARELYMUSICIAN_DSP_SIDECHAIN_H_

#include <array>
#include <cmath>

#include "core/constants.h"
#include "core/control.h"
#include "core/decibels.h"
#include "dsp/compressor.h"

namespace barely {

// Sidechain compressor.
class Sidechain {
 public:
  void Process(double sidechain_frame[kStereoChannelCount],
               const CompressorParams& params) noexcept {
    for (int channel = 0; channel < kStereoChannelCount; ++channel) {
      double sidechain_db = AmplitudeToDecibels(std::abs(sidechain_frame[channel]));
      if (sidechain_db > params.threshold_db) {
        const double overshoot_db = sidechain_db - params.threshold_db;
        sidechain_db = overshoot_db / params.ratio - overshoot_db;
      } else {
        sidechain_db = 0.0;  // no gain reduction
      }

      const double coeff =
          (sidechain_db < sidechain_db_frame_[channel]) ? attack_coeff_ : release_coeff_;
      sidechain_db_frame_[channel] =
          sidechain_db + coeff * (sidechain_db_frame_[channel] - sidechain_db);

      sidechain_frame[channel] =
          std::lerp(1.0, DecibelsToAmplitude(sidechain_db_frame_[channel]), params.mix);
    }
  }

  void SetAttack(double attack, double sample_rate) noexcept {
    attack_coeff_ = GetCoefficient(sample_rate, attack);
  }

  void SetRelease(double release, double sample_rate) noexcept {
    release_coeff_ = GetCoefficient(sample_rate, release);
  }

 private:
  std::array<double, kStereoChannelCount> sidechain_db_frame_ = {};

  double attack_coeff_ = 0.0;
  double release_coeff_ = 0.0;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_SIDECHAIN_H_
