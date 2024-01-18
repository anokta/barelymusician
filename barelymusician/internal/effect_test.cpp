#include "barelymusician/internal/effect.h"

#include <stdint.h>  // NOLINT(modernize-deprecated-headers)

#include <algorithm>
#include <array>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/rational.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

// Returns a test effect definition that produces constant output.
EffectDefinition GetTestDefinition() {
  static const std::array<ControlDefinition, 1> control_definitions = {
      ControlDefinition{0},
  };
  return EffectDefinition(
      [](void** state, int32_t frame_rate) {
        *state = static_cast<void*>(new float{static_cast<float>(frame_rate)});
      },
      [](void** state) { delete static_cast<float*>(*state); },
      [](void** state, float* output_samples, int32_t output_channel_count,
         int32_t output_frame_count) {
        std::fill_n(output_samples, output_channel_count * output_frame_count,
                    *reinterpret_cast<float*>(*state));
      },
      [](void** state, int32_t index, BarelyRational value, BarelyRational /*slope_per_frame*/) {
        *reinterpret_cast<float*>(*state) =
            static_cast<float>(index + 1) * static_cast<float>(Rational(value));
      },
      [](void** /*state*/, const void* /*data*/, int32_t /*size*/) {}, control_definitions);
}

// Tests that the effect gets processed as expected.
TEST(EffectTest, Process) {
  constexpr int kFrameRate = 8000;
  constexpr int kChannelCount = 1;
  constexpr int kFrameCount = 4;
  constexpr int kProcessOrder = 0;

  Effect effect(GetTestDefinition(), kFrameRate, kProcessOrder);
  std::vector<float> buffer(kChannelCount * kFrameCount);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  effect.Process(buffer.data(), kChannelCount, kFrameCount);
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kChannelCount * frame + channel], 0.0f);
    }
  }

  // Process a control message.
  effect.ProcessControlMessage(0, 5, 0);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  effect.Process(buffer.data(), kChannelCount, kFrameCount);
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kChannelCount * frame + channel], 5.0f);
    }
  }
}

}  // namespace
}  // namespace barely::internal
