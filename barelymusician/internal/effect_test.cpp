#include "barelymusician/internal/effect.h"

#include <algorithm>
#include <array>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

// Returns a test effect definition that produces constant output.
EffectDefinition GetTestDefinition() {
  static const std::array<ControlDefinition, 1> control_definitions = {
      ControlDefinition{0.0},
  };
  return EffectDefinition(
      [](void** state, int frame_rate) {
        *state = static_cast<void*>(new double{static_cast<double>(frame_rate)});
      },
      [](void** state) { delete static_cast<double*>(*state); },
      [](void** state, double* output_samples, int output_channel_count, int output_frame_count) {
        std::fill_n(output_samples, output_channel_count * output_frame_count,
                    *reinterpret_cast<double*>(*state));
      },
      [](void** state, int index, double value, double /*slope_per_frame*/) {
        *reinterpret_cast<double*>(*state) = static_cast<double>(index + 1) * value;
      },
      [](void** /*state*/, const void* /*data*/, int /*size*/) {}, control_definitions);
}

// Tests that the effect gets processed as expected.
TEST(EffectTest, Process) {
  constexpr int kFrameRate = 8000;
  constexpr int kChannelCount = 1;
  constexpr int kFrameCount = 4;

  Effect effect(GetTestDefinition(), kFrameRate);
  std::vector<double> buffer(kChannelCount * kFrameCount);

  std::fill(buffer.begin(), buffer.end(), 0.0);
  effect.Process(buffer.data(), kChannelCount, kFrameCount);
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], kFrameRate);
    }
  }

  // Set control to a value.
  effect.SetControl(0, 5.0, 0.0);

  std::fill(buffer.begin(), buffer.end(), 0.0);
  effect.Process(buffer.data(), kChannelCount, kFrameCount);
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 5.0);
    }
  }
}

}  // namespace
}  // namespace barely::internal
