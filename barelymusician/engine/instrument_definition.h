#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_DEFINITION_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_DEFINITION_H_

#include <vector>

#include "barelymusician/engine/parameter.h"

namespace barelyapi {

/// Instrument definition.
struct InstrumentDefinition {
  /// Create function signature.
  ///
  /// @param state Pointer to instrument state.
  /// @param sample_rate Sampling rate in hz.
  using CreateFn = void (*)(void** state, int sample_rate);

  /// Destroy function signature.
  ///
  /// @param state Pointer to instrument state.
  using DestroyFn = void (*)(void** state);

  /// Process function signature.
  ///
  /// @param state Pointer to instrument state.
  /// @param output Output buffer.
  /// @param num_output_channels Number of channels.
  /// @param num_output_frames Number of frames.
  using ProcessFn = void (*)(void** state, float* output,
                             int num_output_channels, int num_output_frames);

  /// Set data function signature.
  ///
  /// @param state Pointer to instrument state.
  /// @param data Data.
  using SetDataFn = void (*)(void** state, void* data);

  /// Set note off function signature.
  ///
  /// @param state Pointer to instrument state.
  /// @param pitch Note pitch.
  using SetNoteOffFn = void (*)(void** state, float pitch);

  /// Set note on function signature.
  ///
  /// @param state Pointer to instrument state.
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  using SetNoteOnFn = void (*)(void** state, float pitch, float intensity);

  /// Set parameter function signature.
  ///
  /// @param state Pointer to instrument state.
  /// @param index Parameter index.
  /// @param value Parameter value.
  using SetParameterFn = void (*)(void** state, int index, float value);

  /// Create function.
  CreateFn create_fn = nullptr;

  /// Destroy function.
  DestroyFn destroy_fn = nullptr;

  /// Process function.
  ProcessFn process_fn = nullptr;

  /// Set data function.
  SetDataFn set_data_fn = nullptr;

  /// Set note off function.
  SetNoteOffFn set_note_off_fn = nullptr;

  /// Set note on function.
  SetNoteOnFn set_note_on_fn = nullptr;

  /// Set parameter function.
  SetParameterFn set_parameter_fn = nullptr;

  /// List of parameter definitions.
  std::vector<ParameterDefinition> parameter_definitions = {};
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_DEFINITION_H_
