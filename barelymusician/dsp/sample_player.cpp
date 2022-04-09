#include "barelymusician/dsp/sample_player.h"

#include <algorithm>
#include <cmath>

namespace barelyapi {

SamplePlayer::SamplePlayer(int sample_rate) noexcept
    : sample_interval_(
          (sample_rate > 0) ? 1.0 / static_cast<double>(sample_rate) : 0.0) {}

double SamplePlayer::Next() noexcept {
  if (!data_ || cursor_ >= length_) {
    // Nothing to play, skip processing.
    return 0.0;
  }
  // TODO(#7): Add a better interpolation method here?
  const double output = data_[static_cast<int>(cursor_)];
  // Update the playback cursor.
  cursor_ += increment_;
  if (cursor_ >= length_ && loop_) {
    // Loop the playback.
    cursor_ = (length_ > 0.0) ? std::fmod(cursor_, length_) : 0.0;
  }
  return output;
}

void SamplePlayer::Reset() noexcept { cursor_ = 0.0; }

void SamplePlayer::SetData(const double* data, int frequency,
                           int length) noexcept {
  data_ = data;
  frequency_ = static_cast<double>(std::max(frequency, 0));
  length_ = static_cast<double>(std::max(length, 0));
  CalculateIncrementPerSample();
}

void SamplePlayer::SetLoop(bool loop) noexcept { loop_ = loop; }

void SamplePlayer::SetSpeed(double speed) noexcept {
  speed_ = std::max(speed, 0.0);
  CalculateIncrementPerSample();
}

void SamplePlayer::CalculateIncrementPerSample() noexcept {
  increment_ = speed_ * frequency_ * sample_interval_;
}

}  // namespace barelyapi
