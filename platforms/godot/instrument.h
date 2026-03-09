#ifndef BARELYMUSICIAN_GODOT_INSTRUMENT_H_
#define BARELYMUSICIAN_GODOT_INSTRUMENT_H_

#include <barelymusician.h>

#include <cstdint>

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/wrapped.hpp"

namespace barely::godot {

class BarelyInstrument : public ::godot::Node {
 public:
  ~BarelyInstrument();

  void set_all_notes_off();
  void set_note_off(float pitch);
  void set_note_on(float pitch, float gain = 1.0f, float pitch_shift = 0.0f);
  bool is_note_on(float pitch) const;

  void set_gain(float gain);
  float get_gain() const { return gain_; }

  void set_pitch_shift(float pitch_shift);
  float get_pitch_shift() const { return pitch_shift_; }

  void set_stereo_pan(float stereo_pan);
  float get_stereo_pan() const { return stereo_pan_; }

  void set_attack(float attack);
  float get_attack() const { return attack_; }

  void set_decay(float decay);
  float get_decay() const { return decay_; }

  void set_sustain(float sustain);
  float get_sustain() const { return sustain_; }

  void set_release(float release);
  float get_release() const { return release_; }

  void set_slice_mode(int slice_mode);
  int get_slice_mode() const { return slice_mode_; }

  void set_osc_mix(float osc_mix);
  float get_osc_mix() const { return osc_mix_; }

  void set_osc_mode(int osc_mode);
  int get_osc_mode() const { return osc_mode_; }

  void set_osc_noise_mix(float osc_noise_mix);
  float get_osc_noise_mix() const { return osc_noise_mix_; }

  void set_osc_pitch_shift(float osc_pitch_shift);
  float get_osc_pitch_shift() const { return osc_pitch_shift_; }

  void set_osc_shape(float osc_shape);
  float get_osc_shape() const { return osc_shape_; }

  void set_osc_skew(float osc_skew);
  float get_osc_skew() const { return osc_skew_; }

  void set_crush_depth(float crush_depth);
  float get_crush_depth() const { return crush_depth_; }

  void set_crush_rate(float crush_rate);
  float get_crush_rate() const { return crush_rate_; }

  void set_distortion_mix(float distortion_mix);
  float get_distortion_mix() const { return distortion_mix_; }

  void set_distortion_drive(float distortion_drive);
  float get_distortion_drive() const { return distortion_drive_; }

  void set_filter_cutoff(float filter_cutoff);
  float get_filter_cutoff() const { return filter_cutoff_; }

  void set_filter_resonance(float filter_resonance);
  float get_filter_resonance() const { return filter_resonance_; }

  void set_filter_tone(float filter_tone);
  float get_filter_tone() const { return filter_tone_; }

  void set_delay_send(float delay_send);
  float get_delay_send() const { return delay_send_; }

  void set_reverb_send(float reverb_send);
  float get_reverb_send() const { return reverb_send_; }

  void set_sidechain_send(float sidechain_send);
  float get_sidechain_send() const { return sidechain_send_; }

  void set_arp_mode(int arp_mode);
  int get_arp_mode() const { return arp_mode_; }

  void set_arp_gate(float arp_gate);
  float get_arp_gate() const { return arp_gate_; }

  void set_arp_rate(float arp_rate);
  float get_arp_rate() const { return arp_rate_; }

  void set_retrigger(bool retrigger);
  bool get_retrigger() const { return retrigger_; }

  void set_voice_count(int voice_count);
  int get_voice_count() const { return voice_count_; }

  void _ready() override;
  void _process(double delta) override;

 private:
  GDCLASS(BarelyInstrument, ::godot::Node);
  static void _bind_methods();

  uint32_t instrument_id_ = 0;

  float gain_ = 1.0f;
  float pitch_shift_ = 0.0f;
  float stereo_pan_ = 0.0f;
  float attack_ = 0.0f;
  float decay_ = 0.0f;
  float sustain_ = 1.0f;
  float release_ = 0.0f;
  int slice_mode_ = BarelySliceMode_kSustain;
  float osc_mix_ = 1.0f;
  int osc_mode_ = BarelyOscMode_kCrossfade;
  float osc_noise_mix_ = 0.0f;
  float osc_pitch_shift_ = 0.0f;
  float osc_shape_ = 0.0f;
  float osc_skew_ = 0.0f;
  float crush_depth_ = 0.0f;
  float crush_rate_ = 0.0f;
  float distortion_mix_ = 0.0f;
  float distortion_drive_ = 0.0f;
  float filter_cutoff_ = 1.0f;
  float filter_resonance_ = 0.5f;
  float filter_tone_ = 0.0f;
  float delay_send_ = 0.0f;
  float reverb_send_ = 0.0f;
  float sidechain_send_ = 0.0f;
  int arp_mode_ = BarelyArpMode_kNone;
  float arp_gate_ = 0.5f;
  float arp_rate_ = 1.0f;
  bool retrigger_ = false;
  int voice_count_ = 8;
};

}  // namespace barely::godot

#endif  // BARELYMUSICIAN_GODOT_INSTRUMENT_H_
