#ifndef BARELYMUSICIAN_COMPOSITION_QUANTIZER_H_
#define BARELYMUSICIAN_COMPOSITION_QUANTIZER_H_

namespace barelyapi {

// Beat quantizer.
class Quantizer {
 public:
  // Constructs new |Quantizer| with the given |num_notes_per_beat|.
  //
  // @param num_notes_per_beat Number of notes per beat.
  explicit Quantizer(int num_notes_per_beat);

  // Returns the quantized duration in beats.
  //
  // @param num_notes Number of notes.
  float GetDurationBeats(int num_notes) const;

 private:
  // Number of notes per beat.
  const int num_notes_per_beat_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_QUANTIZER_H_
