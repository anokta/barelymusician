#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_

#include <cassert>
#include <cmath>

#include "core/constants.h"
#include "dsp/biquad_filter.h"
#include "engine/instrument_params.h"

namespace barely {

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

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_
