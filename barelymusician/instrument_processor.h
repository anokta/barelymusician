#ifndef BARELYMUSICIAN_INSTRUMENT_PROCESSOR_H_
#define BARELYMUSICIAN_INSTRUMENT_PROCESSOR_H_

#include <map>

#include "barelymusician/barelymusician.h"
#include "barelymusician/instrument_event.h"

namespace barelyapi {

/// Class that wraps audio thread functionality of an instrument.
class InstrumentProcessor {
 public:
  /// Constructs new `InstrumentProcessor`.
  ///
  /// @param definition Instrument definition.
  /// @param sample_rate Sampling rate in hz.
  InstrumentProcessor(const BarelyInstrumentDefinition& definition,
                      int sample_rate) noexcept;

  /// Destroys `InstrumentProcessor`.
  ~InstrumentProcessor() noexcept;

  /// Non-copyable and non-movable.
  InstrumentProcessor(const InstrumentProcessor& other) = delete;
  InstrumentProcessor& operator=(const InstrumentProcessor& other) = delete;
  InstrumentProcessor(InstrumentProcessor&& other) noexcept = delete;
  InstrumentProcessor& operator=(InstrumentProcessor&& other) noexcept = delete;

  /// Adds events.
  ///
  /// @param events Map of instrument events by their timestamps.
  void AddEvents(std::multimap<double, InstrumentEvent> events) noexcept;

  /// Processes next output buffer.
  ///
  /// @param output Output buffer.
  /// @param num_output_channels Number of output channels.
  /// @param num_output_frames Number of output frames.
  /// @param timestamp Timestamp in seconds.
  void Process(float* output, int num_output_channels, int num_output_frames,
               double timestamp) noexcept;

 private:
  // Returns corresponding number of samples for given number of `seconds`.
  int GetSamples(double seconds) const noexcept;

  // Returns corresponding number of seconds for given number of `samples`.
  double GetSeconds(int samples) const noexcept;

  // Create function.
  BarelyInstrumentDefinition_CreateCallback create_callback_;

  // Destroy function.
  BarelyInstrumentDefinition_DestroyCallback destroy_callback_;

  // Process function.
  BarelyInstrumentDefinition_ProcessCallback process_callback_;

  // Set data function.
  BarelyInstrumentDefinition_SetDataCallback set_data_callback_;

  // Set note off function.
  BarelyInstrumentDefinition_SetNoteOffCallback set_note_off_callback_;

  // Set note on function.
  BarelyInstrumentDefinition_SetNoteOnCallback set_note_on_callback_;

  // Set parameter function.
  BarelyInstrumentDefinition_SetParameterCallback set_parameter_callback_;

  // Map of events by their timestamps.
  std::multimap<double, InstrumentEvent> events_;

  // Gain in amplitude.
  float gain_ = 1.0f;

  // Sampling rate in hz.
  int sample_rate_ = 0;

  // State.
  void* state_ = nullptr;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_PROCESSOR_H_
