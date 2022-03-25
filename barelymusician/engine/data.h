#ifndef BARELYMUSICIAN_ENGINE_DATA_H_
#define BARELYMUSICIAN_ENGINE_DATA_H_

#include "barelymusician/barelymusician.h"

namespace barelyapi {

/// Class that wraps instrument data.
class Data {
 public:
  /// Definition alias.
  using Definition = barely::DataDefinition;

  /// Constructs new `Data`.
  ///
  /// @param definition Data definition.
  explicit Data(Definition definition) noexcept;

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
  Definition::MoveCallback move_callback_ = nullptr;

  // Destroy callback.
  Definition::DestroyCallback destroy_callback_ = nullptr;

  // Internal data.
  void* data_ = nullptr;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_DATA_H_
