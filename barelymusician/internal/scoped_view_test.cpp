#include "barelymusician/internal/scoped_view.h"

#include <atomic>

#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

using ::testing::IsNull;

// Tests that an observable value can be observed by a single observer as expected.
TEST(ScopedView, CreateDestroy) {
  int data = 5;
  std::atomic<int*> data_ptr = &data;

  {
    ScopedView view(data_ptr);
    EXPECT_THAT(data_ptr, IsNull());
    EXPECT_EQ(*view, data);

    *view = 10;
    EXPECT_EQ(data, 10);
    EXPECT_EQ(*view, 10);
  }

  EXPECT_EQ(data_ptr, &data);
  EXPECT_EQ(data, 10);
}

}  // namespace
}  // namespace barely::internal
