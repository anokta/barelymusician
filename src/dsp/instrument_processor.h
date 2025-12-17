#ifndef BARELYMUSICIAN_DSP_INSTRUMENT_PROCESSOR_H_
#define BARELYMUSICIAN_DSP_INSTRUMENT_PROCESSOR_H_

#include <barelymusician.h>

#include <array>
#include <cmath>
#include <span>

#include "common/constants.h"
#include "common/rng.h"
#include "dsp/biquad_filter.h"
#include "dsp/envelope.h"
#include "dsp/sample_data.h"
#include "dsp/voice.h"

namespace barely {

/// Class that wraps the audio processing of an instrument.
class InstrumentProcessor {
 public:
  /// Constructs a new `InstrumentProcessor`.
  ///
  /// @param control_overrides Span of instrument control overrides.
  /// @param filter_coeffs Filter coefficients.
  /// @param rng Random number generator.
  /// @param sample_rate Sampling rate in hertz.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  InstrumentProcessor(std::span<const BarelyInstrumentControlOverride> control_overrides,
                      const BiquadFilter::Coefficients& filter_coeffs, AudioRng& rng,
                      int sample_rate) noexcept;

  /// Processes the next output samples.
  ///
  /// @tparam kIsSidechainSend Denotes whether the sidechain frame is for send or receive.
  /// @param delay_frame Delay send frame.
  /// @param sidechain_frame Sidechain send frame.
  /// @param output_frame Output frame.
  template <bool kIsSidechainSend = false>
  void Process(float delay_frame[kStereoChannelCount], float sidechain_frame[kStereoChannelCount],
               float output_frame[kStereoChannelCount]) noexcept {
    for (int i = active_voice_count_ - 1; i >= 0; --i) {
      auto& voice = active_voice_states_[i]->voice;
      if constexpr (kIsSidechainSend) {
        if (!voice.IsActive()) {
          std::swap(active_voice_states_[i], active_voice_states_[active_voice_count_ - 1]);
          --active_voice_count_;
          continue;
        }
      }
      voice.Process<kIsSidechainSend>(params_, delay_frame, sidechain_frame, output_frame);
    }
  }

  /// Sets a control value.
  ///
  /// @param type Instrument control type.
  /// @param value Instrument control value.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetControl(InstrumentControlType type, float value) noexcept;

  /// Sets the filter coefficients.
  ///
  /// @param coeff Filter coefficients.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetFilterCoefficients(const BiquadFilter::Coefficients& coeffs) noexcept {
    params_.voice_params.filter_coefficients = coeffs;
  }

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

  /// Sets the sample data.
  ///
  /// @param sample_data Sample data.
  void SetSampleData(SampleData& sample_data) noexcept;

 private:
  static constexpr int kMaxVoiceCount = 16;

  // List of voices with their pitch and timestamp. Voice timestamp is used to determine which voice
  // to steal when there are no free voices available.
  // TODO(#12): Consider a more optimized implementation for voice stealing.
  struct VoiceState {
    Voice voice;
    float pitch = 0.0f;
    float pitch_shift = 0.0f;
    int timestamp = 0;
  };

  // Acquires a new voice.
  [[nodiscard]] Voice& AcquireVoice(float pitch) noexcept;

  std::array<VoiceState, kMaxVoiceCount> voice_states_;
  int voice_count_ = 8;

  std::array<VoiceState*, kMaxVoiceCount> active_voice_states_;
  int active_voice_count_ = 0;

  float sample_interval_ = 0.0f;
  SampleData sample_data_;

  InstrumentParams params_ = {};

  float pitch_shift_ = 0.0f;
  float osc_pitch_shift_ = 0.0f;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_INSTRUMENT_PROCESSOR_H_
