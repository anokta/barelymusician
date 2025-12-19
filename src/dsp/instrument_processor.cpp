#include "dsp/instrument_processor.h"

#include <barelymusician.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <span>

#include "common/constants.h"
#include "common/rng.h"
#include "dsp/biquad_filter.h"
#include "dsp/sample_data.h"
#include "dsp/voice.h"

namespace barely {

// NOLINTNEXTLINE(bugprone-exception-escape)
InstrumentProcessor::InstrumentProcessor(
    std::span<const BarelyInstrumentControlOverride> control_overrides,
    const BiquadFilter::Coefficients& filter_coeffs, AudioRng& rng, VoicePool& voice_pool,
    int sample_rate) noexcept
    : voice_pool_(voice_pool), sample_interval_(1.0f / static_cast<float>(sample_rate)) {
  assert(sample_rate > 0);
  for (const auto& [type, value] : control_overrides) {
    SetControl(static_cast<InstrumentControlType>(type), value);
  }
  params_.voice_params.filter_coefficients = filter_coeffs;
  params_.osc_increment = kReferenceFrequency * sample_interval_;
  params_.slice_increment = sample_interval_;
  params_.rng = &rng;
}

// TODO(#126): This shouldn't be necessary.
InstrumentProcessor::~InstrumentProcessor() noexcept {
  for (int i = 0; i < params_.active_voice_count; ++i) {
    voice_pool_.Release(params_.active_voices[i]);
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void InstrumentProcessor::SetControl(InstrumentControlType type, float value) noexcept {
  switch (type) {
    case InstrumentControlType::kGain:
      params_.voice_params.gain = value;
      break;
    case InstrumentControlType::kPitchShift:
      params_.pitch_shift = value;
      params_.osc_increment = std::pow(2.0f, params_.osc_pitch_shift + params_.pitch_shift) *
                              kReferenceFrequency * sample_interval_;
      params_.slice_increment = std::pow(2.0f, params_.pitch_shift) * sample_interval_;
      break;
    case InstrumentControlType::kRetrigger:
      params_.should_retrigger = static_cast<bool>(value);
      break;
    case InstrumentControlType::kStereoPan:
      params_.voice_params.stereo_pan = value;
      break;
    case InstrumentControlType::kVoiceCount: {
      params_.voice_count = static_cast<int>(value);
      params_.active_voice_count = std::min(params_.active_voice_count, params_.voice_count);
    } break;
    case InstrumentControlType::kAttack:
      params_.adsr.SetAttack(sample_interval_, value);
      break;
    case InstrumentControlType::kDecay:
      params_.adsr.SetDecay(sample_interval_, value);
      break;
    case InstrumentControlType::kSustain:
      params_.adsr.SetSustain(value);
      break;
    case InstrumentControlType::kRelease:
      params_.adsr.SetRelease(sample_interval_, value);
      break;
    case InstrumentControlType::kOscMix:
      params_.voice_params.osc_mix = value;
      break;
    case InstrumentControlType::kOscMode:
      params_.osc_mode = static_cast<OscMode>(value);
      break;
    case InstrumentControlType::kOscNoiseMix:
      params_.voice_params.osc_noise_mix = value;
      break;
    case InstrumentControlType::kOscPitchShift:
      params_.osc_pitch_shift = value;
      params_.osc_increment = std::pow(2.0f, params_.osc_pitch_shift + params_.pitch_shift) *
                              kReferenceFrequency * sample_interval_;
      break;
    case InstrumentControlType::kOscShape:
      params_.voice_params.osc_shape = value;
      break;
    case InstrumentControlType::kOscSkew:
      params_.voice_params.osc_skew = value;
      break;
    case InstrumentControlType::kSliceMode:
      params_.slice_mode = static_cast<SliceMode>(value);
      break;
    case InstrumentControlType::kBitCrusherDepth:
      // Offset the bit depth by 1 to normalize the range.
      params_.voice_params.bit_crusher_range = std::pow(2.0f, value - 1.0f);
      break;
    case InstrumentControlType::kBitCrusherRate:
      params_.voice_params.bit_crusher_increment = value;
      break;
    case InstrumentControlType::kDistortionAmount:
      params_.voice_params.distortion_amount = value;
      break;
    case InstrumentControlType::kDistortionDrive:
      params_.voice_params.distortion_drive = value;
      break;
    case InstrumentControlType::kDelaySend:
      params_.voice_params.delay_send = value;
      break;
    case InstrumentControlType::kSidechainSend:
      params_.voice_params.sidechain_send = value;
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

void InstrumentProcessor::SetNoteControl(float pitch, NoteControlType type, float value) noexcept {
  switch (type) {
    case NoteControlType::kGain:
      for (int i = 0; i < params_.active_voice_count; ++i) {
        if (Voice& voice = voice_pool_.Get(params_.active_voices[i]);
            voice.params().pitch == pitch && voice.IsOn()) {
          voice.set_gain(value);
          break;
        }
      }
      break;
    case NoteControlType::kPitchShift:
      for (int i = 0; i < params_.active_voice_count; ++i) {
        if (Voice& voice = voice_pool_.Get(params_.active_voices[i]);
            voice.params().pitch == pitch && voice.IsOn()) {
          voice.set_pitch(voice.params().pitch, value);
          break;
        }
      }
      break;
    default:
      assert(!"Invalid note control type");
      break;
  }
}

void InstrumentProcessor::SetNoteOff(float pitch) noexcept {
  for (int i = 0; i < params_.active_voice_count; ++i) {
    if (Voice& voice = voice_pool_.Get(params_.active_voices[i]);
        voice.params().pitch == pitch && voice.IsOn() &&
        (params_.sample_data.empty() || params_.slice_mode != SliceMode::kOnce)) {
      voice.Stop();
      break;
    }
  }
}

void InstrumentProcessor::SetNoteOn(
    float pitch, const std::array<float, BarelyNoteControlType_kCount>& note_controls) noexcept {
  Voice* voice = AcquireVoice(pitch);
  if (voice == nullptr) {
    return;
  }
  if (const auto* sample = params_.sample_data.Select(pitch, *params_.rng); sample != nullptr) {
    voice->set_slice(sample);
  }
  voice->Start(params_, pitch, note_controls);
}

void InstrumentProcessor::SetSampleData(SampleData& sample_data) noexcept {
  params_.sample_data.Swap(sample_data);
  for (int i = 0; i < params_.active_voice_count; ++i) {
    Voice& voice = voice_pool_.Get(params_.active_voices[i]);
    if (const auto* sample = params_.sample_data.Select(voice.params().pitch, *params_.rng);
        sample != nullptr) {
      voice.set_slice(sample);
      voice.set_pitch(voice.params().pitch, voice.params().pitch_shift);
    }
  }
}

Voice* InstrumentProcessor::AcquireVoice(float pitch) noexcept {
  if (params_.should_retrigger) {
    for (int i = 0; i < params_.active_voice_count; ++i) {
      Voice& voice = voice_pool_.Get(params_.active_voices[i]);
      if (voice.params().pitch == pitch) {
        for (int j = 0; j < params_.active_voice_count; ++j) {
          ++voice.params().timestamp;
        }
        voice.params().pitch_shift = 0.0;
        voice.params().timestamp = 0;
        return &voice;
      }
    }
  }

  if (params_.active_voice_count < params_.voice_count) {
    for (int i = 0; i < params_.active_voice_count; ++i) {
      ++voice_pool_.Get(params_.active_voices[i]).params().timestamp;
    }

    params_.active_voices[params_.active_voice_count] = voice_pool_.Acquire(params_);

    if (params_.active_voices[params_.active_voice_count] == -1) {
      return nullptr;
    }

    Voice& voice = voice_pool_.Get(params_.active_voices[params_.active_voice_count]);
    voice.params().pitch = pitch;
    voice.params().pitch_shift = 0.0;
    voice.params().timestamp = 0;
    ++params_.active_voice_count;

    return &voice;
  }

  // No voices are available to acquire, steal the oldest active voice.
  int oldest_active_voice_index = 0;
  for (int i = 0; i < params_.active_voice_count; ++i) {
    Voice& voice = voice_pool_.Get(params_.active_voices[i]);
    if (voice.params().timestamp >
        voice_pool_.Get(params_.active_voices[oldest_active_voice_index]).params().timestamp) {
      oldest_active_voice_index = i;
    }
    ++voice.params().timestamp;
  }
  return &voice_pool_.Get(params_.active_voices[oldest_active_voice_index]);
}

}  // namespace barely
