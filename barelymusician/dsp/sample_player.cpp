#include "barelymusician/dsp/sample_player.h"

#include <algorithm>
#include <cmath>

namespace barely {

SamplePlayer::SamplePlayer(int frame_rate) noexcept
    : frame_interval_((frame_rate > 0) ? 1.0f / static_cast<float>(frame_rate) : 0.0f) {}

float SamplePlayer::Next() noexcept {
  if (!data_ || cursor_ >= length_) {
    // Nothing to play, skip processing.
    return 0.0f;
  }
  // TODO(#7): Add a better interpolation method here?
  const float output = data_[static_cast<int>(cursor_)];
  // Update the playback cursor.
  cursor_ += increment_;
  if (cursor_ >= length_ && loop_) {
    // Loop the playback.
    cursor_ = (length_ > 0.0f) ? std::fmod(cursor_, length_) : 0.0f;
  }
  return output;
}

void SamplePlayer::Reset() noexcept { cursor_ = 0.0f; }

void SamplePlayer::SetData(const float* data, int frequency, int length) noexcept {
  data_ = data;
  frequency_ = static_cast<float>(std::max(frequency, 0));
  length_ = static_cast<float>(std::max(length, 0));
  CalculateIncrementPerSample();
}

void SamplePlayer::SetLoop(bool loop) noexcept { loop_ = loop; }

void SamplePlayer::SetSpeed(float speed) noexcept {
  speed_ = std::max(speed, 0.0f);
  CalculateIncrementPerSample();
}

void SamplePlayer::CalculateIncrementPerSample() noexcept {
  increment_ = speed_ * frequency_ * frame_interval_;
}

}  // namespace barely
