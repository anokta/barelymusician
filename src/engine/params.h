#ifndef BARELYMUSICIAN_ENGINE_PARAMS_H_
#define BARELYMUSICIAN_ENGINE_PARAMS_H_

#include <barelymusician.h>

#include <cmath>
#include <cstdint>

#include "core/constants.h"
#include "dsp/compressor.h"
#include "dsp/delay_filter.h"
#include "dsp/envelope.h"
#include "dsp/reverb.h"
#include "dsp/tone_filter.h"

namespace barely {

struct EffectParams {
  CompressorParams comp_params = {};
  CompressorParams sidechain_params = {};
  DelayParams delay_params = {};
  ReverbParams reverb_params = {};
  float gain = 1.0f;
};

struct VoiceParams {
  ToneFilterParams filter_params;

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

struct InstrumentParams {
  VoiceParams voice_params = {};
  Envelope::Adsr adsr = {};

  BarelyOscMode osc_mode = BarelyOscMode_kCrossfade;
  BarelySliceMode slice_mode = BarelySliceMode_kSustain;

  float pitch_shift = 0.0f;
  float osc_pitch_shift = 0.0f;

  // Per-sample increments.
  float osc_increment = 0.0f;
  float slice_increment = 0.0f;

  uint32_t first_slice_index = kInvalidIndex;
  uint32_t first_voice_index = kInvalidIndex;

  uint32_t voice_count = 8;

  bool should_retrigger = false;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_PARAMS_H_
