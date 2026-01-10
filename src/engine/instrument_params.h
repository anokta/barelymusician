#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_PARAMS_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_PARAMS_H_

#include <barelymusician.h>

#include <cstdint>

#include "dsp/envelope.h"
#include "dsp/sample_data.h"
#include "engine/voice_params.h"

namespace barely {

struct InstrumentParams {
  /// Voice parameters.
  VoiceParams voice_params = {};

  /// Envelope adsr.
  Envelope::Adsr adsr = {};

  /// Sample data.
  SampleData sample_data = {};

  /// Oscillator mode.
  OscMode osc_mode = OscMode::kMix;

  /// Slice mode.
  SliceMode slice_mode = SliceMode::kSustain;

  /// Filter type.
  FilterType filter_type = FilterType::kNone;

  /// Filter cutoff frequency.
  float filter_frequency = 0.0f;

  /// Filter resonance quality factor.
  float filter_q = 0.0f;

  /// Pitch shift.
  float pitch_shift = 0.0f;

  /// Oscillator pitch shift.
  float osc_pitch_shift = 0.0f;

  /// Oscillator increment per sample.
  float osc_increment = 0.0f;

  /// Slice increment per sample.
  float slice_increment = 0.0f;

  /// First voice index.
  uint32_t first_voice_index = UINT32_MAX;

  /// Number of voices.
  uint32_t voice_count = 0;

  /// Denotes whether the instrument should retrigger a note.
  bool should_retrigger = false;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_PARAMS_H_
