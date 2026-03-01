#ifndef BARELYMUSICIAN_ENGINE_VOICE_PARAMS_H_
#define BARELYMUSICIAN_ENGINE_VOICE_PARAMS_H_

#include "core/control.h"
#include "dsp/biquad_filter.h"

namespace barely {

struct VoiceParams {
  float filter_frequency = kMinFilterFreq;
  float filter_q = std::sqrt(0.5f);
  float tone_gain_db = 0.0f;

  float bit_crusher_range = 0.0f;
  float bit_crusher_increment = 1.0f;

  float distortion_amount = 0.0f;
  float distortion_drive = 1.0f;

  float gain = 1.0f;

  float osc_mix = 0.0f;
  float osc_noise_mix = 0.0f;

  float osc_shape = 0.0f;
  float osc_skew = 0.0f;

  float stereo_pan = 0.0f;

  float delay_send = 0.0f;
  float reverb_send = 0.0f;
  float sidechain_send = 0.0f;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_VOICE_PARAMS_H_
