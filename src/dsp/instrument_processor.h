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
#include "dsp/voice_pool.h"

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
                      VoicePool& voice_pool, int sample_rate) noexcept;

  /// Destroys `InstrumentProcessor`.
  ~InstrumentProcessor() noexcept;

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
  InstrumentParams params_ = {};

  VoicePool& voice_pool_;
  float sample_interval_ = 0.0f;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_INSTRUMENT_PROCESSOR_H_
