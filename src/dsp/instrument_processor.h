#ifndef BARELYMUSICIAN_DSP_INSTRUMENT_PROCESSOR_H_
#define BARELYMUSICIAN_DSP_INSTRUMENT_PROCESSOR_H_

#include <barelymusician.h>

#include <array>
#include <cmath>
#include <span>

#include "common/restrict.h"
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
  /// @param delay_frame Delay send frame.
  /// @param sidechain_frame Sidechain send frame.
  /// @param is_sidechain_send Denotes whether the sidechain frame is for send or receive.
  /// @param output_frame Output frame.
  // TODO(#174): Template `is_sidechain_send`.
  void Process(float* BARELY_RESTRICT delay_frame, float* BARELY_RESTRICT sidechain_frame,
               bool is_sidechain_send, float* BARELY_RESTRICT output_frame) noexcept {
    for (VoiceState& voice_state : voice_states_) {
      voice_callback_(voice_state.voice, params_, delay_frame, sidechain_frame, is_sidechain_send,
                      output_frame);
    }
  }

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

  // Acquires a new voice.
  Voice& AcquireVoice(float pitch) noexcept;

  VoiceCallback voice_callback_ = Voice::Process<OscMode::kMix, SliceMode::kSustain>;
  std::array<VoiceState, kMaxVoiceCount> voice_states_;
  int voice_count_ = 8;

  float sample_interval_ = 0.0f;
  SampleData sample_data_;

  // TODO(#146): Filter coefficients should likely be calculated in `BarelyInstrument` instead.
  FilterType filter_type_ = FilterType::kNone;
  float filter_frequency_ = 0.0f;
  float filter_q_ = std::sqrt(0.5f);
  OscMode osc_mode_ = OscMode::kMix;
  SliceMode slice_mode_ = SliceMode::kSustain;

  InstrumentParams params_ = {};

  bool should_retrigger_ = false;

  float pitch_shift_ = 0.0f;
  float osc_pitch_shift_ = 0.0f;
  float reference_frequency_ = 0.0f;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_INSTRUMENT_PROCESSOR_H_
