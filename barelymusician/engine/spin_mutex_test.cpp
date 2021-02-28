#include "barelymusician/engine/spin_mutex.h"

#include <algorithm>
#include <mutex>
#include <thread>
#include <vector>

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that the mutex works as expected when called by a single thread.
TEST(SpinMutexTest, SingleThread) {
  SpinMutex mutex;

  EXPECT_TRUE(mutex.try_lock());
  EXPECT_FALSE(mutex.try_lock());

  mutex.unlock();
  EXPECT_TRUE(mutex.try_lock());

  mutex.unlock();
  mutex.lock();
  EXPECT_FALSE(mutex.try_lock());
}

// Tests that the mutex works as expected when called by multiple threads.
TEST(SpinMutexTest, MultipleThreads) {
  const int kNumThreads = 100;

  SpinMutex mutex;

  std::vector<std::thread> threads;
  std::vector<int> values;
  for (int i = 0; i < kNumThreads; ++i) {
    threads.emplace_back([&, i]() {
      std::lock_guard lock(mutex);
      EXPECT_FALSE(mutex.try_lock());
      values.insert(std::lower_bound(values.begin(), values.end(), i), i);
    });
  }

  for (int i = 0; i < kNumThreads; ++i) {
    if (threads[i].joinable()) {
      threads[i].join();
    }
  }

  EXPECT_EQ(values.size(), kNumThreads);
  for (int i = 0; i < kNumThreads; ++i) {
    EXPECT_EQ(values[i], i);
  }

  EXPECT_TRUE(mutex.try_lock());
  EXPECT_FALSE(mutex.try_lock());
}

}  // namespace
}  // namespace barelyapi
