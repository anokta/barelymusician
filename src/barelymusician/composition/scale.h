#ifndef BARELYMUSICIAN_COMPOSITION_SCALE_H_
#define BARELYMUSICIAN_COMPOSITION_SCALE_H_

#include <vector>

namespace barelyapi {

// Musical scale.
class Scale {
 public:
  // Constructs new |Scale| with the given |intervals|.
  //
  // @param intervals Cumulative intervals of an octave in increasing order.
  explicit Scale(const std::vector<float>& intervals);

  // Returns the scale length.
  //
  // @return Scale length.
  int GetLength() const;

  // Returns the corresponding note index for the given |scale_index|.
  //
  // @param scale_index Scale index.
  // @return Note index.
  float GetNoteIndex(float scale_index) const;

 private:
  // Cumulative intervals per each step of an octave in increasing order.
  const std::vector<float> intervals_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_SCALE_H_
