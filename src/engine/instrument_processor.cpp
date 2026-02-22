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
  switch (type) {
    case BarelyInstrumentControlType_kGain:
      params.voice_params.gain = GetGain(value);
      break;
    case BarelyInstrumentControlType_kPitchShift:
      params.pitch_shift = value;
      params.osc_increment = std::pow(2.0f, params.osc_pitch_shift + params.pitch_shift) *
                             kReferenceFrequency / engine_.sample_rate;
      params.slice_increment = std::pow(2.0f, params.pitch_shift) / engine_.sample_rate;
      break;
    case BarelyInstrumentControlType_kStereoPan:
      params.voice_params.stereo_pan = value;
      break;
    case BarelyInstrumentControlType_kRetrigger:
      params.should_retrigger = static_cast<bool>(value);
      break;
    case BarelyInstrumentControlType_kVoiceCount: {
      const uint32_t new_voice_count = static_cast<uint32_t>(value);
      uint32_t active_voice_count = 0;
      uint32_t active_voice_index = params.first_voice_index;
      while (active_voice_index != kInvalidIndex && active_voice_count <= new_voice_count) {
        active_voice_index = engine_.GetVoice(active_voice_index).next_voice_index;
        ++active_voice_count;
      }
      // Release the previously active voices beyond the new voice count.
      while (active_voice_index != kInvalidIndex) {
        auto& voice = engine_.GetVoice(active_voice_index);
        if (voice.prev_voice_index != kInvalidIndex) {
          engine_.GetVoice(voice.prev_voice_index).next_voice_index = kInvalidIndex;
          voice.prev_voice_index = kInvalidIndex;
        }
        const uint32_t next_voice_index = voice.next_voice_index;
        voice.next_voice_index = kInvalidIndex;
        engine_.voice_pool.Release(active_voice_index);
        active_voice_index = next_voice_index;
      }
      params.voice_count = new_voice_count;
    } break;
    case BarelyInstrumentControlType_kAttack:
      params.adsr.SetAttack(engine_.sample_rate, value);
      break;
    case BarelyInstrumentControlType_kDecay:
      params.adsr.SetDecay(engine_.sample_rate, value);
      break;
    case BarelyInstrumentControlType_kSustain:
      params.adsr.SetSustain(value);
      break;
    case BarelyInstrumentControlType_kRelease:
      params.adsr.SetRelease(engine_.sample_rate, value);
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
                             kReferenceFrequency / engine_.sample_rate;
      break;
    case BarelyInstrumentControlType_kOscShape:
      params.voice_params.osc_shape = value;
      break;
    case BarelyInstrumentControlType_kOscSkew:
      params.voice_params.osc_skew = value * kOscSkewRange;
      break;
    case BarelyInstrumentControlType_kSliceMode:
      params.slice_mode = static_cast<SliceMode>(value);
      break;
    case BarelyInstrumentControlType_kBitCrusherDepth:
      params.voice_params.bit_crusher_range = std::pow(2.0f, value * 15.0f);
      break;
    case BarelyInstrumentControlType_kBitCrusherRate:
      params.voice_params.bit_crusher_increment =
          std::min(2.0f * GetFrequency(engine_.sample_rate, value) / engine_.sample_rate, 1.0f);
      break;
    case BarelyInstrumentControlType_kDistortionMix:
      params.voice_params.distortion_amount = value;
      break;
    case BarelyInstrumentControlType_kDistortionDrive:
      params.voice_params.distortion_drive = 1.0f + kDistortionDriveRange * value;
      break;
    case BarelyInstrumentControlType_kDelaySend:
      params.voice_params.delay_send = value;
      break;
    case BarelyInstrumentControlType_kReverbSend:
      params.voice_params.reverb_send = value;
      break;
    case BarelyInstrumentControlType_kSidechainSend:
      params.voice_params.sidechain_send = value;
      break;
    case BarelyInstrumentControlType_kFilterType:
      params.filter_type = static_cast<FilterType>(value);
      params.voice_params.filter_coeffs = GetFilterCoefficients(
          engine_.sample_rate, params.filter_type, params.filter_frequency, params.filter_q);
      break;
    case BarelyInstrumentControlType_kFilterCutoff:
      params.filter_frequency = GetFrequency(engine_.sample_rate, value);
      params.voice_params.filter_coeffs = GetFilterCoefficients(
          engine_.sample_rate, params.filter_type, params.filter_frequency, params.filter_q);
      break;
    case BarelyInstrumentControlType_kFilterResonance:
      params.filter_q = GetFilterQ(value);
      params.voice_params.filter_coeffs = GetFilterCoefficients(
          engine_.sample_rate, params.filter_type, params.filter_frequency, params.filter_q);
      break;
    case BarelyInstrumentControlType_kArpMode:
      [[fallthrough]];
    case BarelyInstrumentControlType_kArpGate:
      [[fallthrough]];
    case BarelyInstrumentControlType_kArpRate:
      break;
    default:
      assert(!"Invalid control type");
      return;
  }
}

void InstrumentProcessor::SetNoteControl(uint32_t note_index, BarelyNoteControlType type,
                                         float value) noexcept {
  const uint32_t voice_index = engine_.note_to_voice[note_index];
  if (!engine_.voice_pool.IsActive(voice_index)) {
    return;
  }
  auto& voice = engine_.GetVoice(voice_index);
  switch (type) {
    case BarelyNoteControlType_kGain:
      voice.note_params.gain = GetGain(value);
      break;
    case BarelyNoteControlType_kPitchShift:
      voice.pitch_shift = value;
      voice.UpdatePitchIncrements(engine_.slice_pool.Get(voice.slice_index));
      break;
    default:
      assert(!"Invalid note control type");
      break;
  }
}

void InstrumentProcessor::SetNoteOff(uint32_t note_index) noexcept {
  const uint32_t voice_index = engine_.note_to_voice[note_index];
  engine_.note_to_voice[note_index] = kInvalidIndex;
  if (!engine_.voice_pool.IsActive(voice_index)) {
    return;
  }
  auto& voice = engine_.GetVoice(voice_index);
  if (const auto& instrument_params = engine_.instrument_params[voice.instrument_index];
      instrument_params.first_slice_index == kInvalidIndex ||
      instrument_params.slice_mode != SliceMode::kOnce) {
    voice.envelope.Stop();
  }
  voice.note_index = kInvalidIndex;
}

void InstrumentProcessor::SetNoteOn(
    uint32_t note_index, uint32_t instrument_index, float pitch,
    const std::array<float, BarelyNoteControlType_kCount>& note_controls) noexcept {
  auto& params = engine_.instrument_params[instrument_index];
  if (const uint32_t voice_index = AcquireVoice(params, pitch); voice_index != kInvalidIndex) {
    engine_.note_to_voice[note_index] = voice_index;
    auto& voice = engine_.GetVoice(voice_index);
    voice.instrument_index = instrument_index;
    voice.note_index = note_index;
    voice.slice_index =
        engine_.slice_pool.Select(params.first_slice_index, pitch, engine_.audio_rng);
    voice.Start(params, engine_.slice_pool.Get(voice.slice_index), pitch, note_controls);
  }
}

uint32_t InstrumentProcessor::AcquireVoice(InstrumentParams& params, float pitch) noexcept {
  if (params.should_retrigger) {
    uint32_t current_voice_index = params.first_voice_index;
    while (current_voice_index != kInvalidIndex) {
      auto& voice = engine_.GetVoice(current_voice_index);
      if (voice.pitch == pitch) {
        const uint32_t retrigger_voice_index = current_voice_index;
        current_voice_index = params.first_voice_index;
        do {
          VoiceState& timestamp_voice = engine_.GetVoice(current_voice_index);
          ++timestamp_voice.timestamp;
          current_voice_index = timestamp_voice.next_voice_index;
        } while (current_voice_index != kInvalidIndex);
        return retrigger_voice_index;
      }
      current_voice_index = voice.next_voice_index;
    }
  }

  uint32_t current_voice_index = params.first_voice_index;
  uint32_t last_voice_index = current_voice_index;
  uint32_t oldest_active_voice_index = current_voice_index;
  uint32_t active_voice_count = 0;
  while (current_voice_index != kInvalidIndex) {
    auto& voice = engine_.GetVoice(current_voice_index);
    if (voice.timestamp > engine_.GetVoice(oldest_active_voice_index).timestamp) {
      oldest_active_voice_index = current_voice_index;
    }
    ++voice.timestamp;
    ++active_voice_count;
    last_voice_index = current_voice_index;
    current_voice_index = voice.next_voice_index;
  }

  // Try to acquire a new voice.
  if (engine_.voice_pool.CanAcquire() && active_voice_count < params.voice_count) {
    const uint32_t new_voice_index = engine_.voice_pool.Acquire();
    VoiceState& new_voice = engine_.GetVoice(new_voice_index);
    new_voice.prev_voice_index = last_voice_index;
    new_voice.next_voice_index = kInvalidIndex;
    if (last_voice_index != kInvalidIndex) {
      engine_.GetVoice(last_voice_index).next_voice_index = new_voice_index;
    } else {
      params.first_voice_index = new_voice_index;
    }
    return new_voice_index;
  }

  // No voices are available to acquire, steal the oldest active voice.
  return oldest_active_voice_index;
}

void InstrumentProcessor::SetSampleData(uint32_t instrument_index,
                                        uint32_t first_slice_index) noexcept {
  auto& params = engine_.instrument_params[instrument_index];
  params.first_slice_index = first_slice_index;
  uint32_t active_voice_index = params.first_voice_index;
  while (active_voice_index != kInvalidIndex) {
    auto& voice = engine_.GetVoice(active_voice_index);
    voice.slice_index =
        engine_.slice_pool.Select(params.first_slice_index, voice.pitch, engine_.audio_rng);
    voice.UpdatePitchIncrements(engine_.slice_pool.Get(voice.slice_index));
    active_voice_index = voice.next_voice_index;
  }
}

}  // namespace barely
