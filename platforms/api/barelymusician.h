#ifndef PLATFORMS_API_BARELYMUSICIAN_H_
#define PLATFORMS_API_BARELYMUSICIAN_H_

#include <cassert>
#include <cstdint>
#include <limits>
#include <utility>
#include <variant>

#include "platforms/capi/barelymusician_v2.h"

namespace barely {

/// Identifier type.
using Id = std::int64_t;

/// Parameter identifier type.
using ParamId = std::int32_t;

/// Parameter definition.
struct ParamDefinition {
  /// Constructs new |ParamDefinition|.
  ///
  /// @param default_value Default value.
  /// @param min_value Minimum value.
  /// @param max_value Maximum value.
  ParamDefinition(float default_value = 0.0f,
                  float min_value = std::numeric_limits<float>::lowest(),
                  float max_value = std::numeric_limits<float>::max());

  /// Constructs new |ParamDefinition| for a boolean value.
  ///
  /// @param default_value Default boolean value.
  ParamDefinition(bool default_value);

  /// Constructs new |ParamDefinition| for an integer value.
  ///
  /// @param default_value Default integer value.
  /// @param min_value Minimum integer value.
  /// @param max_value Maximum integer value.
  ParamDefinition(int default_value,
                  int min_value = std::numeric_limits<int>::lowest(),
                  int max_value = std::numeric_limits<int>::max());

  /// Default value.
  float default_value;

  /// Minimum value.
  float min_value;

  /// Maximum value.
  float max_value;
};

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
  /// Failed precondition error.
  kFailedPrecondition = BarelyStatus_kFailedPrecondition,
  /// Unimplemented error.
  kUnimplemented = BarelyStatus_kUnimplemented,
  /// Internal error.
  kInternal = BarelyStatus_kInternal,
  /// Unknown error.
  kUnknown = BarelyStatus_kUnknown,
};

/// Value or error status.
template <typename ValueType>
class StatusOr {
 public:
  /// Constructs new |StatusOr| with an error status.
  ///
  /// @param error_status Error status.
  StatusOr(Status error_status);

  /// Constructs new |StatusOr| with a value.
  ///
  /// @param value Value.
  StatusOr(ValueType value);

  /// Returns the error status.
  ///
  /// @return Error status.
  Status GetErrorStatus() const;

  /// Returns the value.
  ///
  /// @return Mutable value.
  ValueType& GetValue();

  /// Returns the value.
  ///
  /// @return Value.
  const ValueType& GetValue() const;

  /// Returns whether a value is present or not.
  ///
  /// @return True if a value is present, false otherwise.
  bool IsOk() const;

 private:
  // Value or error status.
  std::variant<Status, ValueType> value_or_;
};

/// BarelyMusician C++ API.
class Api {
 public:
  /// Constructs new |Api|.
  Api();

  /// Constructs new |Api| with an initial sampling rate.
  ///
  /// @param sample_rate Sampling rate in Hz.
  explicit Api(int sample_rate);

  /// Destroys |Api|.
  ~Api();

  /// Non-copyable and non-movable.
  Api(const Api& other) = delete;
  Api& operator=(const Api& other) noexcept = delete;
  Api(Api&& other) = delete;
  Api& operator=(Api&& other) noexcept = delete;

  /// Gets the sampling rate.
  ///
  /// @return Sampling rate in Hz, or error status.
  StatusOr<int> GetSampleRate() const;

  /// Sets the sampling rate.
  ///
  /// @param sample_rate Sampling rate in Hz.
  Status SetSampleRate(int sample_rate);

  /// Updates the internal state at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status Update(double timestamp);

 private:
  // API handle.
  BarelyApi api_;
};

ParamDefinition::ParamDefinition(float default_value, float min_value,
                                 float max_value)
    : default_value(default_value),
      min_value(min_value),
      max_value(max_value) {}

ParamDefinition::ParamDefinition(bool default_value)
    : ParamDefinition(static_cast<float>(default_value)) {}

ParamDefinition::ParamDefinition(int default_value, int min_value,
                                 int max_value)
    : ParamDefinition(static_cast<float>(default_value),
                      static_cast<float>(min_value),
                      static_cast<float>(max_value)) {}

template <typename ValueType>
StatusOr<ValueType>::StatusOr(Status error_status) : value_or_(error_status) {
  assert(error_status != Status::kOk);
}

template <typename ValueType>
StatusOr<ValueType>::StatusOr(ValueType value) : value_or_(std::move(value)) {}

template <typename ValueType>
Status StatusOr<ValueType>::GetErrorStatus() const {
  assert(std::holds_alternative<Status>(value_or_));
  return std::get<Status>(value_or_);
}

template <typename ValueType>
ValueType& StatusOr<ValueType>::GetValue() {
  assert(std::holds_alternative<ValueType>(value_or_));
  return std::get<ValueType>(value_or_);
}

template <typename ValueType>
const ValueType& StatusOr<ValueType>::GetValue() const {
  assert(std::holds_alternative<ValueType>(value_or_));
  return std::get<ValueType>(value_or_);
}

template <typename ValueType>
bool StatusOr<ValueType>::IsOk() const {
  return std::holds_alternative<ValueType>(value_or_);
}

Api::Api() {
  const auto status = BarelyApi_Create(&api_);
  assert(status == BarelyStatus_kOk);
}

Api::Api(int sample_rate) : Api() {
  const auto status = BarelyApi_SetSampleRate(api_, sample_rate);
  assert(status == BarelyStatus_kOk);
}

Api::~Api() {
  const auto status = BarelyApi_Destroy(api_);
  assert(status == BarelyStatus_kOk);
}

StatusOr<int> Api::GetSampleRate() const {
  int sample_rate = 0;
  if (const auto status = BarelyApi_GetSampleRate(api_, &sample_rate);
      status != BarelyStatus_kOk) {
    return static_cast<Status>(status);
  }
  return sample_rate;
}

Status Api::SetSampleRate(int sample_rate) {
  return static_cast<Status>(BarelyApi_SetSampleRate(api_, sample_rate));
}

Status Api::Update(double timestamp) {
  return static_cast<Status>(BarelyApi_Update(api_, timestamp));
}

}  // namespace barely

#endif PLATFORMS_API_BARELYMUSICIAN_H_
