#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_DEFINITION_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_DEFINITION_H_

#include <any>
#include <functional>
#include <vector>

namespace barelyapi {

using InstrumentState = void*;

using CreateInstrumentFn = std::function<void(InstrumentState* state)>;

using DestroyInstrumentFn = std::function<void(InstrumentState* state)>;

using ProcessInstrumentFn = std::function<void(
    InstrumentState* state, float* output, int num_channels, int num_frames)>;

using SetCustomInstrumentDataFn =
    std::function<void(InstrumentState* state, void* data)>;

using SetInstrumentNoteOffFn =
    std::function<void(InstrumentState* state, float note_index)>;

using SetInstrumentNoteOnFn = std::function<void(
    InstrumentState* state, float note_index, float note_intensity)>;

using SetInstrumentParamFn = std::function<void(
    InstrumentState* state, int param_id, float param_value)>;

struct InstrumentDefinition {
  CreateInstrumentFn create_fn;
  DestroyInstrumentFn destroy_fn;
  ProcessInstrumentFn process_fn;
  SetCustomInstrumentDataFn set_custom_data_fn;
  SetInstrumentNoteOffFn set_note_off_fn;
  SetInstrumentNoteOnFn set_note_on_fn;
  SetInstrumentParamFn set_param_fn;
};

struct InstrumentParamDefinition {
  int id;
  float default_value;
  //   float max_value;
  //   float min_value;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_DEFINITION_H_
