#ifndef BARELYMUSICIAN_ENGINE_NOTE_UTILS_H_
#define BARELYMUSICIAN_ENGINE_NOTE_UTILS_H_

#include <vector>

#include "barelymusician/engine/note.h"

namespace barelyapi {

// Returns the quantized note index for the given scale and index.
//
// @param scale Cumulative scale intervals of an octave in increasing order.
// @param scale_index Scale index.
// @return Note index.
float GetRawNoteIndex(const std::vector<float>& scale,
                      const NoteIndex& note_index);

// Returns the quantized position for the given number of beat steps.
//
// @param step Quantized step.
// @param num_steps_per_beat Number of steps per beat.
// @param Beat.
double GetRawPosition(const Position& position);

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_NOTE_UTILS_H_
