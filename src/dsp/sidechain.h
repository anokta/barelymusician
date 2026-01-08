#ifndef BARELYMUSICIAN_DSP_SIDECHAIN_H_
#define BARELYMUSICIAN_DSP_SIDECHAIN_H_

#include <array>
#include <cmath>

#include "core/constants.h"
#include "dsp/decibels.h"

namespace barely {

/// Sidechain compressor.
class Sidechain {
 public:
  /// Processes the next sidechain frame.
  ///
  /// @param sidechain_frame Input/output sidechain frame.
  /// @param mix Sidechain mix.
  /// @param threshold_db Sidechain threshold in decibels.
  /// @param ratio Sidechain ratio.
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

  /// Sets the attack.
  ///
  /// @param attack Attack in seconds.
  /// @param sample_interval Sampling interval in seconds.
  void SetAttack(float attack, float sample_interval) noexcept {
    attack_coeff_ = (attack > 0.0f) ? std::exp(-sample_interval / attack) : 0.0f;
  }

  /// Sets the release.
  ///
  /// @param release Release in seconds.
  /// @param sample_interval Sampling interval in seconds.
  void SetRelease(float release, float sample_interval) noexcept {
    release_coeff_ = (release > 0.0f) ? std::exp(-sample_interval / release) : 0.0f;
  }

 private:
  // Sidechain frame in decibels.
  std::array<float, kStereoChannelCount> sidechain_db_frame_ = {};

  // Attack coefficient.
  float attack_coeff_ = 0.0f;

  // Release coefficient.
  float release_coeff_ = 0.0f;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_SIDECHAIN_H_
