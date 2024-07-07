#include "barelymusician/internal/task.h"

#include "barelymusician/barelymusician.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

// Tests that the task is processed as expected.
TEST(TaskTest, Process) {
  static int task_create_count = 0;
  static int task_destroy_count = 0;
  int task_process_count = 0;
  auto definition = TaskDefinition{
      [](void** state, void* user_data) {
        ++task_create_count;
        *state = user_data;
      },
      [](void** /*state*/) { ++task_destroy_count; },
      [](void** state) {
        int& count = *static_cast<int*>(*state);
        ++count;
      },
  };

  EXPECT_EQ(task_create_count, 0);
  EXPECT_EQ(task_destroy_count, 0);
  EXPECT_EQ(task_process_count, 0);

  {
    Task task(
        definition, 1.0, 2, &task_process_count,
        [](Task* task, double position) { EXPECT_NE(task->GetPosition(), position); },
        [](Task* task, int process_order) { EXPECT_NE(task->GetProcessOrder(), process_order); });

    EXPECT_EQ(task_create_count, 1);
    EXPECT_EQ(task_destroy_count, 0);
    EXPECT_EQ(task_process_count, 0);

    EXPECT_DOUBLE_EQ(task.GetPosition(), 1.0);
    EXPECT_EQ(task.GetProcessOrder(), 2);

    task.SetPosition(-1.0);
    EXPECT_DOUBLE_EQ(task.GetPosition(), -1.0);

    task.SetProcessOrder(10);
    EXPECT_EQ(task.GetProcessOrder(), 10);

    for (int i = 1; i <= 5; ++i) {
      task.Process();
      EXPECT_EQ(task_process_count, i);
    }
  }

  EXPECT_EQ(task_create_count, 1);
  EXPECT_EQ(task_process_count, 5);
  EXPECT_EQ(task_destroy_count, 1);
}

}  // namespace
}  // namespace barely::internal
