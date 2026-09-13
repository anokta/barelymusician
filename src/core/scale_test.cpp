#include "core/scale.h"

#include <barelymusician.h>

#include <array>
#include <cstdint>

#include "gtest/gtest.h"

namespace barely {
namespace {

TEST(ScaleTest, GetPitch) {
  constexpr double kEpsilon = 1e-5;

  constexpr int32_t kPitchCount = 5;
  const std::array<double, kPitchCount> kPitches = {0.0, 0.2, 0.35, 0.5, 0.95};
  constexpr double kRootPitch = 1.75;
  constexpr int32_t kMode = 1;

  const BarelyScale scale = {kRootPitch, kPitches.data(), static_cast<int32_t>(kPitches.size()),
                             kMode};

  constexpr int32_t kOctaveRange = 2;
  for (int32_t octave = -kOctaveRange; octave <= kOctaveRange; ++octave) {
    for (int32_t i = 0; i < kPitchCount; ++i) {
      const int32_t degree = octave * kPitchCount + i;
      const double expected_pitch = kRootPitch +
                                    // NOLINTNEXTLINE(bugprone-integer-division)
                                    static_cast<double>(octave + (i + kMode) / kPitchCount) +
                                    kPitches[(i + kMode) % kPitchCount] - kPitches[kMode];
      EXPECT_NEAR(GetPitch(scale, degree), expected_pitch, kEpsilon) << degree;
    }
  }
}

}  // namespace
}  // namespace barely
