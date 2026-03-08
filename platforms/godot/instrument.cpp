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

void BarelyInstrument::set_note_on(float pitch) {
  BarelyInstrument_SetNoteOn(BarelyEngine::get_singleton()->get(), instrument_id_, pitch);
}

bool BarelyInstrument::is_note_on(float pitch) const {
  bool is_note_on = false;
  BarelyInstrument_IsNoteOn(BarelyEngine::get_singleton()->get(), instrument_id_, pitch,
                            &is_note_on);
  return is_note_on;
}

void BarelyInstrument::set_gain(float gain) {
  gain_ = gain;
  BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_,
                              BarelyInstrumentControlType_kGain, gain_);
}

void BarelyInstrument::set_pitch_shift(float pitch_shift) {
  pitch_shift_ = pitch_shift;
  BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_,
                              BarelyInstrumentControlType_kPitchShift, pitch_shift_);
}

void BarelyInstrument::set_stereo_pan(float stereo_pan) {
  stereo_pan_ = stereo_pan;
  BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_,
                              BarelyInstrumentControlType_kStereoPan, stereo_pan_);
}

void BarelyInstrument::set_attack(float attack) {
  attack_ = attack;
  BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_,
                              BarelyInstrumentControlType_kAttack, attack_);
}

void BarelyInstrument::set_decay(float decay) {
  decay_ = decay;
  BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_,
                              BarelyInstrumentControlType_kDecay, decay_);
}

void BarelyInstrument::set_sustain(float sustain) {
  sustain_ = sustain;
  BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_,
                              BarelyInstrumentControlType_kSustain, sustain_);
}

void BarelyInstrument::set_release(float release) {
  release_ = release;
  BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_,
                              BarelyInstrumentControlType_kRelease, release_);
}

void BarelyInstrument::set_slice_mode(int slice_mode) {
  slice_mode_ = slice_mode;
  BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_,
                              BarelyInstrumentControlType_kSliceMode,
                              static_cast<float>(slice_mode_));
}

void BarelyInstrument::set_osc_mix(float osc_mix) {
  osc_mix_ = osc_mix;
  BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_,
                              BarelyInstrumentControlType_kOscMix, osc_mix_);
}

void BarelyInstrument::set_osc_mode(int osc_mode) {
  osc_mode_ = osc_mode;
  BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_,
                              BarelyInstrumentControlType_kOscMode, static_cast<float>(osc_mode_));
}

void BarelyInstrument::set_osc_noise_mix(float osc_noise_mix) {
  osc_noise_mix_ = osc_noise_mix;
  BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_,
                              BarelyInstrumentControlType_kOscNoiseMix, osc_noise_mix_);
}

void BarelyInstrument::set_osc_pitch_shift(float osc_pitch_shift) {
  osc_pitch_shift_ = osc_pitch_shift;
  BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_,
                              BarelyInstrumentControlType_kOscPitchShift, osc_pitch_shift_);
}

void BarelyInstrument::set_osc_shape(float osc_shape) {
  osc_shape_ = osc_shape;
  BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_,
                              BarelyInstrumentControlType_kOscShape, osc_shape_);
}

void BarelyInstrument::set_osc_skew(float osc_skew) {
  osc_skew_ = osc_skew;
  BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_,
                              BarelyInstrumentControlType_kOscSkew, osc_skew_);
}

void BarelyInstrument::set_crush_depth(float crush_depth) {
  crush_depth_ = crush_depth;
  BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_,
                              BarelyInstrumentControlType_kCrushDepth, crush_depth_);
}

void BarelyInstrument::set_crush_rate(float crush_rate) {
  crush_rate_ = crush_rate;
  BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_,
                              BarelyInstrumentControlType_kCrushRate, crush_rate_);
}

void BarelyInstrument::set_distortion_mix(float distortion_mix) {
  distortion_mix_ = distortion_mix;
  BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_,
                              BarelyInstrumentControlType_kDistortionMix, distortion_mix_);
}

void BarelyInstrument::set_distortion_drive(float distortion_drive) {
  distortion_drive_ = distortion_drive;
  BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_,
                              BarelyInstrumentControlType_kDistortionDrive, distortion_drive_);
}

void BarelyInstrument::set_filter_cutoff(float filter_cutoff) {
  filter_cutoff_ = filter_cutoff;
  BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_,
                              BarelyInstrumentControlType_kFilterCutoff, filter_cutoff_);
}

void BarelyInstrument::set_filter_resonance(float filter_resonance) {
  filter_resonance_ = filter_resonance;
  BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_,
                              BarelyInstrumentControlType_kFilterResonance, filter_resonance_);
}

void BarelyInstrument::set_filter_tone(float filter_tone) {
  filter_tone_ = filter_tone;
  BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_,
                              BarelyInstrumentControlType_kFilterTone, filter_tone_);
}

void BarelyInstrument::set_delay_send(float delay_send) {
  delay_send_ = delay_send;
  BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_,
                              BarelyInstrumentControlType_kDelaySend, delay_send_);
}

void BarelyInstrument::set_reverb_send(float reverb_send) {
  reverb_send_ = reverb_send;
  BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_,
                              BarelyInstrumentControlType_kReverbSend, reverb_send_);
}

void BarelyInstrument::set_sidechain_send(float sidechain_send) {
  sidechain_send_ = sidechain_send;
  BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_,
                              BarelyInstrumentControlType_kSidechainSend, sidechain_send_);
}

void BarelyInstrument::set_arp_mode(int arp_mode) {
  arp_mode_ = arp_mode;
  BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_,
                              BarelyInstrumentControlType_kArpMode, static_cast<float>(arp_mode_));
}

void BarelyInstrument::set_arp_gate(float arp_gate) {
  arp_gate_ = arp_gate;
  BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_,
                              BarelyInstrumentControlType_kArpGate, arp_gate_);
}

void BarelyInstrument::set_arp_rate(float arp_rate) {
  arp_rate_ = arp_rate;
  BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_,
                              BarelyInstrumentControlType_kArpRate, arp_rate_);
}

void BarelyInstrument::set_retrigger(bool retrigger) {
  retrigger_ = retrigger;
  BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_,
                              BarelyInstrumentControlType_kRetrigger, retrigger_ ? 1.0f : 0.0f);
}

void BarelyInstrument::set_voice_count(int voice_count) {
  voice_count_ = voice_count;
  BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_,
                              BarelyInstrumentControlType_kVoiceCount,
                              static_cast<float>(voice_count_));
}

void BarelyInstrument::_ready() {
  ::BarelyEngine* engine = BarelyEngine::get_singleton()->get();
  BarelyEngine_CreateInstrument(engine, &instrument_id_);
}

void BarelyInstrument::_process([[maybe_unused]] double delta) {}

void BarelyInstrument::_bind_methods() {
  ClassDB::bind_integer_constant(get_class_static(), StringName(), "ARP_MODE_NONE",
                                 BarelyArpMode_kNone);
  ClassDB::bind_integer_constant(get_class_static(), StringName(), "ARP_MODE_UP",
                                 BarelyArpMode_kUp);
  ClassDB::bind_integer_constant(get_class_static(), StringName(), "ARP_MODE_DOWN",
                                 BarelyArpMode_kDown);
  ClassDB::bind_integer_constant(get_class_static(), StringName(), "ARP_MODE_RANDOM",
                                 BarelyArpMode_kRandom);

  ClassDB::bind_integer_constant(get_class_static(), StringName(), "OSC_MODE_CROSSFADE",
                                 BarelyOscMode_kCrossfade);
  ClassDB::bind_integer_constant(get_class_static(), StringName(), "OSC_MODE_AM",
                                 BarelyOscMode_kAm);
  ClassDB::bind_integer_constant(get_class_static(), StringName(), "OSC_MODE_FM",
                                 BarelyOscMode_kFm);
  ClassDB::bind_integer_constant(get_class_static(), StringName(), "OSC_MODE_MA",
                                 BarelyOscMode_kMa);
  ClassDB::bind_integer_constant(get_class_static(), StringName(), "OSC_MODE_MF",
                                 BarelyOscMode_kMf);
  ClassDB::bind_integer_constant(get_class_static(), StringName(), "OSC_MODE_RING",
                                 BarelyOscMode_kRing);

  ClassDB::bind_integer_constant(get_class_static(), StringName(), "SLICE_MODE_SUSTAIN",
                                 BarelySliceMode_kSustain);
  ClassDB::bind_integer_constant(get_class_static(), StringName(), "SLICE_MODE_ONCE",
                                 BarelySliceMode_kOnce);
  ClassDB::bind_integer_constant(get_class_static(), StringName(), "SLICE_MODE_LOOP",
                                 BarelySliceMode_kLoop);

  ClassDB::bind_method(D_METHOD("set_all_notes_off"), &BarelyInstrument::set_all_notes_off);
  ClassDB::bind_method(D_METHOD("set_note_off", "pitch"), &BarelyInstrument::set_note_off);
  ClassDB::bind_method(D_METHOD("set_note_on", "pitch"), &BarelyInstrument::set_note_on);
  ClassDB::bind_method(D_METHOD("is_note_on", "pitch"), &BarelyInstrument::is_note_on);

  ClassDB::bind_method(D_METHOD("set_gain", "gain"), &BarelyInstrument::set_gain);
  ClassDB::bind_method(D_METHOD("get_gain"), &BarelyInstrument::get_gain);

  ClassDB::bind_method(D_METHOD("set_pitch_shift", "pitch_shift"),
                       &BarelyInstrument::set_pitch_shift);
  ClassDB::bind_method(D_METHOD("get_pitch_shift"), &BarelyInstrument::get_pitch_shift);

  ClassDB::bind_method(D_METHOD("set_stereo_pan", "stereo_pan"), &BarelyInstrument::set_stereo_pan);
  ClassDB::bind_method(D_METHOD("get_stereo_pan"), &BarelyInstrument::get_stereo_pan);

  ClassDB::bind_method(D_METHOD("set_attack", "attack"), &BarelyInstrument::set_attack);
  ClassDB::bind_method(D_METHOD("get_attack"), &BarelyInstrument::get_attack);

  ClassDB::bind_method(D_METHOD("set_decay", "decay"), &BarelyInstrument::set_decay);
  ClassDB::bind_method(D_METHOD("get_decay"), &BarelyInstrument::get_decay);

  ClassDB::bind_method(D_METHOD("set_sustain", "sustain"), &BarelyInstrument::set_sustain);
  ClassDB::bind_method(D_METHOD("get_sustain"), &BarelyInstrument::get_sustain);

  ClassDB::bind_method(D_METHOD("set_release", "release"), &BarelyInstrument::set_release);
  ClassDB::bind_method(D_METHOD("get_release"), &BarelyInstrument::get_release);

  ClassDB::bind_method(D_METHOD("set_slice_mode", "slice_mode"), &BarelyInstrument::set_slice_mode);
  ClassDB::bind_method(D_METHOD("get_slice_mode"), &BarelyInstrument::get_slice_mode);

  ClassDB::bind_method(D_METHOD("set_osc_mix", "osc_mix"), &BarelyInstrument::set_osc_mix);
  ClassDB::bind_method(D_METHOD("get_osc_mix"), &BarelyInstrument::get_osc_mix);

  ClassDB::bind_method(D_METHOD("set_osc_mode", "osc_mode"), &BarelyInstrument::set_osc_mode);
  ClassDB::bind_method(D_METHOD("get_osc_mode"), &BarelyInstrument::get_osc_mode);

  ClassDB::bind_method(D_METHOD("set_osc_noise_mix", "osc_noise_mix"),
                       &BarelyInstrument::set_osc_noise_mix);
  ClassDB::bind_method(D_METHOD("get_osc_noise_mix"), &BarelyInstrument::get_osc_noise_mix);

  ClassDB::bind_method(D_METHOD("set_osc_pitch_shift", "osc_pitch_shift"),
                       &BarelyInstrument::set_osc_pitch_shift);
  ClassDB::bind_method(D_METHOD("get_osc_pitch_shift"), &BarelyInstrument::get_osc_pitch_shift);

  ClassDB::bind_method(D_METHOD("set_osc_shape", "osc_shape"), &BarelyInstrument::set_osc_shape);
  ClassDB::bind_method(D_METHOD("get_osc_shape"), &BarelyInstrument::get_osc_shape);

  ClassDB::bind_method(D_METHOD("set_osc_skew", "osc_skew"), &BarelyInstrument::set_osc_skew);
  ClassDB::bind_method(D_METHOD("get_osc_skew"), &BarelyInstrument::get_osc_skew);

  ClassDB::bind_method(D_METHOD("set_crush_depth", "crush_depth"),
                       &BarelyInstrument::set_crush_depth);
  ClassDB::bind_method(D_METHOD("get_crush_depth"), &BarelyInstrument::get_crush_depth);

  ClassDB::bind_method(D_METHOD("set_crush_rate", "crush_rate"), &BarelyInstrument::set_crush_rate);
  ClassDB::bind_method(D_METHOD("get_crush_rate"), &BarelyInstrument::get_crush_rate);

  ClassDB::bind_method(D_METHOD("set_distortion_mix", "distortion_mix"),
                       &BarelyInstrument::set_distortion_mix);
  ClassDB::bind_method(D_METHOD("get_distortion_mix"), &BarelyInstrument::get_distortion_mix);

  ClassDB::bind_method(D_METHOD("set_distortion_drive", "distortion_drive"),
                       &BarelyInstrument::set_distortion_drive);
  ClassDB::bind_method(D_METHOD("get_distortion_drive"), &BarelyInstrument::get_distortion_drive);

  ClassDB::bind_method(D_METHOD("set_filter_cutoff", "filter_cutoff"),
                       &BarelyInstrument::set_filter_cutoff);
  ClassDB::bind_method(D_METHOD("get_filter_cutoff"), &BarelyInstrument::get_filter_cutoff);

  ClassDB::bind_method(D_METHOD("set_filter_resonance", "filter_resonance"),
                       &BarelyInstrument::set_filter_resonance);
  ClassDB::bind_method(D_METHOD("get_filter_resonance"), &BarelyInstrument::get_filter_resonance);

  ClassDB::bind_method(D_METHOD("set_filter_tone", "filter_tone"),
                       &BarelyInstrument::set_filter_tone);
  ClassDB::bind_method(D_METHOD("get_filter_tone"), &BarelyInstrument::get_filter_tone);

  ClassDB::bind_method(D_METHOD("set_delay_send", "delay_send"), &BarelyInstrument::set_delay_send);
  ClassDB::bind_method(D_METHOD("get_delay_send"), &BarelyInstrument::get_delay_send);

  ClassDB::bind_method(D_METHOD("set_reverb_send", "reverb_send"),
                       &BarelyInstrument::set_reverb_send);
  ClassDB::bind_method(D_METHOD("get_reverb_send"), &BarelyInstrument::get_reverb_send);

  ClassDB::bind_method(D_METHOD("set_sidechain_send", "sidechain_send"),
                       &BarelyInstrument::set_sidechain_send);
  ClassDB::bind_method(D_METHOD("get_sidechain_send"), &BarelyInstrument::get_sidechain_send);

  ClassDB::bind_method(D_METHOD("set_arp_mode", "arp_mode"), &BarelyInstrument::set_arp_mode);
  ClassDB::bind_method(D_METHOD("get_arp_mode"), &BarelyInstrument::get_arp_mode);

  ClassDB::bind_method(D_METHOD("set_arp_gate", "arp_gate"), &BarelyInstrument::set_arp_gate);
  ClassDB::bind_method(D_METHOD("get_arp_gate"), &BarelyInstrument::get_arp_gate);

  ClassDB::bind_method(D_METHOD("set_arp_rate", "arp_rate"), &BarelyInstrument::set_arp_rate);
  ClassDB::bind_method(D_METHOD("get_arp_rate"), &BarelyInstrument::get_arp_rate);

  ClassDB::bind_method(D_METHOD("set_retrigger", "retrigger"), &BarelyInstrument::set_retrigger);
  ClassDB::bind_method(D_METHOD("get_retrigger"), &BarelyInstrument::get_retrigger);

  ClassDB::bind_method(D_METHOD("set_voice_count", "voice_count"),
                       &BarelyInstrument::set_voice_count);
  ClassDB::bind_method(D_METHOD("get_voice_count"), &BarelyInstrument::get_voice_count);

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
