#include "barelymusician/dsp/dsp_utils.h"

#include <array>

#include "gtest/gtest.h"

namespace barely {
namespace {

// Tests that the expected filter coefficients are generated for an arbitrary
// set of cutoff frequencies.
TEST(DspUtilsTest, GetFilterCoefficient) {
  constexpr double kEpsilon = 1e-2;
  constexpr int kFrameRate = 8000;

  constexpr int kCutoffCount = 5;
  constexpr std::array<double, kCutoffCount> kCutoffs = {
      0.0, 100.0, 500.0, 1000.0, 8000.0,
  };
  constexpr std::array<double, kCutoffCount> kExpectedCoefficients = {
      1.00, 0.92, 0.68, 0.46, 0.00,
  };

  for (int i = 0; i < kCutoffCount; ++i) {
    EXPECT_NEAR(GetFilterCoefficient(kFrameRate, kCutoffs[i]),
                kExpectedCoefficients[i], kEpsilon);
  }
}

// Tests that converting arbitrary pitches returns the expected frequencies.
TEST(DspUtilsTest, GetFrequency) {
  constexpr double kEpsilon = 1e-2;

  constexpr int kPitchCount = 5;
  constexpr std::array<double, kPitchCount> kPitches = {
      -4.0, -0.75, 0.0, 2.0, 3.3,
  };
  constexpr std::array<double, kPitchCount> kFrequencies = {
      27.50, 261.62, 440.00, 1760.00, 4333.63,
  };

  for (int i = 0; i < kPitchCount; ++i) {
    EXPECT_NEAR(GetFrequency(kPitches[i]), kFrequencies[i], kEpsilon);
  }
}

}  // namespace
}  // namespace barely
