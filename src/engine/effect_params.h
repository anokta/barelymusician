#ifndef BARELYMUSICIAN_ENGINE_EFFECT_PARAMS_H_
#define BARELYMUSICIAN_ENGINE_EFFECT_PARAMS_H_

#include "dsp/compressor.h"
#include "dsp/delay_filter.h"
#include "dsp/reverb.h"

namespace barely {

struct EffectParams {
  CompressorParams comp_params = {};
  CompressorParams sidechain_params = {};
  DelayParams delay_params = {};
  ReverbParams reverb_params = {};
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_EFFECT_PARAMS_H_
