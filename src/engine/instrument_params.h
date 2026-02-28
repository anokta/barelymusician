#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_PARAMS_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_PARAMS_H_

#include <barelymusician.h>

#include <cmath>
#include <cstdint>

#include "core/constants.h"
#include "dsp/envelope.h"
#include "engine/voice_params.h"

namespace barely {

struct InstrumentParams {
  VoiceParams voice_params = {};
  Envelope::Adsr adsr = {};

  OscMode osc_mode = OscMode::kCrossfade;
  SliceMode slice_mode = SliceMode::kSustain;

  FilterType filter_type = FilterType::kNone;
  float filter_frequency = kMinFilterFreq;
  float filter_q = std::sqrt(0.5f);

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

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_PARAMS_H_
