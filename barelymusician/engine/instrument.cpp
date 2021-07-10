#include "barelymusician/engine/instrument.h"

#include <algorithm>
#include <any>
#include <utility>

#include "barelymusician/engine/instrument_definition.h"

namespace barelyapi {

namespace {

// Dummy set custom instrument data function that does nothing.
void NoopSetCustomInstrumentDataFn(InstrumentState* /*state*/,
                                   std::any /*data*/) {}

// Dummy set instrument note off function that does nothing.
void NoopSetInstrumentNoteOffFn(InstrumentState* /*state*/, float /*pitch*/) {}

// Dummy set instrument note on function that does nothing.
void NoopSetInstrumentNoteOnFn(InstrumentState* /*state*/, float /*pitch*/,
                               float /*intensity*/) {}

// Dummy set instrument parameter function that does nothing.
void NoopSetInstrumentParamFn(InstrumentState* /*state*/, int /*id*/,
                              float /*value*/) {}

// Process instrument function that fills the output buffer with zeros.
void ZeroFillProcessInstrumentFn(InstrumentState* /*state*/,
                                 int /*sample_rate*/, float* output,
                                 int num_channels, int num_frames) {
  std::fill_n(output, num_channels * num_frames, 0.0f);
}

}  // namespace

Instrument::Instrument(int sample_rate, InstrumentDefinition definition)
    : destroy_fn_(std::move(definition.destroy_fn)),
      process_fn_(definition.process_fn ? std::move(definition.process_fn)
                                        : &ZeroFillProcessInstrumentFn),
      set_custom_data_fn_(definition.set_custom_data_fn
                              ? std::move(definition.set_custom_data_fn)
                              : &NoopSetCustomInstrumentDataFn),
      set_note_off_fn_(definition.set_note_off_fn
                           ? std::move(definition.set_note_off_fn)
                           : &NoopSetInstrumentNoteOffFn),
      set_note_on_fn_(definition.set_note_on_fn
                          ? std::move(definition.set_note_on_fn)
                          : &NoopSetInstrumentNoteOnFn),
      set_param_fn_(definition.set_param_fn ? std::move(definition.set_param_fn)
                                            : &NoopSetInstrumentParamFn) {
  if (definition.create_fn) {
    definition.create_fn(&state_, sample_rate);
  }
}

Instrument::~Instrument() {
  // Make sure to call |destroy_fn_| only if it's still valid (e.g., not moved).
  if (destroy_fn_) {
    destroy_fn_(&state_);
  }
}

void Instrument::Process(int sample_rate, float* output, int num_channels,
                         int num_frames) {
  process_fn_(&state_, sample_rate, output, num_channels, num_frames);
}

void Instrument::SetCustomData(std::any data) {
  set_custom_data_fn_(&state_, std::move(data));
}

void Instrument::SetNoteOff(float pitch) { set_note_off_fn_(&state_, pitch); }

void Instrument::SetNoteOn(float pitch, float intensity) {
  set_note_on_fn_(&state_, pitch, intensity);
}

void Instrument::SetParam(int id, float value) {
  set_param_fn_(&state_, id, value);
}

}  // namespace barelyapi
