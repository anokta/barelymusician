#ifndef BARELYMUSICIAN_DSP_INSTRUMENT_POOL_H_
#define BARELYMUSICIAN_DSP_INSTRUMENT_POOL_H_

#include <array>
#include <atomic>
#include <utility>

#include "common/constants.h"
#include "dsp/biquad_filter.h"
#include "dsp/instrument_params.h"
#include "dsp/voice_pool.h"

namespace barely {

/// Maximum number of instruments.
inline constexpr int kMaxInstrumentCount = 1024;

inline void SetInstrumentControl(barely::InstrumentParams& params, float sample_interval,
                                 InstrumentControlType type, float value) noexcept {
  switch (type) {
    case InstrumentControlType::kGain:
      params.voice_params.gain = value;
      break;
    case InstrumentControlType::kPitchShift:
      params.pitch_shift = value;
      params.osc_increment = std::pow(2.0f, params.osc_pitch_shift + params.pitch_shift) *
                             barely::kReferenceFrequency * sample_interval;
      params.slice_increment = std::pow(2.0f, params.pitch_shift) * sample_interval;
      break;
    case InstrumentControlType::kRetrigger:
      params.should_retrigger = static_cast<bool>(value);
      break;
    case InstrumentControlType::kStereoPan:
      params.voice_params.stereo_pan = value;
      break;
    case InstrumentControlType::kVoiceCount:
      params.voice_count = static_cast<int>(value);
      params.active_voice_count = std::min(params.active_voice_count, params.voice_count);
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
      params.osc_mode = static_cast<barely::OscMode>(value);
      break;
    case InstrumentControlType::kOscNoiseMix:
      params.voice_params.osc_noise_mix = value;
      break;
    case InstrumentControlType::kOscPitchShift:
      params.osc_pitch_shift = value;
      params.osc_increment = std::pow(2.0f, params.osc_pitch_shift + params.pitch_shift) *
                             barely::kReferenceFrequency * sample_interval;
      break;
    case InstrumentControlType::kOscShape:
      params.voice_params.osc_shape = value;
      break;
    case InstrumentControlType::kOscSkew:
      params.voice_params.osc_skew = value;
      break;
    case InstrumentControlType::kSliceMode:
      params.slice_mode = static_cast<barely::SliceMode>(value);
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
      [[fallthrough]];
    case InstrumentControlType::kFilterFrequency:
      [[fallthrough]];
    case InstrumentControlType::kFilterQ:
      [[fallthrough]];
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

class InstrumentPool {
 public:
  InstrumentPool() noexcept {
    // TODO(#126): This can be avoided by switching to intrusive lists.
    for (int i = 0; i < kMaxInstrumentCount; ++i) {
      active_instruments_[i] = i;
    }
  }

  [[nodiscard]] InstrumentIndex Create(
      std::span<const BarelyInstrumentControlOverride> control_overrides,
      const barely::BiquadFilter::Coefficients& filter_coeffs, float sample_interval) noexcept {
    if (active_instrument_count_ >= kMaxInstrumentCount) {
      return -1;  // TODO(#126): Handle failure properly.
    }

    const InstrumentIndex index = active_instrument_count_;
    InstrumentParams& params = Get(active_instruments_[index]);
    for (const auto& [type, value] : control_overrides) {
      SetInstrumentControl(params, sample_interval, static_cast<InstrumentControlType>(type),
                           value);
    }
    params.voice_params.filter_coefficients = filter_coeffs;
    params.osc_increment = kReferenceFrequency * sample_interval;
    params.slice_increment = sample_interval;

    ++active_instrument_count_;

    return index;
  }

  void Destroy(InstrumentIndex index) noexcept {
    // TODO(#126): This can be avoided by switching to intrusive lists.
    for (int i = 0; i < active_instrument_count_; ++i) {
      if (active_instruments_[i] == index) {
        std::swap(active_instruments_[i], active_instruments_[active_instrument_count_ - 1]);
        --active_instrument_count_;
        break;
      }
    }
  }

  [[nodiscard]] InstrumentParams& Get(InstrumentIndex index) noexcept {
    assert(index >= 0);
    assert(index < kMaxInstrumentCount);
    return instruments_[index];
  }
  [[nodiscard]] const InstrumentParams& Get(InstrumentIndex index) const noexcept {
    return Get(index);
  }

 private:
  std::array<InstrumentIndex, kMaxInstrumentCount> active_instruments_;
  std::array<InstrumentParams, kMaxInstrumentCount> instruments_;

  std::atomic<int> active_instrument_count_ = 0;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_INSTRUMENT_POOL_H_
