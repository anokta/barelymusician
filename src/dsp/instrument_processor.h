#ifndef BARELYMUSICIAN_DSP_INSTRUMENT_PROCESSOR_H_
#define BARELYMUSICIAN_DSP_INSTRUMENT_PROCESSOR_H_

#include <array>

#include "dsp/envelope.h"
#include "dsp/gain_processor.h"
#include "dsp/sample_data.h"
#include "dsp/voice.h"

namespace barely::internal {

/// Class that wraps the audio processing of an instrument.
class InstrumentProcessor {
 public:
  /// Constructs a new `InstrumentProcessor`.
  ///
  /// @param sample_rate Sampling rate in hertz.
  /// @param reference_frequency Reference frequency in hertz.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  InstrumentProcessor(int sample_rate, float reference_frequency) noexcept;

  /// Processes the next output samples.
  ///
  /// @param output_samples Array of mono output samples.
  /// @param output_sample_count Number of output samples.
  void Process(float* output_samples, int output_sample_count) noexcept;

  /// Sets a control value.
  ///
  /// @param type Control type.
  /// @param value Control value.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetControl(ControlType type, float value) noexcept;

  /// Sets a note control value.
  ///
  /// @param pitch Note pitch.
  /// @param type Note control type.
  /// @param value Note control value.
  void SetNoteControl(float pitch, NoteControlType type, float value) noexcept;

  /// Sets a note off.
  ///
  /// @param pitch Note pitch.
  void SetNoteOff(float pitch) noexcept;

  /// Sets a note on.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  void SetNoteOn(float pitch, float intensity) noexcept;

  /// Sets the reference frequency.
  ///
  /// @param reference_frequency Reference frequency in hertz.
  void SetReferenceFrequency(float reference_frequency) noexcept;

  /// Sets the sample data.
  ///
  /// @param sample_data Sample data.
  void SetSampleData(SampleData& sample_data) noexcept;

 private:
  static constexpr int kMaxVoiceCount = 20;

  // List of voices with their pitch and timestamp. Voice timestamp is used to determine which voice
  // to steal when there are no free voices available.
  // TODO(#12): Consider a more optimized implementation for voice stealing.
  struct VoiceState {
    Voice voice;
    float pitch = 0.0f;
    float pitch_shift = 0.0f;
    int timestamp = 0;
  };

  /// Acquires a new voice.
  Voice& AcquireVoice(float pitch) noexcept;

  template <bool kShouldAccumulate>
  void ProcessVoice(Voice& voice, float* output_samples, int output_sample_count) noexcept {
    for (int i = 0; i < output_sample_count; ++i) {
      if (!voice.IsActive()) {
        if constexpr (!kShouldAccumulate) {
          std::fill(output_samples + i, output_samples + output_sample_count, 0.0f);
        }
        return;
      }
      if constexpr (kShouldAccumulate) {
        output_samples[i] += voice_callback_(voice, bit_crusher_max_value_, bit_crusher_step_,
                                             filter_coefficient_, oscillator_mix_, pulse_width_);
      } else {
        output_samples[i] = voice_callback_(voice, bit_crusher_max_value_, bit_crusher_step_,
                                            filter_coefficient_, oscillator_mix_, pulse_width_);
      }
    }
  }

  VoiceCallback voice_callback_ = Voice::Next<FilterType::kNone, OscillatorMode::kMix,
                                              OscillatorShape::kNone, SamplePlaybackMode::kNone>;
  std::array<VoiceState, kMaxVoiceCount> voice_states_;
  int voice_count_ = 8;

  float sample_interval_ = 0.0f;
  Envelope::Adsr adsr_;

  GainProcessor gain_processor_;
  SampleData sample_data_;

  FilterType filter_type_ = FilterType::kNone;
  OscillatorMode oscillator_mode_ = OscillatorMode::kMix;
  OscillatorShape oscillator_shape_ = OscillatorShape::kNone;
  SamplePlaybackMode sample_playback_mode_ = SamplePlaybackMode::kNone;

  float bit_crusher_max_value_ = 1.0f;
  float bit_crusher_step_ = 1.0f;
  float filter_coefficient_ = 1.0f;
  float oscillator_mix_ = 0.0f;
  float pulse_width_ = 0.5f;

  bool should_retrigger_ = false;

  float pitch_shift_ = 0.0f;
  float oscillator_pitch_shift_ = 0.0f;
  float reference_frequency_ = 0.0f;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_DSP_INSTRUMENT_PROCESSOR_H_
