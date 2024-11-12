#include "internal/instrument_processor.h"

#include <algorithm>
#include <cassert>
#include <cmath>

#include "barelymusician.h"
#include "dsp/one_pole_filter.h"
#include "dsp/oscillator.h"
#include "dsp/voice.h"
#include "internal/sample_data.h"

namespace barely::internal {

namespace {

// Returns the frequency ratio of a given `pitch`.
double FrequencyRatioFromPitch(double pitch) { return std::pow(2.0, pitch); }

// Returns the frequency of a given `pitch`.
double FrequencyFromPitch(double pitch, double reference_frequency) noexcept {
  return reference_frequency * FrequencyRatioFromPitch(pitch);
}

template <FilterType kFilterType, OscillatorShape kOscillatorShape>
VoiceCallback GetVoiceCallback(SamplePlaybackMode sample_playback_mode) {
  switch (sample_playback_mode) {
    case SamplePlaybackMode::kNone:
      return Voice::ProcessVoice<kFilterType, kOscillatorShape, SamplePlaybackMode::kNone>;
    case SamplePlaybackMode::kOnce:
      return Voice::ProcessVoice<kFilterType, kOscillatorShape, SamplePlaybackMode::kOnce>;
    case SamplePlaybackMode::kSustain:
      return Voice::ProcessVoice<kFilterType, kOscillatorShape, SamplePlaybackMode::kSustain>;
    case SamplePlaybackMode::kLoop:
      return Voice::ProcessVoice<kFilterType, kOscillatorShape, SamplePlaybackMode::kLoop>;
    default:
      assert(!"Invalid sample playback mode");
      return Voice::ProcessVoice<kFilterType, kOscillatorShape, SamplePlaybackMode::kNone>;
  }
}

template <FilterType kFilterType>
VoiceCallback GetVoiceCallback(OscillatorShape oscillator_shape,
                               SamplePlaybackMode sample_playback_mode) {
  switch (oscillator_shape) {
    case OscillatorShape::kNone:
      return GetVoiceCallback<kFilterType, OscillatorShape::kNone>(sample_playback_mode);
    case OscillatorShape::kSine:
      return GetVoiceCallback<kFilterType, OscillatorShape::kSine>(sample_playback_mode);
    case OscillatorShape::kSaw:
      return GetVoiceCallback<kFilterType, OscillatorShape::kSaw>(sample_playback_mode);
    case OscillatorShape::kSquare:
      return GetVoiceCallback<kFilterType, OscillatorShape::kSquare>(sample_playback_mode);
    case OscillatorShape::kNoise:
      return GetVoiceCallback<kFilterType, OscillatorShape::kNoise>(sample_playback_mode);
    default:
      assert(!"Invalid oscillator shape");
      return GetVoiceCallback<kFilterType, OscillatorShape::kNone>(sample_playback_mode);
  }
}

VoiceCallback GetVoiceCallback(FilterType filter_type, OscillatorShape oscillator_shape,
                               SamplePlaybackMode sample_playback_mode) {
  switch (filter_type) {
    case FilterType::kNone:
      return GetVoiceCallback<FilterType::kNone>(oscillator_shape, sample_playback_mode);
    case FilterType::kLowPass:
      return GetVoiceCallback<FilterType::kLowPass>(oscillator_shape, sample_playback_mode);
    case FilterType::kHighPass:
      return GetVoiceCallback<FilterType::kHighPass>(oscillator_shape, sample_playback_mode);
    default:
      assert(!"Invalid filter type");
      return GetVoiceCallback<FilterType::kNone>(oscillator_shape, sample_playback_mode);
  }
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
InstrumentProcessor::InstrumentProcessor(int sample_rate, double reference_frequency) noexcept
    : adsr_(sample_rate),
      gain_processor_(sample_rate),
      reference_frequency_(reference_frequency),
      sample_rate_(sample_rate),
      sample_interval_(1.0 / static_cast<double>(sample_rate)) {
  assert(sample_rate > 0);
}

void InstrumentProcessor::Process(double* output_samples, int output_sample_count) noexcept {
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
    std::fill_n(output_samples, output_sample_count, 0.0);
  }
  gain_processor_.Process(output_samples, output_sample_count);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void InstrumentProcessor::SetControl(ControlType type, double value) noexcept {
  switch (type) {
    case ControlType::kGain:
      gain_processor_.SetGain(value);
      break;
    case ControlType::kVoiceCount: {
      const int voice_count = static_cast<int>(value);
      for (int i = voice_count_; i < voice_count; ++i) {
        voice_states_[i].voice.Reset();
      }
      voice_count_ = voice_count;
    } break;
    case ControlType::kOscillatorShape:
      oscillator_shape_ = static_cast<OscillatorShape>(value);
      voice_callback_ = GetVoiceCallback(filter_type_, oscillator_shape_, sample_playback_mode_);
      break;
    case ControlType::kSamplePlaybackMode:
      sample_playback_mode_ = static_cast<SamplePlaybackMode>(value);
      voice_callback_ = GetVoiceCallback(filter_type_, oscillator_shape_, sample_playback_mode_);
      break;
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
    case ControlType::kPitchShift:
      pitch_shift_ = value;
      for (int i = 0; i < voice_count_; ++i) {
        if (Voice& voice = voice_states_[i].voice; voice.IsActive()) {
          const double shifted_pitch =
              voice_states_[i].pitch + pitch_shift_ + voice_states_[i].pitch_shift;
          voice.set_oscillator_increment(FrequencyFromPitch(shifted_pitch, reference_frequency_) *
                                         sample_interval_);
          voice.set_sample_player_speed(
              FrequencyRatioFromPitch(shifted_pitch - voice_states_[i].root_pitch),
              sample_interval_);
        }
      }
      break;
    case ControlType::kRetrigger:
      should_retrigger_ = static_cast<bool>(value);
      break;
    case ControlType::kFilterType:
      filter_type_ = static_cast<FilterType>(value);
      voice_callback_ = GetVoiceCallback(filter_type_, oscillator_shape_, sample_playback_mode_);
      break;
    case ControlType::kFilterFrequency: {
      filter_coefficient_ = GetFilterCoefficient(sample_rate_, value);
    } break;
    default:
      assert(!"Invalid control type");
      return;
  }
}

void InstrumentProcessor::SetNoteControl(double pitch, NoteControlType type,
                                         double value) noexcept {
  switch (type) {
    case NoteControlType::kPitchShift:
      for (int i = 0; i < voice_count_; ++i) {
        if (Voice& voice = voice_states_[i].voice;
            voice_states_[i].pitch == pitch && voice.IsActive()) {
          voice_states_[i].pitch_shift = value;
          const double shifted_pitch =
              voice_states_[i].pitch + pitch_shift_ + voice_states_[i].pitch_shift;
          voice.set_oscillator_increment(FrequencyFromPitch(shifted_pitch, reference_frequency_) *
                                         sample_interval_);
          voice.set_sample_player_speed(
              FrequencyRatioFromPitch(shifted_pitch - voice_states_[i].root_pitch),
              sample_interval_);
          break;
        }
      }
      break;
    default:
      assert(!"Invalid note control type");
      break;
  }
}

void InstrumentProcessor::SetNoteOff(double pitch) noexcept {
  for (int i = 0; i < voice_count_; ++i) {
    if (voice_states_[i].pitch == pitch && voice_states_[i].voice.IsActive()) {
      if (sample_playback_mode_ == SamplePlaybackMode::kOnce) {
        voice_states_[i].voice.Stop<true>();
      } else {
        voice_states_[i].voice.Stop<false>();
      }
    }
  }
}

void InstrumentProcessor::SetNoteOn(double pitch, double intensity) noexcept {
  if (voice_count_ == 0) {
    // No voices available.
    return;
  }
  VoiceState& voice_state = AcquireVoice(pitch);
  voice_state.pitch = pitch;
  voice_state.pitch_shift = 0.0;
  voice_state.timestamp = 0;

  Voice& voice = voice_state.voice;
  const double shifted_pitch = pitch + pitch_shift_;
  voice.set_oscillator_increment(FrequencyFromPitch(shifted_pitch, reference_frequency_) *
                                 sample_interval_);
  if (const auto* sample = sample_data_.Select(pitch); sample != nullptr) {
    voice_state.root_pitch = sample->root_pitch;
    voice.set_sample_player_slice(sample);
    voice.set_sample_player_speed(FrequencyRatioFromPitch(shifted_pitch - sample->root_pitch),
                                  sample_interval_);
  }
  voice.Start(adsr_, intensity);
}

void InstrumentProcessor::SetReferenceFrequency(double reference_frequency) noexcept {
  assert(reference_frequency_ != reference_frequency);
  reference_frequency_ = reference_frequency;
  for (int i = 0; i < voice_count_; ++i) {
    if (auto& voice = voice_states_[i].voice; voice.IsActive()) {
      const double shifted_pitch =
          voice_states_[i].pitch + pitch_shift_ + voice_states_[i].pitch_shift;
      voice.set_oscillator_increment(FrequencyFromPitch(shifted_pitch, reference_frequency_) *
                                     sample_interval_);
    }
  }
}

void InstrumentProcessor::SetSampleData(SampleData& sample_data) noexcept {
  sample_data_.Swap(sample_data);
  for (int i = 0; i < voice_count_; ++i) {
    if (Voice& voice = voice_states_[i].voice; !voice.IsActive()) {
      voice.set_sample_player_slice(nullptr);
    } else if (const auto* sample = sample_data_.Select(voice_states_[i].pitch);
               sample != nullptr) {
      voice_states_[i].root_pitch = sample->root_pitch;
      voice.set_sample_player_slice(sample);
      voice.set_sample_player_speed(
          FrequencyRatioFromPitch(voice_states_[i].pitch + pitch_shift_ - sample->root_pitch),
          sample_interval_);
    }
  }
}

InstrumentProcessor::VoiceState& InstrumentProcessor::AcquireVoice(double pitch) noexcept {
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
  return voice_states_[voice_index];
}

}  // namespace barely::internal
