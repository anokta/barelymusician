#include "barelymusician/common/id_generator.h"

#include <unordered_set>

#include "barelymusician/common/id.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

// Tests that all generated ids are unique for an arbitrary number of calls.
TEST(IdGeneratorTest, Generate) {
  IdGenerator id_generator;
  std::unordered_set<Id> generated_ids;
  for (int i = 0; i < 100; ++i) {
    EXPECT_TRUE(generated_ids.emplace(id_generator.Generate()).second);
  }
}

}  // namespace
}  // namespace barely
