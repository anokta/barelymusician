#ifndef BARELYMUSICIAN_INSTRUMENT_DATA_H_
#define BARELYMUSICIAN_INSTRUMENT_DATA_H_

#include "barelymusician/api/instrument.h"

namespace barelyapi {

/// Class that wraps instrument data.
class Data {
 public:
  /// Constructs new `Data`.
  ///
  /// @param definition Data definition.
  explicit Data(barely::DataDefinition definition) noexcept;

  /// Default constructor.
  Data() = default;

  /// Destroys `Data`.
  ~Data() noexcept;

  /// Non-copyable.
  Data(const Data& other) = delete;
  Data& operator=(const Data& other) = delete;

  /// Constructs new `Data` via move.
  ///
  /// @param other Other data.
  Data(Data&& other) noexcept;

  /// Assigns `Data` via move.
  ///
  /// @param other Other data.
  Data& operator=(Data&& other) noexcept;

  /// Returns mutable data.
  ///
  /// @return Mutable data.
  [[nodiscard]] void* GetMutable() const noexcept;

 private:
  // Move callback.
  barely::DataDefinition::MoveCallback move_callback_ = nullptr;

  // Destroy callback.
  barely::DataDefinition::DestroyCallback destroy_callback_ = nullptr;

  // Internal data.
  void* data_ = nullptr;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_DATA_H_
