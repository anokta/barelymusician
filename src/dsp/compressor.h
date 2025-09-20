#ifndef BARELYMUSICIAN_DSP_COMPRESSOR_H_
#define BARELYMUSICIAN_DSP_COMPRESSOR_H_

#include <algorithm>
#include <array>
#include <cmath>

#include "common/constants.h"
#include "dsp/control.h"
#include "dsp/decibels.h"

namespace barely {

struct CompressorParams {
  /// Compressor mix.
  float mix = 0.0f;

  /// Compressor threshold in decibels.
  float threshold_db = 0.0f;

  /// Compressor ratio.
  float ratio = 1.0f;

  /// Approaches parameters.
  ///
  /// @param params Compressor parameters to approach to.
  void Approach(const CompressorParams& params) noexcept {
    ApproachValue(mix, params.mix);
    ApproachValue(threshold_db, params.threshold_db);
    ApproachValue(ratio, params.ratio);
  }
};

/// Compressor.
class Compressor {
 public:
  /// Constructs a new `Compressor`.
  ///
  /// @param sample_rate Sampling rate in hertz.
  explicit Compressor(int sample_rate) noexcept
      : sample_interval_(1.0f / static_cast<float>(sample_rate)) {}

  /// Processes the next compressor frame.
  ///
  /// @param frame Input/output frame.
  /// @param params Compressor parameters.
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

  // Last peak value in decibels.
  float peak_db_ = kMinDecibels;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_COMPRESSOR_H_
