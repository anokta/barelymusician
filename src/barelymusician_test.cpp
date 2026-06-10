#include <barelymusician.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "gtest/gtest.h"

namespace barely {
namespace {

constexpr int kSampleRate = 48000;

TEST(BarelyEngineTest, CreateDestroyEngine) {
  // Failures.
  EXPECT_FALSE(BarelyEngine_Create(nullptr, nullptr, 0, nullptr));
  EXPECT_FALSE(BarelyEngine_Destroy(nullptr));

  // Success.
  const BarelyEngineConfig config = BARELY_ENGINE_CONFIG_DEFAULT(kSampleRate);
  int32_t allocation_size = 0;
  BarelyEngineConfig_GetRequiredAllocationSize(&config, &allocation_size);
  std::vector<std::byte> allocation(allocation_size);
  BarelyEngine* engine = nullptr;
  EXPECT_TRUE(BarelyEngine_Create(&config, allocation.data(), allocation_size, &engine));
  EXPECT_TRUE(BarelyEngine_Destroy(engine));
}

TEST(BarelyEngineTest, CreateDestroyInstrument) {
  const BarelyEngineConfig config = BARELY_ENGINE_CONFIG_DEFAULT(kSampleRate);
  int32_t allocation_size = 0;
  BarelyEngineConfig_GetRequiredAllocationSize(&config, &allocation_size);
  std::vector<std::byte> allocation(allocation_size);
  BarelyEngine* engine = nullptr;
  EXPECT_TRUE(BarelyEngine_Create(&config, allocation.data(), allocation_size, &engine));

  // Failures.
  EXPECT_FALSE(BarelyEngine_CreateInstrument(engine, nullptr));
  EXPECT_FALSE(BarelyInstrument_Destroy(nullptr, {}));

  // Success.
  uint32_t instrument_id = 0;
  EXPECT_TRUE(BarelyEngine_CreateInstrument(engine, &instrument_id));

  EXPECT_TRUE(BarelyInstrument_Destroy(engine, instrument_id));
  EXPECT_TRUE(BarelyEngine_Destroy(engine));
}

TEST(BarelyEngineTest, CreateDestroyPerformer) {
  const BarelyEngineConfig config = BARELY_ENGINE_CONFIG_DEFAULT(kSampleRate);
  int32_t allocation_size = 0;
  BarelyEngineConfig_GetRequiredAllocationSize(&config, &allocation_size);
  std::vector<std::byte> allocation(allocation_size);
  BarelyEngine* engine = nullptr;
  EXPECT_TRUE(BarelyEngine_Create(&config, allocation.data(), allocation_size, &engine));

  // Failures.
  EXPECT_FALSE(BarelyEngine_CreatePerformer(engine, nullptr));
  EXPECT_FALSE(BarelyPerformer_Destroy(nullptr, {}));

  // Success.
  uint32_t performer_id = 0;
  EXPECT_TRUE(BarelyEngine_CreatePerformer(engine, &performer_id));

  EXPECT_TRUE(BarelyPerformer_Destroy(engine, performer_id));
  EXPECT_TRUE(BarelyEngine_Destroy(engine));
}

TEST(EngineTest, CreateDestroyEngine) { [[maybe_unused]] const Engine engine(kSampleRate); }

TEST(EngineTest, CreateDestroyInstrument) {
  Engine engine(kSampleRate);
  [[maybe_unused]] const auto instrument = engine.CreateInstrument();
}

TEST(EngineTest, CreateDestroyPerformer) {
  Engine engine(kSampleRate);
  engine.CreatePerformer().Destroy();
}

TEST(EngineTest, CreateDestroynstrument) {
  Engine engine(kSampleRate);
  engine.CreateInstrument().Destroy();
}

TEST(EngineTest, GenerateRandomNumber) {
  constexpr int kValueCount = 1000;
  constexpr int kMin = -7;
  constexpr int kMax = 35;

  Engine engine(1);
  for (int i = 0; i < kValueCount; ++i) {
    const int value = engine.GenerateRandomNumber(kMin, kMax);
    EXPECT_GE(value, kMin);
    EXPECT_LT(value, kMax);
  }
}

TEST(EngineTest, ResetSeed) {
  constexpr int kSeed = 1;
  constexpr int kValueCount = 10;

  Engine engine(1);
  engine.ResetSeed(kSeed);

  // Generate some random values.
  std::array<double, kValueCount> values;
  for (int i = 0; i < kValueCount; ++i) {
    values[i] = engine.GenerateRandomNumber();
  }

  // Reset the seed with the same value.
  engine.ResetSeed(kSeed);
  // Validate that the same numbers are generated for the next `kValueCount`.
  for (int i = 0; i < kValueCount; ++i) {
    EXPECT_DOUBLE_EQ(engine.GenerateRandomNumber(), values[i]);
  }
}

TEST(ScaleTest, GetPitch) {
  constexpr float kEpsilon = 1e-5f;

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
      EXPECT_NEAR(scale.GetPitch(degree), expected_pitch, kEpsilon) << degree;
    }
  }
}

}  // namespace
}  // namespace barely
