#include "barelymusician/engine/task.h"

#include "gtest/gtest.h"
#include "barelymusician/engine/number.h"

namespace barely::internal {
namespace {

TEST(TaskTest, Process) {
  const Integer kTotalProcessCount = 3;

  struct TestData {
    Integer create_count = 0;
    Integer destroy_count = 0;
    Integer process_count = 0;
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

    EXPECT_EQ(test_data.create_count, 1);
    EXPECT_EQ(test_data.destroy_count, 0);
    EXPECT_EQ(test_data.process_count, 0);

    for (Integer i = 1; i <= kTotalProcessCount; ++i) {
      task.Process();

      EXPECT_EQ(test_data.create_count, 1);
      EXPECT_EQ(test_data.destroy_count, 0);
      EXPECT_EQ(test_data.process_count, i);
    }
  }

  EXPECT_EQ(test_data.create_count, 1);
  EXPECT_EQ(test_data.destroy_count, 1);
  EXPECT_EQ(test_data.process_count, kTotalProcessCount);
}

}  // namespace
}  // namespace barely::internal
