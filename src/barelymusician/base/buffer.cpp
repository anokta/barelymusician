#include "barelymusician/base/buffer.h"

#include "barelymusician/base/logging.h"

namespace barelyapi {

Buffer::Buffer(int num_channels, int num_frames)
    : num_channels_(num_channels), data_(num_channels_ * num_frames) {
  DCHECK_GE(num_channels_, 0);
  DCHECK_GE(num_frames, 0);
  frames_.reserve(num_frames);
  float* it = data_.data();
  for (int i = 0; i < num_frames; ++i) {
    frames_.emplace_back(it, num_channels_);
    it += num_channels_;
  }
}

}  // namespace barelyapi
