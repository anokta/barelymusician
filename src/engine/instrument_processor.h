#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_

#include <cassert>
#include <cmath>

#include "core/constants.h"
#include "dsp/biquad_filter.h"
#include "engine/instrument_params.h"

namespace barely {

inline void SetInstrumentControl(InstrumentParams& params, float sample_interval,
                                 BarelyInstrumentControlType type, float value) noexcept {
  switch (type) {
    case BarelyInstrumentControlType_kGain:
      params.voice_params.gain = value;
      break;
    case BarelyInstrumentControlType_kPitchShift:
      params.pitch_shift = value;
      params.osc_increment = std::pow(2.0f, params.osc_pitch_shift + params.pitch_shift) *
                             kReferenceFrequency * sample_interval;
      params.slice_increment = std::pow(2.0f, params.pitch_shift) * sample_interval;
      break;
    case BarelyInstrumentControlType_kRetrigger:
      params.should_retrigger = static_cast<bool>(value);
      break;
    case BarelyInstrumentControlType_kStereoPan:
      params.voice_params.stereo_pan = value;
      break;
    case BarelyInstrumentControlType_kVoiceCount:
      params.voice_count = static_cast<uint32_t>(value);
      break;
    case BarelyInstrumentControlType_kAttack:
      params.adsr.SetAttack(sample_interval, value);
      break;
    case BarelyInstrumentControlType_kDecay:
      params.adsr.SetDecay(sample_interval, value);
      break;
    case BarelyInstrumentControlType_kSustain:
      params.adsr.SetSustain(value);
      break;
    case BarelyInstrumentControlType_kRelease:
      params.adsr.SetRelease(sample_interval, value);
      break;
    case BarelyInstrumentControlType_kOscMix:
      params.voice_params.osc_mix = value;
      break;
    case BarelyInstrumentControlType_kOscMode:
      params.osc_mode = static_cast<OscMode>(value);
      break;
    case BarelyInstrumentControlType_kOscNoiseMix:
      params.voice_params.osc_noise_mix = value;
      break;
    case BarelyInstrumentControlType_kOscPitchShift:
      params.osc_pitch_shift = value;
      params.osc_increment = std::pow(2.0f, params.osc_pitch_shift + params.pitch_shift) *
                             kReferenceFrequency * sample_interval;
      break;
    case BarelyInstrumentControlType_kOscShape:
      params.voice_params.osc_shape = value;
      break;
    case BarelyInstrumentControlType_kOscSkew:
      params.voice_params.osc_skew = value;
      break;
    case BarelyInstrumentControlType_kSliceMode:
      params.slice_mode = static_cast<SliceMode>(value);
      break;
    case BarelyInstrumentControlType_kBitCrusherDepth:
      // Offset the bit depth by 1 to normalize the range.
      params.voice_params.bit_crusher_range = std::pow(2.0f, value - 1.0f);
      break;
    case BarelyInstrumentControlType_kBitCrusherRate:
      params.voice_params.bit_crusher_increment = value;
      break;
    case BarelyInstrumentControlType_kDistortionAmount:
      params.voice_params.distortion_amount = value;
      break;
    case BarelyInstrumentControlType_kDistortionDrive:
      params.voice_params.distortion_drive = value;
      break;
    case BarelyInstrumentControlType_kDelaySend:
      params.voice_params.delay_send = value;
      break;
    case BarelyInstrumentControlType_kSidechainSend:
      params.voice_params.sidechain_send = value;
      break;
    case BarelyInstrumentControlType_kFilterType:
      params.filter_type = static_cast<FilterType>(value);
      params.voice_params.filter_coeffs = GetFilterCoefficients(
          sample_interval, params.filter_type, params.filter_frequency, params.filter_q);
      break;
    case BarelyInstrumentControlType_kFilterFrequency:
      params.filter_frequency = value;
      params.voice_params.filter_coeffs = GetFilterCoefficients(
          sample_interval, params.filter_type, params.filter_frequency, params.filter_q);
      break;
    case BarelyInstrumentControlType_kFilterQ:
      params.filter_q = value;
      params.voice_params.filter_coeffs = GetFilterCoefficients(
          sample_interval, params.filter_type, params.filter_frequency, params.filter_q);
      break;
    case BarelyInstrumentControlType_kArpMode:
      [[fallthrough]];
    case BarelyInstrumentControlType_kArpGateRatio:
      [[fallthrough]];
    case BarelyInstrumentControlType_kArpRate:
      break;
    default:
      assert(!"Invalid control type");
      return;
  }
}

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_
