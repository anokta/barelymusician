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
  double gain = 1.0;
};

struct VoiceParams {
  ToneFilterParams filter_params;

  double bit_crusher_range = 0.0;
  double bit_crusher_increment = 1.0;

  double distortion_amount = 0.0;
  double distortion_drive = 1.0;

  double gain = 1.0;

  double osc_mix = 0.0;
  double osc_noise_mix = 0.0;

  double osc_shape = 0.0;
  double osc_skew = 0.0;

  double stereo_pan = 0.0;

  double delay_send = 0.0;
  double reverb_send = 0.0;
  double sidechain_send = 0.0;
};

struct InstrumentParams {
  VoiceParams voice_params = {};
  Envelope::Adsr adsr = {};

  BarelyOscMode osc_mode = BarelyOscMode_kCrossfade;
  BarelySliceMode slice_mode = BarelySliceMode_kSustain;

  double pitch_shift = 0.0;
  double osc_pitch_shift = 0.0;

  // Per-sample increments.
  double osc_increment = 0.0;
  double slice_increment = 0.0;

  uint32_t first_slice_index = kInvalidIndex;
  uint32_t first_voice_index = kInvalidIndex;

  uint32_t voice_count = 8;

  bool should_retrigger = false;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_PARAMS_H_
