#ifndef BARELYMUSICIAN_COMPOSITION_NOTE_INTENSITY_H_
#define BARELYMUSICIAN_COMPOSITION_NOTE_INTENSITY_H_

#include <variant>

namespace barelyapi {

/// Note intensity type.
using NoteIntensity = std::variant<float>;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_NOTE_INTENSITY_H_
