#ifndef BARELYMUSICIAN_DSP_INSTRUMENT_PROCESSOR_H_
#define BARELYMUSICIAN_DSP_INSTRUMENT_PROCESSOR_H_

#include <array>
#include <cmath>
#include <span>

#include "barelymusician.h"
#include "common/rng.h"
#include "dsp/envelope.h"
#include "dsp/sample_data.h"
#include "dsp/voice.h"

namespace barely {

/// Class that wraps the audio processing of an instrument.
class InstrumentProcessor {
 public:
  /// Constructs a new `InstrumentProcessor`.
  ///
  /// @param control_overrides Span of control overrides.
  /// @param rng Random number generator.
  /// @param sample_rate Sampling rate in hertz.
  /// @param reference_frequency Reference frequency in hertz.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  InstrumentProcessor(std::span<const BarelyControlOverride> control_overrides, AudioRng& rng,
                      int sample_rate, float reference_frequency) noexcept;

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
  /// @param note_controls Array of note controls.
  void SetNoteOn(float pitch,
                 const std::array<float, BarelyNoteControlType_kCount>& note_controls) noexcept;

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
        output_samples[i] += voice_callback_(voice, voice_params_);
      } else {
        output_samples[i] = voice_callback_(voice, voice_params_);
      }
    }
  }

  VoiceCallback voice_callback_ = Voice::Next<OscMode::kMix, SliceMode::kSustain>;
  std::array<VoiceState, kMaxVoiceCount> voice_states_;
  int voice_count_ = 8;

  float sample_interval_ = 0.0f;
  Envelope::Adsr adsr_;

  SampleData sample_data_;

  // TODO(#146): Filter coefficients should likely be calculated in `BarelyInstrument` instead.
  FilterType filter_type_ = FilterType::kNone;
  float filter_frequency_ = 0.0f;
  float filter_q_ = std::sqrt(0.5f);
  OscMode osc_mode_ = OscMode::kMix;
  SliceMode slice_mode_ = SliceMode::kSustain;

  Voice::Params voice_params_ = {};

  bool should_retrigger_ = false;

  float pitch_shift_ = 0.0f;
  float osc_pitch_shift_ = 0.0f;
  float reference_frequency_ = 0.0f;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_INSTRUMENT_PROCESSOR_H_
