#ifndef BARELYMUSICIAN_INTERNAL_SCOPED_VIEW_H_
#define BARELYMUSICIAN_INTERNAL_SCOPED_VIEW_H_

#include <atomic>

namespace barely::internal {

// Scoped exclusive view of atomic pointer to data.
template <typename DataType>
class ScopedView {
 public:
  // Constructs a new `ScopedView` with `data`.
  explicit ScopedView(std::atomic<DataType*>& data) noexcept;

  // Destroys `ScopedView`.
  ~ScopedView() noexcept;

  // Non-copyable and non-movable.
  ScopedView(const ScopedView& other) noexcept = delete;
  ScopedView& operator=(const ScopedView& other) noexcept = delete;
  ScopedView(ScopedView&& other) noexcept = delete;
  ScopedView& operator=(ScopedView&& other) noexcept = delete;

  // Member access operators.
  DataType& operator*() noexcept;
  DataType* operator->() noexcept;
  const DataType& operator*() const noexcept;
  const DataType* operator->() const noexcept;

 private:
  // Pointer reference to data.
  std::atomic<DataType*>& data_;

  // Pointer to view.
  DataType* view_;
};

template <typename DataType>
ScopedView<DataType>::ScopedView(std::atomic<DataType*>& data) noexcept
    : data_(data), view_(data_.exchange(nullptr)) {}

template <typename DataType>
ScopedView<DataType>::~ScopedView() noexcept {
  data_ = view_;
}

template <typename DataType>
DataType& ScopedView<DataType>::operator*() noexcept {
  return *view_;
}

template <typename DataType>
DataType* ScopedView<DataType>::operator->() noexcept {
  return view_;
}

template <typename DataType>
const DataType& ScopedView<DataType>::operator*() const noexcept {
  return *view_;
}

template <typename DataType>
const DataType* ScopedView<DataType>::operator->() const noexcept {
  return view_;
}

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_SCOPED_VIEW_H_
