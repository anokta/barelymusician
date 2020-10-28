#ifndef BARELYMUSICIAN_BASE_STATUS_H_
#define BARELYMUSICIAN_BASE_STATUS_H_

#include <variant>

#include "barelymusician/base/logging.h"

namespace barelyapi {

// Status codes.
enum class Status {
  kOk,               // Success.
  kInternal,         // Internal error.
  kInvalidArgument,  // Invalid argument error.
  kNotFound,         // Not found error.
  kUnimplemented,    // Unimplemented error.
  kUnknown,          // Unknown error.
};

// Value or error status.
template <typename ValueType>
using StatusOr = std::variant<ValueType, Status>;

// Returns |status_or| status.
template <typename ValueType>
Status GetStatus(const StatusOr<ValueType>& status_or) {
  DCHECK(std::holds_alternative<Status>(status_or));
  return std::get<ValueType>(status_or);
}

// Returns |status_or| value.
template <typename ValueType>
ValueType& GetValue(StatusOr<ValueType>& status_or) {
  DCHECK(std::holds_alternative<ValueType>(status_or));
  return std::get<ValueType>(status_or);
}

// Returns |status_or| value.
template <typename ValueType>
const ValueType& GetValue(const StatusOr<ValueType>& status_or) {
  DCHECK(std::holds_alternative<ValueType>(status_or));
  return std::get<ValueType>(status_or);
}

// Returns whether |status| is ok.
inline bool IsOk(Status status) { return status == Status::kOk; }

// Returns whether |status_or| is ok.
template <typename ValueType>
inline bool IsOk(const StatusOr<ValueType>& status_or) {
  return std::holds_alternative<ValueType>(status_or);
}

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_BASE_STATUS_H_
