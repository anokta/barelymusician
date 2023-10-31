#ifndef BARELYMUSICIAN_INTERNAL_STATUS_OR_H_
#define BARELYMUSICIAN_INTERNAL_STATUS_OR_H_

#include <compare>
#include <string>
#include <utility>
#include <variant>

#include "barelymusician/barelymusician.h"

namespace barely::internal {

/// Status.
class Status {
 public:
  /// Enum values.
  enum Enum : BarelyStatus {
    /// Success.
    kOk = BarelyStatus_kOk,
    /// Invalid argument error.
    kInvalidArgument = BarelyStatus_kInvalidArgument,
    /// Not found error.
    kNotFound = BarelyStatus_kNotFound,
    /// Unimplemented error.
    kUnimplemented = BarelyStatus_kUnimplemented,
    /// Internal error.
    kInternal = BarelyStatus_kInternal,
  };

  /// Returns a new `Status` with `Status::kOk`.
  ///
  /// @return Status.
  static Status Ok() noexcept { return Status(Status::kOk); }

  /// Returns a new `Status` with `Status::kInvalidArgument`.
  ///
  /// @return Status.
  static Status InvalidArgument() noexcept {
    return Status(Status::kInvalidArgument);
  }

  /// Returns a new `Status` with `Status::kNotFound`.
  ///
  /// @return Status.
  static Status NotFound() noexcept { return Status(Status::kNotFound); }

  /// Returns a new `Status` with `Status::kUnimplemented`.
  ///
  /// @return Status.
  static Status Unimplemented() noexcept {
    return Status(Status::kUnimplemented);
  }

  /// Returns a new `Status` with `Status::kInternal`.
  ///
  /// @return Status.
  static Status Internal() noexcept { return Status(Status::kInternal); }

  /// Constructs a new `Status` from a raw type.
  ///
  /// @param status Raw status enum.
  // NOLINTNEXTLINE(google-explicit-constructor)
  Status(BarelyStatus status) noexcept : status_(static_cast<Enum>(status)) {}

  /// Returns the enum value.
  ///
  /// @return Enum value.
  // NOLINTNEXTLINE(google-explicit-constructor)
  operator Enum() const noexcept { return status_; }

  /// Enum comparators.
  auto operator<=>(Enum status) const noexcept { return status_ <=> status; }

  /// Returns whether the status is okay or not.
  ///
  /// @return True if okay, false otherwise.
  [[nodiscard]] bool IsOk() const noexcept { return status_ == kOk; }

  /// Returns the status string.
  ///
  /// @return Status string.
  [[nodiscard]] std::string ToString() const noexcept {
    switch (status_) {
      case kOk:
        return "Ok";
      case kInvalidArgument:
        return "Invalid argument error";
      case kNotFound:
        return "Not found error";
      case kUnimplemented:
        return "Unimplemented error";
      case kInternal:
        return "Internal error";
      default:
        return "Unknown error";
    }
  }

 private:
  // Constructs a new `Status` from enum type.
  explicit Status(Enum status) noexcept : status_(status) {}

  // Enum value.
  Enum status_;
};

/// Value, or an error status.
template <typename ValueType>
class StatusOr {
 public:
  /// Constructs a new `StatusOr` with an error status.
  ///
  /// @param error_status Error status.
  // NOLINTNEXTLINE(google-explicit-constructor)
  StatusOr(Status error_status) noexcept : value_or_(error_status) {
    assert(!error_status.IsOk());
  }

  /// Constructs a new `StatusOr` with a value.
  ///
  /// @param value Value.
  // NOLINTNEXTLINE(google-explicit-constructor)
  StatusOr(ValueType value) noexcept : value_or_(std::move(value)) {}

  /// Member access operators.
  const ValueType& operator*() const noexcept { return GetValue(); }
  const ValueType* operator->() const noexcept { return &GetValue(); }
  ValueType& operator*() noexcept { return GetValue(); }
  ValueType* operator->() noexcept { return &GetValue(); }

  /// Returns the contained error status.
  ///
  /// @return Error status.
  [[nodiscard]] Status GetErrorStatus() const noexcept {
    assert(std::holds_alternative<Status>(value_or_));
    return std::get<Status>(value_or_);
  }

  /// Returns the contained value.
  ///
  /// @return Value.
  [[nodiscard]] const ValueType& GetValue() const noexcept {
    assert(std::holds_alternative<ValueType>(value_or_));
    return std::get<ValueType>(value_or_);
  }

  /// Returns the contained value.
  ///
  /// @return Mutable value.
  [[nodiscard]] ValueType& GetValue() noexcept {
    assert(std::holds_alternative<ValueType>(value_or_));
    return std::get<ValueType>(value_or_);
  }

  /// Returns whether a value is contained or not.
  ///
  /// @return True if contained, false otherwise.
  [[nodiscard]] bool IsOk() const noexcept {
    return std::holds_alternative<ValueType>(value_or_);
  }

 private:
  // Value or an error status.
  std::variant<Status, ValueType> value_or_;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_STATUS_OR_H_
