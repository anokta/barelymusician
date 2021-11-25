#ifndef BARELYMUSICIAN_COMMON_STATUS_H_
#define BARELYMUSICIAN_COMMON_STATUS_H_

#include <cassert>
#include <string>
#include <utility>
#include <variant>

namespace barely {

/// Status codes.
enum class Status {
  kOk,                  // Success.
  kInvalidArgument,     // Invalid argument error.
  kNotFound,            // Not found error.
  kAlreadyExists,       // Already exists error.
  kFailedPrecondition,  // Failed precondition error.
  kUnimplemented,       // Unimplemented error.
  kInternal,            // Internal error.
  kUnknown,             // Unknown error.
};

/// Value or error status.
template <typename ValueType>
using StatusOr = std::variant<Status, ValueType>;

/// Returns |status_or| status.
///
/// @param status_or Value or error status.
/// @return Error status.
template <typename ValueType>
Status GetStatusOrStatus(const StatusOr<ValueType>& status_or) {
  assert(std::holds_alternative<Status>(status_or));
  return std::get<Status>(status_or);
}

/// Returns |status_or| value.
///
/// @param status_or Value or error status.
/// @return Value.
template <typename ValueType>
ValueType& GetStatusOrValue(StatusOr<ValueType>& status_or) {
  assert(std::holds_alternative<ValueType>(status_or));
  return std::get<ValueType>(status_or);
}

/// Returns |status_or| value.
///
/// @param status_or Value or error status.
/// @return Value.
template <typename ValueType>
const ValueType& GetStatusOrValue(const StatusOr<ValueType>& status_or) {
  assert(std::holds_alternative<ValueType>(status_or));
  return std::get<ValueType>(status_or);
}

/// Returns |status_or| value.
///
/// @param status_or Value or error status.
/// @return Value.
template <typename ValueType>
ValueType&& GetStatusOrValue(StatusOr<ValueType>&& status_or) {
  assert(std::holds_alternative<ValueType>(status_or));
  return std::move(std::get<ValueType>(status_or));
}

/// Returns whether |status_or| is ok, i.e., holding a value.
///
/// @param status_or Value or error status.
/// @return True if ok.
template <typename ValueType>
bool IsOk(const StatusOr<ValueType>& status_or) {
  return std::holds_alternative<ValueType>(status_or);
}

/// Returns whether |status| is ok.
///
/// @param status Status.
/// @return True if ok.
inline bool IsOk(Status status) { return status == Status::kOk; }

/// Returns status string.
///
/// @param status_or Value or error status.
/// @return String.
template <typename ValueType>
std::string ToString(const StatusOr<ValueType>& status_or) {
  return ToString(GetStatusOrStatus(status_or));
}

/// Returns status string.
///
/// @param status Status.
/// @return String.
inline std::string ToString(Status status) {
  switch (status) {
    case Status::kOk:
      return "Ok";
    case Status::kInvalidArgument:
      return "Invalid argument error";
    case Status::kNotFound:
      return "Not found error";
    case Status::kAlreadyExists:
      return "Already exists error";
    case Status::kFailedPrecondition:
      return "Failed precondition error";
    case Status::kUnimplemented:
      return "Unimplemented error";
    case Status::kInternal:
      return "Internal error";
    case Status::kUnknown:
    default:
      return "Unknown error";
  }
}

}  // namespace barely

#endif  // BARELYMUSICIAN_COMMON_STATUS_H_
