#ifndef BARELYMUSICIAN_COMPOSITION_NOTE_UTILS_H_
#define BARELYMUSICIAN_COMPOSITION_NOTE_UTILS_H_

#include <vector>

namespace barelyapi {

float GetScaledNoteIndex(float note_index, const std::vector<float>& scale);

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_NOTE_UTILS_H_
