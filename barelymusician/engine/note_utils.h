#ifndef BARELYMUSICIAN_ENGINE_NOTE_UTILS_H_
#define BARELYMUSICIAN_ENGINE_NOTE_UTILS_H_

#include <vector>

#include "barelymusician/engine/note.h"

namespace barelyapi {

// Returns the raw note index for the given scale and quantized note index.
//
// @param scale Cumulative scale intervals of an octave in increasing order.
// @param note_index Quantized note index.
// @return Raw note index.
float GetRawNoteIndex(const std::vector<float>& scale,
                      const QuantizedNoteIndex& note_index);

// Returns the raw position for the given quantized position.
//
// @param position Quantized position.
// @return Raw position.
double GetRawPosition(const QuantizedPosition& position);

// Returns quantized position.
double QuantizePosition(double position, double resolution,
                        double amount = 1.0);

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_NOTE_UTILS_H_
