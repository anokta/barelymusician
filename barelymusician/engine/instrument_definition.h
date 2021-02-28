#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_DEFINITION_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_DEFINITION_H_

#include <any>
#include <functional>
#include <optional>
#include <vector>

namespace barelyapi {

/// Instrument state type.
using InstrumentState = std::any;

/// Instrument create function signature.
using CreateInstrumentFn =
    std::function<void(InstrumentState* state, int sample_rate)>;

/// Instrument destroy function signature.
using DestroyInstrumentFn = std::function<void(InstrumentState* state)>;

/// Instrument process function signature.
using ProcessInstrumentFn = std::function<void(
    InstrumentState* state, float* output, int num_channels, int num_frames)>;

/// Instrument set custom data function signature.
using SetCustomInstrumentDataFn =
    std::function<void(InstrumentState* state, void* data)>;

/// Instrument set note off function signature.
using SetInstrumentNoteOffFn =
    std::function<void(InstrumentState* state, float pitch)>;

/// Instrument set note on function signature.
using SetInstrumentNoteOnFn =
    std::function<void(InstrumentState* state, float pitch, float intensity)>;

/// Instrument set parameter function signature.
using SetInstrumentParamFn =
    std::function<void(InstrumentState* state, int id, float value)>;

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
};

/// Instrument parameter definition.
struct InstrumentParamDefinition {
  /// Parameter id.
  int id;

  /// Parameter default value.
  float default_value;

  /// Parameter minimum value.
  std::optional<float> min_value;

  /// Parameter maximum value.
  std::optional<float> max_value;
};

/// List of instrument parameter type.
using InstrumentParamDefinitions = std::vector<InstrumentParamDefinition>;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_DEFINITION_H_
