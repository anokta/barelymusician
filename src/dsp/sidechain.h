#ifndef BARELYMUSICIAN_DSP_SIDECHAIN_H_
#define BARELYMUSICIAN_DSP_SIDECHAIN_H_

#include <array>
#include <cmath>

#include "core/constants.h"
#include "core/decibels.h"

namespace barely {

// Sidechain compressor.
class Sidechain {
 public:
  void Process(float sidechain_frame[kStereoChannelCount], float mix, float threshold_db,
               float ratio) noexcept {
    for (int channel = 0; channel < kStereoChannelCount; ++channel) {
      float sidechain_db = AmplitudeToDecibels(std::abs(sidechain_frame[channel]));
      if (sidechain_db > threshold_db) {
        const float overshoot_db = sidechain_db - threshold_db;
        sidechain_db = overshoot_db / ratio - overshoot_db;
      } else {
        sidechain_db = 0.0f;  // no gain reduction
      }

      // TODO(#174): Merge this with the envelope implementation.
      const float coeff =
          (sidechain_db < sidechain_db_frame_[channel]) ? attack_coeff_ : release_coeff_;
      sidechain_db_frame_[channel] = std::lerp(sidechain_db, sidechain_db_frame_[channel], coeff);

      sidechain_frame[channel] =
          std::lerp(1.0f, DecibelsToAmplitude(sidechain_db_frame_[channel]), mix);
    }
  }

  void SetAttack(float attack, float sample_rate) noexcept {
    attack_coeff_ = (attack > 0.0f) ? std::exp(-1.0f / (attack * sample_rate)) : 0.0f;
  }

  void SetRelease(float release, float sample_rate) noexcept {
    release_coeff_ = (release > 0.0f) ? std::exp(-1.0f / (release * sample_rate)) : 0.0f;
  }

 private:
  std::array<float, kStereoChannelCount> sidechain_db_frame_ = {};

  float attack_coeff_ = 0.0f;
  float release_coeff_ = 0.0f;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_SIDECHAIN_H_
