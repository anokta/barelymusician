
#include "barelycomposer.h"

#include <array>

#include "gtest/gtest-param-test.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

/// Common note values in relation to quarter note beat duration.
constexpr double kQuarterNotesPerBeat = 1.0;
constexpr double kEighthNotesPerBeat = 2.0;
constexpr double kEighthTripletNotesPerBeat = 3.0;
constexpr double kSixteenthNotesPerBeat = 4.0;
constexpr double kSixteenthTripletNotesPerBeat = 6.0;
constexpr double kThirtySecondNotesPerBeat = 8.0;
constexpr double kThirtySecondTripletNotesPerBeat = 12.0;

class QuantizationTestWithParam : public testing::TestWithParam<double> {};

// Tests that the position gets quantized as expected with respect to the given resolution.
TEST_P(QuantizationTestWithParam, GetPosition) {
  constexpr double kPosition = 0.99;
  const Quantization quantization(1.0 / GetParam());
  EXPECT_DOUBLE_EQ(quantization.GetPosition(kPosition), 1.0);
  EXPECT_DOUBLE_EQ(quantization.GetPosition(1.0 - kPosition), 0.0);
}

INSTANTIATE_TEST_SUITE_P(QuantizationTest, QuantizationTestWithParam,
                         testing::Values(kQuarterNotesPerBeat, kEighthNotesPerBeat,
                                         kEighthTripletNotesPerBeat, kSixteenthNotesPerBeat,
                                         kSixteenthTripletNotesPerBeat, kThirtySecondNotesPerBeat,
                                         kThirtySecondTripletNotesPerBeat));

TEST(ScaleTest, GetPitch) {
  constexpr int kPitchCount = 5;
  const std::array<float, kPitchCount> kPitches = {0.0f, 0.2f, 0.35f, 0.5f, 0.95f};
  constexpr float kRootPitch = 1.75f;
  constexpr int kMode = 1;

  const Scale scale = {kPitches, kRootPitch, kMode};

  constexpr int kOctaveRange = 2;
  for (int octave = -kOctaveRange; octave <= kOctaveRange; ++octave) {
    for (int i = 0; i < kPitchCount; ++i) {
      const int degree = octave * kPitchCount + i;
      const float expected_pitch = kRootPitch +
                                   // NOLINTNEXTLINE(bugprone-integer-division)
                                   static_cast<float>(octave + (i + kMode) / kPitchCount) +
                                   kPitches[(i + kMode) % kPitchCount] - kPitches[kMode];
      EXPECT_FLOAT_EQ(scale.GetPitch(degree), expected_pitch) << degree;
    }
  }
}

}  // namespace
}  // namespace barely
