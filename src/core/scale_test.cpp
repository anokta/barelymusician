#include "core/scale.h"

#include <barelymusician.h>

#include <array>
#include <cstdint>

#include "gtest/gtest.h"

namespace barely {
namespace {

TEST(ScaleTest, GetPitch) {
  constexpr float kEpsilon = 1e-5f;

  constexpr int32_t kPitchCount = 5;
  const std::array<float, kPitchCount> kPitches = {0.0f, 0.2f, 0.35f, 0.5f, 0.95f};
  constexpr float kRootPitch = 1.75f;
  constexpr int32_t kMode = 1;

  const BarelyScale scale = {kPitches.data(), static_cast<int32_t>(kPitches.size()), kRootPitch,
                             kMode};

  constexpr int32_t kOctaveRange = 2;
  for (int32_t octave = -kOctaveRange; octave <= kOctaveRange; ++octave) {
    for (int32_t i = 0; i < kPitchCount; ++i) {
      const int32_t degree = octave * kPitchCount + i;
      const float expected_pitch = kRootPitch +
                                   // NOLINTNEXTLINE(bugprone-integer-division)
                                   static_cast<float>(octave + (i + kMode) / kPitchCount) +
                                   kPitches[(i + kMode) % kPitchCount] - kPitches[kMode];
      EXPECT_NEAR(GetPitch(scale, degree), expected_pitch, kEpsilon) << degree;
    }
  }
}

}  // namespace
}  // namespace barely
