#ifndef BARELYMUSICIAN_DSP_VOICE_POOL_H_
#define BARELYMUSICIAN_DSP_VOICE_POOL_H_

#include <array>
#include <cstdint>
#include <utility>

#include "common/constants.h"
#include "common/pool.h"
#include "dsp/voice.h"
#include "engine/instrument_params.h"

namespace barely {

using InstrumentPool = Pool<BarelyInstrument, BARELYMUSICIAN_MAX_INSTRUMENT_COUNT>;

using VoicePool = Pool<Voice, BARELYMUSICIAN_MAX_VOICE_COUNT>;

[[nodiscard]] inline Voice* AcquireVoice(VoicePool& voice_pool, InstrumentParams& instrument_params,
                                         float pitch) noexcept {
  if (instrument_params.should_retrigger) {
    uint32_t current_voice_index = instrument_params.first_voice_index;
    while (current_voice_index != 0) {
      Voice& voice = voice_pool.Get(current_voice_index);
      if (voice.pitch() == pitch) {
        while (current_voice_index != 0) {
          voice = voice_pool.Get(current_voice_index);
          voice.increment_timestamp();
          current_voice_index = voice.next_instrument_voice_index;
        }
        return &voice;
      }
      current_voice_index = voice.next_instrument_voice_index;
    }
  }

  uint32_t current_voice_index = instrument_params.first_voice_index;
  uint32_t oldest_active_voice_index = 0;
  uint32_t active_voice_count = 0;
  while (current_voice_index != 0) {
    Voice& voice = voice_pool.Get(current_voice_index);
    if (oldest_active_voice_index == 0 ||
        voice.timestamp() > voice_pool.Get(oldest_active_voice_index).timestamp()) {
      oldest_active_voice_index = current_voice_index;
    }
    voice.increment_timestamp();
    ++active_voice_count;
    if (voice.next_instrument_voice_index == 0) {  // TODO(#126): Remove double checking here.
      break;
    }
    current_voice_index = voice.next_instrument_voice_index;
  }

  // Acquire new voice.
  if (voice_pool.GetActiveCount() < voice_pool.Count() &&
      active_voice_count < static_cast<uint32_t>(instrument_params.voice_count)) {
    const uint32_t new_voice_index = voice_pool.Acquire();
    Voice& new_voice = voice_pool.Get(new_voice_index);
    new_voice.previous_instrument_voice_index = current_voice_index;
    new_voice.next_instrument_voice_index = 0;
    if (current_voice_index != 0) {
      voice_pool.Get(current_voice_index).next_instrument_voice_index = new_voice_index;
    } else {
      instrument_params.first_voice_index = new_voice_index;
    }
    return &new_voice;
  }

  // No voices are available to acquire, steal the oldest active voice.
  return &voice_pool.Get(oldest_active_voice_index);
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
        if (voice.previous_instrument_voice_index != 0) {
          voice_pool.Get(voice.previous_instrument_voice_index).next_instrument_voice_index =
              voice.next_instrument_voice_index;
          if (voice.next_instrument_voice_index != 0) {
            voice_pool.Get(voice.next_instrument_voice_index).previous_instrument_voice_index =
                voice.previous_instrument_voice_index;
          }
          voice.previous_instrument_voice_index = 0;
        } else {
          params.first_voice_index = voice.next_instrument_voice_index;
          if (voice.next_instrument_voice_index != 0) {
            voice_pool.Get(voice.next_instrument_voice_index).previous_instrument_voice_index = 0;
          }
        }
        voice.next_instrument_voice_index = 0;
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
