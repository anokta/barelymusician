#include "barelymusician/dsp/sample_player.h"

#include <algorithm>
#include <cmath>

#include "barelymusician/barelymusician.h"

namespace barely {

SamplePlayer::SamplePlayer(Integer frame_rate) noexcept
    : frame_interval_((frame_rate > 0) ? 1.0 / static_cast<Real>(frame_rate)
                                       : 0.0) {}

Real SamplePlayer::Next() noexcept {
  if (!data_ || cursor_ >= length_) {
    // Nothing to play, skip processing.
    return 0.0;
  }
  // TODO(#7): Add a better interpolation method here?
  const Real output = data_[static_cast<Integer>(cursor_)];
  // Update the playback cursor.
  cursor_ += increment_;
  if (cursor_ >= length_ && loop_) {
    // Loop the playback.
    cursor_ = (length_ > 0.0) ? std::fmod(cursor_, length_) : 0.0;
  }
  return output;
}

void SamplePlayer::Reset() noexcept { cursor_ = 0.0; }

void SamplePlayer::SetData(const Real* data, Integer frequency,
                           Integer length) noexcept {
  data_ = data;
  frequency_ = static_cast<Real>(std::max(frequency, static_cast<Integer>(0)));
  length_ = static_cast<Real>(std::max(length, static_cast<Integer>(0)));
  CalculateIncrementPerSample();
}

void SamplePlayer::SetLoop(bool loop) noexcept { loop_ = loop; }

void SamplePlayer::SetSpeed(Real speed) noexcept {
  speed_ = std::max(speed, 0.0);
  CalculateIncrementPerSample();
}

void SamplePlayer::CalculateIncrementPerSample() noexcept {
  increment_ = speed_ * frequency_ * frame_interval_;
}

}  // namespace barely
