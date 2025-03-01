#include "dsp/instrument_processor.h"

#include <algorithm>
#include <cassert>
#include <cmath>

#include "barelymusician.h"
#include "dsp/biquad_filter.h"
#include "dsp/sample_data.h"
#include "dsp/voice.h"

namespace barely {

namespace {

template <SamplePlaybackMode kSamplePlaybackMode>
VoiceCallback GetVoiceCallback(OscMode oscillator_mode) {
  switch (oscillator_mode) {
    case OscMode::kNone:
      return Voice::Next<OscMode::kNone, kSamplePlaybackMode>;
    case OscMode::kMix:
      return Voice::Next<OscMode::kMix, kSamplePlaybackMode>;
    case OscMode::kAm:
      return Voice::Next<OscMode::kAm, kSamplePlaybackMode>;
    case OscMode::kEnvelopeFollower:
      return Voice::Next<OscMode::kEnvelopeFollower, kSamplePlaybackMode>;
    case OscMode::kFm:
      return Voice::Next<OscMode::kFm, kSamplePlaybackMode>;
    case OscMode::kMf:
      return Voice::Next<OscMode::kMf, kSamplePlaybackMode>;
    case OscMode::kRing:
      return Voice::Next<OscMode::kRing, kSamplePlaybackMode>;
    default:
      assert(!"Invalid oscillator mode");
      return Voice::Next<OscMode::kMix, kSamplePlaybackMode>;
  }
}

VoiceCallback GetVoiceCallback(OscMode oscillator_mode, const SampleData& sample_data,
                               SamplePlaybackMode sample_playback_mode) {
  if (sample_data.empty()) {
    return GetVoiceCallback<SamplePlaybackMode::kNone>(oscillator_mode);
  }
  switch (sample_playback_mode) {
    case SamplePlaybackMode::kNone:
      return GetVoiceCallback<SamplePlaybackMode::kNone>(oscillator_mode);
    case SamplePlaybackMode::kOnce:
      return GetVoiceCallback<SamplePlaybackMode::kOnce>(oscillator_mode);
    case SamplePlaybackMode::kSustain:
      return GetVoiceCallback<SamplePlaybackMode::kSustain>(oscillator_mode);
    case SamplePlaybackMode::kLoop:
      return GetVoiceCallback<SamplePlaybackMode::kLoop>(oscillator_mode);
    default:
      assert(!"Invalid sample playback mode");
      return GetVoiceCallback<SamplePlaybackMode::kNone>(oscillator_mode);
  }
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
InstrumentProcessor::InstrumentProcessor(int sample_rate, float reference_frequency) noexcept
    : sample_interval_(1.0f / static_cast<float>(sample_rate)),
      adsr_(sample_interval_),
      gain_processor_(sample_rate),
      reference_frequency_(reference_frequency) {
  assert(sample_rate > 0);
}

void InstrumentProcessor::Process(float* output_samples, int output_sample_count) noexcept {
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
  gain_processor_.Process(output_samples, output_sample_count);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void InstrumentProcessor::SetControl(ControlType type, float value) noexcept {
  switch (type) {
    case ControlType::kGain:
      gain_processor_.SetGain(value);
      break;
    case ControlType::kPitchShift:
      pitch_shift_ = value;
      for (int i = 0; i < voice_count_; ++i) {
        if (Voice& voice = voice_states_[i].voice; voice.IsActive()) {
          const float shifted_pitch =
              voice_states_[i].pitch + pitch_shift_ + voice_states_[i].pitch_shift;
          const float osc_shifted_pitch = shifted_pitch + osc_pitch_shift_;
          voice.set_osc_increment(osc_shifted_pitch, reference_frequency_, sample_interval_);
          voice.set_sample_player_increment(shifted_pitch, sample_interval_);
        }
      }
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
      adsr_.SetAttack(value);
      break;
    case ControlType::kDecay:
      adsr_.SetDecay(value);
      break;
    case ControlType::kSustain:
      adsr_.SetSustain(value);
      break;
    case ControlType::kRelease:
      adsr_.SetRelease(value);
      break;
    case ControlType::kOscMix:
      voice_params_.osc_mix = value;
      break;
    case ControlType::kOscMode:
      osc_mode_ = static_cast<OscMode>(value);
      voice_callback_ = GetVoiceCallback(osc_mode_, sample_data_, sample_playback_mode_);
      break;
    case ControlType::kOscNoiseRatio:
      voice_params_.osc_noise_ratio = value;
      break;
    case ControlType::kOscPitchShift:
      osc_pitch_shift_ = value;
      for (int i = 0; i < voice_count_; ++i) {
        if (Voice& voice = voice_states_[i].voice; voice.IsActive()) {
          const float shifted_pitch =
              voice_states_[i].pitch + pitch_shift_ + voice_states_[i].pitch_shift;
          const float osc_shifted_pitch = shifted_pitch + osc_pitch_shift_;
          voice.set_osc_increment(osc_shifted_pitch, reference_frequency_, sample_interval_);
          voice.set_sample_player_increment(shifted_pitch, sample_interval_);
        }
      }
      break;
    case ControlType::kOscShape:
      voice_params_.osc_shape = value;
      break;
    case ControlType::kOscSkew:
      voice_params_.osc_skew = value;
      break;
    case ControlType::kSamplePlaybackMode:
      sample_playback_mode_ = static_cast<SamplePlaybackMode>(value);
      voice_callback_ = GetVoiceCallback(osc_mode_, sample_data_, sample_playback_mode_);
      break;
    case ControlType::kFilterType:
      filter_type_ = static_cast<FilterType>(value);
      voice_params_.filter_coefficients =
          GetFilterCoefficients(sample_interval_, filter_type_, filter_frequency_, filter_q_);
      break;
    case ControlType::kFilterFrequency:
      filter_frequency_ = value;
      voice_params_.filter_coefficients =
          GetFilterCoefficients(sample_interval_, filter_type_, filter_frequency_, filter_q_);
      break;
    case ControlType::kFilterQ:
      filter_q_ = value;
      voice_params_.filter_coefficients =
          GetFilterCoefficients(sample_interval_, filter_type_, filter_frequency_, filter_q_);
      break;
    case ControlType::kBitCrusherDepth:
      // Offset the bit depth by 1 to normalize the range.
      voice_params_.bit_crusher_range = (value < 16.0f) ? std::pow(2.0f, value - 1.0f) : 0.0f;
      break;
    case ControlType::kBitCrusherRate:
      voice_params_.bit_crusher_increment = value;
      break;
    default:
      assert(!"Invalid control type");
      return;
  }
}

void InstrumentProcessor::SetNoteControl(float pitch, NoteControlType type, float value) noexcept {
  switch (type) {
    case NoteControlType::kPitchShift:
      for (int i = 0; i < voice_count_; ++i) {
        if (Voice& voice = voice_states_[i].voice;
            voice_states_[i].pitch == pitch && voice.IsActive()) {
          voice_states_[i].pitch_shift = value;
          const float shifted_pitch =
              voice_states_[i].pitch + pitch_shift_ + voice_states_[i].pitch_shift;
          const float osc_shifted_pitch = shifted_pitch + osc_pitch_shift_;
          voice.set_osc_increment(osc_shifted_pitch, reference_frequency_, sample_interval_);
          voice.set_sample_player_increment(shifted_pitch, sample_interval_);
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
    if (voice_states_[i].pitch == pitch && voice_states_[i].voice.IsActive()) {
      if (!sample_data_.empty() && sample_playback_mode_ == SamplePlaybackMode::kOnce) {
        voice_states_[i].voice.Stop<true>();
      } else {
        voice_states_[i].voice.Stop<false>();
      }
    }
  }
}

void InstrumentProcessor::SetNoteOn(float pitch, float intensity) noexcept {
  if (voice_count_ == 0) {
    // No voices available.
    return;
  }
  Voice& voice = AcquireVoice(pitch);
  const float shifted_pitch = pitch + pitch_shift_;
  const float osc_shifted_pitch = shifted_pitch + osc_pitch_shift_;
  voice.set_osc_increment(osc_shifted_pitch, reference_frequency_, sample_interval_);
  if (const auto* sample = sample_data_.Select(pitch); sample != nullptr) {
    voice.set_sample_player_slice(sample);
    voice.set_sample_player_increment(shifted_pitch, sample_interval_);
  }
  voice.Start(adsr_, intensity);
}

void InstrumentProcessor::SetReferenceFrequency(float reference_frequency) noexcept {
  assert(reference_frequency_ != reference_frequency);
  reference_frequency_ = reference_frequency;
  for (int i = 0; i < voice_count_; ++i) {
    if (auto& voice = voice_states_[i].voice; voice.IsActive()) {
      const float osc_shifted_pitch =
          voice_states_[i].pitch + pitch_shift_ + voice_states_[i].pitch_shift + osc_pitch_shift_;
      voice.set_osc_increment(osc_shifted_pitch, reference_frequency_, sample_interval_);
    }
  }
}

void InstrumentProcessor::SetSampleData(SampleData& sample_data) noexcept {
  sample_data_.Swap(sample_data);
  voice_callback_ = GetVoiceCallback(osc_mode_, sample_data_, sample_playback_mode_);
  for (int i = 0; i < voice_count_; ++i) {
    if (Voice& voice = voice_states_[i].voice; !voice.IsActive()) {
      voice.set_sample_player_slice(nullptr);
    } else if (const auto* sample = sample_data_.Select(voice_states_[i].pitch);
               sample != nullptr) {
      const float shifted_pitch =
          voice_states_[i].pitch + pitch_shift_ + voice_states_[i].pitch_shift;
      voice.set_sample_player_slice(sample);
      voice.set_sample_player_increment(shifted_pitch, sample_interval_);
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
