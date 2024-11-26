#ifndef BARELYMUSICIAN_DSP_SAMPLE_PLAYER_H_
#define BARELYMUSICIAN_DSP_SAMPLE_PLAYER_H_

#include <cassert>
#include <cmath>

#include "barelymusician.h"

namespace barely::internal {

/// Sample player that generates output samples from a sample data slice.
class SamplePlayer {
 public:
  /// Returns whether the sample player is currently active or not.
  ///
  /// @return True if active, false otherwise.
  bool IsActive() const noexcept {
    assert(slice_ != nullptr);
    return static_cast<int>(cursor_) < slice_->sample_count;
  }

  /// Generates the next output sample.
  ///
  /// @tparam kMode Sample playback mode.
  /// @return Next output sample.
  template <SamplePlaybackMode kMode>
  [[nodiscard]] float Next() noexcept {
    if constexpr (kMode != SamplePlaybackMode::kNone) {
      assert(slice_ != nullptr);
      if (static_cast<int>(cursor_) >= slice_->sample_count) {
        return 0.0f;
      }
      // TODO(#7): Add a better interpolation method here?
      const float output = slice_->samples[static_cast<int>(cursor_)];
      cursor_ += increment_;
      if constexpr (kMode == SamplePlaybackMode::kLoop) {
        if (static_cast<int>(cursor_) >= slice_->sample_count) {
          cursor_ = std::fmod(cursor_, static_cast<float>(slice_->sample_count));
        }
      }
      return output;
    } else {
      return 0.0f;
    }
  }

  /// Resets the state.
  void Reset() noexcept { cursor_ = 0.0f; }

  /// Sets the slice.
  ///
  /// @param slice Pointer to sample data slice.
  void SetSlice(const SampleDataSlice* slice) noexcept { slice_ = slice; }

  /// Sets the increment per sample.
  ///
  /// @param pitch Note pitch.
  /// @param sample_interval Sample interval in seconds.
  void SetIncrement(float pitch, float sample_interval) noexcept {
    assert(sample_interval >= 0.0f);
    increment_ =
        (slice_ != nullptr && slice_->sample_count > 0)
            ? std::pow(2.0f, pitch - slice_->root_pitch) * slice_->sample_rate * sample_interval
            : 0.0f;
  }

 private:
  // Pointer to sample data slice.
  const SampleDataSlice* slice_ = nullptr;

  // Playback cursor.
  float cursor_ = 0.0f;

  // Increment per sample.
  float increment_ = 0.0f;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_DSP_SAMPLE_PLAYER_H_
