#ifndef BARELYMUSICIAN_ENGINE_ID_H_
#define BARELYMUSICIAN_ENGINE_ID_H_

#include "barelymusician/barelymusician.h"

namespace barelyapi {

/// Identifier alias.
using Id = BarelyId;

/// Invalid identifier.
constexpr Id kInvalid = BarelyId_kInvalid;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_ID_H_
