#include "engine/instrument_processor.h"

#include <barelymusician.h>

#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>

#include "core/constants.h"
#include "dsp/biquad_filter.h"

namespace barely {

void InstrumentProcessor::SetControl(uint32_t instrument_index, BarelyInstrumentControlType type,
                                     float value) noexcept {
  auto& params = engine_.instrument_params[instrument_index];

  // TODO(#126): clean this up?
  if (type == BarelyInstrumentControlType_kVoiceCount) {
    const uint32_t new_voice_count = static_cast<uint32_t>(value);
    uint32_t active_voice_count = 0;
    uint32_t active_voice_index = params.first_voice_index;
    while (active_voice_index != 0 && active_voice_count <= new_voice_count) {
      active_voice_index = engine_.voice_pool.Get(active_voice_index).next_voice_index;
      ++active_voice_count;
    }
    while (active_voice_index != 0) {
      auto& voice = engine_.voice_pool.Get(active_voice_index);
      if (voice.previous_voice_index != 0) {
        engine_.voice_pool.Get(voice.previous_voice_index).next_voice_index = 0;
        voice.previous_voice_index = 0;
      }
      engine_.voice_pool.Release(active_voice_index);
      active_voice_index = voice.next_voice_index;
      voice.next_voice_index = 0;
    }
  }

  switch (type) {
    case BarelyInstrumentControlType_kGain:
      params.voice_params.gain = value;
      break;
    case BarelyInstrumentControlType_kPitchShift:
      params.pitch_shift = value;
      params.osc_increment = std::pow(2.0f, params.osc_pitch_shift + params.pitch_shift) *
                             kReferenceFrequency * engine_.sample_interval;
      params.slice_increment = std::pow(2.0f, params.pitch_shift) * engine_.sample_interval;
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
      params.adsr.SetAttack(engine_.sample_interval, value);
      break;
    case BarelyInstrumentControlType_kDecay:
      params.adsr.SetDecay(engine_.sample_interval, value);
      break;
    case BarelyInstrumentControlType_kSustain:
      params.adsr.SetSustain(value);
      break;
    case BarelyInstrumentControlType_kRelease:
      params.adsr.SetRelease(engine_.sample_interval, value);
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
                             kReferenceFrequency * engine_.sample_interval;
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
          engine_.sample_interval, params.filter_type, params.filter_frequency, params.filter_q);
      break;
    case BarelyInstrumentControlType_kFilterFrequency:
      params.filter_frequency = value;
      params.voice_params.filter_coeffs = GetFilterCoefficients(
          engine_.sample_interval, params.filter_type, params.filter_frequency, params.filter_q);
      break;
    case BarelyInstrumentControlType_kFilterQ:
      params.filter_q = value;
      params.voice_params.filter_coeffs = GetFilterCoefficients(
          engine_.sample_interval, params.filter_type, params.filter_frequency, params.filter_q);
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

void InstrumentProcessor::SetNoteControl(uint32_t instrument_index, float pitch,
                                         BarelyNoteControlType type, float value) noexcept {
  auto& params = engine_.instrument_params[instrument_index];
  switch (type) {
    case BarelyNoteControlType_kGain: {
      uint32_t voice_index = params.first_voice_index;
      while (voice_index != 0) {
        auto& voice = engine_.voice_pool.Get(voice_index);
        if (voice.pitch == pitch && voice.IsOn()) {
          voice.note_params.gain = value;
          break;
        }
        voice_index = voice.next_voice_index;
      }
    } break;
    case BarelyNoteControlType_kPitchShift: {
      uint32_t voice_index = params.first_voice_index;
      while (voice_index != 0) {
        auto& voice = engine_.voice_pool.Get(voice_index);
        if (voice.pitch == pitch && voice.IsOn()) {
          voice.pitch_shift = value;
          voice.UpdatePitchIncrements();
          break;
        }
        voice_index = voice.next_voice_index;
      }
    } break;
    default:
      assert(!"Invalid note control type");
      break;
  }
}

void InstrumentProcessor::SetNoteOff(uint32_t instrument_index, float pitch) noexcept {
  auto& params = engine_.instrument_params[instrument_index];
  uint32_t active_voice_index = params.first_voice_index;
  while (active_voice_index != 0) {
    auto& voice = engine_.voice_pool.Get(active_voice_index);
    if (voice.pitch == pitch && voice.IsOn() &&
        (params.sample_data.empty() || params.slice_mode != SliceMode::kOnce)) {
      voice.Stop();
      break;
    }
    active_voice_index = voice.next_voice_index;
  }
}

void InstrumentProcessor::SetNoteOn(
    uint32_t instrument_index, float pitch,
    const std::array<float, BarelyNoteControlType_kCount>& note_controls) noexcept {
  auto& params = engine_.instrument_params[instrument_index];
  if (const uint32_t voice_index = AcquireVoice(instrument_index, pitch); voice_index > 0) {
    engine_.voice_pool.Get(voice_index)
        .Start(params, instrument_index, params.sample_data.Select(pitch, engine_.audio_rng), pitch,
               note_controls);
  }
}

uint32_t InstrumentProcessor::AcquireVoice(uint32_t instrument_index, float pitch) noexcept {
  auto& params = engine_.instrument_params[instrument_index];
  if (params.should_retrigger) {
    uint32_t current_voice_index = params.first_voice_index;
    while (current_voice_index != 0) {
      VoiceState& voice = engine_.voice_pool.Get(current_voice_index);
      if (voice.pitch == pitch) {
        while (current_voice_index != 0) {
          voice = engine_.voice_pool.Get(current_voice_index);
          ++voice.timestamp;
          current_voice_index = voice.next_voice_index;
        }
        return engine_.voice_pool.GetIndex(voice);
      }
      current_voice_index = voice.next_voice_index;
    }
  }

  uint32_t current_voice_index = params.first_voice_index;
  uint32_t oldest_active_voice_index = 0;
  uint32_t active_voice_count = 0;
  while (current_voice_index != 0) {
    VoiceState& voice = engine_.voice_pool.Get(current_voice_index);
    if (oldest_active_voice_index == 0 ||
        voice.timestamp > engine_.voice_pool.Get(oldest_active_voice_index).timestamp) {
      oldest_active_voice_index = current_voice_index;
    }
    ++voice.timestamp;
    ++active_voice_count;
    if (voice.next_voice_index == 0) {  // TODO(#126): Remove double checking here.
      break;
    }
    current_voice_index = voice.next_voice_index;
  }

  // Acquire new voice.
  if (engine_.voice_pool.GetActiveCount() < engine_.voice_pool.Count() &&
      active_voice_count < params.voice_count) {
    const uint32_t new_voice_index = engine_.voice_pool.Acquire();
    VoiceState& new_voice = engine_.voice_pool.Get(new_voice_index);
    new_voice.previous_voice_index = current_voice_index;
    new_voice.next_voice_index = 0;
    if (current_voice_index != 0) {
      engine_.voice_pool.Get(current_voice_index).next_voice_index = new_voice_index;
    } else {
      params.first_voice_index = new_voice_index;
    }
    return new_voice_index;
  }

  // No voices are available to acquire, steal the oldest active voice.
  return oldest_active_voice_index;
}

void InstrumentProcessor::SetSampleData(uint32_t instrument_index,
                                        SampleData& sample_data) noexcept {
  auto& params = engine_.instrument_params[instrument_index];
  params.sample_data.Swap(sample_data);
  uint32_t active_voice_index = params.first_voice_index;
  while (active_voice_index != 0) {
    auto& voice = engine_.voice_pool.Get(active_voice_index);
    voice.slice = params.sample_data.Select(voice.pitch, engine_.audio_rng);
    voice.UpdatePitchIncrements();
    active_voice_index = voice.next_voice_index;
  }
}

}  // namespace barely
