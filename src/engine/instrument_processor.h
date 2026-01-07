#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_

#include <barelymusician.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>

#include "core/constants.h"
#include "core/pool.h"
#include "core/rng.h"
#include "dsp/distortion.h"
#include "dsp/sample_data.h"
#include "dsp/sample_generators.h"
#include "engine/instrument_params.h"
#include "engine/voice_state.h"

namespace barely {

class InstrumentProcessor {
 public:
  explicit InstrumentProcessor(AudioRng& rng, float sample_interval) noexcept
      : rng_(rng), sample_interval_(sample_interval) {}

  void SetControl(uint32_t instrument_index, BarelyInstrumentControlType type,
                  float value) noexcept;
  void SetNoteControl(uint32_t instrument_index, float pitch, BarelyNoteControlType type,
                      float value) noexcept;
  void SetNoteOff(uint32_t instrument_index, float pitch) noexcept;
  void SetNoteOn(uint32_t instrument_index, float pitch,
                 const std::array<float, BarelyNoteControlType_kCount>& note_controls) noexcept;
  void SetSampleData(uint32_t instrument_index, SampleData& sample_data) noexcept;

  void Init(uint32_t instrument_index) noexcept { params_[instrument_index] = {}; }

  template <bool kIsSidechainSend = false>
  void ProcessAllVoices(float delay_frame[kStereoChannelCount],
                        float sidechain_frame[kStereoChannelCount],
                        float output_frame[kStereoChannelCount]) noexcept {
    for (uint32_t i = 0; i < voice_pool_.GetActiveCount();) {
      VoiceState& voice = voice_pool_.GetActive(i);
      InstrumentParams& params = params_[voice.instrument_index];
      if constexpr (kIsSidechainSend) {
        if (!voice.IsActive()) {
          if (voice.previous_voice_index != 0) {
            voice_pool_.Get(voice.previous_voice_index).next_voice_index = voice.next_voice_index;
            if (voice.next_voice_index != 0) {
              voice_pool_.Get(voice.next_voice_index).previous_voice_index =
                  voice.previous_voice_index;
            }
            voice.previous_voice_index = 0;
          } else {
            params.first_voice_index = voice.next_voice_index;
            if (voice.next_voice_index != 0) {
              voice_pool_.Get(voice.next_voice_index).previous_voice_index = 0;
            }
          }
          voice.next_voice_index = 0;
          voice_pool_.Release(voice_pool_.GetIndex(voice));
          continue;
        }
      }
      ProcessVoice<kIsSidechainSend>(voice, params, delay_frame, sidechain_frame, output_frame);
      ++i;
    }
  }

 private:
  [[nodiscard]] uint32_t AcquireVoice(uint32_t instrument_index, float pitch) noexcept;

  /// Processes the next output frame of a voice.
  ///
  /// @tparam kIsSidechainSend Denotes whether the sidechain frame is for send or receive.
  /// @param voice Voice.
  /// @param instrument_params Instrument parameters.
  /// @param delay_frame Delay send frame.
  /// @param sidechain_frame Sidechain send frame.
  /// @param output_frame Output frame.
  template <bool kIsSidechainSend = false>
  void ProcessVoice(VoiceState& voice, const InstrumentParams& instrument_params,
                    float delay_frame[kStereoChannelCount],
                    float sidechain_frame[kStereoChannelCount],
                    float output_frame[kStereoChannelCount]) noexcept {
    if constexpr (kIsSidechainSend) {
      if (voice.params.sidechain_send <= 0.0f) {
        return;
      }
    } else {
      if (voice.params.sidechain_send > 0.0f) {
        return;
      }
    }

    if (instrument_params.slice_mode == SliceMode::kOnce &&
        static_cast<int>(voice.slice_offset) >= voice.slice->sample_count) {
      voice.envelope.Stop();
    }

    const float skewed_osc_phase = std::min(1.0f, (1.0f + voice.params.osc_skew) * voice.osc_phase);
    const float osc_sample = (1.0f - voice.params.osc_noise_mix) *
                                 GenerateOscSample(skewed_osc_phase, voice.params.osc_shape) +
                             voice.params.osc_noise_mix * rng_.Generate();
    const float osc_output = voice.params.osc_mix * osc_sample;

    const bool has_slice = (voice.slice != nullptr);
    const float slice_sample =
        has_slice ? GenerateSliceSample(*voice.slice, voice.slice_offset) : 0.0f;
    const float slice_output = (1.0f - voice.params.osc_mix) * slice_sample;

    float output = voice.envelope.Next();

    if (instrument_params.osc_mode == OscMode::kMix || instrument_params.osc_mode == OscMode::kMf) {
      output *= osc_output + slice_output;
    } else if (instrument_params.osc_mode == OscMode::kFm) {
      output *= slice_sample;
    } else if (instrument_params.osc_mode == OscMode::kRing) {
      output *= osc_output * slice_sample + slice_output;
    } else if (instrument_params.osc_mode == OscMode::kAm) {
      output *= std::abs(osc_output) * slice_sample + slice_output;
    } else if (instrument_params.osc_mode == OscMode::kEnvelopeFollower) {
      output *= osc_output * std::abs(slice_sample) + slice_output;
    }

    // TODO(#146): These effects should ideally be bypassed completely when they are disabled.
    output = voice.bit_crusher.Next(output, voice.params.bit_crusher_range,
                                    voice.params.bit_crusher_increment);
    output = Distortion(output, voice.params.distortion_amount, voice.params.distortion_drive);
    output = voice.filter.Next(output, voice.params.filter_coeffs);

    output *= voice.params.gain;

    float osc_increment = instrument_params.osc_increment * voice.note_params.osc_increment;
    if (instrument_params.osc_mode == OscMode::kMf) {
      osc_increment += slice_sample * osc_increment;
    }
    voice.osc_phase += osc_increment;
    if (voice.osc_phase >= 1.0f) {
      voice.osc_phase -= 1.0f;
    }

    float slice_increment = instrument_params.slice_increment * voice.note_params.slice_increment;
    if (slice_increment > 0) {
      if (instrument_params.osc_mode == OscMode::kFm) {
        slice_increment += osc_output * slice_increment;
      }
      voice.slice_offset += slice_increment;
      if (instrument_params.slice_mode == SliceMode::kLoop) {
        if (has_slice && static_cast<int>(voice.slice_offset) >= voice.slice->sample_count) {
          voice.slice_offset =
              std::fmod(voice.slice_offset, static_cast<float>(voice.slice->sample_count));
        }
      }
    }

    const float left_gain = 0.5f * (1.0f - voice.params.stereo_pan);
    const float right_gain = 1.0f - left_gain;

    float left_output = left_gain * output;
    float right_output = right_gain * output;

    if constexpr (kIsSidechainSend) {
      sidechain_frame[0] += voice.params.sidechain_send * left_output;
      sidechain_frame[1] += voice.params.sidechain_send * right_output;
    } else {
      if (voice.params.sidechain_send < 0.0f) {
        const float sidechain_send = -voice.params.sidechain_send;
        left_output = std::lerp(left_output, sidechain_frame[0] * left_output, sidechain_send);
        right_output = std::lerp(right_output, sidechain_frame[1] * right_output, sidechain_send);
      }
    }

    delay_frame[0] += voice.params.delay_send * left_output;
    delay_frame[1] += voice.params.delay_send * right_output;

    output_frame[0] += left_output;
    output_frame[1] += right_output;

    voice.Approach(instrument_params.voice_params);
  }

  // Array of instrument parameters (indices should match `InstrumentController::instrument_pool_`).
  std::array<InstrumentParams, BARELYMUSICIAN_MAX_INSTRUMENT_COUNT + 1> params_ = {};

  // Voice pool.
  Pool<VoiceState, BARELYMUSICIAN_MAX_VOICE_COUNT> voice_pool_ = {};

  AudioRng& rng_;
  float sample_interval_ = 0.0f;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_
