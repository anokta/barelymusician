#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_DEFINITION_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_DEFINITION_H_

#include <any>
#include <functional>
#include <vector>

#include "barelymusician/engine/param_definition.h"

namespace barelyapi {

/// Instrument state type.
using InstrumentState = std::any;

/// Instrument create function signature.
///
/// @param state Pointer to instrument state.
/// @param sample_rate Sampling rate in hz.
using CreateInstrumentFn =
    std::function<void(InstrumentState* state, int sample_rate)>;

/// Instrument destroy function signature.
///
/// @param state Pointer to instrument state.
using DestroyInstrumentFn = std::function<void(InstrumentState* state)>;

/// Instrument process function signature.
///
/// @param state Pointer to instrument state.
/// @param output Output buffer.
/// @param num_channels Number of channels.
/// @param num_frames Number of frames.
using ProcessInstrumentFn = std::function<void(
    InstrumentState* state, float* output, int num_channels, int num_frames)>;

/// Instrument set custom data function signature.
///
/// @param state Pointer to instrument state.
/// @param data Custom data.
using SetCustomInstrumentDataFn =
    std::function<void(InstrumentState* state, std::any data)>;

/// Instrument set note off function signature.
///
/// @param state Pointer to instrument state.
/// @param pitch Note pitch.
using SetInstrumentNoteOffFn =
    std::function<void(InstrumentState* state, float pitch)>;

/// Instrument set note on function signature.
///
/// @param state Pointer to instrument state.
/// @param pitch Note pitch.
/// @param intensity Note intensity.
using SetInstrumentNoteOnFn =
    std::function<void(InstrumentState* state, float pitch, float intensity)>;

/// Instrument set parameter function signature.
///
/// @param state Pointer to instrument state.
/// @param id Parameter identifier.
/// @param value Parameter value.
using SetInstrumentParamFn =
    std::function<void(InstrumentState* state, ParamId id, float value)>;

/// Instrument definition.
struct InstrumentDefinition {
  /// Create function.
  CreateInstrumentFn create_fn;

  /// Destroy function.
  DestroyInstrumentFn destroy_fn;

  /// Process function.
  ProcessInstrumentFn process_fn;

  /// Set custom data function.
  SetCustomInstrumentDataFn set_custom_data_fn;

  /// Set note off function.
  SetInstrumentNoteOffFn set_note_off_fn;

  /// Set note on function.
  SetInstrumentNoteOnFn set_note_on_fn;

  /// Set parameter function.
  SetInstrumentParamFn set_param_fn;

  /// Parameter definitions.
  std::vector<ParamDefinition> param_definitions;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_DEFINITION_H_
