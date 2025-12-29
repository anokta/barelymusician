#ifndef BARELYMUSICIAN_DSP_VOICE_POOL_H_
#define BARELYMUSICIAN_DSP_VOICE_POOL_H_

#include <array>
#include <cstdint>
#include <utility>

#include "common/constants.h"
#include "common/pool.h"
#include "dsp/instrument_params.h"
#include "dsp/voice.h"

namespace barely {

using InstrumentPool = Pool<BarelyInstrument, BARELYMUSICIAN_MAX_INSTRUMENT_COUNT>;

using VoicePool = Pool<Voice, BARELYMUSICIAN_MAX_VOICE_COUNT>;

[[nodiscard]] inline Voice* AcquireVoice(VoicePool& voice_pool, InstrumentParams& instrument_params,
                                         float pitch) noexcept {
  if (instrument_params.should_retrigger) {
    for (uint32_t i = 0; i < instrument_params.active_voice_count; ++i) {
      Voice& voice = voice_pool.Get(instrument_params.active_voices[i]);
      if (voice.pitch() == pitch) {
        for (uint32_t j = 0; j < instrument_params.active_voice_count; ++j) {
          voice.increment_timestamp();
        }
        return &voice;
      }
    }
  }

  if (voice_pool.GetActiveCount() < voice_pool.Count() &&
      instrument_params.active_voice_count < instrument_params.voice_count) {
    for (uint32_t i = 0; i < instrument_params.active_voice_count; ++i) {
      voice_pool.Get(instrument_params.active_voices[i]).increment_timestamp();
    }

    // Acquire new voice.
    const uint32_t voice_index = voice_pool.Acquire();
    instrument_params.active_voices[instrument_params.active_voice_count++] = voice_index;
    return &voice_pool.Get(voice_index);
  }

  // No voices are available to acquire, steal th e oldest active voice.
  uint32_t oldest_active_voice_index = 0;
  for (uint32_t i = 0; i < instrument_params.active_voice_count; ++i) {
    Voice& voice = voice_pool.Get(instrument_params.active_voices[i]);
    if (voice.timestamp() >
        voice_pool.Get(instrument_params.active_voices[oldest_active_voice_index]).timestamp()) {
      oldest_active_voice_index = i;
    }
    voice.increment_timestamp();
  }
  return &voice_pool.Get(instrument_params.active_voices[oldest_active_voice_index]);
}

/// Processes the next output samples.
///
/// @tparam kIsSidechainSend Denotes whether the sidechain frame is for send or receive.
/// @param rng Random number generator.
/// @param voice_pool Voice pool.
/// @param delay_frame Delay send frame.
/// @param sidechain_frame Sidechain send frame.
/// @param output_frame Output frame.
template <bool kIsSidechainSend = false>
inline void ProcessAllVoices(AudioRng& rng, VoicePool& voice_pool,
                             float delay_frame[kStereoChannelCount],
                             float sidechain_frame[kStereoChannelCount],
                             float output_frame[kStereoChannelCount]) noexcept {
  for (uint32_t i = 0; i < voice_pool.GetActiveCount();) {
    Voice& voice = voice_pool.GetActive(i);
    InstrumentParams& params = *voice.instrument_params_;
    if constexpr (kIsSidechainSend) {
      if (!voice.IsActive()) {
        for (uint32_t j = 0; j < params.active_voice_count; ++j) {
          if (params.active_voices[j] == voice_pool.GetIndex(voice)) {
            std::swap(params.active_voices[j], params.active_voices[params.active_voice_count - 1]);
            --params.active_voice_count;
            break;
          }
        }
        voice_pool.Release(voice_pool.GetIndex(voice));
        continue;
      }
    }
    voice.Process<kIsSidechainSend>(params, rng, delay_frame, sidechain_frame, output_frame);
    ++i;
  }
}

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_VOICE_POOL_H_
