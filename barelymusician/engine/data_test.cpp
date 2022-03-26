#include "barelymusician/engine/data.h"

#include <string>
#include <utility>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::ElementsAre;

namespace barelyapi {
namespace {

// Tests that data stores simple data as expected.
TEST(DataTest, SimpleData) {
  int value = 4;
  const Data data(Data::Definition(
      {[](void* other_data, void** out_data) {
         *out_data =
             static_cast<void*>(new int(*static_cast<int*>(other_data)));
       },
       [](void* this_data) { delete static_cast<int*>(this_data); }, &value}));
  EXPECT_EQ(*static_cast<int*>(data.GetMutable()), 4);
}

// Tests that data stores complex data as expected.
TEST(DataTest, ComplexData) {
  std::vector<std::string> value = {"test", "data"};
  const Data data(Data::Definition(
      {[](void* other_data, void** out_data) {
         *out_data = static_cast<void*>(new std::vector<std::string>(
             std::move(*static_cast<std::vector<std::string>*>(other_data))));
       },
       [](void* this_data) {
         delete static_cast<std::vector<std::string>*>(this_data);
       },
       &value}));
  EXPECT_THAT(*static_cast<std::vector<std::string>*>(data.GetMutable()),
              ElementsAre("test", "data"));
}

}  // namespace
}  // namespace barelyapi
