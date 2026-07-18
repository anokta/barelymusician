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
  EXPECT_TRUE(BarelyEngine_Create(nullptr, nullptr, 0) == nullptr);

  // Success.
  const BarelyEngineConfig config = BARELY_ENGINE_CONFIG_DEFAULT(kSampleRate);
  const int32_t allocation_size = BarelyEngineConfig_GetRequiredAllocationSize(&config);
  std::vector<std::byte> allocation(allocation_size);
  BarelyEngine* engine = BarelyEngine_Create(&config, allocation.data(), allocation_size);
  EXPECT_TRUE(engine != nullptr);

  BarelyEngine_Destroy(engine);
}

TEST(BarelyEngineTest, CreateDestroyInstrument) {
  const BarelyEngineConfig config = BARELY_ENGINE_CONFIG_DEFAULT(kSampleRate);
  const int32_t allocation_size = BarelyEngineConfig_GetRequiredAllocationSize(&config);
  std::vector<std::byte> allocation(allocation_size);
  BarelyEngine* engine = BarelyEngine_Create(&config, allocation.data(), allocation_size);
  EXPECT_TRUE(engine != nullptr);

  const uint32_t instrument_id = BarelyEngine_CreateInstrument(engine);
  EXPECT_NE(instrument_id, 0);

  BarelyInstrument_Destroy(engine, instrument_id);
  BarelyEngine_Destroy(engine);
}

TEST(BarelyEngineTest, CreateDestroyPerformer) {
  const BarelyEngineConfig config = BARELY_ENGINE_CONFIG_DEFAULT(kSampleRate);
  const int32_t allocation_size = BarelyEngineConfig_GetRequiredAllocationSize(&config);
  std::vector<std::byte> allocation(allocation_size);
  BarelyEngine* engine = BarelyEngine_Create(&config, allocation.data(), allocation_size);
  EXPECT_TRUE(engine != nullptr);

  const uint32_t performer_id = BarelyEngine_CreatePerformer(engine);
  EXPECT_NE(performer_id, 0);

  BarelyPerformer_Destroy(engine, performer_id);
  BarelyEngine_Destroy(engine);
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

}  // namespace
}  // namespace barely
