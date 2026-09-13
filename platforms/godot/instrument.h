#ifndef BARELYMUSICIAN_GODOT_INSTRUMENT_H_
#define BARELYMUSICIAN_GODOT_INSTRUMENT_H_

#include <barelymusician.h>

#include <cstdint>
#include <unordered_set>
#include <vector>

#include "godot/engine.h"
#include "godot_cpp/classes/audio_stream_wav.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/typed_array.hpp"

namespace barely::godot {

#define BARELY_GODOT_INSTRUMENT_CONTROLS(X)               \
  X(Gain, gain, double, 1.0)                              \
  X(PitchShift, pitch_shift, double, 0.0)                 \
  X(StereoPan, stereo_pan, double, 0.0)                   \
  X(Attack, attack, double, 0.05)                         \
  X(Decay, decay, double, 0.0)                            \
  X(Sustain, sustain, double, 1.0)                        \
  X(Release, release, double, 0.2)                        \
  X(SliceMode, slice_mode, int, BarelySliceMode_kSustain) \
  X(OscMix, osc_mix, double, 1.0)                         \
  X(OscMode, osc_mode, int, BarelyOscMode_kCrossfade)     \
  X(OscNoiseMix, osc_noise_mix, double, 0.0)              \
  X(OscPitchShift, osc_pitch_shift, double, 0.0)          \
  X(OscShape, osc_shape, double, 0.0)                     \
  X(OscSkew, osc_skew, double, 0.0)                       \
  X(CrushDepth, crush_depth, double, 0.0)                 \
  X(CrushRate, crush_rate, double, 0.0)                   \
  X(DistortionMix, distortion_mix, double, 0.0)           \
  X(DistortionDrive, distortion_drive, double, 0.0)       \
  X(FilterCutoff, filter_cutoff, double, 1.0)             \
  X(FilterResonance, filter_resonance, double, 0.5)       \
  X(FilterTone, filter_tone, double, 0.0)                 \
  X(DelaySend, delay_send, double, 0.0)                   \
  X(ReverbSend, reverb_send, double, 0.0)                 \
  X(SidechainSend, sidechain_send, double, 0.0)           \
  X(Retrigger, retrigger, bool, false)                    \
  X(VoiceCount, voice_count, int, 8)

#define BARELY_DECLARE_GODOT_INSTRUMENT_CONTROL(Name, name, type, default)            \
 private:                                                                             \
  type name##_ = default;                                                             \
                                                                                      \
 public:                                                                              \
  void set_##name(type name) {                                                        \
    if (name##_ == name) return;                                                      \
    name##_ = name;                                                                   \
    BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_, \
                                BarelyInstrumentControlType_k##Name,                  \
                                static_cast<double>(name##_));                        \
  }                                                                                   \
  type get_##name() const { return name##_; }

class BarelySliceResource : public ::godot::Resource {
 public:
  void set_stream(const ::godot::Ref<::godot::AudioStreamWAV>& stream);
  void set_root_pitch(double root_pitch);

  ::godot::Ref<::godot::AudioStreamWAV> get_stream() const { return stream_; }
  double get_root_pitch() const { return root_pitch_; }

 private:
  GDCLASS(BarelySliceResource, ::godot::Resource);
  static void _bind_methods();

  ::godot::Ref<::godot::AudioStreamWAV> stream_;
  double root_pitch_ = 0.0;
};

class BarelyInstrument : public ::godot::Node {
 public:
  BarelyInstrument();
  ~BarelyInstrument();

  void set_all_notes_off();
  void set_note_off(double pitch);
  void set_note_on(double pitch, double gain = 1.0, double pitch_shift = 0.0);
  bool is_note_on(double pitch) const { return pitches_.contains(pitch); }

  void set_slices(const ::godot::TypedArray<::godot::Ref<BarelySliceResource>>& slices);
  ::godot::TypedArray<::godot::Ref<BarelySliceResource>> get_slices() const { return slices_; };

 private:
  GDCLASS(BarelyInstrument, ::godot::Node);
  static void _bind_methods();
  void _on_slice_changed();

  uint32_t instrument_id_ = 0;
  ::godot::TypedArray<::godot::Ref<BarelySliceResource>> slices_;
  // TODO(#181): Remove heap allocations.
  std::vector<std::vector<double>> slice_buffers_;
  std::unordered_set<double> pitches_;

  BARELY_GODOT_INSTRUMENT_CONTROLS(BARELY_DECLARE_GODOT_INSTRUMENT_CONTROL);
};

}  // namespace barely::godot

#endif  // BARELYMUSICIAN_GODOT_INSTRUMENT_H_
