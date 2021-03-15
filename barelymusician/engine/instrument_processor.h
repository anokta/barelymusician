#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_

#include <map>

#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_event.h"

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
  /// @param output Pointer to the output buffer.
  /// @param num_channels Number of output channels.
  /// @param num_frames Number of output frames.
  /// @param timestamp Timestamp in seconds.
  void Process(float* output, int num_channels, int num_frames,
               double timestamp);

  /// Resets instrument.
  ///
  /// @param sample_rate System sampling rate in Hz.
  void Reset(int sample_rate);

  /// Schedules instrument event at a given timestamp.
  ///
  /// @param event Instrument event.
  /// @param timestamp Timestamp in seconds.
  void ScheduleEvent(InstrumentEvent event, double timestamp);

  /// Schedules multiple instrument events at given timestamps.
  ///
  /// @param events List of instrument events with their timestamps.
  void ScheduleEvents(std::multimap<double, InstrumentEvent> events);

 private:
  // Sampling rate in Hz.
  int sample_rate_;

  // Instrument definition.
  InstrumentDefinition definition_;

  // List of scheduled instrument events.
  std::multimap<double, InstrumentEvent> events_;

  // Instrument state.
  InstrumentState state_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_
