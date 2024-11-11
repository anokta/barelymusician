#ifndef BARELYMUSICIAN_DSP_VOICE_H_
#define BARELYMUSICIAN_DSP_VOICE_H_

#include <algorithm>
#include <array>

#include "barelymusician.h"
#include "dsp/envelope.h"
#include "dsp/one_pole_filter.h"
#include "dsp/oscillator.h"
#include "dsp/sample_player.h"
#include "dsp/voice.h"

namespace barely::internal {

struct VoiceData {
  VoiceData(int sample_rate) noexcept : adsr(sample_rate) {}

  Envelope::Adsr adsr;
  double filter_coefficient = 1.0;
  FilterCallback filter_callback = kFilterCallbacks[BarelyFilterType_kNone];
  OscillatorCallback oscillator_callback = kOscillatorCallbacks[BarelyOscillatorShape_kNone];
};

/// Class that wraps an instrument voice.
class Voice {
 public:
  template <SamplePlaybackMode kSamplePlaybackMode>
  static double ProcessVoice(Voice& voice) noexcept {
    return voice.Next<kSamplePlaybackMode>();
  }

  /// Constructs a new `Voice`.
  ///
  /// @param voice_data Voice data.
  Voice(const VoiceData& voice_data) noexcept
      : envelope_(voice_data.adsr), voice_data_(voice_data) {}

  /// Returns whether the voice is currently active (i.e., playing).
  ///
  /// @return True if active.
  [[nodiscard]] bool IsActive() const noexcept { return envelope_.IsActive(); }

  /// Returns the next output sample.
  ///
  /// @return Output sample.
  template <SamplePlaybackMode kSamplePlaybackMode>
  double Next() noexcept {
    if constexpr (kSamplePlaybackMode == SamplePlaybackMode::kOnce) {
      if (!is_sample_player_active()) {
        envelope_.Reset();
        return 0.0;
      }
    }
    const double output =
        gain_ * envelope_.Next() *
        (voice_data_.oscillator_callback(oscillator_phase_) +
         PlaySample<kSamplePlaybackMode>(sample_player_slice_, sample_player_increment_,
                                         sample_player_cursor_));
    // Update the phasor.
    oscillator_phase_ += oscillator_increment_;
    if (oscillator_phase_ >= 1.0) {
      oscillator_phase_ -= 1.0;
    }
    return voice_data_.filter_callback(output, voice_data_.filter_coefficient, filter_state_);
  }

  /// Resets the voice.
  void Reset() noexcept { return envelope_.Reset(); }

  /// Starts the voice.
  void Start() noexcept {
    filter_state_ = 0.0;
    oscillator_phase_ = 0.0;
    sample_player_cursor_ = 0.0;
    envelope_.Start();
  }

  /// Stops the voice.
  template <bool IsSamplePlayedOnce>
  void Stop() noexcept {
    if constexpr (!IsSamplePlayedOnce) {
      envelope_.Stop();
    } else if (!is_sample_player_active()) {
      envelope_.Reset();
    }
  }

  [[nodiscard]] double gain() const noexcept { return gain_; }
  void set_gain(double gain) noexcept { gain_ = gain; }

  void set_oscillator_increment(double oscillator_increment) noexcept {
    oscillator_increment_ = oscillator_increment;
  }

  bool is_sample_player_active() const noexcept {
    return sample_player_slice_ != nullptr &&
           static_cast<int>(sample_player_cursor_) < sample_player_slice_->sample_count;
  }
  void set_sample_player_slice(const SampleDataSlice* sample_player_slice) noexcept {
    sample_player_slice_ = sample_player_slice;
  }
  void set_sample_player_speed(double speed, double sample_interval) noexcept {
    sample_player_increment_ =
        (sample_player_slice_ != nullptr && sample_player_slice_->sample_count > 0)
            ? speed * sample_player_slice_->sample_rate * sample_interval
            : 0.0;
  }

 private:
  Envelope envelope_;
  const VoiceData& voice_data_;

  double filter_state_ = 0.0;

  double gain_ = 0.0;

  double oscillator_increment_ = 0.0;
  double oscillator_phase_ = 0.0;

  const SampleDataSlice* sample_player_slice_ = nullptr;
  double sample_player_cursor_ = 0.0;
  double sample_player_increment_ = 0.0;
};

/// Voice callback signature alias.
///
/// @param voice Mutable voice.
/// @return Processed output value.
// TODO(#144): Clean this up by calling each carrier by template types.
using VoiceCallback = double (*)(Voice& voice);

/// Array of voice callbacks for each mode.
inline constexpr std::array<VoiceCallback, static_cast<int>(BarelySamplePlaybackMode_kCount)>
    kVoiceCallbacks = {
        &Voice::ProcessVoice<SamplePlaybackMode::kNone>,
        &Voice::ProcessVoice<SamplePlaybackMode::kOnce>,
        &Voice::ProcessVoice<SamplePlaybackMode::kSustain>,
        &Voice::ProcessVoice<SamplePlaybackMode::kLoop>,
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_DSP_VOICE_H_
