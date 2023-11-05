#include "barelymusician/internal/event.h"

#include "barelymusician/barelymusician.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

// Tests that the event gets processed as expected.
TEST(EventTest, Process) {
  constexpr int kTotalProcessCount = 3;

  struct TestData {
    int create_count = 0;
    int destroy_count = 0;
    int process_count = 0;
    double pitch = 0.0;
  };
  TestData test_data = {};

  EXPECT_EQ(test_data.create_count, 0);
  EXPECT_EQ(test_data.destroy_count, 0);
  EXPECT_EQ(test_data.process_count, 0);
  EXPECT_DOUBLE_EQ(test_data.pitch, 0.0);

  {
    Event<NoteOffEventDefinition, double> event(
        NoteOffEventDefinition{
            [](void** state, void* user_data) {
              *state = user_data;
              ++static_cast<TestData*>(*state)->create_count;
            },
            [](void** state) { ++static_cast<TestData*>(*state)->destroy_count; },
            [](void** state, double pitch) {
              auto& test_data = *static_cast<TestData*>(*state);
              ++test_data.process_count;
              test_data.pitch = pitch;
            },
        },
        static_cast<void*>(&test_data));

    // Event should be created.
    EXPECT_EQ(test_data.create_count, 1);
    EXPECT_EQ(test_data.destroy_count, 0);
    EXPECT_EQ(test_data.process_count, 0);
    EXPECT_DOUBLE_EQ(test_data.pitch, 0.0);

    for (int i = 1; i <= kTotalProcessCount; ++i) {
      event.Process(static_cast<double>(i));

      // Event should be processed.
      EXPECT_EQ(test_data.create_count, 1);
      EXPECT_EQ(test_data.destroy_count, 0);
      EXPECT_EQ(test_data.process_count, i);
      EXPECT_DOUBLE_EQ(test_data.pitch, static_cast<double>(i));
    }
  }

  // Event should be destroyed.
  EXPECT_EQ(test_data.create_count, 1);
  EXPECT_EQ(test_data.destroy_count, 1);
  EXPECT_EQ(test_data.process_count, kTotalProcessCount);
  EXPECT_DOUBLE_EQ(test_data.pitch, static_cast<double>(kTotalProcessCount));
}

}  // namespace
}  // namespace barely::internal
