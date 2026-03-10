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
using ::godot::StringName;
using ::godot::Variant;

#define BARELY_BIND_GODOT_ENUM_VALUE(EnumType, Name, Value)                \
  ClassDB::bind_integer_constant(get_class_static(), StringName(), #Value, \
                                 Barely##EnumType##_k##Name);
#define BARELY_BIND_GODOT_INSTRUMENT_CONTROL(Name, name, ...)                                   \
  ClassDB::bind_method(D_METHOD(BARELY_STR(set_##name), #name), &BarelyInstrument::set_##name); \
  ClassDB::bind_method(D_METHOD(BARELY_STR(get_##name)), &BarelyInstrument::get_##name);
#define BARELY_SET_DEFAULT_GODOT_INSTRUMENT_CONTROL(Name, name, type, default) set_##name(default);

BarelyInstrument::BarelyInstrument() {
  BarelyEngine_CreateInstrument(BarelyEngine::get_singleton()->get(), &instrument_id_);
  BARELY_GODOT_INSTRUMENT_CONTROLS(BARELY_SET_DEFAULT_GODOT_INSTRUMENT_CONTROL);
}

BarelyInstrument::~BarelyInstrument() {
  BarelyEngine_DestroyInstrument(BarelyEngine::get_singleton()->get(), instrument_id_);
  instrument_id_ = 0;
}

void BarelyInstrument::set_all_notes_off() {
  BarelyInstrument_SetAllNotesOff(BarelyEngine::get_singleton()->get(), instrument_id_);
}

void BarelyInstrument::set_note_off(float pitch) {
  BarelyInstrument_SetNoteOff(BarelyEngine::get_singleton()->get(), instrument_id_, pitch);
}

void BarelyInstrument::set_note_on(float pitch, float gain, float pitch_shift) {
  ::BarelyEngine* engine = BarelyEngine::get_singleton()->get();
  BarelyInstrument_SetNoteOn(engine, instrument_id_, pitch);
  if (gain != 1.0f) {
    BarelyInstrument_SetNoteControl(engine, instrument_id_, pitch, BarelyNoteControlType_kGain,
                                    gain);
  }
  if (pitch_shift != 0.0f) {
    BarelyInstrument_SetNoteControl(engine, instrument_id_, pitch,
                                    BarelyNoteControlType_kPitchShift, pitch_shift);
  }
}

bool BarelyInstrument::is_note_on(float pitch) const {
  bool is_note_on = false;
  BarelyInstrument_IsNoteOn(BarelyEngine::get_singleton()->get(), instrument_id_, pitch,
                            &is_note_on);
  return is_note_on;
}

void BarelyInstrument::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_all_notes_off"), &BarelyInstrument::set_all_notes_off);
  ClassDB::bind_method(D_METHOD("set_note_off", "pitch"), &BarelyInstrument::set_note_off);
  ClassDB::bind_method(D_METHOD("set_note_on", "pitch", "gain", "pitch_shift"),
                       &BarelyInstrument::set_note_on, DEFVAL(1.0f), DEFVAL(0.0f));
  ClassDB::bind_method(D_METHOD("is_note_on", "pitch"), &BarelyInstrument::is_note_on);

  BARELY_BIND_GODOT_ENUM_VALUE(ArpMode, None, "ARP_MODE_NONE");
  BARELY_BIND_GODOT_ENUM_VALUE(ArpMode, Up, "ARP_MODE_UP");
  BARELY_BIND_GODOT_ENUM_VALUE(ArpMode, Down, "ARP_MODE_DOWN");
  BARELY_BIND_GODOT_ENUM_VALUE(ArpMode, Random, "ARP_MODE_RANDOM");

  BARELY_BIND_GODOT_ENUM_VALUE(OscMode, Crossfade, "OSC_MODE_CROSSFADE");
  BARELY_BIND_GODOT_ENUM_VALUE(OscMode, Am, "OSC_MODE_AM");
  BARELY_BIND_GODOT_ENUM_VALUE(OscMode, Fm, "OSC_MODE_FM");
  BARELY_BIND_GODOT_ENUM_VALUE(OscMode, Ma, "OSC_MODE_MA");
  BARELY_BIND_GODOT_ENUM_VALUE(OscMode, Mf, "OSC_MODE_MF");
  BARELY_BIND_GODOT_ENUM_VALUE(OscMode, Ring, "OSC_MODE_RING");

  BARELY_BIND_GODOT_ENUM_VALUE(SliceMode, Sustain, "SLICE_MODE_SUSTAIN");
  BARELY_BIND_GODOT_ENUM_VALUE(SliceMode, Once, "SLICE_MODE_ONCE");
  BARELY_BIND_GODOT_ENUM_VALUE(SliceMode, Loop, "SLICE_MODE_LOOP");

  BARELY_GODOT_INSTRUMENT_CONTROLS(BARELY_BIND_GODOT_INSTRUMENT_CONTROL);

  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "gain", PropertyHint::PROPERTY_HINT_RANGE, "0,1,0.01"),
               "set_gain", "get_gain");
  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "pitch_shift", PropertyHint::PROPERTY_HINT_RANGE, "-2,2,0.01"),
      "set_pitch_shift", "get_pitch_shift");
  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "stereo_pan", PropertyHint::PROPERTY_HINT_RANGE, "-1,1,0.01"),
      "set_stereo_pan", "get_stereo_pan");
  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "attack", PropertyHint::PROPERTY_HINT_RANGE, "0,8,0.01"),
      "set_attack", "get_attack");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "decay", PropertyHint::PROPERTY_HINT_RANGE, "0,8,0.01"),
               "set_decay", "get_decay");
  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "sustain", PropertyHint::PROPERTY_HINT_RANGE, "0,1,0.01"),
      "set_sustain", "get_sustain");
  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "release", PropertyHint::PROPERTY_HINT_RANGE, "0,8,0.01"),
      "set_release", "get_release");
  ADD_PROPERTY(PropertyInfo(Variant::INT, "slice_mode", PropertyHint::PROPERTY_HINT_ENUM,
                            "Sustain,Loop,Once"),
               "set_slice_mode", "get_slice_mode");
  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "osc_mix", PropertyHint::PROPERTY_HINT_RANGE, "0,1,0.01"),
      "set_osc_mix", "get_osc_mix");
  ADD_PROPERTY(PropertyInfo(Variant::INT, "osc_mode", PropertyHint::PROPERTY_HINT_ENUM,
                            "Crossfade,AM,FM,MA,MF,Ring"),
               "set_osc_mode", "get_osc_mode");
  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "osc_noise_mix", PropertyHint::PROPERTY_HINT_RANGE, "0,1,0.01"),
      "set_osc_noise_mix", "get_osc_noise_mix");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "osc_pitch_shift"), "set_osc_pitch_shift",
               "get_osc_pitch_shift");
  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "osc_shape", PropertyHint::PROPERTY_HINT_RANGE, "0,1,0.01"),
      "set_osc_shape", "get_osc_shape");
  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "osc_skew", PropertyHint::PROPERTY_HINT_RANGE, "-1,1,0.01"),
      "set_osc_skew", "get_osc_skew");
  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "crush_depth", PropertyHint::PROPERTY_HINT_RANGE, "0,1,0.01"),
      "set_crush_depth", "get_crush_depth");
  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "crush_rate", PropertyHint::PROPERTY_HINT_RANGE, "0,1,0.01"),
      "set_crush_rate", "get_crush_rate");
  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "distortion_mix", PropertyHint::PROPERTY_HINT_RANGE, "0,1,0.01"),
      "set_distortion_mix", "get_distortion_mix");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "distortion_drive", PropertyHint::PROPERTY_HINT_RANGE,
                            "0,1,0.01"),
               "set_distortion_drive", "get_distortion_drive");
  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "filter_cutoff", PropertyHint::PROPERTY_HINT_RANGE, "0,1,0.01"),
      "set_filter_cutoff", "get_filter_cutoff");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "filter_resonance", PropertyHint::PROPERTY_HINT_RANGE,
                            "0,1,0.01"),
               "set_filter_resonance", "get_filter_resonance");
  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "filter_tone", PropertyHint::PROPERTY_HINT_RANGE, "-1,1,0.01"),
      "set_filter_tone", "get_filter_tone");
  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "delay_send", PropertyHint::PROPERTY_HINT_RANGE, "0,1,0.01"),
      "set_delay_send", "get_delay_send");
  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "reverb_send", PropertyHint::PROPERTY_HINT_RANGE, "0,2,0.01"),
      "set_reverb_send", "get_reverb_send");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "sidechain_send", PropertyHint::PROPERTY_HINT_RANGE,
                            "-1,1,0.01"),
               "set_sidechain_send", "get_sidechain_send");
  ADD_PROPERTY(PropertyInfo(Variant::INT, "arp_mode", PropertyHint::PROPERTY_HINT_ENUM,
                            "None,Up,Down,Random"),
               "set_arp_mode", "get_arp_mode");
  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "arp_gate", PropertyHint::PROPERTY_HINT_RANGE, "0,1,0.01"),
      "set_arp_gate", "get_arp_gate");
  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "arp_rate", PropertyHint::PROPERTY_HINT_RANGE, "0,16,0.01"),
      "set_arp_rate", "get_arp_rate");
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "retrigger"), "set_retrigger", "get_retrigger");
  ADD_PROPERTY(PropertyInfo(Variant::INT, "voice_count", PropertyHint::PROPERTY_HINT_RANGE, "1,16"),
               "set_voice_count", "get_voice_count");
}

}  // namespace barely::godot
