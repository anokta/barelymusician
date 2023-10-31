#include "barelymusician/internal/status_or.h"

#include "gtest/gtest.h"

namespace barely::internal {
namespace {

// Tests that the value gets returned as expected.
TEST(StatusOrTest, StatusOrValue) {
  StatusOr<int> status_or(6);
  EXPECT_TRUE(status_or.IsOk());
  EXPECT_EQ(*status_or, 6);
}

// Tests that the error gets returned as expected.
TEST(StatusOrTest, StatusOrError) {
  StatusOr<int> status_or(Status::Internal());
  EXPECT_FALSE(status_or.IsOk());
  EXPECT_EQ(status_or.GetErrorStatus(), Status::kInternal);
}

}  // namespace
}  // namespace barely::internal
