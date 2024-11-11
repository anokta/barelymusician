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

// Maximum number of voices allowed to be set.
constexpr int kMaxVoiceCount = 20;

// Returns the frequency ratio of a given `pitch`.
double FrequencyRatioFromPitch(double pitch) { return std::pow(2.0, pitch); }

// Returns the frequency of a given `pitch`.
double FrequencyFromPitch(double pitch, double reference_frequency) noexcept {
  return reference_frequency * FrequencyRatioFromPitch(pitch);
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
InstrumentProcessor::InstrumentProcessor(int sample_rate, double reference_frequency) noexcept
    : voice_data_(sample_rate),
      voice_states_(kMaxVoiceCount, {Voice(voice_data_)}),
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
      voice_data_.oscillator_callback = kOscillatorCallbacks[static_cast<int>(value)];
      break;
    case ControlType::kSamplePlaybackMode:
      is_sample_played_once_ =
          (static_cast<SamplePlaybackMode>(value) == SamplePlaybackMode::kOnce);
      voice_callback_ = kVoiceCallbacks[static_cast<int>(value)];
      break;
    case ControlType::kAttack:
      voice_data_.adsr.SetAttack(value);
      break;
    case ControlType::kDecay:
      voice_data_.adsr.SetDecay(value);
      break;
    case ControlType::kSustain:
      voice_data_.adsr.SetSustain(value);
      break;
    case ControlType::kRelease:
      voice_data_.adsr.SetRelease(value);
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
      voice_data_.filter_callback = kFilterCallbacks[static_cast<int>(value)];
      break;
    case ControlType::kFilterFrequency: {
      voice_data_.filter_coefficient = GetFilterCoefficient(sample_rate_, value);
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
      if (is_sample_played_once_) {
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
  voice.set_gain(intensity);
  voice.Start();
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
