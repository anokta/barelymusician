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

  /// Implements `CustomInstrument`.
  void Process(double* output_samples, int output_channel_count, int output_frame_count) noexcept;
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetControl(int id, double value) noexcept;
  void SetData(const void* data, int size) noexcept;
  void SetNoteControl(double /*pitch*/, int /*id*/, double /*value*/) noexcept {}
  void SetNoteOff(double pitch) noexcept;
  void SetNoteOn(double pitch, double intensity) noexcept;

 private:
  struct SampleData {
    const double* data = nullptr;
    int length = 0;
    int frame_rate = 0;
  };
  std::unordered_map<double, SampleData> sample_data_;
  int frame_rate_ = 0;
  double reference_frequency_ = 0.0;
  double pitch_shift_ = 0.0;
  GainProcessor gain_processor_;
  PolyphonicVoice voice_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_INTERNAL_INSTRUMENT_PROCESSOR_H_
