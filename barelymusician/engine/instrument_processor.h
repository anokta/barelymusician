#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_

#include <any>

#include "barelymusician/engine/instrument_definition.h"

namespace barelyapi {

/// Instrument processor that wraps the audio thread calls of an instrument.
class InstrumentProcessor {
 public:
  /// Constructs new |InstrumentProcessor|.
  ///
  /// @param sample_rate Sampling rate in Hz.
  /// @param definition InstrumentProcessor definition.
  InstrumentProcessor(int sample_rate, InstrumentDefinition definition);

  /// Destroys |InstrumentProcessor|.
  ~InstrumentProcessor();

  /// Copyable and movable.
  InstrumentProcessor(const InstrumentProcessor& other) = default;
  InstrumentProcessor& operator=(const InstrumentProcessor& other) = default;
  InstrumentProcessor(InstrumentProcessor&& other) = default;
  InstrumentProcessor& operator=(InstrumentProcessor&& other) = default;

  /// Processes the next output buffer.
  ///
  /// @param sample_rate Sampling rate in Hz.
  /// @param output Pointer to the output buffer.
  /// @param num_channels Number of output channels.
  /// @param num_frames Number of output frames.
  void Process(int sample_rate, float* output, int num_channels,
               int num_frames);

  /// Sets custom data.
  ///
  /// @param data Custom data.
  void SetCustomData(std::any data);

  /// Sets note off.
  ///
  /// @param pitch Note pitch.
  void SetNoteOff(float pitch);

  /// Sets note on.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  void SetNoteOn(float pitch, float intensity);

  /// Sets parameter.
  ///
  /// @param id Parameter id.
  /// @param value Parameter value.
  void SetParam(int id, float value);

 private:
  // Instrument destroy function.
  DestroyInstrumentFn destroy_fn_;

  // Instrument process function.
  ProcessInstrumentFn process_fn_;

  // Instrument set custom data function.
  SetCustomInstrumentDataFn set_custom_data_fn_;

  // Instrument set note off function.
  SetInstrumentNoteOffFn set_note_off_fn_;

  // Instrument set note on function.
  SetInstrumentNoteOnFn set_note_on_fn_;

  // Instrument set parameter function.
  SetInstrumentParamFn set_param_fn_;

  // Instrument state.
  InstrumentState state_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_
