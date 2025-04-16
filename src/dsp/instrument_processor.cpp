#include "dsp/instrument_processor.h"

#include <barelymusician.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <span>

#include "common/rng.h"
#include "dsp/biquad_filter.h"
#include "dsp/sample_data.h"
#include "dsp/voice.h"

namespace barely {

namespace {

template <SliceMode kSliceMode>
VoiceCallback GetVoiceCallback(OscMode osc_mode) {
  switch (osc_mode) {
    case OscMode::kMix:
      return Voice::Next<OscMode::kMix, kSliceMode>;
    case OscMode::kAm:
      return Voice::Next<OscMode::kAm, kSliceMode>;
    case OscMode::kEnvelopeFollower:
      return Voice::Next<OscMode::kEnvelopeFollower, kSliceMode>;
    case OscMode::kFm:
      return Voice::Next<OscMode::kFm, kSliceMode>;
    case OscMode::kMf:
      return Voice::Next<OscMode::kMf, kSliceMode>;
    case OscMode::kRing:
      return Voice::Next<OscMode::kRing, kSliceMode>;
    default:
      assert(!"Invalid oscillator mode");
      return Voice::Next<OscMode::kMix, kSliceMode>;
  }
}

VoiceCallback GetVoiceCallback(OscMode osc_mode, SliceMode slice_mode) {
  switch (slice_mode) {
    case SliceMode::kSustain:
      return GetVoiceCallback<SliceMode::kSustain>(osc_mode);
    case SliceMode::kLoop:
      return GetVoiceCallback<SliceMode::kLoop>(osc_mode);
    case SliceMode::kOnce:
      return GetVoiceCallback<SliceMode::kOnce>(osc_mode);
    default:
      assert(!"Invalid slice mode");
      return GetVoiceCallback<SliceMode::kSustain>(osc_mode);
  }
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
InstrumentProcessor::InstrumentProcessor(std::span<const BarelyControlOverride> control_overrides,
                                         AudioRng& rng, int sample_rate,
                                         float reference_frequency) noexcept
    : sample_interval_(1.0f / static_cast<float>(sample_rate)),
      reference_frequency_(reference_frequency) {
  assert(sample_rate > 0);
  for (const auto& [type, value] : control_overrides) {
    SetControl(static_cast<ControlType>(type), value);
  }
  params_.osc_increment = reference_frequency * sample_interval_;
  params_.slice_increment = sample_interval_;
  params_.rng = &rng;
}

bool InstrumentProcessor::Process(float* output_samples, int output_sample_count) noexcept {
  bool has_active_voice = false;
  for (int i = 0; i < voice_count_; ++i) {
    if (voice_states_[i].voice.IsActive()) {
      if (has_active_voice) {
        ProcessVoice<true>(voice_states_[i].voice, output_samples, output_sample_count);
      } else {
        ProcessVoice<false>(voice_states_[i].voice, output_samples, output_sample_count);
        has_active_voice = true;
      }
    }
  }
  if (!has_active_voice) {
    std::fill_n(output_samples, output_sample_count, 0.0f);
  }
  return has_active_voice;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void InstrumentProcessor::SetControl(ControlType type, float value) noexcept {
  switch (type) {
    case ControlType::kGain:
      params_.voice_params.gain = value;
      break;
    case ControlType::kPitchShift:
      pitch_shift_ = value;
      params_.osc_increment =
          std::pow(2.0f, osc_pitch_shift_ + pitch_shift_) * reference_frequency_ * sample_interval_;
      params_.slice_increment = std::pow(2.0f, pitch_shift_) * sample_interval_;
      break;
    case ControlType::kRetrigger:
      should_retrigger_ = static_cast<bool>(value);
      break;
    case ControlType::kVoiceCount: {
      const int voice_count = static_cast<int>(value);
      for (int i = voice_count_; i < voice_count; ++i) {
        voice_states_[i].voice.Reset();
      }
      voice_count_ = voice_count;
    } break;
    case ControlType::kAttack:
      params_.adsr.SetAttack(sample_interval_, value);
      break;
    case ControlType::kDecay:
      params_.adsr.SetDecay(sample_interval_, value);
      break;
    case ControlType::kSustain:
      params_.adsr.SetSustain(value);
      break;
    case ControlType::kRelease:
      params_.adsr.SetRelease(sample_interval_, value);
      break;
    case ControlType::kOscMix:
      params_.voice_params.osc_mix = value;
      break;
    case ControlType::kOscMode:
      osc_mode_ = static_cast<OscMode>(value);
      voice_callback_ = GetVoiceCallback(osc_mode_, slice_mode_);
      break;
    case ControlType::kOscNoiseMix:
      params_.voice_params.osc_noise_mix = value;
      break;
    case ControlType::kOscPitchShift:
      osc_pitch_shift_ = value;
      params_.osc_increment =
          std::pow(2.0f, osc_pitch_shift_ + pitch_shift_) * reference_frequency_ * sample_interval_;
      break;
    case ControlType::kOscShape:
      params_.voice_params.osc_shape = value;
      break;
    case ControlType::kOscSkew:
      params_.voice_params.osc_skew = value;
      break;
    case ControlType::kSliceMode:
      slice_mode_ = static_cast<SliceMode>(value);
      voice_callback_ = GetVoiceCallback(osc_mode_, slice_mode_);
      break;
    case ControlType::kFilterType:
      filter_type_ = static_cast<FilterType>(value);
      params_.voice_params.filter_coefficients =
          GetFilterCoefficients(sample_interval_, filter_type_, filter_frequency_, filter_q_);
      break;
    case ControlType::kFilterFrequency:
      filter_frequency_ = value;
      params_.voice_params.filter_coefficients =
          GetFilterCoefficients(sample_interval_, filter_type_, filter_frequency_, filter_q_);
      break;
    case ControlType::kFilterQ:
      filter_q_ = value;
      params_.voice_params.filter_coefficients =
          GetFilterCoefficients(sample_interval_, filter_type_, filter_frequency_, filter_q_);
      break;
    case ControlType::kBitCrusherDepth:
      // Offset the bit depth by 1 to normalize the range.
      params_.voice_params.bit_crusher_range = std::pow(2.0f, value - 1.0f);
      break;
    case ControlType::kBitCrusherRate:
      params_.voice_params.bit_crusher_increment = value;
      break;
    default:
      assert(!"Invalid control type");
      return;
  }
}

void InstrumentProcessor::SetNoteControl(float pitch, NoteControlType type, float value) noexcept {
  switch (type) {
    case NoteControlType::kGain:
      for (int i = 0; i < voice_count_; ++i) {
        if (Voice& voice = voice_states_[i].voice;
            voice_states_[i].pitch == pitch && voice.IsActive()) {
          voice.set_gain(value);
          break;
        }
      }
      break;
    case NoteControlType::kPitchShift:
      for (int i = 0; i < voice_count_; ++i) {
        if (Voice& voice = voice_states_[i].voice;
            voice_states_[i].pitch == pitch && voice.IsActive()) {
          voice_states_[i].pitch_shift = value;
          voice.set_pitch(voice_states_[i].pitch + voice_states_[i].pitch_shift);
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
  for (int i = 0; i < voice_count_; ++i) {
    if (voice_states_[i].pitch == pitch && voice_states_[i].voice.IsActive() &&
        (sample_data_.empty() || slice_mode_ != SliceMode::kOnce)) {
      voice_states_[i].voice.Stop();
    }
  }
}

void InstrumentProcessor::SetNoteOn(
    float pitch, const std::array<float, BarelyNoteControlType_kCount>& note_controls) noexcept {
  if (voice_count_ == 0) {
    // No voices available.
    return;
  }
  Voice& voice = AcquireVoice(pitch);
  if (const auto* sample = sample_data_.Select(pitch, *params_.rng); sample != nullptr) {
    voice.set_slice(sample);
  }
  voice.Start(params_, pitch, note_controls);
}

void InstrumentProcessor::SetSampleData(SampleData& sample_data) noexcept {
  sample_data_.Swap(sample_data);
  for (int i = 0; i < voice_count_; ++i) {
    if (Voice& voice = voice_states_[i].voice; !voice.IsActive()) {
      voice.set_slice(nullptr);
    } else if (const auto* sample = sample_data_.Select(voice_states_[i].pitch, *params_.rng);
               sample != nullptr) {
      voice.set_slice(sample);
      voice.set_pitch(voice_states_[i].pitch + voice_states_[i].pitch_shift);
    }
  }
}

Voice& InstrumentProcessor::AcquireVoice(float pitch) noexcept {
  int voice_index = -1;
  int oldest_voice_index = 0;
  for (int i = 0; i < voice_count_; ++i) {
    if (should_retrigger_ && voice_states_[i].pitch == pitch) {
      // Retrigger the existing voice.
      voice_index = i;
    }

    if (voice_states_[i].voice.IsActive()) {
      // Increment timestamp.
      ++voice_states_[i].timestamp;
      if (voice_states_[i].timestamp > voice_states_[oldest_voice_index].timestamp) {
        oldest_voice_index = i;
      }
    } else if (voice_index == -1) {
      // Acquire a free voice.
      voice_index = i;
    }
  }
  if (voice_index == -1) {
    // If no voices are available to acquire, steal the oldest active voice.
    voice_index = oldest_voice_index;
  }
  VoiceState& voice_state = voice_states_[voice_index];
  voice_state.pitch = pitch;
  voice_state.pitch_shift = 0.0;
  voice_state.timestamp = 0;
  return voice_state.voice;
}

}  // namespace barely
