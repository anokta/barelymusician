#include "dsp/instrument_processor.h"

#include <barelymusician.h>

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

namespace {

template <SliceMode kSliceMode, bool kIsSidechainSend>
VoiceCallback GetVoiceCallback(OscMode osc_mode) noexcept {
  switch (osc_mode) {
    case OscMode::kMix:
      return Voice::Process<OscMode::kMix, kSliceMode, kIsSidechainSend>;
    case OscMode::kAm:
      return Voice::Process<OscMode::kAm, kSliceMode, kIsSidechainSend>;
    case OscMode::kEnvelopeFollower:
      return Voice::Process<OscMode::kEnvelopeFollower, kSliceMode, kIsSidechainSend>;
    case OscMode::kFm:
      return Voice::Process<OscMode::kFm, kSliceMode, kIsSidechainSend>;
    case OscMode::kMf:
      return Voice::Process<OscMode::kMf, kSliceMode, kIsSidechainSend>;
    case OscMode::kRing:
      return Voice::Process<OscMode::kRing, kSliceMode, kIsSidechainSend>;
    default:
      assert(!"Invalid oscillator mode");
      return Voice::Process<OscMode::kMix, kSliceMode, kIsSidechainSend>;
  }
}

template <bool kIsSidechainSend>
VoiceCallback GetVoiceCallback(OscMode osc_mode, SliceMode slice_mode) noexcept {
  switch (slice_mode) {
    case SliceMode::kSustain:
      return GetVoiceCallback<SliceMode::kSustain, kIsSidechainSend>(osc_mode);
    case SliceMode::kLoop:
      return GetVoiceCallback<SliceMode::kLoop, kIsSidechainSend>(osc_mode);
    case SliceMode::kOnce:
      return GetVoiceCallback<SliceMode::kOnce, kIsSidechainSend>(osc_mode);
    default:
      assert(!"Invalid slice mode");
      return GetVoiceCallback<SliceMode::kSustain, kIsSidechainSend>(osc_mode);
  }
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
InstrumentProcessor::InstrumentProcessor(
    std::span<const BarelyInstrumentControlOverride> control_overrides,
    const BiquadFilter::Coefficients& filter_coeffs, AudioRng& rng, int sample_rate) noexcept
    : sample_interval_(1.0f / static_cast<float>(sample_rate)) {
  assert(sample_rate > 0);
  for (const auto& [type, value] : control_overrides) {
    SetControl(static_cast<InstrumentControlType>(type), value);
  }
  params_.voice_params.filter_coefficients = filter_coeffs;
  params_.osc_increment = kReferenceFrequency * sample_interval_;
  params_.slice_increment = sample_interval_;
  params_.rng = &rng;
  for (int i = 0; i < kMaxVoiceCount; ++i) {
    active_voice_states_[i] = &voice_states_[i];
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void InstrumentProcessor::SetControl(InstrumentControlType type, float value) noexcept {
  switch (type) {
    case InstrumentControlType::kGain:
      params_.voice_params.gain = value;
      break;
    case InstrumentControlType::kPitchShift:
      pitch_shift_ = value;
      params_.osc_increment =
          std::pow(2.0f, osc_pitch_shift_ + pitch_shift_) * kReferenceFrequency * sample_interval_;
      params_.slice_increment = std::pow(2.0f, pitch_shift_) * sample_interval_;
      break;
    case InstrumentControlType::kRetrigger:
      params_.should_retrigger = static_cast<bool>(value);
      break;
    case InstrumentControlType::kStereoPan:
      params_.voice_params.stereo_pan = value;
      break;
    case InstrumentControlType::kVoiceCount: {
      voice_count_ = static_cast<int>(value);
      active_voice_count_ = std::min(active_voice_count_, voice_count_);
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
      osc_mode_ = static_cast<OscMode>(value);
      voice_callback_send_ = GetVoiceCallback<true>(osc_mode_, slice_mode_);
      voice_callback_receive_ = GetVoiceCallback<false>(osc_mode_, slice_mode_);
      break;
    case InstrumentControlType::kOscNoiseMix:
      params_.voice_params.osc_noise_mix = value;
      break;
    case InstrumentControlType::kOscPitchShift:
      osc_pitch_shift_ = value;
      params_.osc_increment =
          std::pow(2.0f, osc_pitch_shift_ + pitch_shift_) * kReferenceFrequency * sample_interval_;
      break;
    case InstrumentControlType::kOscShape:
      params_.voice_params.osc_shape = value;
      break;
    case InstrumentControlType::kOscSkew:
      params_.voice_params.osc_skew = value;
      break;
    case InstrumentControlType::kSliceMode:
      slice_mode_ = static_cast<SliceMode>(value);
      voice_callback_send_ = GetVoiceCallback<true>(osc_mode_, slice_mode_);
      voice_callback_receive_ = GetVoiceCallback<false>(osc_mode_, slice_mode_);
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
      for (int i = 0; i < active_voice_count_; ++i) {
        if (Voice& voice = active_voice_states_[i]->voice;
            active_voice_states_[i]->pitch == pitch && voice.IsOn()) {
          voice.set_gain(value);
          break;
        }
      }
      break;
    case NoteControlType::kPitchShift:
      for (int i = 0; i < active_voice_count_; ++i) {
        if (Voice& voice = active_voice_states_[i]->voice;
            active_voice_states_[i]->pitch == pitch && voice.IsOn()) {
          active_voice_states_[i]->pitch_shift = value;
          voice.set_pitch(active_voice_states_[i]->pitch + active_voice_states_[i]->pitch_shift);
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
  for (int i = 0; i < active_voice_count_; ++i) {
    if (active_voice_states_[i]->pitch == pitch && active_voice_states_[i]->voice.IsOn() &&
        (sample_data_.empty() || slice_mode_ != SliceMode::kOnce)) {
      active_voice_states_[i]->voice.Stop();
      break;
    }
  }
}

void InstrumentProcessor::SetNoteOn(
    float pitch, const std::array<float, BarelyNoteControlType_kCount>& note_controls) noexcept {
  Voice& voice = AcquireVoice(pitch);
  if (const auto* sample = sample_data_.Select(pitch, *params_.rng); sample != nullptr) {
    voice.set_slice(sample);
  }
  voice.Start(params_, pitch, note_controls);
}

void InstrumentProcessor::SetSampleData(SampleData& sample_data) noexcept {
  sample_data_.Swap(sample_data);
  for (int i = 0; i < active_voice_count_; ++i) {
    if (const auto* sample = sample_data_.Select(active_voice_states_[i]->pitch, *params_.rng);
        sample != nullptr) {
      active_voice_states_[i]->voice.set_slice(sample);
      active_voice_states_[i]->voice.set_pitch(active_voice_states_[i]->pitch +
                                               active_voice_states_[i]->pitch_shift);
    }
  }
}

Voice& InstrumentProcessor::AcquireVoice(float pitch) noexcept {
  int voice_index = -1;
  if (params_.should_retrigger) {
    for (int i = 0; i < active_voice_count_; ++i) {
      if (active_voice_states_[i]->pitch == pitch) {
        voice_index = i;
        break;
      }
      ++active_voice_states_[i]->timestamp;
    }
    if (voice_index == -1) {
      voice_index = active_voice_count_++;
    } else {
      for (int i = voice_index + 1; i < active_voice_count_; ++i) {
        ++active_voice_states_[i]->timestamp;
      }
    }
  } else if (active_voice_count_ < voice_count_) {
    for (int i = 0; i < active_voice_count_; ++i) {
      ++active_voice_states_[i]->timestamp;
    }
    voice_index = active_voice_count_++;
  } else {  // no voices are available to acquire, steal the oldest active voice.
    int oldest_voice_index = 0;
    for (int i = 0; i < voice_count_; ++i) {
      if (voice_states_[i].timestamp > voice_states_[oldest_voice_index].timestamp) {
        oldest_voice_index = i;
      }
      ++voice_states_[i].timestamp;
    }
    voice_index = oldest_voice_index;
  }
  VoiceState& voice_state = *active_voice_states_[voice_index];
  voice_state.pitch = pitch;
  voice_state.pitch_shift = 0.0;
  voice_state.timestamp = 0;
  return voice_state.voice;
}

}  // namespace barely
