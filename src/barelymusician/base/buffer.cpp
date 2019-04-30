#include "barelymusician/base/buffer.h"

#include <memory>

#include "barelymusician/base/logging.h"

namespace barelyapi {

Buffer::Buffer(int num_channels, int num_frames)
    : num_channels_(num_channels),
      num_frames_(num_frames),
      data_(num_channels_ * num_frames_) {
  DCHECK_GE(num_channels_, 0);
  DCHECK_GE(num_frames_, 0);
}

void Buffer::Clear() { std::fill(data_.begin(), data_.end(), 0.0f); }

}  // namespace barelyapi
