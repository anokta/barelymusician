#ifndef BARELYMUSICIAN_DSP_VOICE_PARAMS_H_
#define BARELYMUSICIAN_DSP_VOICE_PARAMS_H_

#include "dsp/biquad_filter.h"

namespace barely {

/// Voice parameters.
struct VoiceParams {
  /// Filter coefficients.
  BiquadFilter::Coeffs filter_coeffs = {};

  /// Bit crusher range (for bit depth reduction).
  float bit_crusher_range = 0.0f;

  /// Bit crusher increment (for sample rate reduction).
  float bit_crusher_increment = 1.0f;

  /// Distortion amount.
  float distortion_amount = 0.0f;

  /// Distortion drive.
  float distortion_drive = 1.0f;

  /// Gain in linear amplitude.
  float gain = 1.0f;

  /// Oscillator mix.
  float osc_mix = 0.0f;

  /// Oscillator noise mix.
  float osc_noise_mix = 0.0f;

  /// Oscillator shape.
  float osc_shape = 0.0f;

  /// Oscillator skew.
  float osc_skew = 0.0f;

  /// Stereo pan.
  float stereo_pan = 0.0f;

  /// Delay send.
  float delay_send = 0.0f;

  /// Sidechain send.
  float sidechain_send = 0.0f;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_VOICE_PARAMS_H_
