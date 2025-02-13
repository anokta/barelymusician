#include "barelymusician.h"

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

}  // namespace
}  // namespace barely
