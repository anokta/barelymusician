#ifndef BARELYAPI_BASE_FRAME_H_
#define BARELYAPI_BASE_FRAME_H_

namespace barelyapi {

// Audio frame view that refers to a set of samples.
class Frame {
 public:
  // Constructs new |Frame| with the given |begin| pointer and |size|.
  //
  // @param begin Pointer to the first sample in frame.
  // @param size Frame size.
  Frame(float* begin, int size);

  // Returns a reference to the sample at the given |index|.
  //
  // @param index Sample index.
  // @return Reference to the sample.
  float& operator[](int index) { return begin_[index]; }
  const float& operator[](int index) const { return begin_[index]; }

  // Returns an iterator pointing to the first sample in frame.
  //
  // @return Iterator to the beginning of the frame.
  float* begin() { return begin_; }
  const float* begin() const { return begin_; }

  // Returns an iterator pointing to the past-the-end sample of the buffer.
  //
  // @return Iterator to the end of the buffer.
  float* end() { return begin_ + size_; }
  const float* end() const { return begin_ + size_; }

  // Returns the frame size.
  //
  // @return Frame size.
  int size() const { return size_; }

 private:
  // Iterator pointing to the first sample in frame.
  float* const begin_;

  // Frame size.
  const int size_;
};

}  // namespace barelyapi

#endif  // BARELYAPI_BASE_FRAME_H_
