#include "dsp/instrument_processor.h"

#include <algorithm>
#include <cassert>

#include "barelymusician.h"
#include "dsp/sample_data.h"
#include "dsp/voice.h"

namespace barely::internal {

namespace {

template <OscillatorMode kOscillatorMode, OscillatorShape kOscillatorShape,
          SamplePlaybackMode kSamplePlaybackMode>
VoiceCallback GetVoiceCallback(FilterType filter_type) {
  switch (filter_type) {
    case FilterType::kNone:
      return Voice::Next<FilterType::kNone, kOscillatorMode, kOscillatorShape, kSamplePlaybackMode>;
    case FilterType::kLowPass:
      return Voice::Next<FilterType::kLowPass, kOscillatorMode, kOscillatorShape,
                         kSamplePlaybackMode>;
    case FilterType::kHighPass:
      return Voice::Next<FilterType::kHighPass, kOscillatorMode, kOscillatorShape,
                         kSamplePlaybackMode>;
    default:
      assert(!"Invalid filter type");
      return Voice::Next<FilterType::kNone, kOscillatorMode, kOscillatorShape, kSamplePlaybackMode>;
  }
}

template <OscillatorShape kOscillatorShape, SamplePlaybackMode kSamplePlaybackMode>
VoiceCallback GetVoiceCallback(FilterType filter_type, OscillatorMode oscillator_mode) {
  switch (oscillator_mode) {
    case OscillatorMode::kMix:
      return GetVoiceCallback<OscillatorMode::kMix, kOscillatorShape, kSamplePlaybackMode>(
          filter_type);
    case OscillatorMode::kAm:
      return GetVoiceCallback<OscillatorMode::kAm, kOscillatorShape, kSamplePlaybackMode>(
          filter_type);
    case OscillatorMode::kRing:
      return GetVoiceCallback<OscillatorMode::kRing, kOscillatorShape, kSamplePlaybackMode>(
          filter_type);
    default:
      assert(!"Invalid oscillator mode");
      return GetVoiceCallback<OscillatorMode::kMix, kOscillatorShape, kSamplePlaybackMode>(
          filter_type);
  }
}

template <SamplePlaybackMode kSamplePlaybackMode>
VoiceCallback GetVoiceCallback(FilterType filter_type, OscillatorMode oscillator_mode,
                               OscillatorShape oscillator_shape) {
  switch (oscillator_shape) {
    case OscillatorShape::kNone:
      return GetVoiceCallback<OscillatorShape::kNone, kSamplePlaybackMode>(filter_type,
                                                                           oscillator_mode);
    case OscillatorShape::kSine:
      return GetVoiceCallback<OscillatorShape::kSine, kSamplePlaybackMode>(filter_type,
                                                                           oscillator_mode);
    case OscillatorShape::kSaw:
      return GetVoiceCallback<OscillatorShape::kSaw, kSamplePlaybackMode>(filter_type,
                                                                          oscillator_mode);
    case OscillatorShape::kSquare:
      return GetVoiceCallback<OscillatorShape::kSquare, kSamplePlaybackMode>(filter_type,
                                                                             oscillator_mode);
    case OscillatorShape::kNoise:
      return GetVoiceCallback<OscillatorShape::kNoise, kSamplePlaybackMode>(filter_type,
                                                                            oscillator_mode);
    default:
      assert(!"Invalid oscillator shape");
      return GetVoiceCallback<OscillatorShape::kNone, kSamplePlaybackMode>(filter_type,
                                                                           oscillator_mode);
  }
}

VoiceCallback GetVoiceCallback(FilterType filter_type, OscillatorMode oscillator_mode,
                               OscillatorShape oscillator_shape, const SampleData& sample_data,
                               SamplePlaybackMode sample_playback_mode) {
  if (sample_data.empty()) {
    return GetVoiceCallback<SamplePlaybackMode::kNone>(filter_type, oscillator_mode,
                                                       oscillator_shape);
  }
  switch (sample_playback_mode) {
    case SamplePlaybackMode::kNone:
      return GetVoiceCallback<SamplePlaybackMode::kNone>(filter_type, oscillator_mode,
                                                         oscillator_shape);
    case SamplePlaybackMode::kOnce:
      return GetVoiceCallback<SamplePlaybackMode::kOnce>(filter_type, oscillator_mode,
                                                         oscillator_shape);
    case SamplePlaybackMode::kSustain:
      return GetVoiceCallback<SamplePlaybackMode::kSustain>(filter_type, oscillator_mode,
                                                            oscillator_shape);
    case SamplePlaybackMode::kLoop:
      return GetVoiceCallback<SamplePlaybackMode::kLoop>(filter_type, oscillator_mode,
                                                         oscillator_shape);
    default:
      assert(!"Invalid sample playback mode");
      return GetVoiceCallback<SamplePlaybackMode::kNone>(filter_type, oscillator_mode,
                                                         oscillator_shape);
  }
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
InstrumentProcessor::InstrumentProcessor(int sample_rate, double reference_frequency) noexcept
    : sample_interval_(1.0 / static_cast<double>(sample_rate)),
      adsr_(sample_interval_),
      gain_processor_(sample_rate),
      reference_frequency_(reference_frequency) {
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
    case ControlType::kPitchShift:
      pitch_shift_ = value;
      for (int i = 0; i < voice_count_; ++i) {
        if (Voice& voice = voice_states_[i].voice; voice.IsActive()) {
          const double shifted_pitch =
              voice_states_[i].pitch + pitch_shift_ + voice_states_[i].pitch_shift;
          const double oscillator_shifted_pitch = shifted_pitch + oscillator_pitch_shift_;
          voice.set_oscillator_increment(oscillator_shifted_pitch, reference_frequency_,
                                         sample_interval_);
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
    case ControlType::kOscillatorMix:
      oscillator_mix_ = value;
      break;
    case ControlType::kOscillatorPitchShift:
      oscillator_pitch_shift_ = value;
      for (int i = 0; i < voice_count_; ++i) {
        if (Voice& voice = voice_states_[i].voice; voice.IsActive()) {
          const double shifted_pitch =
              voice_states_[i].pitch + pitch_shift_ + voice_states_[i].pitch_shift;
          const double oscillator_shifted_pitch = shifted_pitch + oscillator_pitch_shift_;
          voice.set_oscillator_increment(oscillator_shifted_pitch, reference_frequency_,
                                         sample_interval_);
          voice.set_sample_player_increment(shifted_pitch, sample_interval_);
        }
      }
      break;
    case ControlType::kOscillatorMode:
      oscillator_mode_ = static_cast<OscillatorMode>(value);
      voice_callback_ = GetVoiceCallback(filter_type_, oscillator_mode_, oscillator_shape_,
                                         sample_data_, sample_playback_mode_);
      break;
    case ControlType::kOscillatorShape:
      oscillator_shape_ = static_cast<OscillatorShape>(value);
      voice_callback_ = GetVoiceCallback(filter_type_, oscillator_mode_, oscillator_shape_,
                                         sample_data_, sample_playback_mode_);
      break;
    case ControlType::kPulseWidth:
      pulse_width_ = value;
      break;
    case ControlType::kSamplePlaybackMode:
      sample_playback_mode_ = static_cast<SamplePlaybackMode>(value);
      voice_callback_ = GetVoiceCallback(filter_type_, oscillator_mode_, oscillator_shape_,
                                         sample_data_, sample_playback_mode_);
      break;
    case ControlType::kFilterType:
      filter_type_ = static_cast<FilterType>(value);
      voice_callback_ = GetVoiceCallback(filter_type_, oscillator_mode_, oscillator_shape_,
                                         sample_data_, sample_playback_mode_);
      break;
    case ControlType::kFilterFrequency: {
      filter_coefficient_ = value;
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
          const double oscillator_shifted_pitch = shifted_pitch + oscillator_pitch_shift_;
          voice.set_oscillator_increment(oscillator_shifted_pitch, reference_frequency_,
                                         sample_interval_);
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

void InstrumentProcessor::SetNoteOff(double pitch) noexcept {
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

void InstrumentProcessor::SetNoteOn(double pitch, double intensity) noexcept {
  if (voice_count_ == 0) {
    // No voices available.
    return;
  }
  Voice& voice = AcquireVoice(pitch);
  const double shifted_pitch = pitch + pitch_shift_;
  const double oscillator_shifted_pitch = shifted_pitch + oscillator_pitch_shift_;
  voice.set_oscillator_increment(oscillator_shifted_pitch, reference_frequency_, sample_interval_);
  if (const auto* sample = sample_data_.Select(pitch); sample != nullptr) {
    voice.set_sample_player_slice(sample);
    voice.set_sample_player_increment(shifted_pitch, sample_interval_);
  }
  voice.Start(adsr_, intensity);
}

void InstrumentProcessor::SetReferenceFrequency(double reference_frequency) noexcept {
  assert(reference_frequency_ != reference_frequency);
  reference_frequency_ = reference_frequency;
  for (int i = 0; i < voice_count_; ++i) {
    if (auto& voice = voice_states_[i].voice; voice.IsActive()) {
      const double oscillator_shifted_pitch = voice_states_[i].pitch + pitch_shift_ +
                                              voice_states_[i].pitch_shift +
                                              oscillator_pitch_shift_;
      voice.set_oscillator_increment(oscillator_shifted_pitch, reference_frequency_,
                                     sample_interval_);
    }
  }
}

void InstrumentProcessor::SetSampleData(SampleData& sample_data) noexcept {
  sample_data_.Swap(sample_data);
  voice_callback_ = GetVoiceCallback(filter_type_, oscillator_mode_, oscillator_shape_,
                                     sample_data_, sample_playback_mode_);
  for (int i = 0; i < voice_count_; ++i) {
    if (Voice& voice = voice_states_[i].voice; !voice.IsActive()) {
      voice.set_sample_player_slice(nullptr);
    } else if (const auto* sample = sample_data_.Select(voice_states_[i].pitch);
               sample != nullptr) {
      const double shifted_pitch =
          voice_states_[i].pitch + pitch_shift_ + voice_states_[i].pitch_shift;
      voice.set_sample_player_slice(sample);
      voice.set_sample_player_increment(shifted_pitch, sample_interval_);
    }
  }
}

Voice& InstrumentProcessor::AcquireVoice(double pitch) noexcept {
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

}  // namespace barely::internal
