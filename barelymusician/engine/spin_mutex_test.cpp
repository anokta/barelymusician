#include "barelymusician/engine/spin_mutex.h"

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
  const int kNumThreads = 10;
  const int kThreadDelayMs = 50;

  SpinMutex mutex;

  std::vector<std::thread> threads;
  std::vector<int> values;
  for (int i = 0; i < kNumThreads; ++i) {
    threads.emplace_back([&, i]() {
      if (i == 0) {
        EXPECT_TRUE(mutex.try_lock());
      } else {
        ASSERT_FALSE(mutex.try_lock());
        mutex.lock();
      }
      values.push_back(i);
      std::this_thread::sleep_for(std::chrono::milliseconds(kThreadDelayMs));
      mutex.unlock();
    });
  }

  for (int i = 0; i < kNumThreads; ++i) {
    if (threads[i].joinable()) {
      threads[i].join();
    }
  }
  EXPECT_EQ(values.size(), kNumThreads);
  EXPECT_EQ(values.front(), 0);

  EXPECT_TRUE(mutex.try_lock());
  EXPECT_FALSE(mutex.try_lock());
}

}  // namespace
}  // namespace barelyapi
