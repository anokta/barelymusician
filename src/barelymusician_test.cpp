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

TEST(BarelyEngineTest, CreateDestroyLfo) {
  const BarelyEngineConfig config = BARELY_ENGINE_CONFIG_DEFAULT(kSampleRate);
  const int32_t allocation_size = BarelyEngineConfig_GetRequiredAllocationSize(&config);
  std::vector<std::byte> allocation(allocation_size);
  BarelyEngine* engine = BarelyEngine_Create(&config, allocation.data(), allocation_size);
  EXPECT_TRUE(engine != nullptr);

  const uint32_t lfo_id = BarelyEngine_CreateLfo(engine);
  EXPECT_NE(lfo_id, 0);

  BarelyLfo_Destroy(engine, lfo_id);
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

TEST(EngineTest, CreateDestroynstrument) {
  Engine engine(kSampleRate);
  engine.CreateInstrument().Destroy();
}

TEST(EngineTest, CreateDestroyLfo) {
  Engine engine(kSampleRate);
  engine.CreateLfo().Destroy();
}

TEST(EngineTest, CreateDestroyPerformer) {
  Engine engine(kSampleRate);
  engine.CreatePerformer().Destroy();
}

}  // namespace
}  // namespace barely
