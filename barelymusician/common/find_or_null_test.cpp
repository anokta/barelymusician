#include "barelymusician/common/find_or_null.h"

#include <unordered_map>

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that querying a value of a const map returns the expected reference.
TEST(FindOrNullTest, FindOrNullConstMap) {
  const std::unordered_map<int, float> kConstMap = {{1, 10.0f}, {2, 20.0f}};

  EXPECT_EQ(FindOrNull(kConstMap, 0), nullptr);
  EXPECT_FLOAT_EQ(*FindOrNull(kConstMap, 1), 10.0f);

  const float* const_value = FindOrNull(kConstMap, 2);
  EXPECT_FLOAT_EQ(*const_value, 20.0f);
}

// Tests that querying a value of a mutable map returns the expected reference.
TEST(FindOrNullTest, FindOrNullMutableMap) {
  std::unordered_map<int, float> mutable_map;
  mutable_map.emplace(3, 30.0f);
  mutable_map.emplace(5, 50.0f);

  EXPECT_FLOAT_EQ(*FindOrNull(mutable_map, 3), 30.0f);
  EXPECT_EQ(FindOrNull(mutable_map, 4), nullptr);

  float* mutable_value = FindOrNull(mutable_map, 5);
  EXPECT_FLOAT_EQ(*mutable_value, 50.0f);
}

}  // namespace
}  // namespace barelyapi
