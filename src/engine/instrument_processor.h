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
#include "dsp/sample_generators.h"
#include "engine/engine_state.h"
#include "engine/instrument_params.h"
#include "engine/slice_state.h"
#include "engine/voice_state.h"

namespace barely {

class InstrumentProcessor {
 public:
  explicit InstrumentProcessor(EngineState& engine) noexcept : engine_(engine) {}

  void SetControl(uint32_t instrument_index, BarelyInstrumentControlType type,
                  float value) noexcept;
  void SetNoteControl(uint32_t note_index, BarelyNoteControlType type, float value) noexcept;
  void SetNoteOff(uint32_t note_index) noexcept;
  void SetNoteOn(uint32_t note_index, uint32_t instrument_index, float pitch,
                 const std::array<float, BarelyNoteControlType_kCount>& note_controls) noexcept;
  void SetSampleData(uint32_t instrument_index, uint32_t first_slice_index) noexcept;

  void Init(uint32_t instrument_index) noexcept {
    engine_.instrument_params[instrument_index] = {};
  }

  template <bool kIsSidechainSend = false>
  void ProcessAllVoices(float delay_frame[kStereoChannelCount],
                        float reverb_frame[kStereoChannelCount],
                        float sidechain_frame[kStereoChannelCount],
                        float output_frame[kStereoChannelCount]) noexcept {
    for (uint32_t i = 0; i < engine_.voice_pool.ActiveCount();) {
      const uint32_t voice_index = engine_.voice_pool.GetActive(i);
      VoiceState& voice = engine_.GetVoice(voice_index);
      InstrumentParams& params = engine_.instrument_params[voice.instrument_index];
      if constexpr (kIsSidechainSend) {
        if (!voice.IsActive()) {
          ReleaseVoice(voice, params);
          engine_.voice_pool.Release(voice_index);
          continue;
        }
      }
      ProcessVoice<kIsSidechainSend>(voice, params, delay_frame, reverb_frame, sidechain_frame,
                                     output_frame);
      ++i;
    }
  }

 private:
  [[nodiscard]] uint32_t AcquireVoice(InstrumentParams& params, float pitch) noexcept;

  void ReleaseVoice(VoiceState& voice, InstrumentParams& params) noexcept {
    if (voice.prev_voice_index != kInvalidIndex) {
      engine_.GetVoice(voice.prev_voice_index).next_voice_index = voice.next_voice_index;
      if (voice.next_voice_index != kInvalidIndex) {
        engine_.GetVoice(voice.next_voice_index).prev_voice_index = voice.prev_voice_index;
      }
      voice.prev_voice_index = kInvalidIndex;
    } else {
      params.first_voice_index = voice.next_voice_index;
      if (voice.next_voice_index != kInvalidIndex) {
        engine_.GetVoice(voice.next_voice_index).prev_voice_index = kInvalidIndex;
      }
    }
    voice.next_voice_index = kInvalidIndex;
    if (voice.note_index != kInvalidIndex) {
      engine_.note_to_voice[voice.note_index] = kInvalidIndex;
    }
  }

  template <bool kIsSidechainSend = false>
  void ProcessVoice(VoiceState& voice, const InstrumentParams& instrument_params,
                    float delay_frame[kStereoChannelCount], float reverb_frame[kStereoChannelCount],
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

    const SliceState* slice = engine_.slice_pool.Get(voice.slice_index);

    if (instrument_params.slice_mode == SliceMode::kOnce && slice != nullptr &&
        static_cast<int32_t>(voice.slice_offset) >= slice->sample_count) {
      voice.envelope.Stop();
    }

    const float skewed_osc_phase = std::min(1.0f, (1.0f + voice.params.osc_skew) * voice.osc_phase);
    const float osc_sample = (1.0f - voice.params.osc_noise_mix) *
                                 GenerateOscSample(skewed_osc_phase, voice.params.osc_shape) +
                             voice.params.osc_noise_mix * engine_.audio_rng.Generate();
    const float osc_output = voice.params.osc_mix * osc_sample;

    const float slice_sample =
        (slice != nullptr)
            ? GenerateSliceSample(slice->samples, slice->sample_count, voice.slice_offset)
            : 0.0f;
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
        if (slice != nullptr && static_cast<int32_t>(voice.slice_offset) >= slice->sample_count) {
          voice.slice_offset =
              std::fmod(voice.slice_offset, static_cast<float>(slice->sample_count));
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

    reverb_frame[0] += voice.params.reverb_send * left_output;
    reverb_frame[1] += voice.params.reverb_send * right_output;

    output_frame[0] += left_output;
    output_frame[1] += right_output;

    voice.Approach(instrument_params.voice_params);
  }

  EngineState& engine_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_
