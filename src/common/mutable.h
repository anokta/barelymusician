#ifndef BARELYMUSICIAN_COMMON_MUTABLE_H_
#define BARELYMUSICIAN_COMMON_MUTABLE_H_

#include <atomic>
#include <memory>
#include <utility>

namespace barely {

/// Mutable data template with a real-time safe view.
template <typename DataType>
class Mutable {
 public:
  /// Returns scoped view to data.
  ///
  /// @return Scoped immutable view to data.
  auto GetScopedView() noexcept { return ScopedView(data_); }

  /// Updates data.
  ///
  /// @param new_data New data.
  void Update(DataType new_data) noexcept {
    auto new_data_holder = std::make_unique<DataType>(std::move(new_data));
    for (auto* data = data_holder_.get(); !data_.compare_exchange_weak(
             data, new_data_holder.get(), std::memory_order_release, std::memory_order_relaxed);) {
      data = data_holder_.get();
    }
    data_holder_ = std::move(new_data_holder);
  }

 private:
  // Scoped immutable view to data.
  class ScopedView {
   public:
    // Constructs new `ScopedView` with `data`.
    explicit ScopedView(std::atomic<DataType*>& data) noexcept
        : data_(data), view_(data_.exchange(nullptr)) {}

    // Destroys `ScopedView`.
    ~ScopedView() noexcept { data_ = view_; }

    // Non-copyable and non-movable.
    ScopedView(const ScopedView& other) noexcept = delete;
    ScopedView& operator=(const ScopedView& other) noexcept = delete;
    ScopedView(ScopedView&& other) noexcept = delete;
    ScopedView& operator=(ScopedView&& other) noexcept = delete;

    // Member access operators.
    const DataType& operator*() const noexcept { return *view_; }
    const DataType* operator->() const noexcept { return view_; }

   private:
    // Pointer to data.
    std::atomic<DataType*>& data_;

    // Pointer to view.
    DataType* view_;
  };

  // Data holder.
  std::unique_ptr<DataType> data_holder_ = std::make_unique<DataType>();

  // Pointer to data.
  std::atomic<DataType*> data_ = data_holder_.get();
};

}  // namespace barely

#endif  // BARELYMUSICIAN_COMMON_MUTABLE_H_
