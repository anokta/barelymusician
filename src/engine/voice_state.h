#ifndef BARELYMUSICIAN_ENGINE_VOICE_STATE_H_
#define BARELYMUSICIAN_ENGINE_VOICE_STATE_H_

#include <array>
#include <cstdint>

#include "core/control.h"
#include "dsp/biquad_filter.h"
#include "dsp/bit_crusher.h"
#include "dsp/envelope.h"
#include "engine/instrument_params.h"
#include "engine/note_params.h"
#include "engine/voice_params.h"

namespace barely {

struct VoiceState {
  /// Bit crusher.
  BitCrusher bit_crusher = {};

  /// ADSR envelope.
  Envelope envelope = {};

  /// Biquad filter.
  BiquadFilter filter = {};

  /// Note parameters.
  NoteParams note_params = {};

  /// Voice parameters.
  VoiceParams params = {};

  /// Voice pitch.
  float pitch = 0.0f;

  /// Voice pitch shift.
  float pitch_shift = 0.0f;

  /// Oscillator phase.
  float osc_phase = 0.0f;

  /// Slice offset in samples.
  float slice_offset = 0.0f;

  /// Instrument index.
  uint32_t instrument_index = UINT32_MAX;

  /// Note index.
  uint32_t note_index = UINT32_MAX;

  /// Slice index.
  uint32_t slice_index = UINT32_MAX;

  /// Previous voice index.
  uint32_t prev_voice_index = UINT32_MAX;

  /// Next voice index.
  uint32_t next_voice_index = UINT32_MAX;

  /// Voice timestamp (incremented in each voice start for round-robin voice stealing).
  uint32_t timestamp = 0;

  /// Returns whether the voice is currently active (i.e., playing).
  ///
  /// @return True if active.
  [[nodiscard]] constexpr bool IsActive() const noexcept { return envelope.IsActive(); }

  /// Returns whether the voice is currently on.
  ///
  /// @return True if on.
  [[nodiscard]] constexpr bool IsOn() const noexcept { return envelope.IsOn(); }

  /// Approaches the voice to the new parameters with smooth interpolation.
  ///
  /// @param new_params New voice parameters.
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

    ApproachValue(params.delay_send, new_params.delay_send);
    ApproachValue(params.sidechain_send, new_params.sidechain_send);
  }

  /// Starts the voice.
  ///
  /// @param instrument_params Instrument parameters.
  /// @param slice Pointer to slice.
  /// @param note_pitch Note pitch.
  /// @param note_controls Array of note controls.
  void Start(const InstrumentParams& instrument_params, const SliceState* slice, float note_pitch,
             const std::array<float, BarelyNoteControlType_kCount>& note_controls) noexcept {
    const float note_gain = note_controls[BarelyNoteControlType_kGain];
    const float note_pitch_shift = note_controls[BarelyNoteControlType_kPitchShift];
    note_params.gain = note_gain;
    params = instrument_params.voice_params;
    params.gain *= note_gain;
    pitch = note_pitch;
    pitch_shift = note_pitch_shift;
    UpdatePitchIncrements(slice);
    bit_crusher.Reset();
    filter.Reset();
    osc_phase = 0.0f;
    slice_offset = 0.0f;
    envelope.Start(instrument_params.adsr);
    timestamp = 0;
  }

  /// Updates the pitch increments.
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
