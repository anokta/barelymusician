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
  void Process(float sidechain_frame[kStereoChannelCount],
               const CompressorParams& params) noexcept {
    for (int channel = 0; channel < kStereoChannelCount; ++channel) {
      float sidechain_db = AmplitudeToDecibels(std::abs(sidechain_frame[channel]));
      if (sidechain_db > params.threshold_db) {
        const float overshoot_db = sidechain_db - params.threshold_db;
        sidechain_db = overshoot_db / params.ratio - overshoot_db;
      } else {
        sidechain_db = 0.0f;  // no gain reduction
      }

      const float coeff =
          (sidechain_db < sidechain_db_frame_[channel]) ? attack_coeff_ : release_coeff_;
      sidechain_db_frame_[channel] =
          sidechain_db + coeff * (sidechain_db_frame_[channel] - sidechain_db);

      sidechain_frame[channel] =
          std::lerp(1.0f, DecibelsToAmplitude(sidechain_db_frame_[channel]), params.mix);
    }
  }

  void SetAttack(float attack, float sample_rate) noexcept {
    attack_coeff_ = GetEnvelopeCoefficient(sample_rate, attack);
  }

  void SetRelease(float release, float sample_rate) noexcept {
    release_coeff_ = GetEnvelopeCoefficient(sample_rate, release);
  }

 private:
  std::array<float, kStereoChannelCount> sidechain_db_frame_ = {};

  float attack_coeff_ = 0.0f;
  float release_coeff_ = 0.0f;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_SIDECHAIN_H_
