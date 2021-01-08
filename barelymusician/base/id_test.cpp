#include "barelymusician/base/id.h"

#include <unordered_set>

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that all generated ids are unique for arbitrary number of calls.
TEST(IdTest, GetNextId) {
  ResetIdCount();
  std::unordered_set<int> ids;
  for (int i = 0; i < 100; ++i) {
    EXPECT_TRUE(ids.emplace(GetNextId()).second);
  }
}

}  // namespace
}  // namespace barelyapi
