#include "barelymusician/conductor/find_or_null.h"

#include <unordered_map>

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that querying a value of a const map returns the expected reference.
TEST(FindOrNullTest, FindOrNullConstMap) {
  const std::unordered_map<int, double> kConstMap = {{1, 10.0}, {2, 20.0}};

  EXPECT_EQ(FindOrNull(kConstMap, 0), nullptr);
  EXPECT_DOUBLE_EQ(*FindOrNull(kConstMap, 1), 10.0);

  const double* const_value = FindOrNull(kConstMap, 2);
  EXPECT_DOUBLE_EQ(*const_value, 20.0);
}

// Tests that querying a value of a mutable map returns the expected reference.
TEST(FindOrNullTest, FindOrNullMutableMap) {
  std::unordered_map<int, double> mutable_map;
  mutable_map.emplace(3, 30.0);
  mutable_map.emplace(5, 50.0);

  EXPECT_DOUBLE_EQ(*FindOrNull(mutable_map, 3), 30.0);
  EXPECT_EQ(FindOrNull(mutable_map, 4), nullptr);

  double* mutable_value = FindOrNull(mutable_map, 5);
  EXPECT_DOUBLE_EQ(*mutable_value, 50.0);
}

}  // namespace
}  // namespace barelyapi
