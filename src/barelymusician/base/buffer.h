#ifndef BARELYAPI_BASE_BUFFER_H_
#define BARELYAPI_BASE_BUFFER_H_

#include <iterator>
#include <vector>

#include "barelymusician/base/frame.h"

namespace barelyapi {

// Interleaved audio buffer.
class Buffer {
 public:
  // Constructs new |Buffer| with the given |num_channels| and |num_frames|.
  //
  // @param num_channels Number of channels per frame.
  // @param num_channels Number of frames.
  Buffer(int num_channels, int num_frames);

  // Returns a reference to the frame at the given |index|.
  //
  // @param Frame index.
  // @return Reference to the frame.
  Frame& operator[](int index) { return frames_[index]; }
  const Frame& operator[](int index) const { return frames_[index]; }

  // Returns an iterator pointing to the first frame of the buffer.
  //
  // @return Iterator to the beginning of the buffer.
  std::vector<Frame>::iterator begin() { return frames_.begin(); }
  std::vector<Frame>::const_iterator begin() const { return frames_.begin(); }

  // Clears the buffer.
  void clear() { std::fill(data_.begin(), data_.end(), 0.0f); }

  // Returns an iterator pointing to the past-the-end frame of the buffer.
  //
  // @return Iterator to the end of the buffer.
  std::vector<Frame>::iterator end() { return frames_.end(); }
  std::vector<Frame>::const_iterator end() const { return frames_.end(); }

  // Returns the number of channels per frame.
  //
  // @return Number of channels per frame.
  int num_channels() const { return num_channels_; }

  // Returns the number of frames.
  //
  // @return Number of frames.
  int num_frames() const { return static_cast<int>(frames_.size()); }

  // Returns the buffer size.
  //
  // @return Buffer size.
  int size() const { return static_cast<int>(data_.size()); }

 private:
  // Number of channels.
  const int num_channels_;

  // Buffer data.
  std::vector<float> data_;

  // Buffer frames.
  std::vector<Frame> frames_;
};

}  // namespace barelyapi

#endif  // BARELYAPI_BASE_BUFFER_H_
