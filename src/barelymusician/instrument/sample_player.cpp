#include "barelymusician/instrument/sample_player.h"

#include <algorithm>
#include <cmath>

#include "barelymusician/base/logging.h"

namespace barelyapi {

SamplePlayer::SamplePlayer(float sample_interval, int frequency,
                           const float* data, int length)
    : sample_ratio_(static_cast<float>(frequency) * sample_interval),
      data_(data),
      length_(static_cast<float>(length)),
      loop_(false),
      speed_(1.0f),
      cursor_(0.0f) {
  DCHECK_GE(sample_ratio_, 0.0f);
  DCHECK(data_);
  DCHECK_GT(length_, 0.0f);
}

float SamplePlayer::Next() {
  if (cursor_ >= length_) {
    // Playback is finished, skip processing.
    return 0.0f;
  }
  DCHECK(data_);
  const float output = data_[static_cast<int>(cursor_)];
  // Update the playback cursor.
  cursor_ += speed_ * sample_ratio_;
  if (cursor_ >= length_ && loop_) {
    // Loop the playback.
    cursor_ = std::fmod(cursor_, length_);
  }
  return output;
}

void SamplePlayer::Reset() { cursor_ = 0.0f; }

void SamplePlayer::SetSpeed(float speed) { speed_ = std::max(speed, 0.0f); }

void SamplePlayer::SetLoop(bool loop) { loop_ = loop; }

}  // namespace barelyapi
