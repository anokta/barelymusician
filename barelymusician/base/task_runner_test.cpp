#include "barelymusician/base/task_runner.h"

#include <functional>
#include <mutex>
#include <thread>
#include <vector>

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that adding a single synchronous task gets run as expected.
TEST(TaskRunnerTest, RunSingleTask) {
  const int kNumMaxTasks = 10;
  const int kInitialValue = 1;
  const int kExpectedValue = 5;

  TaskRunner task_runner(kNumMaxTasks);

  int value = kInitialValue;
  const auto task = [&value, kExpectedValue]() { value = kExpectedValue; };

  task_runner.Add(task);
  EXPECT_EQ(value, kInitialValue);

  task_runner.Run();
  EXPECT_EQ(value, kExpectedValue);
}

// Tests that adding multiple tasks from concurrent threads gets run in order.
TEST(TaskRunnerTest, RunMultipleTasksConcurrently) {
  const int kNumProducers = 10;
  const int kProducerDelayMs = 50;
  const int kConsumerDelayMs = 20;

  std::mutex values_mutex;
  std::vector<int> values;
  values.reserve(kNumProducers);
  const auto push_value = [&values_mutex, &values](int i) {
    std::lock_guard<std::mutex> lock(values_mutex);
    values.emplace_back(i);
  };

  TaskRunner task_runner(kNumProducers);

  // Run producer threads.
  std::mutex producer_mutex;
  std::vector<std::thread> producers;
  int value = 0;
  for (int i = 0; i < kNumProducers; ++i) {
    const auto produce = [&producer_mutex, &task_runner, push_value, &value,
                          kProducerDelayMs]() {
      std::lock_guard<std::mutex> lock(producer_mutex);
      std::this_thread::sleep_for(std::chrono::milliseconds(kProducerDelayMs));
      task_runner.Add(std::bind(push_value, value++));
    };
    producers.emplace_back(produce);
  }
  EXPECT_EQ(values.size(), 0);
  // Run consumer thread.
  const auto consume = [&task_runner, &values, kNumProducers,
                        kConsumerDelayMs]() {
    while (values.size() < kNumProducers) {
      task_runner.Run();
      std::this_thread::sleep_for(std::chrono::milliseconds(kConsumerDelayMs));
    }
  };
  std::thread consumer(consume);
  // Wait for all threads to complete.
  for (int i = 0; i < kNumProducers; ++i) {
    if (producers[i].joinable()) {
      producers[i].join();
    }
  }
  if (consumer.joinable()) {
    consumer.join();
  }

  ASSERT_EQ(values.size(), kNumProducers);
  for (int i = 0; i < kNumProducers; ++i) {
    EXPECT_EQ(values[i], i);
  }
}

// Tests that adding more tasks than the maximum capacity results in dropping
// the exceeding tasks as expected.
TEST(TaskRunnerTest, RunTasksExeedingMaxSize) {
  const int kMaxSize = 1;
  const int kNumTasksToAdd = 4;
  const int kNumTaskRuns = 2;

  TaskRunner task_runner(kMaxSize);

  // Verify that number of tasks to add exceeds the capacity.
  EXPECT_GT(kNumTasksToAdd, kMaxSize);

  int counter = 0;
  const auto task = [&counter]() { ++counter; };
  for (int n = 0; n < kNumTaskRuns; ++n) {
    for (int i = 0; i < kNumTasksToAdd; ++i) {
      task_runner.Add(task);
    }
    // Verify that tasks added after |kNumMaxTasks| are dropped.
    task_runner.Run();
    EXPECT_EQ(counter, (n + 1) * kMaxSize);
  }
}

}  // namespace
}  // namespace barelyapi
