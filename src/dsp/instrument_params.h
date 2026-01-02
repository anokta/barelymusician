#ifndef BARELYMUSICIAN_DSP_INSTRUMENT_PARAMS_H_
#define BARELYMUSICIAN_DSP_INSTRUMENT_PARAMS_H_

#include <barelymusician.h>

#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>

#include "common/constants.h"
#include "common/rng.h"
#include "dsp/biquad_filter.h"
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

  /// First active voice index.
  uint32_t first_active_voice_index = 0;

  /// Number of voices.
  uint32_t voice_count = 0;

  /// Denotes whether the instrument should retrigger a note.
  bool should_retrigger = false;
};

inline void SetInstrumentControl(InstrumentParams& params, float sample_interval,
                                 InstrumentControlType type, float value) noexcept {
  switch (type) {
    case InstrumentControlType::kGain:
      params.voice_params.gain = value;
      break;
    case InstrumentControlType::kPitchShift:
      params.pitch_shift = value;
      params.osc_increment = std::pow(2.0f, params.osc_pitch_shift + params.pitch_shift) *
                             kReferenceFrequency * sample_interval;
      params.slice_increment = std::pow(2.0f, params.pitch_shift) * sample_interval;
      break;
    case InstrumentControlType::kRetrigger:
      params.should_retrigger = static_cast<bool>(value);
      break;
    case InstrumentControlType::kStereoPan:
      params.voice_params.stereo_pan = value;
      break;
    case InstrumentControlType::kVoiceCount:
      params.voice_count = static_cast<uint32_t>(value);
      break;
    case InstrumentControlType::kAttack:
      params.adsr.SetAttack(sample_interval, value);
      break;
    case InstrumentControlType::kDecay:
      params.adsr.SetDecay(sample_interval, value);
      break;
    case InstrumentControlType::kSustain:
      params.adsr.SetSustain(value);
      break;
    case InstrumentControlType::kRelease:
      params.adsr.SetRelease(sample_interval, value);
      break;
    case InstrumentControlType::kOscMix:
      params.voice_params.osc_mix = value;
      break;
    case InstrumentControlType::kOscMode:
      params.osc_mode = static_cast<OscMode>(value);
      break;
    case InstrumentControlType::kOscNoiseMix:
      params.voice_params.osc_noise_mix = value;
      break;
    case InstrumentControlType::kOscPitchShift:
      params.osc_pitch_shift = value;
      params.osc_increment = std::pow(2.0f, params.osc_pitch_shift + params.pitch_shift) *
                             kReferenceFrequency * sample_interval;
      break;
    case InstrumentControlType::kOscShape:
      params.voice_params.osc_shape = value;
      break;
    case InstrumentControlType::kOscSkew:
      params.voice_params.osc_skew = value;
      break;
    case InstrumentControlType::kSliceMode:
      params.slice_mode = static_cast<SliceMode>(value);
      break;
    case InstrumentControlType::kBitCrusherDepth:
      // Offset the bit depth by 1 to normalize the range.
      params.voice_params.bit_crusher_range = std::pow(2.0f, value - 1.0f);
      break;
    case InstrumentControlType::kBitCrusherRate:
      params.voice_params.bit_crusher_increment = value;
      break;
    case InstrumentControlType::kDistortionAmount:
      params.voice_params.distortion_amount = value;
      break;
    case InstrumentControlType::kDistortionDrive:
      params.voice_params.distortion_drive = value;
      break;
    case InstrumentControlType::kDelaySend:
      params.voice_params.delay_send = value;
      break;
    case InstrumentControlType::kSidechainSend:
      params.voice_params.sidechain_send = value;
      break;
    case InstrumentControlType::kFilterType:
      params.filter_type = static_cast<FilterType>(value);
      params.voice_params.filter_coeffs = GetFilterCoefficients(
          sample_interval, params.filter_type, params.filter_frequency, params.filter_q);
      break;
    case InstrumentControlType::kFilterFrequency:
      params.filter_frequency = value;
      params.voice_params.filter_coeffs = GetFilterCoefficients(
          sample_interval, params.filter_type, params.filter_frequency, params.filter_q);
      break;
    case InstrumentControlType::kFilterQ:
      params.filter_q = value;
      params.voice_params.filter_coeffs = GetFilterCoefficients(
          sample_interval, params.filter_type, params.filter_frequency, params.filter_q);
      break;
    case InstrumentControlType::kArpMode:
      [[fallthrough]];
    case InstrumentControlType::kArpGateRatio:
      [[fallthrough]];
    case InstrumentControlType::kArpRate:
      break;
    default:
      assert(!"Invalid control type");
      return;
  }
}

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_INSTRUMENT_PARAMS_H_
