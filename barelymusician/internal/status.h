#ifndef BARELYMUSICIAN_INTERNAL_STATUS_H_
#define BARELYMUSICIAN_INTERNAL_STATUS_H_

#include "barelymusician/barelymusician.h"

namespace barely::internal {

/// Status.
using Status = barely::Status;

/// Value or error status.
template <typename ValueType>
using StatusOr = barely::StatusOr<ValueType>;

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_STATUS_H_
