#include "barelymusician/dsp/dsp_utils.h"

#include <array>

#include "barelymusician/barelymusician.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

// Tests that the expected filter coefficients are generated for an arbitrary set of cutoff
// frequencies.
TEST(DspUtilsTest, GetFilterCoefficient) {
  constexpr float kEpsilon = 1e-2f;
  constexpr int kFrameRate = 8000;

  constexpr int kCutoffCount = 5;
  constexpr std::array<float, kCutoffCount> kCutoffs = {
      0.0f, 100.0f, 500.0f, 1000.0f, 8000.0f,
  };
  constexpr std::array<float, kCutoffCount> kExpectedCoefficients = {
      1.00f, 0.92f, 0.68f, 0.46f, 0.00f,
  };

  for (int i = 0; i < kCutoffCount; ++i) {
    EXPECT_NEAR(GetFilterCoefficient(kFrameRate, kCutoffs[i]), kExpectedCoefficients[i], kEpsilon);
  }
}

// Tests that converting arbitrary pitches returns the expected frequencies.
TEST(DspUtilsTest, GetFrequency) {
  constexpr float kEpsilon = 1e-2f;

  constexpr int kPitchCount = 5;
  constexpr std::array<Rational, kPitchCount> kPitches = {
      -4, Rational(-3, 4), 0, 2, Rational(33, 10),
  };
  constexpr std::array<float, kPitchCount> kFrequencies = {
      27.50f, 261.62f, 440.00f, 1760.00f, 4333.63f,
  };

  for (int i = 0; i < kPitchCount; ++i) {
    EXPECT_NEAR(GetFrequency(kPitches[i]), kFrequencies[i], kEpsilon);
  }
}

}  // namespace
}  // namespace barely
