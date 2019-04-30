#ifndef BARELYAPI_BASE_BUFFER_H_
#define BARELYAPI_BASE_BUFFER_H_

#include <iterator>
#include <vector>

namespace barelyapi {

// Interleaved audio buffer.
// TODO(#39): Introduce |Frame| structure for easier interleaved operations.
class Buffer {
 public:
  // Constructs new |Buffer| with the given |num_channels| and |num_frames|.
  //
  // @param num_channels Number of channels per frame.
  // @param num_channels Number of frames.
  Buffer(int num_channels, int num_frames);

  // Clears the buffer.
  void Clear();

  // Returns the number of channels per frame.
  //
  // @return Number of channels per frame.
  int num_channels() const { return num_channels_; }

  // Returns the number of frames.
  //
  // @return Number of frames.
  int num_frames() const { return num_frames_; }

  // Returns the buffer size.
  //
  // @return Buffer size.
  int size() const { return num_channels_ * num_frames_; }

  // Returns an iterator pointing to the first sample of the buffer.
  //
  // @return Iterator to the beginning of the buffer.
  std::vector<float>::iterator begin() { return data_.begin(); }
  std::vector<float>::const_iterator begin() const { return data_.begin(); }

  // Returns an iterator pointing to the past-the-end sample of the buffer.
  //
  // @return Iterator to the end of the buffer.
  std::vector<float>::iterator end() { return data_.end(); }
  std::vector<float>::const_iterator end() const { return data_.end(); }

  // Returns a reference to the sample at the given |index|.
  //
  // @param 
  // @return Iterator to the beginning of the buffer.
  std::vector<float>::reference operator[](int index) { return data_[index]; }
  std::vector<float>::const_reference operator[](int index) const {
    return data_[index];
  }

 private:
  // Number of channels.
  const int num_channels_;

  // Number of frames.
  const int num_frames_;

  // Buffer data.
  std::vector<float> data_;
};

}  // namespace barelyapi

#endif  // BARELYAPI_BASE_BUFFER_H_
