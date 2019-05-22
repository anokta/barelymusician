#ifndef BARELYMUSICIAN_COMPOSITION_SCALE_H_
#define BARELYMUSICIAN_COMPOSITION_SCALE_H_

#include <vector>

namespace barelyapi {

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

  // Returns the scaled note index for the given |relative_note_index|.
  //
  // @param relative_note_index Relative note index.
  // @return Scaled note index.
  float GetNoteIndex(float relative_note_index) const;

 private:
  // Cumulative intervals per each step of an octave in increasing order.
  const std::vector<float> intervals_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_SCALE_H_
