#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_

#include <map>

#include "barelymusician/engine/instrument_data.h"
#include "barelymusician/engine/instrument_definition.h"

namespace barelyapi {

/// Instrument processor that wraps the audio thread calls of an instrument.
class InstrumentProcessor {
 public:
  /// Constructs new |InstrumentProcessor|.
  ///
  /// @param definition Instrument definition.
  /// @param sample_rate Sampling rate in Hz.
  InstrumentProcessor(InstrumentDefinition definition, int sample_rate);

  /// Destroys |InstrumentProcessor|.
  ~InstrumentProcessor();

  /// Copyable and movable.
  InstrumentProcessor(const InstrumentProcessor& other) = default;
  InstrumentProcessor& operator=(const InstrumentProcessor& other) = default;
  InstrumentProcessor(InstrumentProcessor&& other) = default;
  InstrumentProcessor& operator=(InstrumentProcessor&& other) = default;

  /// Processes the next output buffer at a given timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  /// @param output Pointer to the output buffer.
  /// @param num_channels Number of output channels.
  /// @param num_frames Number of output frames.
  void Process(double timestamp, float* output, int num_channels,
               int num_frames);

  /// Resets instrument.
  ///
  /// @param sample_rate System sampling rate in Hz.
  void Reset(int sample_rate);

  /// Sets instrument data at a given timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  /// @param data Instrument data.
  void SetData(double timestamp, InstrumentData data);

 private:
  // Sampling rate in Hz.
  int sample_rate_;

  // Instrument definition.
  InstrumentDefinition definition_;

  // Instrument state.
  InstrumentState state_;

  // List of scheduled instrument data.
  std::multimap<double, InstrumentData> data_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_
