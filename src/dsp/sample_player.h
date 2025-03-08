#ifndef BARELYMUSICIAN_DSP_SAMPLE_PLAYER_H_
#define BARELYMUSICIAN_DSP_SAMPLE_PLAYER_H_

#include <cassert>
#include <cmath>

#include "barelymusician.h"

namespace barely {

/// Sample player that generates output samples from a sample data slice.
class SamplePlayer {
 public:
  /// Returns the output sample.
  ///
  /// @tparam kMode Slice mode.
  /// @return Next output sample.
  template <SliceMode kMode>
  [[nodiscard]] float GetOutput() const noexcept {
    if constexpr (kMode != SliceMode::kNone) {
      assert(slice_ != nullptr);
      const int sample_index = static_cast<int>(cursor_);
      if (sample_index >= slice_->sample_count) {
        return 0.0f;
      }
      return std::lerp(slice_->samples[sample_index],
                       slice_->samples[(sample_index + 1) % slice_->sample_count],
                       cursor_ - static_cast<float>(sample_index));
    } else {
      return 0.0f;
    }
  }

  /// Returns whether the sample player is currently active or not.
  ///
  /// @return True if active, false otherwise.
  bool IsActive() const noexcept {
    assert(slice_ != nullptr);
    return static_cast<int>(cursor_) < slice_->sample_count;
  }

  // TODO(#146): Clean this up to merge the functionality with `SetIncrement`.
  /// Increments the phase.
  ///
  /// @param increment_shift Phase increment shift.
  template <SliceMode kMode>
  void Increment(float increment_shift = 0.0f) noexcept {
    cursor_ += increment_ * (1.0f + increment_shift);
    if constexpr (kMode == SliceMode::kLoop) {
      if (static_cast<int>(cursor_) >= slice_->sample_count) {
        cursor_ = std::fmod(cursor_, static_cast<float>(slice_->sample_count));
      }
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

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_SAMPLE_PLAYER_H_
