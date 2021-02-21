#ifndef BARELYMUSICIAN_ENGINE_SPIN_MUTEX_H_
#define BARELYMUSICIAN_ENGINE_SPIN_MUTEX_H_

#include <atomic>

namespace barelyapi {

/// Simple spin mutex.
///
/// @note Function names are set to conform |Lockable| type requirements.
class SpinMutex {
 public:
  void lock() noexcept {
    while (!try_lock()) {
      // Spin until lock is acquired.
    }
  }

  bool try_lock() noexcept {
    return !lock_.test_and_set(std::memory_order_acquire);
  }

  void unlock() noexcept { lock_.clear(std::memory_order_release); }

 private:
  std::atomic_flag lock_ = ATOMIC_FLAG_INIT;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_SPIN_MUTEX_H_
