#ifndef BARELYMUSICIAN_INTERNAL_OBSERVABLE_H_
#define BARELYMUSICIAN_INTERNAL_OBSERVABLE_H_

#include <cassert>
#include <memory>
#include <new>
#include <utility>

namespace barely::internal {

/// Observable template.
///
/// `Observable` and `Observer` essentially mimic a pair of `std::shared_ptr` and `std::weak_ptr`
/// with the following simplifications to limit behavior:
/// * `Observable` is the sole maintainer of the data, acting more similar to `std::unique_ptr`.
/// * `Observable` and `Observer` are *not* thread-safe.
/// * `Observer` does not "lock" when accessing the data, since no synchronization is needed.
template <typename DataType>
class Observer;
template <typename DataType>
class Observable {
 public:
  /// Constructs a new `Observable`.
  ///
  /// @param args Arguments.
  template <typename... Args>
  explicit Observable(Args... args) noexcept;

  /// Destroys `Observable`.
  ~Observable() noexcept;

  /// Non-copyable.
  Observable(const Observable& other) noexcept = delete;
  Observable& operator=(const Observable& other) noexcept = delete;

  /// Movable.
  Observable(Observable&& other) noexcept;
  Observable& operator=(Observable&& other) noexcept;

  /// Member access operators.
  DataType& operator*() const noexcept;
  DataType* operator->() const noexcept;
  DataType* get() const noexcept;

  /// Returns a new observer.
  ///
  /// @return Observer.
  Observer<DataType> Observe() const noexcept;

 private:
  // Ensures that `View` is only visible to `Observer`.
  template <typename T>
  friend class Observer;

  // Observable view.
  struct View {
    // Raw data.
    std::unique_ptr<DataType> data = nullptr;

    // Number of observers.
    std::unique_ptr<int> observer_count = nullptr;
  };
  View* view_ = nullptr;
};

/// Observer.
template <typename DataType>
class Observer {
 public:
  /// Destroys `Observer`.
  ~Observer() noexcept;

  /// Non-copyable.
  Observer(const Observer& other) noexcept = delete;
  Observer& operator=(const Observer& other) noexcept = delete;

  /// Movable.
  Observer(Observer&& other) noexcept;
  Observer& operator=(Observer&& other) noexcept;

  /// Member access operators.
  DataType& operator*() const noexcept;
  DataType* operator->() const noexcept;
  DataType* get() const noexcept { return view_->data.get(); }

  /// Returns whether the observed data is valid or not.
  ///
  /// @return True if valid, false otherwise.
  operator bool() const noexcept;

 private:
  // Ensures that `Observer` can only be created by `Observable`.
  template <typename T>
  friend class Observable;

  // Observable view alias.
  using View = typename Observable<DataType>::View;

  // Constructs a new `Observer` with a given `view`.
  explicit Observer(View* view) noexcept;

  // Observable view.
  const View* view_ = nullptr;
};

template <typename DataType>
template <typename... Args>
Observable<DataType>::Observable(Args... args) noexcept
    : view_(new(std::nothrow) View{std::make_unique<DataType>(args...), std::make_unique<int>(0)}) {
}

template <typename DataType>
Observable<DataType>::~Observable() noexcept {
  assert(!view_ || view_->data);
  if (view_) {
    view_->data.reset();
    if (*view_->observer_count == 0) {
      delete view_;
      view_ = nullptr;
    }
  }
}

template <typename DataType>
Observable<DataType>::Observable(Observable<DataType>&& other) noexcept
    : view_(std::exchange(other.view_, nullptr)) {}

template <typename DataType>
Observable<DataType>& Observable<DataType>::operator=(Observable<DataType>&& other) noexcept {
  if (*this != other) {
    view_ = std::exchange(other.view_, nullptr);
  }
  return *this;
}

template <typename DataType>
DataType& Observable<DataType>::operator*() const noexcept {
  assert(view_);
  assert(view_->data);
  return *view_->data;
}

template <typename DataType>
DataType* Observable<DataType>::operator->() const noexcept {
  assert(view_);
  assert(view_->data);
  return view_->data.get();
}

template <typename DataType>
DataType* Observable<DataType>::get() const noexcept {
  return view_ ? view_->data.get() : nullptr;
}

template <typename DataType>
Observer<DataType> Observable<DataType>::Observe() const noexcept {
  return Observer<DataType>(view_);
}

template <typename DataType>
Observer<DataType>::~Observer() noexcept {
  assert(!view_ || view_->observer_count);
  if (view_ && --(*view_->observer_count) == 0 && !view_->data) {
    delete view_;
    view_ = nullptr;
  }
}

template <typename DataType>
Observer<DataType>::Observer(Observer<DataType>&& other) noexcept
    : view_(std::exchange(other.view_, nullptr)) {}

template <typename DataType>
Observer<DataType>& Observer<DataType>::operator=(Observer<DataType>&& other) noexcept {
  if (*this != other) {
    view_ = std::exchange(other.view_, nullptr);
  }
  return *this;
}

template <typename DataType>
Observer<DataType>::operator bool() const noexcept {
  return view_ && view_->data;
}

template <typename DataType>
DataType& Observer<DataType>::operator*() const noexcept {
  assert(view_);
  assert(view_->data);
  return *view_->data;
}

template <typename DataType>
DataType* Observer<DataType>::operator->() const noexcept {
  assert(view_);
  assert(view_->data);
  return view_->data.get();
}

template <typename DataType>
Observer<DataType>::Observer(View* view) noexcept : view_(view) {
  assert(view_);
  assert(view_->observer_count);
  ++(*view_->observer_count);
}

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_OBSERVABLE_H_
