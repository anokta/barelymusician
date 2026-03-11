#ifndef BARELYMUSICIAN_GODOT_INSTRUMENT_H_
#define BARELYMUSICIAN_GODOT_INSTRUMENT_H_

#include <barelymusician.h>

#include <cstdint>

#include "godot/engine.h"
#include "godot_cpp/classes/audio_stream_wav.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/wrapped.hpp"

namespace barely::godot {

#define BARELY_GODOT_INSTRUMENT_CONTROLS(X)               \
  X(Gain, gain, float, 1.0f)                              \
  X(PitchShift, pitch_shift, float, 0.0f)                 \
  X(StereoPan, stereo_pan, float, 0.0f)                   \
  X(Attack, attack, float, 0.05f)                         \
  X(Decay, decay, float, 0.0f)                            \
  X(Sustain, sustain, float, 1.0f)                        \
  X(Release, release, float, 0.1f)                        \
  X(SliceMode, slice_mode, int, BarelySliceMode_kSustain) \
  X(OscMix, osc_mix, float, 1.0f)                         \
  X(OscMode, osc_mode, int, BarelyOscMode_kCrossfade)     \
  X(OscNoiseMix, osc_noise_mix, float, 0.0f)              \
  X(OscPitchShift, osc_pitch_shift, float, 0.0f)          \
  X(OscShape, osc_shape, float, 0.0f)                     \
  X(OscSkew, osc_skew, float, 0.0f)                       \
  X(CrushDepth, crush_depth, float, 0.0f)                 \
  X(CrushRate, crush_rate, float, 0.0f)                   \
  X(DistortionMix, distortion_mix, float, 0.0f)           \
  X(DistortionDrive, distortion_drive, float, 0.0f)       \
  X(FilterCutoff, filter_cutoff, float, 1.0f)             \
  X(FilterResonance, filter_resonance, float, 0.5f)       \
  X(FilterTone, filter_tone, float, 0.0f)                 \
  X(DelaySend, delay_send, float, 0.0f)                   \
  X(ReverbSend, reverb_send, float, 0.0f)                 \
  X(SidechainSend, sidechain_send, float, 0.0f)           \
  X(ArpMode, arp_mode, int, BarelyArpMode_kNone)          \
  X(ArpGate, arp_gate, float, 0.5f)                       \
  X(ArpRate, arp_rate, float, 2.0f)                       \
  X(Retrigger, retrigger, bool, false)                    \
  X(VoiceCount, voice_count, int, 8)

#define BARELY_DECLARE_GODOT_INSTRUMENT_CONTROL(Name, name, type, default)                         \
 private:                                                                                          \
  type name##_;                                                                                    \
                                                                                                   \
 public:                                                                                           \
  void set_##name(type name) {                                                                     \
    name##_ = name;                                                                                \
    BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), instrument_id_,              \
                                BarelyInstrumentControlType_k##Name, static_cast<float>(name##_)); \
  }                                                                                                \
  type get_##name() const { return name##_; }

class BarelySliceResource : public ::godot::Resource {
 public:
  void set_stream(const ::godot::Ref<::godot::AudioStreamWAV>& stream);
  void set_root_pitch(float root_pitch);

  ::godot::Ref<::godot::AudioStreamWAV> get_stream() const { return stream_; }
  float get_root_pitch() const { return root_pitch_; }

 private:
  GDCLASS(BarelySliceResource, ::godot::Resource);
  static void _bind_methods();

  ::godot::Ref<::godot::AudioStreamWAV> stream_;
  float root_pitch_ = 0.0f;
};

class BarelyInstrument : public ::godot::Node {
 public:
  BarelyInstrument();
  ~BarelyInstrument();

  void set_all_notes_off();
  void set_note_off(float pitch);
  void set_note_on(float pitch, float gain = 1.0f, float pitch_shift = 0.0f);
  bool is_note_on(float pitch) const;

  void set_slice(const ::godot::Ref<BarelySliceResource>& slice);
  ::godot::Ref<BarelySliceResource> get_slice() const { return slice_; }

 private:
  GDCLASS(BarelyInstrument, ::godot::Node);
  static void _bind_methods();

  static void _note_event_callback(BarelyEventType type, float pitch, void* user_data);
  void _handle_note_event(BarelyEventType type, float pitch);
  void _on_slice_changed();

  uint32_t instrument_id_ = 0;
  ::godot::Ref<BarelySliceResource> slice_;
  std::vector<float> slice_buffer_;  // TODO(#181): Remove heap allocation.

  BARELY_GODOT_INSTRUMENT_CONTROLS(BARELY_DECLARE_GODOT_INSTRUMENT_CONTROL);
};

}  // namespace barely::godot

#endif  // BARELYMUSICIAN_GODOT_INSTRUMENT_H_
