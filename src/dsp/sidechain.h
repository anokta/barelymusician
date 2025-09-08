#ifndef BARELYMUSICIAN_DSP_SIDECHAIN_H_
#define BARELYMUSICIAN_DSP_SIDECHAIN_H_

#include <cmath>
#include <vector>

#include "dsp/decibels.h"

namespace barely {

/// Sidechain compressor.
class Sidechain {
 public:
  /// Constructs a new `Sidechain`.
  ///
  /// @param sample_rate Sampling rate in hertz.
  /// @param max_channel_count Maximum number of channels.
  Sidechain(int sample_rate, int max_channel_count) noexcept
      : sample_interval_(1.0f / static_cast<float>(sample_rate)),
        sidechain_db_frame_(max_channel_count, 0.0f) {}

  /// Processes the next sidechain frame.
  ///
  /// @param input_frame Input frame.
  /// @param sidechain_frame Sidechain frame.
  /// @param channel_count Number of channels.
  void Process(float* sidechain_frame, int channel_count, float mix, float threshold_db,
               float ratio) noexcept {
    assert(channel_count <= static_cast<int>(sidechain_db_frame_.size()));

    for (int channel = 0; channel < channel_count; ++channel) {
      const float input_gain_db = AmplitudeToDecibels(std::abs(sidechain_frame[channel]));

      float sidechain_db = 0.0f;
      if (input_gain_db > threshold_db) {
        const float overshoot_db = input_gain_db - threshold_db;
        sidechain_db = overshoot_db / ratio - overshoot_db;
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
  void SetAttack(float attack) noexcept {
    attack_coeff_ = (attack > 0.0f) ? std::exp(-sample_interval_ / attack) : 0.0f;
  }

  /// Sets the release.
  ///
  /// @param release Release in seconds.
  void SetRelease(float release) noexcept {
    release_coeff_ = (release > 0.0f) ? std::exp(-sample_interval_ / release) : 0.0f;
  }

 private:
  // Sample interval.
  float sample_interval_ = 0.0f;

  // Attack coefficient.
  float attack_coeff_ = 0.0f;

  // Release coefficient.
  float release_coeff_ = 0.0f;

  // Sidechain frame in decibels.
  std::vector<float> sidechain_db_frame_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_SIDECHAIN_H_
