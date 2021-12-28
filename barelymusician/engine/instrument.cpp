#include "barelymusician/engine/instrument.h"

#include <algorithm>
#include <any>
#include <utility>

#include "barelymusician/engine/instrument_definition.h"

namespace barely {

namespace {

// Dummy set custom instrument data function that does nothing.
void NoopSetCustomInstrumentDataFn(InstrumentState* /*state*/,
                                   std::any /*data*/) noexcept {}

// Dummy set instrument note off function that does nothing.
void NoopSetInstrumentNoteOffFn(InstrumentState* /*state*/,
                                float /*pitch*/) noexcept {}

// Dummy set instrument note on function that does nothing.
void NoopSetInstrumentNoteOnFn(InstrumentState* /*state*/, float /*pitch*/,
                               float /*intensity*/) noexcept {}

// Dummy set instrument parameter function that does nothing.
void NoopSetInstrumentParamFn(InstrumentState* /*state*/, int /*index*/,
                              float /*value*/) noexcept {}

// Process instrument function that fills the output buffer with zeros.
void ZeroFillProcessInstrumentFn(InstrumentState* /*state*/, float* output,
                                 int num_channels, int num_frames) noexcept {
  std::fill_n(output, num_channels * num_frames, 0.0f);
}

}  // namespace

Instrument::Instrument(int sample_rate,
                       InstrumentDefinition definition) noexcept
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
                                            : &NoopSetInstrumentParamFn),
      gain_(1.0f) {
  if (definition.create_fn) {
    definition.create_fn(&state_, sample_rate);
  }
}

Instrument::~Instrument() noexcept {
  // Make sure to call |destroy_fn_| only if it's still valid (e.g., not moved).
  if (destroy_fn_) {
    destroy_fn_(&state_);
  }
}

void Instrument::Process(float* output, int num_channels,
                         int num_frames) noexcept {
  process_fn_(&state_, output, num_channels, num_frames);
  // TODO(#88): Revisit gain processing.
  for (int i = 0; i < num_channels * num_frames; ++i) {
    output[i] *= gain_;
  }
}

void Instrument::SetCustomData(std::any data) noexcept {
  set_custom_data_fn_(&state_, std::move(data));
}

void Instrument::SetGain(float gain) noexcept { gain_ = gain; }

void Instrument::SetNoteOff(float pitch) noexcept {
  set_note_off_fn_(&state_, pitch);
}

void Instrument::SetNoteOn(float pitch, float intensity) noexcept {
  set_note_on_fn_(&state_, pitch, intensity);
}

void Instrument::SetParam(int index, float value) noexcept {
  set_param_fn_(&state_, index, value);
}

}  // namespace barely
