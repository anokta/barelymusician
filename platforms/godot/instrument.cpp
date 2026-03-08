#include "godot/instrument.h"

#include <barelymusician.h>

#include "godot/engine.h"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/object.hpp"

namespace barely::godot {

using ::godot::ClassDB;
using ::godot::D_METHOD;
using ::godot::PropertyHint;
using ::godot::PropertyInfo;
using ::godot::Variant;

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

void BarelyInstrument::set_gain(float gain) {
  gain_ = gain;
  BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_,
                              BarelyInstrumentControlType_kGain, gain_);
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

  ClassDB::bind_method(D_METHOD("set_gain", "gain"), &BarelyInstrument::set_gain);
  ClassDB::bind_method(D_METHOD("get_gain"), &BarelyInstrument::get_gain);

  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "gain", PropertyHint::PROPERTY_HINT_RANGE, "0,1,0.01"),
               "set_gain", "get_gain");
}

}  // namespace barely::godot
