#ifndef BARELYMUSICIAN_DSP_VOICE_POOL_H_
#define BARELYMUSICIAN_DSP_VOICE_POOL_H_

#include <array>
#include <utility>

#include "dsp/voice.h"

namespace barely {

/// Maximum number of active voices.
inline constexpr int kMaxActiveVoiceCount = 256;

/// Voice pool.
class VoicePool {
 public:
  VoicePool() noexcept {
    // TODO(#126): This can be avoided by switching to intrusive lists.
    for (int i = 0; i < kMaxActiveVoiceCount; ++i) {
      active_voices_[i].voice_index = i;
    }
  }

  [[nodiscard]] VoiceIndex Acquire(InstrumentParams& instrument_params) noexcept {
    if (active_voice_count_ < kMaxActiveVoiceCount) {
      ActiveVoice& active_voice = active_voices_[active_voice_count_++];
      active_voice.instrument_params = &instrument_params;
      return active_voice.voice_index;
    }
    return -1;  // TODO(#126): Handle this gracefully with intrusive list.
  }

  [[nodiscard]] Voice& Get(VoiceIndex index) noexcept {
    assert(index >= 0);
    assert(index < kMaxActiveVoiceCount);
    return voices_[index];
  }
  [[nodiscard]] const Voice& Get(VoiceIndex index) const noexcept { return Get(index); }

  /// Processes the next output samples.
  ///
  /// @tparam kIsSidechainSend Denotes whether the sidechain frame is for send or receive.
  /// @param delay_frame Delay send frame.
  /// @param sidechain_frame Sidechain send frame.
  /// @param output_frame Output frame.
  template <bool kIsSidechainSend = false>
  void Process(float delay_frame[kStereoChannelCount], float sidechain_frame[kStereoChannelCount],
               float output_frame[kStereoChannelCount]) noexcept {
    for (int i = 0; i < active_voice_count_; ++i) {
      Voice& voice = Get(active_voices_[i].voice_index);
      InstrumentParams* params = active_voices_[i].instrument_params;
      assert(params != nullptr);
      if constexpr (kIsSidechainSend) {
        if (!voice.IsActive()) {
          for (int j = 0; j < params->active_voice_count; ++j) {
            if (params->active_voices[j] == active_voices_[i].voice_index) {
              std::swap(params->active_voices[j],
                        params->active_voices[params->active_voice_count - 1]);
              --params->active_voice_count;
              break;
            }
          }
          std::swap(active_voices_[i], active_voices_[active_voice_count_ - 1]);
          --active_voice_count_;
          continue;
        }
      }
      voice.Process<kIsSidechainSend>(*params, delay_frame, sidechain_frame, output_frame);
    }
  }

  void Release(VoiceIndex index) noexcept {
    // TODO(#126): This can be avoided by switching to intrusive lists.
    for (int i = 0; i < active_voice_count_; ++i) {
      if (active_voices_[i].voice_index == index) {
        std::swap(active_voices_[i], active_voices_[active_voice_count_ - 1]);
        --active_voice_count_;
        break;
      }
    }
  }

 private:
  struct ActiveVoice {
    InstrumentParams* instrument_params = nullptr;
    VoiceIndex voice_index = 0;
  };
  std::array<ActiveVoice, kMaxActiveVoiceCount> active_voices_;
  int active_voice_count_ = 0;

  std::array<Voice, kMaxActiveVoiceCount> voices_;
  int voice_count_ = 0;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_VOICE_POOL_H_
