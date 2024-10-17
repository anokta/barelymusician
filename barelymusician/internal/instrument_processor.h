#ifndef BARELYMUSICIAN_INTERNAL_INSTRUMENT_PROCESSOR_H_
#define BARELYMUSICIAN_INTERNAL_INSTRUMENT_PROCESSOR_H_

#include <vector>

#include "barelymusician/dsp/gain_processor.h"
#include "barelymusician/dsp/voice.h"
#include "barelymusician/internal/sample_data.h"

namespace barely {

/// Class that wraps the audio processing of an instrument.
class InstrumentProcessor {
 public:
  /// Constructs a new `InstrumentProcessor`.
  ///
  /// @param frame_rate Frame rate in hertz.
  /// @param reference_frequency Reference frequency in hertz.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  InstrumentProcessor(int frame_rate, double reference_frequency) noexcept;

  /// Processes output samples.
  ///
  /// @param output_samples Array of interleaved output samples.
  /// @param output_channel_count Number of output channels.
  /// @param output_frame_count Number of output frames.
  void Process(double* output_samples, int output_channel_count, int output_frame_count) noexcept;

  /// Sets a control value.
  ///
  /// @param index Control index.
  /// @param value Control value.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetControl(int index, double value) noexcept;

  /// Sets a note control value.
  ///
  /// @param pitch Note pitch.
  /// @param index Note control index.
  /// @param value Note control value.
  void SetNoteControl(double /*pitch*/, int /*index*/, double /*value*/) noexcept {}

  /// Sets a note off.
  ///
  /// @param pitch Note pitch.
  void SetNoteOff(double pitch) noexcept;

  /// Sets a note on.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  void SetNoteOn(double pitch, double intensity) noexcept;

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
    int timestamp = 0;
  };
  Voice& AcquireVoice(double pitch) noexcept;
  std::vector<VoiceState> voice_states_;
  int voice_count_ = 0;

  GainProcessor gain_processor_;
  SampleData sample_data_;

  bool should_retrigger_ = false;

  double reference_frequency_ = 0.0;
  double pitch_shift_ = 0.0;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_INTERNAL_INSTRUMENT_PROCESSOR_H_
