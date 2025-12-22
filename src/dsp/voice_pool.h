#ifndef BARELYMUSICIAN_DSP_VOICE_POOL_H_
#define BARELYMUSICIAN_DSP_VOICE_POOL_H_

#include <array>
#include <utility>

#include "dsp/instrument_pool.h"
#include "dsp/voice.h"

namespace barely {

/// Maximum number of active voices.
inline constexpr int kMaxActiveVoiceCount = BARELYMUSICIAN_MAX_VOICE_COUNT;

/// Voice pool.
class VoicePool {
 public:
  VoicePool() noexcept {
    // TODO(#126): This can be avoided by switching to intrusive lists.
    for (int i = 0; i < kMaxActiveVoiceCount; ++i) {
      active_voices_[i].voice_index = i;
    }
  }

  [[nodiscard]] Voice* Acquire(InstrumentIndex instrument_index,
                               InstrumentParams& instrument_params, float pitch) noexcept {
    if (instrument_params.should_retrigger) {
      for (int i = 0; i < instrument_params.active_voice_count; ++i) {
        Voice& voice = Get(instrument_params.active_voices[i]);
        if (voice.pitch() == pitch) {
          for (int j = 0; j < instrument_params.active_voice_count; ++j) {
            voice.increment_timestamp();
          }
          return &voice;
        }
      }
    }

    if (active_voice_count_ < kMaxActiveVoiceCount &&
        instrument_params.active_voice_count < instrument_params.voice_count) {
      for (int i = 0; i < instrument_params.active_voice_count; ++i) {
        Get(instrument_params.active_voices[i]).increment_timestamp();
      }

      // Acquire new voice.
      ActiveVoice& active_voice = active_voices_[active_voice_count_++];
      active_voice.instrument_index = instrument_index;
      instrument_params.active_voices[instrument_params.active_voice_count++] =
          active_voice.voice_index;

      Voice& voice = Get(active_voice.voice_index);

      return &voice;
    }

    // No voices are available to acquire, steal the oldest active voice.
    int oldest_active_voice_index = 0;
    for (int i = 0; i < instrument_params.active_voice_count; ++i) {
      Voice& voice = Get(instrument_params.active_voices[i]);
      if (voice.timestamp() >
          Get(instrument_params.active_voices[oldest_active_voice_index]).timestamp()) {
        oldest_active_voice_index = i;
      }
      voice.increment_timestamp();
    }
    return &Get(instrument_params.active_voices[oldest_active_voice_index]);
  }

  [[nodiscard]] Voice& Get(VoiceIndex index) noexcept {
    assert(index >= 0);
    assert(index < kMaxActiveVoiceCount);
    return voices_[index];
  }

  /// Processes the next output samples.
  ///
  /// @tparam kIsSidechainSend Denotes whether the sidechain frame is for send or receive.
  /// @param rng Random number generator.
  /// @param instrument_pool Instrument pool.
  /// @param delay_frame Delay send frame.
  /// @param sidechain_frame Sidechain send frame.
  /// @param output_frame Output frame.
  template <bool kIsSidechainSend = false>
  void Process(AudioRng& rng, InstrumentPool& instrument_pool,
               float delay_frame[kStereoChannelCount], float sidechain_frame[kStereoChannelCount],
               float output_frame[kStereoChannelCount]) noexcept {
    for (int i = 0; i < active_voice_count_; ++i) {
      Voice& voice = Get(active_voices_[i].voice_index);
      InstrumentParams& params = instrument_pool.Get(active_voices_[i].instrument_index);
      if constexpr (kIsSidechainSend) {
        if (!voice.IsActive()) {
          for (int j = 0; j < params.active_voice_count; ++j) {
            if (params.active_voices[j] == active_voices_[i].voice_index) {
              std::swap(params.active_voices[j],
                        params.active_voices[params.active_voice_count - 1]);
              --params.active_voice_count;
              break;
            }
          }
          std::swap(active_voices_[i], active_voices_[active_voice_count_ - 1]);
          --active_voice_count_;
          continue;
        }
      }
      voice.Process<kIsSidechainSend>(params, rng, delay_frame, sidechain_frame, output_frame);
    }
  }

  void Release(InstrumentIndex instrument_index) noexcept {
    for (int i = 0; i < active_voice_count_;) {
      if (active_voices_[i].instrument_index == instrument_index) {
        std::swap(active_voices_[i], active_voices_[active_voice_count_ - 1]);
        --active_voice_count_;
      } else {
        ++i;
      }
    }
  }

 private:
  struct ActiveVoice {
    InstrumentIndex instrument_index = 0;
    VoiceIndex voice_index = 0;
  };

  std::array<ActiveVoice, kMaxActiveVoiceCount> active_voices_;
  std::array<Voice, kMaxActiveVoiceCount> voices_;

  int active_voice_count_ = 0;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_VOICE_POOL_H_
