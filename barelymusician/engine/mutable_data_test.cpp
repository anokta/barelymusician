#include "barelymusician/engine/mutable_data.h"

#include <algorithm>
#include <memory>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

using ::testing::ElementsAre;

// Tests that the mutable data manages simple data as expected.
TEST(MutableDataTest, SimpleData) {
  MutableData<int> data;
  EXPECT_EQ(*data.GetScopedView(), 0);

  data.Update(5);
  EXPECT_EQ(*data.GetScopedView(), 5);

  data.Update(-10);
  const auto view = data.GetScopedView();
  EXPECT_EQ(*view, -10);
}

// Tests that the mutable data manages complex data as expected.
TEST(MutableDataTest, ComplexData) {
  MutableData<std::vector<std::string>> data;
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
