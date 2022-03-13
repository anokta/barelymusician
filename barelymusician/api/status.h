#ifndef BARELYMUSICIAN_API_STATUS_H_
#define BARELYMUSICIAN_API_STATUS_H_

// NOLINTBEGIN
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Status enum alias.
typedef int32_t BarelyStatus;

/// Status enum values.
enum BarelyStatus_Values {
  /// Success.
  BarelyStatus_kOk = 0,
  /// Invalid argument error.
  BarelyStatus_kInvalidArgument = 1,
  /// Not found error.
  BarelyStatus_kNotFound = 2,
  /// Already exists error.
  BarelyStatus_kAlreadyExists = 3,
  /// Unimplemented error.
  BarelyStatus_kUnimplemented = 4,
  /// Internal error.
  BarelyStatus_kInternal = 5,
  /// Unknown error.
  BarelyStatus_kUnknown = 6,
};

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
// NOLINTEND

#ifdef __cplusplus
#include <cassert>
#include <string>
#include <variant>

namespace barely {

/// Status.
enum class Status : BarelyStatus {
  /// Success.
  kOk = BarelyStatus_kOk,
  /// Invalid argument error.
  kInvalidArgument = BarelyStatus_kInvalidArgument,
  /// Not found error.
  kNotFound = BarelyStatus_kNotFound,
  /// Already exists error.
  kAlreadyExists = BarelyStatus_kAlreadyExists,
  /// Unimplemented error.
  kUnimplemented = BarelyStatus_kUnimplemented,
  /// Internal error.
  kInternal = BarelyStatus_kInternal,
  /// Unknown error.
  kUnknown = BarelyStatus_kUnknown,
};

/// Returns whether status is okay or not.
///
/// @param status Status.
/// @return True if okay, false otherwise.
inline bool IsOk(Status status) { return status == Status::kOk; }

/// Returns status string.
///
/// @param status Status.
/// @return Status string.
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
    case Status::kUnimplemented:
      return "Unimplemented error";
    case Status::kInternal:
      return "Internal error";
    case Status::kUnknown:
    default:
      return "Unknown error";
  }
}

/// Value or error status.
template <typename ValueType>
class StatusOr {
 public:
  /// Constructs new `StatusOr` with an error status.
  ///
  /// @param error_status Error status.
  // NOLINTNEXTLINE(google-explicit-constructor)
  StatusOr(Status error_status) : value_or_(error_status) {
    assert(error_status != Status::kOk);
  }

  /// Constructs new `StatusOr` with a value.
  ///
  /// @param value Value.
  // NOLINTNEXTLINE(google-explicit-constructor)
  StatusOr(ValueType value) : value_or_(std::move(value)) {}

  /// Returns contained error status.
  ///
  /// @return Error status.
  [[nodiscard]] Status GetErrorStatus() const {
    assert(std::holds_alternative<Status>(value_or_));
    return std::get<Status>(value_or_);
  }

  /// Returns contained value.
  ///
  /// @return Value.
  [[nodiscard]] const ValueType& GetValue() const {
    assert(std::holds_alternative<ValueType>(value_or_));
    return std::get<ValueType>(value_or_);
  }

  /// Returns contained value.
  ///
  /// @return Mutable value.
  [[nodiscard]] ValueType& GetValue() {
    assert(std::holds_alternative<ValueType>(value_or_));
    return std::get<ValueType>(value_or_);
  }

  /// Returns whether value is contained or not.
  ///
  /// @return True if contained, false otherwise.
  [[nodiscard]] bool IsOk() const {
    return std::holds_alternative<ValueType>(value_or_);
  }

 private:
  // Value or error status.
  std::variant<Status, ValueType> value_or_;
};

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_API_STATUS_H_
