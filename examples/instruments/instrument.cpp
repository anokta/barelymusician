#include "examples/instruments/instrument.h"

#include <typeinfo>

#include "barelymusician/base/logging.h"

namespace barelyapi {
namespace examples {

namespace {

Instrument* GetInstrument(InstrumentState* state) {
  DCHECK(state);
  // DCHECK(state->type() == typeid(std::unique_ptr<Instrument>));
  return std::any_cast<Instrument*>(*state);
}

}  // namespace

InstrumentDefinition GetInstrumentDefinition(CreateFn create_fn) {
  return InstrumentDefinition{
      .create_fn =
          [create_fn](InstrumentState* state, int sample_rate) {
            *state =
                std::make_any<Instrument*>(create_fn(sample_rate).release());
          },
      .destroy_fn =
          [](InstrumentState* state) {
            Instrument* instrument = GetInstrument(state);
            DCHECK(instrument);
            delete instrument;
          },
      .process_fn =
          [](InstrumentState* state, float* output, int num_channels,
             int num_frames) {
            Instrument* instrument = GetInstrument(state);
            DCHECK(instrument);
            instrument->Process(output, num_channels, num_frames);
          },
      .set_note_off_fn =
          [](InstrumentState* state, float note_index) {
            Instrument* instrument = GetInstrument(state);
            DCHECK(instrument);
            instrument->NoteOff(note_index);
          },
      .set_note_on_fn =
          [](InstrumentState* state, float note_index, float note_intensity) {
            Instrument* instrument = GetInstrument(state);
            DCHECK(instrument);
            instrument->NoteOn(note_index, note_intensity);
          },
      .set_param_fn =
          [](InstrumentState* state, int param_id, float param_value) {
            Instrument* instrument = GetInstrument(state);
            DCHECK(instrument);
            instrument->SetParam(param_id, param_value);
          }};
}

}  // namespace examples
}  // namespace barelyapi
