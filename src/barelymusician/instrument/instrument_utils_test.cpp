#include "barelymusician/instrument/instrument_utils.h"

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tolerated error margin.
const float kEpsilon = 1e-1f;

// Tests that converting arbitrary note indices returns expected frequencies.
TEST(InstrumentUtilsTest, FrequencyFromNoteIndex) {
  const int kNumIndices = 4;
  const float kIndices[kNumIndices] = {21.0f, 60.0f, 69.0f, 90.5f};
  const float kFrequencies[kNumIndices] = {27.5f, 261.6f, 440.0f, 1523.3f};

  for (int i = 0; i < kNumIndices; ++i) {
    EXPECT_NEAR(kFrequencies[i], FrequencyFromNoteIndex(kIndices[i]), kEpsilon);
  }
}

}  // namespace
}  // namespace barelyapi
