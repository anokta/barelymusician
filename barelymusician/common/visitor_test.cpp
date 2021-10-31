#include "barelymusician/common/visitor.h"

#include <variant>
#include <vector>

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that all variants are visited when set with an auto definition.
TEST(VisitorTest, AutoVisit) {
  std::vector<std::variant<bool, int, double>> variants = {bool{true}, int{3},
                                                           double{6.0}};
  int result = 0;
  for (const auto& variant : variants) {
    std::visit(
        Visitor{[&](const auto& value) { result += static_cast<int>(value); }},
        variant);
  }
  EXPECT_EQ(result, 10);
}

// Tests that all variants are visited when set with explicit definitions.
TEST(VisitorTest, ExplicitVisit) {
  std::vector<std::variant<bool, int, double>> variants = {bool{true}, int{3},
                                                           double{6.0}};
  int result = 0;
  for (const auto& variant : variants) {
    std::visit(Visitor{[&](bool bool_value) {
                         result += static_cast<int>(bool_value);
                       },
                       [&](int int_value) { result += int_value; },
                       [&](double double_value) {
                         result += static_cast<int>(double_value);
                       }},
               variant);
  }
  EXPECT_EQ(result, 10);
}

}  // namespace
}  // namespace barelyapi
