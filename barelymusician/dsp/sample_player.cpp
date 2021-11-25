#include "barelymusician/dsp/sample_player.h"

#include <cassert>
#include <cmath>

namespace barely {

SamplePlayer::SamplePlayer(int sample_rate)
    : sample_interval_(1.0f / static_cast<float>(sample_rate)),
      data_(nullptr),
      frequency_(0.0f),
      length_(0.0f),
      loop_(false),
      speed_(1.0f),
      cursor_(0.0f),
      increment_(0.0f) {
  assert(sample_rate > 0);
}

float SamplePlayer::Next() {
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

void SamplePlayer::Reset() { cursor_ = 0.0f; }

void SamplePlayer::SetData(const float* data, int frequency, int length) {
  assert(frequency >= 0);
  assert(length >= 0);
  data_ = data;
  frequency_ = static_cast<float>(frequency);
  length_ = static_cast<float>(length);
  CalculateIncrementPerSample();
}

void SamplePlayer::SetLoop(bool loop) { loop_ = loop; }

void SamplePlayer::SetSpeed(float speed) {
  assert(speed >= 0.0f);
  speed_ = speed;
  CalculateIncrementPerSample();
}

void SamplePlayer::CalculateIncrementPerSample() {
  increment_ = speed_ * frequency_ * sample_interval_;
}

}  // namespace barely
