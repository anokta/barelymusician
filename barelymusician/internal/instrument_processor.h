#ifndef BARELYMUSICIAN_INTERNAL_INSTRUMENT_PROCESSOR_H_
#define BARELYMUSICIAN_INTERNAL_INSTRUMENT_PROCESSOR_H_

#include <vector>

#include "barelymusician/dsp/gain_processor.h"
#include "barelymusician/dsp/polyphonic_voice.h"

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
  /// @param id Control identifier.
  /// @param value Control value.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetControl(int id, double value) noexcept;

  /// Sets data.
  ///
  /// @param data Data.
  /// @param size Data size in bytes.
  void SetData(const void* data, int size) noexcept;

  /// Sets a note control value.
  ///
  /// @param pitch Note pitch.
  /// @param id Note control identifier.
  /// @param value Note control value.
  void SetNoteControl(double /*pitch*/, int /*id*/, double /*value*/) noexcept {}

  /// Sets a note off.
  ///
  /// @param pitch Note pitch.
  void SetNoteOff(double pitch) noexcept;

  /// Sets a note on.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  void SetNoteOn(double pitch, double intensity) noexcept;

 private:
  struct SampleData {
    double pitch = 0.0;
    const double* data = nullptr;
    int length = 0;
    int frame_rate = 0;
    auto operator<=>(double other_pitch) const noexcept { return pitch <=> other_pitch; }
  };
  const SampleData* SelectSampleData(double pitch) const noexcept;
  std::vector<SampleData> sample_data_;
  double reference_frequency_ = 0.0;
  double pitch_shift_ = 0.0;
  GainProcessor gain_processor_;
  PolyphonicVoice voice_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_INTERNAL_INSTRUMENT_PROCESSOR_H_
