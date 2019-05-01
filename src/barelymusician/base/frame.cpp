#include "barelymusician/base/frame.h"

#include "barelymusician/base/logging.h"

namespace barelyapi {

Frame::Frame(float* begin, int size) : begin_(begin), size_(size) {
  DCHECK(begin_);
  DCHECK_GE(size_, 0);
}

}  // namespace barelyapi
