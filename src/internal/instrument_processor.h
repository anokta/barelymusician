#ifndef BARELYMUSICIAN_INTERNAL_INSTRUMENT_PROCESSOR_H_
#define BARELYMUSICIAN_INTERNAL_INSTRUMENT_PROCESSOR_H_

#include <vector>

#include "dsp/gain_processor.h"
#include "dsp/voice.h"
#include "internal/sample_data.h"

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
  // List of voices with their pitch and timestamp. Voice timestamp is used to determine which voice
  // to steal when there are no free voices available.
  // TODO(#12): Consider a more optimized implementation for voice stealing.
  struct VoiceState {
    Voice voice;
    double pitch = 0.0;
    double pitch_shift = 0.0;
    double root_pitch = 0.0;
    int timestamp = 0;
  };
  VoiceState& AcquireVoice(double pitch) noexcept;
  std::vector<VoiceState> voice_states_;
  int voice_count_ = 8;

  GainProcessor gain_processor_;
  SampleData sample_data_;

  bool should_retrigger_ = false;

  double reference_frequency_ = 0.0;
  double pitch_shift_ = 0.0;

  int sample_rate_ = 0;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_INSTRUMENT_PROCESSOR_H_
