#include "barelymusician/barelymusician.h"

#include "gtest/gtest.h"

namespace barely {
namespace {

TEST(BarelyMusicianTest, CreateDestroyMusician) {
  // Failures.
  EXPECT_FALSE(BarelyMusician_Create(0, nullptr));
  EXPECT_FALSE(BarelyMusician_Destroy(nullptr));

  // Success.
  BarelyMusician* musician = nullptr;
  EXPECT_TRUE(BarelyMusician_Create(1, &musician));
  EXPECT_TRUE(BarelyMusician_Destroy(musician));
}

TEST(BarelyMusicianTest, CreateDestroyInstrument) {
  BarelyMusician* musician = nullptr;
  ASSERT_TRUE(BarelyMusician_Create(1, &musician));

  // Failures.
  EXPECT_FALSE(BarelyInstrument_Create(musician, {}, nullptr));
  EXPECT_FALSE(BarelyInstrument_Destroy(nullptr));

  // Success.
  BarelyInstrument* instrument = nullptr;
  EXPECT_TRUE(BarelyInstrument_Create(musician, {}, &instrument));
  EXPECT_NE(instrument, nullptr);

  EXPECT_TRUE(BarelyInstrument_Destroy(instrument));
  EXPECT_TRUE(BarelyMusician_Destroy(musician));
}

TEST(BarelyMusicianTest, CreateDestroyPerformer) {
  BarelyMusician* musician = nullptr;
  ASSERT_TRUE(BarelyMusician_Create(1, &musician));

  // Failures.
  EXPECT_FALSE(BarelyPerformer_Create(musician, 0, nullptr));
  EXPECT_FALSE(BarelyPerformer_Destroy(nullptr));

  // Success.
  BarelyPerformer* performer = nullptr;
  EXPECT_TRUE(BarelyPerformer_Create(musician, 0, &performer));
  EXPECT_NE(performer, nullptr);

  EXPECT_TRUE(BarelyPerformer_Destroy(performer));
  EXPECT_TRUE(BarelyMusician_Destroy(musician));
}

TEST(MusicianTest, CreateDestroyMusician) { [[maybe_unused]] const Musician musician(1); }

TEST(MusicianTest, CreateDestroyInstrument) {
  const Musician musician(1);
  [[maybe_unused]] const Instrument instrument(musician, InstrumentDefinition({}));
}

TEST(MusicianTest, CreateDestroyPerformer) {
  const Musician musician(1);
  [[maybe_unused]] const Performer performer(musician);
}

}  // namespace
}  // namespace barely
