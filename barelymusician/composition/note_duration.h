#ifndef BARELYMUSICIAN_COMPOSITION_NOTE_DURATION_H_
#define BARELYMUSICIAN_COMPOSITION_NOTE_DURATION_H_

#include <variant>

namespace barelyapi {

/// Note duration type.
using NoteDuration = std::variant<double>;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_NOTE_DURATION_H_
