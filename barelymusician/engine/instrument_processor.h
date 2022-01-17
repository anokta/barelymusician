#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_

#include <map>

#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_event.h"

namespace barelyapi {

/// Class that wraps audio thread functionality of an instrument.
class InstrumentProcessor {
 public:
  /// Constructs new `InstrumentProcessor`.
  ///
  /// @param definition Instrument definition.
  /// @param param_values Parameter values.
  /// @param sample_rate Sampling rate in hz.
  InstrumentProcessor(InstrumentDefinition definition,
                      std::vector<float> param_values,
                      int sample_rate) noexcept;

  /// Destroys `InstrumentProcessor`.
  ~InstrumentProcessor() noexcept;

  /// Non-copyable and non-movable.
  InstrumentProcessor(const InstrumentProcessor& other) = delete;
  InstrumentProcessor& operator=(const InstrumentProcessor& other) = delete;
  InstrumentProcessor(InstrumentProcessor&& other) noexcept = delete;
  InstrumentProcessor& operator=(InstrumentProcessor&& other) noexcept = delete;

  /// Merges events.
  ///
  /// @param events Map of instrument events by their timestamps.
  void MergeEvents(std::multimap<double, InstrumentEvent> events) noexcept;

  /// Processes next output buffer.
  ///
  /// @param output Output buffer.
  /// @param num_output_channels Number of output channels.
  /// @param num_output_frames Number of output frames.
  /// @param timestamp Timestamp in seconds.
  void Process(float* output, int num_output_channels, int num_output_frames,
               double timestamp) noexcept;

  /// Resets instrument.
  ///
  /// @param param_values List of parameter values.
  /// @param sample_rate Sampling rate in hz.
  void Reset(std::vector<float> param_values, int sample_rate) noexcept;

 private:
  // Create function.
  InstrumentDefinition::CreateFn create_fn_;

  // Destroy function.
  InstrumentDefinition::DestroyFn destroy_fn_;

  // Process function.
  InstrumentDefinition::ProcessFn process_fn_;

  // Set data function.
  InstrumentDefinition::SetDataFn set_data_fn_;

  // Set note off function.
  InstrumentDefinition::SetNoteOffFn set_note_off_fn_;

  // Set note on function.
  InstrumentDefinition::SetNoteOnFn set_note_on_fn_;

  // Set parameter function.
  InstrumentDefinition::SetParamFn set_param_fn_;

  // Map of events by their timestamps.
  std::multimap<double, InstrumentEvent> events_;

  // Gain in amplitude.
  float gain_;

  // Sampling rate in hz.
  int sample_rate_;

  // State.
  void* state_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_
