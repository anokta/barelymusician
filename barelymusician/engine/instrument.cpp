#include "barelymusician/engine/instrument.h"

#include <algorithm>
#include <any>
#include <utility>

#include "barelymusician/engine/instrument_definition.h"

namespace barelyapi {

Instrument::Instrument(int sample_rate, InstrumentDefinition definition)
    : definition_(std::move(definition)) {
  if (definition_.create_fn) {
    definition_.create_fn(&state_, sample_rate);
  }
}

Instrument::~Instrument() {
  if (definition_.destroy_fn) {
    definition_.destroy_fn(&state_);
  }
}

void Instrument::Process(int sample_rate, float* output, int num_channels,
                         int num_frames) {
  if (definition_.process_fn) {
    definition_.process_fn(&state_, sample_rate, output, num_channels,
                           num_frames);
  } else {
    std::fill_n(output, num_channels * num_frames, 0.0f);
  }
}

void Instrument::SetCustomData(std::any data) {
  if (definition_.set_custom_data_fn) {
    definition_.set_custom_data_fn(&state_, std::move(data));
  }
}

void Instrument::SetNoteOff(float pitch) {
  if (definition_.set_note_off_fn) {
    definition_.set_note_off_fn(&state_, pitch);
  }
}

void Instrument::SetNoteOn(float pitch, float intensity) {
  if (definition_.set_note_on_fn) {
    definition_.set_note_on_fn(&state_, pitch, intensity);
  }
}

void Instrument::SetParam(int id, float value) {
  if (definition_.set_param_fn) {
    definition_.set_param_fn(&state_, id, value);
  }
}

}  // namespace barelyapi
