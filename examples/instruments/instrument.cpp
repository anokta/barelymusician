#include "examples/instruments/instrument.h"

#include "barelymusician/base/logging.h"

namespace barelyapi {
namespace examples {

InstrumentDefinition GetInstrumentDefinition(CreateFn create_fn) {
  return InstrumentDefinition{
      .create_fn =
          [create_fn](InstrumentState* state) {
            *state = create_fn().release();
          },
      .destroy_fn =
          [](InstrumentState* state) {
            Instrument* instrument = reinterpret_cast<Instrument*>(*state);
            DCHECK(instrument);
            delete instrument;
          },
      .process_fn =
          [](InstrumentState* state, float* output, int num_channels,
             int num_frames) {
            Instrument* instrument = reinterpret_cast<Instrument*>(*state);
            DCHECK(instrument);
            instrument->Process(output, num_channels, num_frames);
          },
      .set_note_off_fn =
          [](InstrumentState* state, float note_index) {
            Instrument* instrument = reinterpret_cast<Instrument*>(*state);
            DCHECK(instrument);
            instrument->NoteOff(note_index);
          },
      .set_custom_data_fn =
          [](InstrumentState* state, void* data) {
            Instrument* instrument = reinterpret_cast<Instrument*>(*state);
            DCHECK(instrument);
            instrument->SetCustomData(data);
          },
      .set_note_on_fn =
          [](InstrumentState* state, float note_index, float note_intensity) {
            Instrument* instrument = reinterpret_cast<Instrument*>(*state);
            DCHECK(instrument);
            instrument->NoteOn(note_index, note_intensity);
          },
      .set_param_fn =
          [](InstrumentState* state, int param_id, float param_value) {
            Instrument* instrument = reinterpret_cast<Instrument*>(*state);
            DCHECK(instrument);
            instrument->SetParam(param_id, param_value);
          }};
}

}  // namespace examples
}  // namespace barelyapi
