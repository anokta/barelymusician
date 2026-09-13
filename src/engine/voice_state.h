#ifndef BARELYMUSICIAN_ENGINE_VOICE_STATE_H_
#define BARELYMUSICIAN_ENGINE_VOICE_STATE_H_

#include <array>
#include <cstdint>

#include "core/constants.h"
#include "core/control.h"
#include "dsp/bit_crusher.h"
#include "dsp/envelope.h"
#include "dsp/tone_filter.h"
#include "engine/params.h"

namespace barely {

struct VoiceState {
  BitCrusher bit_crusher = {};
  Envelope envelope = {};
  ToneFilter filter = {};

  VoiceParams params = {};

  struct {
    double gain = 1.0;
    double osc_increment = 0.0;
    double slice_increment = 0.0;
  } note_params = {};

  double pitch = 0.0;
  double pitch_shift = 0.0;

  double osc_phase = 0.0;
  double slice_offset = 0.0;

  uint32_t instrument_index = kInvalidIndex;
  uint32_t slice_index = kInvalidIndex;

  uint32_t prev_voice_index = kInvalidIndex;
  uint32_t next_voice_index = kInvalidIndex;

  uint32_t timestamp = 0;  // incremented in each voice start for round-robin voice stealing.

  bool stop_on_slice_end = false;

  void Approach(const VoiceParams& new_params, double coeff) noexcept {
    params.filter_params.Approach(new_params.filter_params, coeff);
    ApproachValue(params.gain, note_params.gain * new_params.gain, coeff);
    ApproachValue(params.bit_crusher_increment, new_params.bit_crusher_increment, coeff);
    ApproachValue(params.bit_crusher_range, new_params.bit_crusher_range, coeff);
    ApproachValue(params.distortion_amount, new_params.distortion_amount, coeff);
    ApproachValue(params.distortion_drive, new_params.distortion_drive, coeff);
    ApproachValue(params.osc_mix, new_params.osc_mix, coeff);
    ApproachValue(params.osc_noise_mix, new_params.osc_noise_mix, coeff);
    ApproachValue(params.osc_shape, new_params.osc_shape, coeff);
    ApproachValue(params.osc_skew, new_params.osc_skew, coeff);
    ApproachValue(params.stereo_pan, new_params.stereo_pan, coeff);
    ApproachValue(params.delay_send, new_params.delay_send, coeff);
    ApproachValue(params.reverb_send, new_params.reverb_send, coeff);
    ApproachValue(params.sidechain_send, new_params.sidechain_send, coeff);
  }

  void Start(const InstrumentParams& instrument_params, const SliceState* slice,
             double note_pitch) noexcept {
    params = instrument_params.voice_params;
    note_params = {.gain = 1.0};
    pitch = note_pitch;
    pitch_shift = 0.0;
    UpdatePitchIncrements(slice);
    bit_crusher.Reset();
    filter.Reset();
    osc_phase = 0.0;
    slice_offset = 0.0;
    stop_on_slice_end = false;
    envelope.Start(instrument_params.adsr);
    timestamp = 0;
  }

  void UpdatePitchIncrements(const SliceState* slice) noexcept {
    const double shifted_pitch = pitch + pitch_shift;
    note_params.osc_increment = std::pow(2.0, shifted_pitch);
    note_params.slice_increment =
        (slice != nullptr && slice->sample_count > 0)
            ? slice->sample_rate *
                  std::pow(2.0, shifted_pitch - static_cast<double>(slice->root_pitch))
            : 0.0;
  }
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_VOICE_STATE_H_
