#ifndef BARELYMUSICIAN_DSP_INSTRUMENT_PARAMS_H_
#define BARELYMUSICIAN_DSP_INSTRUMENT_PARAMS_H_

#include <barelymusician.h>

#include <array>
#include <cmath>

#include "common/rng.h"
#include "dsp/envelope.h"
#include "dsp/sample_data.h"
#include "dsp/voice_params.h"

namespace barely {

/// Instrument index.
using InstrumentIndex = int;

/// Instrument parameters.
struct InstrumentParams {
  // TODO(#12): Consider a more optimized implementation for voice stealing.
  // TODO(#126): Convert to intrusive list.
  static constexpr int kMaxVoiceCount = 16;
  std::array<VoiceIndex, kMaxVoiceCount> active_voices;
  int active_voice_count = 0;

  /// Voice parameters.
  VoiceParams voice_params = {};

  /// Envelope adsr.
  Envelope::Adsr adsr = {};

  /// Sample data.
  SampleData sample_data;

  /// Number of voices.
  int voice_count = 8;

  /// Oscillator mode.
  OscMode osc_mode = OscMode::kMix;

  /// Slice mode.
  SliceMode slice_mode = SliceMode::kSustain;

  /// Filter type.
  FilterType filter_type = FilterType::kNone;

  /// Filter cutoff frequency.
  float filter_frequency = 0.0f;

  /// Filter resonance quality factor.
  float filter_q = std::sqrt(0.5f);

  /// Pitch shift.
  float pitch_shift = 0.0f;

  /// Oscillator pitch shift.
  float osc_pitch_shift = 0.0f;

  /// Oscillator increment per sample.
  float osc_increment = 0.0f;

  /// Slice increment per sample.
  float slice_increment = 0.0f;

  /// Denotes whether the instrument should retrigger a note.
  bool should_retrigger = false;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_INSTRUMENT_PARAMS_H_
