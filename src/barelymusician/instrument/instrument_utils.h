#ifndef BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_UTILS_H_
#define BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_UTILS_H_

namespace barelyapi {

// Converts a note index value into the corresponding pitch frequency.
//
// @param index Note index.
// @return Frequency in Hz.
float FrequencyFromNoteIndex(float index);

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_UTILS_H_
