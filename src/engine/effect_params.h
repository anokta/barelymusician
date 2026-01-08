#ifndef BARELYMUSICIAN_ENGINE_EFFECT_PARAMS_H_
#define BARELYMUSICIAN_ENGINE_EFFECT_PARAMS_H_

#include "dsp/compressor.h"
#include "dsp/delay_filter.h"

namespace barely {

/// Effect parameters.
struct EffectParams {
  /// Compressor parameters.
  CompressorParams compressor_params = {};

  /// Delay parameters.
  DelayParams delay_params = {};

  /// Sidechain mix.
  float sidechain_mix = 1.0f;

  /// Sidechain threshold in decibels.
  float sidechain_threshold_db = 0.0f;

  /// Sidechain ratio.
  float sidechain_ratio = 1.0f;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_EFFECT_PARAMS_H_
