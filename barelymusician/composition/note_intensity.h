#ifndef BARELYMUSICIAN_COMPOSITION_NOTE_INTENSITY_H_
#define BARELYMUSICIAN_COMPOSITION_NOTE_INTENSITY_H_

#include <variant>

namespace barely {

/// Note intensity type.
using NoteIntensity = std::variant<float>;

}  // namespace barely

#endif  // BARELYMUSICIAN_COMPOSITION_NOTE_INTENSITY_H_
