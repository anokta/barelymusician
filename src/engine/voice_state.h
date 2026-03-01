#ifndef BARELYMUSICIAN_ENGINE_VOICE_STATE_H_
#define BARELYMUSICIAN_ENGINE_VOICE_STATE_H_

#include <array>
#include <cstdint>

#include "core/constants.h"
#include "core/control.h"
#include "dsp/biquad_filter.h"
#include "dsp/bit_crusher.h"
#include "dsp/envelope.h"
#include "engine/instrument_params.h"
#include "engine/voice_params.h"

namespace barely {

struct VoiceState {
  BitCrusher bit_crusher = {};
  Envelope envelope = {};
  BiquadFilter filter = {};
  BiquadFilter tone_filter = {};

  VoiceParams params = {};

  struct {
    float gain = 1.0f;
    float osc_increment = 0.0f;
    float slice_increment = 0.0f;
  } note_params = {};

  float pitch = 0.0f;
  float pitch_shift = 0.0f;

  float osc_phase = 0.0f;
  float slice_offset = 0.0f;

  uint32_t instrument_index = kInvalidIndex;
  uint32_t note_index = kInvalidIndex;
  uint32_t slice_index = kInvalidIndex;

  uint32_t prev_voice_index = kInvalidIndex;
  uint32_t next_voice_index = kInvalidIndex;

  uint32_t timestamp = 0;  // incremented in each voice start for round-robin voice stealing.

  bool stop_on_slice_end = false;

  void Approach(const VoiceParams& new_params) noexcept {
    ApproachValue(params.gain, note_params.gain * new_params.gain);
    ApproachValue(params.bit_crusher_increment, new_params.bit_crusher_increment);
    ApproachValue(params.bit_crusher_range, new_params.bit_crusher_range);
    ApproachValue(params.distortion_amount, new_params.distortion_amount);
    ApproachValue(params.distortion_drive, new_params.distortion_drive);
    ApproachValue(params.osc_mix, new_params.osc_mix);
    ApproachValue(params.osc_noise_mix, new_params.osc_noise_mix);
    ApproachValue(params.osc_shape, new_params.osc_shape);
    ApproachValue(params.osc_skew, new_params.osc_skew);
    ApproachValue(params.stereo_pan, new_params.stereo_pan);

    ApproachValue(params.filter_coeffs.a1, new_params.filter_coeffs.a1);
    ApproachValue(params.filter_coeffs.a2, new_params.filter_coeffs.a2);
    ApproachValue(params.filter_coeffs.b0, new_params.filter_coeffs.b0);
    ApproachValue(params.filter_coeffs.b1, new_params.filter_coeffs.b1);
    ApproachValue(params.filter_coeffs.b2, new_params.filter_coeffs.b2);
    ApproachValue(params.tone_filter_coeffs.a1, new_params.tone_filter_coeffs.a1);
    ApproachValue(params.tone_filter_coeffs.a2, new_params.tone_filter_coeffs.a2);
    ApproachValue(params.tone_filter_coeffs.b0, new_params.tone_filter_coeffs.b0);
    ApproachValue(params.tone_filter_coeffs.b1, new_params.tone_filter_coeffs.b1);
    ApproachValue(params.tone_filter_coeffs.b2, new_params.tone_filter_coeffs.b2);

    ApproachValue(params.delay_send, new_params.delay_send);
    ApproachValue(params.reverb_send, new_params.reverb_send);
    ApproachValue(params.sidechain_send, new_params.sidechain_send);
  }

  void Start(const InstrumentParams& instrument_params, const SliceState* slice,
             float note_pitch) noexcept {
    params = instrument_params.voice_params;
    note_params.gain = 1.0f;
    pitch = note_pitch;
    pitch_shift = 0.0f;
    UpdatePitchIncrements(slice);
    bit_crusher.Reset();
    filter.Reset();
    osc_phase = 0.0f;
    slice_offset = 0.0f;
    stop_on_slice_end = false;
    envelope.Start(instrument_params.adsr);
    timestamp = 0;
  }

  void UpdatePitchIncrements(const SliceState* slice) noexcept {
    const float shifted_pitch = pitch + pitch_shift;
    note_params.osc_increment = std::pow(2.0f, shifted_pitch);
    note_params.slice_increment =
        (slice != nullptr && slice->sample_count > 0)
            ? slice->sample_rate * std::pow(2.0f, shifted_pitch - slice->root_pitch)
            : 0.0f;
  }
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_VOICE_STATE_H_
