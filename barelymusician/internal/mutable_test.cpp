#include "barelymusician/internal/mutable.h"

#include <string>
#include <vector>

#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

using ::testing::ElementsAre;

// Tests that mutable manages simple data as expected.
TEST(MutableTest, SimpleData) {
  Mutable<int> data;
  EXPECT_EQ(*data.GetScopedView(), 0);

  data.Update(5);
  EXPECT_EQ(*data.GetScopedView(), 5);

  data.Update(-10);
  const auto view = data.GetScopedView();
  EXPECT_EQ(*view, -10);
}

// Tests that mutable manages complex data as expected.
TEST(MutableTest, ComplexData) {
  Mutable<std::vector<std::string>> data;
  EXPECT_TRUE(data.GetScopedView()->empty());

  data.Update({"test", "data"});
  EXPECT_THAT(*data.GetScopedView(), ElementsAre("test", "data"));

  data.Update({"other"});
  {
    const auto view = data.GetScopedView();
    EXPECT_THAT(*view, ElementsAre("other"));
  }

  data.Update({});
  EXPECT_TRUE(data.GetScopedView()->empty());
}

}  // namespace
}  // namespace barely::internal
