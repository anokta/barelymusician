#include "barelymusician/dsp/sample_player.h"

#include <algorithm>
#include <cmath>

#include "barelymusician/base/logging.h"

namespace barelyapi {

SamplePlayer::SamplePlayer(float sample_interval)
    : sample_interval_(sample_interval),
      data_(nullptr),
      frequency_(0.0f),
      length_(0.0f),
      loop_(false),
      speed_(1.0f),
      cursor_(0.0f),
      increment_(0.0f) {
  DCHECK_GE(sample_interval_, 0.0f);
}

float SamplePlayer::Next() {
  if (data_ == nullptr || cursor_ >= length_) {
    // Nothing to play, skip processing.
    return 0.0f;
  }
  // TODO(#7): Add a better interpolation method here?
  const float output = data_[static_cast<int>(std::round(cursor_))];
  // Update the playback cursor.
  cursor_ += increment_;
  if (cursor_ >= length_ && loop_) {
    // Loop the playback.
    cursor_ = std::fmod(cursor_, length_);
  }
  return output;
}

void SamplePlayer::Reset() { cursor_ = 0.0f; }

void SamplePlayer::SetData(const float* data, int frequency, int length) {
  data_ = data;
  frequency_ = std::max(static_cast<float>(frequency), 0.0f);
  length_ = std::max(static_cast<float>(length), 0.0f);
  CalculateIncrementPerSample();
}

void SamplePlayer::SetLoop(bool loop) { loop_ = loop; }

void SamplePlayer::SetSpeed(float speed) {
  speed_ = std::max(speed, 0.0f);
  CalculateIncrementPerSample();
}

void SamplePlayer::CalculateIncrementPerSample() {
  increment_ = speed_ * frequency_ * sample_interval_;
}

}  // namespace barelyapi
