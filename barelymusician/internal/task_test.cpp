#include "barelymusician/internal/task.h"

#include "barelymusician/barelymusician.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

// Tests that the task gets processed as expected.
TEST(TaskTest, Process) {
  constexpr int kTotalProcessCount = 3;

  struct TestData {
    int create_count = 0;
    int destroy_count = 0;
    int process_count = 0;
  };
  TestData test_data = {};

  EXPECT_EQ(test_data.create_count, 0);
  EXPECT_EQ(test_data.destroy_count, 0);
  EXPECT_EQ(test_data.process_count, 0);

  {
    Task task(TaskDefinition{
                  [](void** state, void* user_data) {
                    *state = user_data;
                    ++static_cast<TestData*>(*state)->create_count;
                  },
                  [](void** state) {
                    ++static_cast<TestData*>(*state)->destroy_count;
                  },
                  [](void** state) {
                    ++static_cast<TestData*>(*state)->process_count;
                  },
              },
              static_cast<void*>(&test_data));

    // Task should be created.
    EXPECT_EQ(test_data.create_count, 1);
    EXPECT_EQ(test_data.destroy_count, 0);
    EXPECT_EQ(test_data.process_count, 0);

    for (int i = 1; i <= kTotalProcessCount; ++i) {
      task.Process();

      // Task should be processed.
      EXPECT_EQ(test_data.create_count, 1);
      EXPECT_EQ(test_data.destroy_count, 0);
      EXPECT_EQ(test_data.process_count, i);
    }
  }

  // Task should be destroyed.
  EXPECT_EQ(test_data.create_count, 1);
  EXPECT_EQ(test_data.destroy_count, 1);
  EXPECT_EQ(test_data.process_count, kTotalProcessCount);
}

}  // namespace
}  // namespace barely::internal
