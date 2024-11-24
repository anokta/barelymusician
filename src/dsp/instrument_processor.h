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
  InstrumentProcessor(int sample_rate, double reference_frequency) noexcept;

  /// Processes the next output samples.
  ///
  /// @param output_samples Array of mono output samples.
  /// @param output_sample_count Number of output samples.
  void Process(double* output_samples, int output_sample_count) noexcept;

  /// Sets a control value.
  ///
  /// @param type Control type.
  /// @param value Control value.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetControl(ControlType type, double value) noexcept;

  /// Sets a note control value.
  ///
  /// @param pitch Note pitch.
  /// @param type Note control type.
  /// @param value Note control value.
  void SetNoteControl(double pitch, NoteControlType type, double value) noexcept;

  /// Sets a note off.
  ///
  /// @param pitch Note pitch.
  void SetNoteOff(double pitch) noexcept;

  /// Sets a note on.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  void SetNoteOn(double pitch, double intensity) noexcept;

  /// Sets the reference frequency.
  ///
  /// @param reference_frequency Reference frequency in hertz.
  void SetReferenceFrequency(double reference_frequency) noexcept;

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
    double pitch = 0.0;
    double pitch_shift = 0.0;
    int timestamp = 0;
  };

  /// Acquires a new voice.
  Voice& AcquireVoice(double pitch) noexcept;

  template <bool kShouldAccumulate>
  void ProcessVoice(Voice& voice, double* output_samples, int output_sample_count) noexcept {
    for (int i = 0; i < output_sample_count; ++i) {
      if (!voice.IsActive()) {
        if constexpr (!kShouldAccumulate) {
          std::fill(output_samples + i, output_samples + output_sample_count, 0.0);
        }
        return;
      }
      if constexpr (kShouldAccumulate) {
        output_samples[i] +=
            voice_callback_(voice, filter_coefficient_, oscillator_mix_, pulse_width_);
      } else {
        output_samples[i] =
            voice_callback_(voice, filter_coefficient_, oscillator_mix_, pulse_width_);
      }
    }
  }

  VoiceCallback voice_callback_ =
      Voice::Next<FilterType::kNone, OscillatorShape::kNone, SamplePlaybackMode::kNone>;
  std::array<VoiceState, kMaxVoiceCount> voice_states_;
  int voice_count_ = 8;

  double sample_interval_ = 0.0;
  Envelope::Adsr adsr_;

  GainProcessor gain_processor_;
  SampleData sample_data_;

  FilterType filter_type_ = FilterType::kNone;
  OscillatorShape oscillator_shape_ = OscillatorShape::kNone;
  SamplePlaybackMode sample_playback_mode_ = SamplePlaybackMode::kNone;

  double filter_coefficient_ = 1.0;
  double oscillator_mix_ = 0.0;
  double pulse_width_ = 0.5;

  bool should_retrigger_ = false;

  double pitch_shift_ = 0.0;
  double oscillator_pitch_shift_ = 0.0;
  double reference_frequency_ = 0.0;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_DSP_INSTRUMENT_PROCESSOR_H_
