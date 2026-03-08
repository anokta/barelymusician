#include "godot/instrument.h"

#include <barelymusician.h>

#include "godot/engine.h"
#include "godot_cpp/core/class_db.hpp"

namespace barely::godot {

using ::godot::ClassDB;
using ::godot::D_METHOD;

BarelyInstrument::~BarelyInstrument() {
  BarelyEngine_DestroyInstrument(BarelyEngine::get_singleton()->get(), instrument_id_);
  instrument_id_ = 0;
}

void BarelyInstrument::set_note_off(float pitch) {
  BarelyInstrument_SetNoteOff(BarelyEngine::get_singleton()->get(), instrument_id_, pitch);
}

void BarelyInstrument::set_note_on(float pitch) {
  BarelyInstrument_SetNoteOn(BarelyEngine::get_singleton()->get(), instrument_id_, pitch);
}

void BarelyInstrument::_ready() {
  ::BarelyEngine* engine = BarelyEngine::get_singleton()->get();
  BarelyEngine_CreateInstrument(engine, &instrument_id_);
  // TODO(#181): Expose controls.
  BarelyInstrument_SetControl(engine, instrument_id_, BarelyInstrumentControlType_kOscMix, 0.5f);
}

void BarelyInstrument::_process([[maybe_unused]] double delta) {}

void BarelyInstrument::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_note_off", "pitch"), &BarelyInstrument::set_note_off);
  ClassDB::bind_method(D_METHOD("set_note_on", "pitch"), &BarelyInstrument::set_note_on);
}

}  // namespace barely::godot
