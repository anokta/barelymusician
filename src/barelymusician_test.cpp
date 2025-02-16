#include "barelymusician.h"

#include <array>

#include "gtest/gtest-param-test.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

TEST(BarelyEngineTest, CreateDestroyEngine) {
  // Failures.
  EXPECT_FALSE(BarelyEngine_Create(0, nullptr));
  EXPECT_FALSE(BarelyEngine_Destroy(nullptr));

  // Success.
  BarelyEngineHandle engine = nullptr;
  EXPECT_TRUE(BarelyEngine_Create(1, &engine));
  EXPECT_TRUE(BarelyEngine_Destroy(engine));
}

TEST(BarelyEngineTest, CreateDestroyInstrument) {
  BarelyEngineHandle engine = nullptr;
  ASSERT_TRUE(BarelyEngine_Create(1, &engine));

  // Failures.
  EXPECT_FALSE(BarelyInstrument_Create(engine, nullptr));
  EXPECT_FALSE(BarelyInstrument_Destroy(nullptr));

  // Success.
  BarelyInstrumentHandle instrument = nullptr;
  EXPECT_TRUE(BarelyInstrument_Create(engine, &instrument));
  EXPECT_NE(instrument, nullptr);

  EXPECT_TRUE(BarelyInstrument_Destroy(instrument));
  EXPECT_TRUE(BarelyEngine_Destroy(engine));
}

TEST(BarelyEngineTest, CreateDestroyPerformer) {
  BarelyEngineHandle engine = nullptr;
  ASSERT_TRUE(BarelyEngine_Create(1, &engine));

  // Failures.
  EXPECT_FALSE(BarelyPerformer_Create(engine, nullptr));
  EXPECT_FALSE(BarelyPerformer_Destroy(nullptr));

  // Success.
  BarelyPerformerHandle performer = nullptr;
  EXPECT_TRUE(BarelyPerformer_Create(engine, &performer));
  EXPECT_NE(performer, nullptr);

  EXPECT_TRUE(BarelyPerformer_Destroy(performer));
  EXPECT_TRUE(BarelyEngine_Destroy(engine));
}

TEST(EngineTest, CreateDestroyEngine) { [[maybe_unused]] const Engine engine(1); }

TEST(EngineTest, CreateDestroyInstrument) {
  Engine engine(1);
  [[maybe_unused]] const auto instrument = engine.CreateInstrument();
}

TEST(EngineTest, CreateDestroyPerformer) {
  Engine engine(1);
  [[maybe_unused]] const auto performer = engine.CreatePerformer();
}

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
