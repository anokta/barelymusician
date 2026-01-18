#ifndef BARELYMUSICIAN_ENGINE_EFFECT_PARAMS_H_
#define BARELYMUSICIAN_ENGINE_EFFECT_PARAMS_H_

#include "dsp/compressor.h"
#include "dsp/delay_filter.h"

namespace barely {

struct EffectParams {
  CompressorParams compressor_params = {};
  DelayParams delay_params = {};

  float sidechain_mix = 1.0f;
  float sidechain_threshold_db = 0.0f;
  float sidechain_ratio = 1.0f;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_EFFECT_PARAMS_H_
