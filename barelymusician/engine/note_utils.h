#ifndef BARELYMUSICIAN_ENGINE_NOTE_UTILS_H_
#define BARELYMUSICIAN_ENGINE_NOTE_UTILS_H_

#include <vector>

#include "barelymusician/engine/note.h"

namespace barelyapi {

// Returns pitch for the given scale and scale index.
//
// @param scale Cumulative scale intervals of an octave in increasing order.
// @param scale_index Scale index.
// @return Pitch.
float GetPitch(const std::vector<float>& scale, float scale_index);

// Returns quantized position for the given number of beat steps.
//
// @param step Quantized step.
// @param num_steps Number of steps per beat.
// @return Raw position.
double GetPosition(int step, int num_steps);

// Returns quantized position.
double QuantizePosition(double position, double resolution,
                        double amount = 1.0);

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_NOTE_UTILS_H_
