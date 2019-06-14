#ifndef BARELYMUSICIAN_COMPOSITION_NOTE_UTILS_H_
#define BARELYMUSICIAN_COMPOSITION_NOTE_UTILS_H_

#include <vector>

namespace barelyapi {

float GetQuantizedBeatDuration(int num_notes, int num_notes_per_beat);

float GetQuantizedNoteIntex(const std::vector<float>& scale, float scale_index);

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_NOTE_UTILS_H_
