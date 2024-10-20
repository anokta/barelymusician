#include "barelymusician/internal/instrument_processor.h"

#include <cassert>
#include <cmath>

#include "barelymusician/barelymusician.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/dsp/sample_player.h"
#include "barelymusician/dsp/voice.h"
#include "barelymusician/internal/sample_data.h"

namespace barely {

namespace {

// Maximum number of voices allowed to be set.
constexpr int kMaxVoiceCount = 32;

// Returns the frequency ratio of a given `pitch`.
double FrequencyRatioFromPitch(double pitch) { return std::pow(2.0, pitch); }

// Returns the frequency of a given `pitch`.
double FrequencyFromPitch(double pitch, double reference_frequency) noexcept {
  return reference_frequency * FrequencyRatioFromPitch(pitch);
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
InstrumentProcessor::InstrumentProcessor(int frame_rate, double reference_frequency) noexcept
    : voice_states_(kMaxVoiceCount, {Voice(frame_rate)}),
      gain_processor_(frame_rate),
      reference_frequency_(reference_frequency) {}

void InstrumentProcessor::Process(double* output_samples, int output_channel_count,
                                  int output_frame_count) noexcept {
  for (int frame = 0; frame < output_frame_count; ++frame) {
    double mono_sample = 0.0;
    for (int i = 0; i < voice_count_; ++i) {
      if (voice_states_[i].voice.IsActive()) {
        mono_sample += voice_states_[i].voice.Next();
      }
    }
    for (int channel = 0; channel < output_channel_count; ++channel) {
      output_samples[output_channel_count * frame + channel] = mono_sample;
    }
  }
  gain_processor_.Process(output_samples, output_channel_count, output_frame_count);
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
        // Copy over the voice settings.
        voice_states_[i].voice = voice_states_[0].voice;
        voice_states_[i].voice.Reset();
      }
      voice_count_ = voice_count;
    } break;
    case ControlType::kOscillatorType:
      for (int i = 0; i < voice_count_; ++i) {
        voice_states_[i].voice.oscillator().SetType(
            static_cast<OscillatorType>(static_cast<int>(value)));
      };
      break;
    case ControlType::kSamplePlaybackMode:
      for (int i = 0; i < voice_count_; ++i) {
        voice_states_[i].voice.set_sample_playback_mode(static_cast<SamplePlaybackMode>(value));
      };
      break;
    case ControlType::kAttack:
      for (int i = 0; i < voice_count_; ++i) {
        voice_states_[i].voice.envelope().SetAttack(value);
      };
      break;
    case ControlType::kDecay:
      for (int i = 0; i < voice_count_; ++i) {
        voice_states_[i].voice.envelope().SetDecay(value);
      };
      break;
    case ControlType::kSustain:
      for (int i = 0; i < voice_count_; ++i) {
        voice_states_[i].voice.envelope().SetSustain(value);
      };
      break;
    case ControlType::kRelease:
      for (int i = 0; i < voice_count_; ++i) {
        voice_states_[i].voice.envelope().SetRelease(value);
      };
      break;
    case ControlType::kPitchShift:
      pitch_shift_ = value;
      for (int i = 0; i < voice_count_; ++i) {
        if (Voice& voice = voice_states_[i].voice; voice.IsActive()) {
          const double shifted_pitch =
              voice_states_[i].pitch + pitch_shift_ + voice_states_[i].pitch_shift;
          voice.oscillator().SetFrequency(FrequencyFromPitch(shifted_pitch, reference_frequency_));
          voice.sample_player().SetSpeed(
              FrequencyRatioFromPitch(shifted_pitch - voice_states_[i].root_pitch));
        }
      }
      break;
    case ControlType::kRetrigger:
      should_retrigger_ = static_cast<bool>(value);
      break;
    default:
      assert(!"Invalid control type");
      break;
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
          voice.oscillator().SetFrequency(FrequencyFromPitch(shifted_pitch, reference_frequency_));
          voice.sample_player().SetSpeed(
              FrequencyRatioFromPitch(shifted_pitch - voice_states_[i].root_pitch));
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
      voice_states_[i].voice.Stop();
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
  voice.oscillator().SetFrequency(FrequencyFromPitch(shifted_pitch, reference_frequency_));
  if (const auto* sample = sample_data_.Select(pitch); sample != nullptr) {
    voice_state.root_pitch = sample->root_pitch;
    voice.sample_player().SetData(sample->samples, sample->sample_rate, sample->sample_count);
    voice.sample_player().SetSpeed(FrequencyRatioFromPitch(shifted_pitch - sample->root_pitch));
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
      voice.oscillator().SetFrequency(FrequencyFromPitch(shifted_pitch, reference_frequency_));
    }
  }
}

void InstrumentProcessor::SetSampleData(SampleData& sample_data) noexcept {
  sample_data_.Swap(sample_data);
  for (int i = 0; i < voice_count_; ++i) {
    if (Voice& voice = voice_states_[i].voice; !voice.IsActive()) {
      voice.sample_player().SetData(nullptr, 0, 0);
    } else if (const auto* sample = sample_data_.Select(voice_states_[i].pitch);
               sample != nullptr) {
      voice_states_[i].root_pitch = sample->root_pitch;
      voice.sample_player().SetData(sample->samples, sample->sample_rate, sample->sample_count);
      voice.sample_player().SetSpeed(
          FrequencyRatioFromPitch(voice_states_[i].pitch + pitch_shift_ - sample->root_pitch));
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

}  // namespace barely
