#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_

#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_event.h"
#include "barelymusician/engine/instrument_param.h"

namespace barelyapi {

/// Instrument processor that wraps the audio thread calls of an instrument.
class InstrumentProcessor {
 public:
  /// Constructs new |InstrumentProcessor|.
  ///
  /// @param sample_rate Sampling rate in Hz.
  /// @param definition Instrument definition.
  InstrumentProcessor(int sample_rate, InstrumentDefinition definition);

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

  /// Schedules instrument events.
  ///
  /// @param events List of timestamped instrument processor events.
  void Schedule(InstrumentProcessorEvents events);

 private:
  // Sampling rate in Hz.
  int sample_rate_;

  // Instrument definition.
  InstrumentDefinition definition_;

  // List of scheduled events.
  InstrumentProcessorEvents events_;

  // Instrument state.
  InstrumentState state_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_
