#ifndef BARELYMUSICIAN_GODOT_INSTRUMENT_H_
#define BARELYMUSICIAN_GODOT_INSTRUMENT_H_

#include <barelymusician.h>

#include <cstdint>
#include <unordered_set>
#include <vector>

#include "godot/engine.h"
#include "godot/resources.h"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/node2d.hpp"
#include "godot_cpp/classes/node3d.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/typed_array.hpp"

namespace barely::godot {

#define BARELY_GODOT_INSTRUMENT_CONTROLS(ClassName, X)               \
  /*X(ClassName, Gain, gain, float, 1.0f)*/                          \
  X(ClassName, PitchShift, pitch_shift, float, 0.0f)                 \
  /*X(ClassName, StereoPan, stereo_pan, float, 0.0f)*/               \
  X(ClassName, Attack, attack, float, 0.05f)                         \
  X(ClassName, Decay, decay, float, 0.0f)                            \
  X(ClassName, Sustain, sustain, float, 1.0f)                        \
  X(ClassName, Release, release, float, 0.2f)                        \
  X(ClassName, SliceMode, slice_mode, int, BarelySliceMode_kSustain) \
  X(ClassName, OscMix, osc_mix, float, 1.0f)                         \
  X(ClassName, OscMode, osc_mode, int, BarelyOscMode_kCrossfade)     \
  X(ClassName, OscNoiseMix, osc_noise_mix, float, 0.0f)              \
  X(ClassName, OscPitchShift, osc_pitch_shift, float, 0.0f)          \
  X(ClassName, OscShape, osc_shape, float, 0.0f)                     \
  X(ClassName, OscSkew, osc_skew, float, 0.0f)                       \
  X(ClassName, CrushDepth, crush_depth, float, 0.0f)                 \
  X(ClassName, CrushRate, crush_rate, float, 0.0f)                   \
  X(ClassName, DistortionMix, distortion_mix, float, 0.0f)           \
  X(ClassName, DistortionDrive, distortion_drive, float, 0.0f)       \
  X(ClassName, FilterCutoff, filter_cutoff, float, 1.0f)             \
  X(ClassName, FilterResonance, filter_resonance, float, 0.5f)       \
  X(ClassName, FilterTone, filter_tone, float, 0.0f)                 \
  X(ClassName, DelaySend, delay_send, float, 0.0f)                   \
  X(ClassName, ReverbSend, reverb_send, float, 0.0f)                 \
  X(ClassName, SidechainSend, sidechain_send, float, 0.0f)           \
  X(ClassName, Retrigger, retrigger, bool, false)                    \
  X(ClassName, VoiceCount, voice_count, int, 8)

#define BARELY_DECLARE_GODOT_INSTRUMENT_CONTROL(ClassName, Name, name, type, default)              \
 private:                                                                                          \
  type name##_ = default;                                                                          \
                                                                                                   \
 public:                                                                                           \
  void set_##name(type name) {                                                                     \
    if (name##_ == name) return;                                                                   \
    name##_ = name;                                                                                \
    BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), state_.get_instrument_id(),  \
                                BarelyInstrumentControlType_k##Name, static_cast<float>(name##_)); \
  }                                                                                                \
  type get_##name() const { return name##_; }

#define BARELY_DECLARE_GODOT_INSTRUMENT(ClassName, NodeName)                              \
 public:                                                                                  \
  ClassName();                                                                            \
  void on_slice_changed() { state_.on_slice_changed(); }                                  \
  void set_all_notes_off() { state_.set_all_notes_off(); }                                \
  void set_note_off(float pitch) { state_.set_note_off(pitch); }                          \
  void set_note_on(float pitch, float gain = 1.0f, float pitch_shift = 0.0f) {            \
    state_.set_note_on(pitch, gain, pitch_shift);                                         \
  }                                                                                       \
  void set_slices(const ::godot::TypedArray<::godot::Ref<BarelySliceResource>>& slices) { \
    state_.set_slices(slices);                                                            \
  }                                                                                       \
  ::godot::TypedArray<::godot::Ref<BarelySliceResource>> get_slices() const {             \
    return state_.get_slices();                                                           \
  };                                                                                      \
  bool is_note_on(float pitch) const { return state_.is_note_on(pitch); }                 \
                                                                                          \
 private:                                                                                 \
  GDCLASS(ClassName, NodeName);                                                           \
  static void _bind_methods();                                                            \
  BarelyInstrumentState state_;                                                           \
  BARELY_GODOT_INSTRUMENT_CONTROLS(ClassName, BARELY_DECLARE_GODOT_INSTRUMENT_CONTROL);

struct BarelyInstrumentState {
 public:
  explicit BarelyInstrumentState(::godot::Node& instrument);
  ~BarelyInstrumentState();

  void on_slice_changed();
  void set_all_notes_off();
  void set_note_off(float pitch);
  void set_note_on(float pitch, float gain, float pitch_shift);
  void set_slices(const ::godot::TypedArray<::godot::Ref<BarelySliceResource>>& slices);
  uint32_t get_instrument_id() const { return instrument_id_; }
  ::godot::TypedArray<::godot::Ref<BarelySliceResource>> get_slices() const { return slices_; };
  bool is_note_on(float pitch) const { return pitches_.contains(pitch); }

  ::godot::Node* instrument_ = nullptr;
  uint32_t instrument_id_ = 0;
  ::godot::TypedArray<::godot::Ref<BarelySliceResource>> slices_;
  // TODO(#181): Remove heap allocations.
  std::vector<std::vector<float>> slice_buffers_;
  std::unordered_set<float> pitches_;
};

class BarelyInstrument : public ::godot::Node {
  BARELY_DECLARE_GODOT_INSTRUMENT(BarelyInstrument, ::godot::Node);
  BARELY_DECLARE_GODOT_INSTRUMENT_CONTROL(BarelyInstrument, Gain, gain, float, 1.0f);
  BARELY_DECLARE_GODOT_INSTRUMENT_CONTROL(BarelyInstrument, StereoPan, stereo_pan, float, 0.0f);
};

class BarelyInstrument2D : public ::godot::Node2D {
  BARELY_DECLARE_GODOT_INSTRUMENT(BarelyInstrument2D, ::godot::Node2D);

 public:
  void _process(double delta) override;

  void set_gain(float gain) { gain_ = gain; }
  void set_max_distance(float max_distance) { max_distance_ = max_distance; }
  void set_attenuation(float attenuation) { attenuation_ = attenuation; }
  void set_panning_strength(float panning_strength) { panning_strength_ = panning_strength; }
  float get_gain() const { return gain_; }
  float get_max_distance() const { return max_distance_; }
  float get_attenuation() const { return attenuation_; }
  float get_panning_strength() const { return panning_strength_; }

 private:
  float gain_ = 1.0f;
  float max_distance_ = 2000.0f;
  float attenuation_ = 1.0f;
  float panning_strength_ = 1.0f;
  float spatial_gain_ = 1.0f;
  float spatial_pan_ = 0.0f;
};

class BarelyInstrument3D : public ::godot::Node3D {
  BARELY_DECLARE_GODOT_INSTRUMENT(BarelyInstrument3D, ::godot::Node3D);

 public:
  void _process(double delta) override;

  void set_gain(float gain) { gain_ = gain; }
  void set_attenuation_model(int attenuation_model) { attenuation_model_ = attenuation_model; }
  void set_unit_size(float unit_size) { unit_size_ = unit_size; }
  void set_max_distance(float max_distance) { max_distance_ = max_distance; }
  void set_panning_strength(float panning_strength) { panning_strength_ = panning_strength; }
  float get_gain() const { return gain_; }
  int get_attenuation_model() const { return attenuation_model_; }
  float get_unit_size() const { return unit_size_; }
  float get_max_distance() const { return max_distance_; }
  float get_panning_strength() const { return panning_strength_; }

 private:
  float gain_ = 1.0f;
  int attenuation_model_ = 0;
  float unit_size_ = 10.0f;
  float max_distance_ = 0.0f;
  float panning_strength_ = 1.0f;
  float spatial_gain_ = 1.0f;
  float spatial_pan_ = 0.0f;
};

}  // namespace barely::godot

#endif  // BARELYMUSICIAN_GODOT_INSTRUMENT_H_
