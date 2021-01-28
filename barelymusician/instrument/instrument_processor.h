#ifndef BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_PROCESSOR_H_
#define BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_PROCESSOR_H_

#include <map>

#include "barelymusician/instrument/instrument_data.h"
#include "barelymusician/instrument/instrument_definition.h"

namespace barelyapi {

/// Instrument processor that wraps the audio thread calls of an instrument.
class InstrumentProcessor {
 public:
  /// Constructs new |InstrumentProcessor|.
  ///
  /// @param definition Instrument definition.
  explicit InstrumentProcessor(InstrumentDefinition definition);

  /// Destroys |InstrumentProcessor|.
  ~InstrumentProcessor();

  /// Non-copyable.
  InstrumentProcessor(const InstrumentProcessor& other) = delete;
  InstrumentProcessor& operator=(const InstrumentProcessor& other) = delete;

  /// Movable.
  InstrumentProcessor(InstrumentProcessor&& other) noexcept;
  InstrumentProcessor& operator=(InstrumentProcessor&& other) noexcept;

  /// Processes the next output buffer at a given timestamp.
  ///
  /// @param sample_rate Sampling rate in Hz.
  /// @param timestamp Timestamp in seconds.
  /// @param output Pointer to the output buffer.
  /// @param num_channels Number of output channels.
  /// @param num_frames Number of output frames.
  void Process(int sample_rate, double timestamp, float* output,
               int num_channels, int num_frames);

  /// Sets instrument data at a given timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  /// @param data Instrument data.
  void SetData(double timestamp, InstrumentData data);

 private:
  // Instrument definition.
  InstrumentDefinition definition_;

  // Instrument state.
  InstrumentState state_;

  // List of scheduled instrument data.
  std::multimap<double, InstrumentData> data_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_PROCESSOR_H_
