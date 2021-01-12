#include "barelymusician/base/id_generator.h"

#include <unordered_set>

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that all generated ids are unique for arbitrary number of calls.
TEST(IdGeneratorTest, Next) {
  IdGenerator id_generator;
  std::unordered_set<int> ids;
  for (int i = 0; i < 100; ++i) {
    EXPECT_TRUE(ids.emplace(id_generator.Next()).second);
  }
}

}  // namespace
}  // namespace barelyapi
