#ifndef BARELYMUSICIAN_INTERNAL_MUTABLE_DATA_H_
#define BARELYMUSICIAN_INTERNAL_MUTABLE_DATA_H_

#include <atomic>
#include <memory>
#include <utility>

#include "barelymusician/internal/scoped_view.h"

namespace barely::internal {

/// Mutable data with a real-time safe view.
template <typename DataType>
class MutableData {
 public:
  /// Returns scoped view to data.
  ///
  /// @return Scoped immutable view to data.
  auto GetScopedView() noexcept;

  /// Updates data.
  ///
  /// @param new_data New data.
  void Update(DataType new_data) noexcept;

 private:
  // Data holder.
  std::unique_ptr<DataType> data_holder_ = std::make_unique<DataType>();

  // Pointer to data.
  std::atomic<DataType*> data_ = data_holder_.get();
};

template <typename DataType>
auto MutableData<DataType>::GetScopedView() noexcept {
  return ScopedView(data_);
}

template <typename DataType>
void MutableData<DataType>::Update(DataType new_data) noexcept {
  auto new_data_holder = std::make_unique<DataType>(std::move(new_data));
  for (auto* data = data_holder_.get(); !data_.compare_exchange_weak(
           data, new_data_holder.get(), std::memory_order_release, std::memory_order_relaxed);) {
    data = data_holder_.get();
  }
  data_holder_ = std::move(new_data_holder);
}

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_MUTABLE_DATA_H_
