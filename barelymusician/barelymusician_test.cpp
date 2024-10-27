#include "barelymusician/barelymusician.h"

#include "gtest/gtest.h"

namespace barely {
namespace {

TEST(BarelyMusicianTest, CreateDestroyMusician) {
  // Failures.
  EXPECT_FALSE(BarelyMusician_Create(0, nullptr));
  EXPECT_FALSE(BarelyMusician_Destroy(nullptr));

  // Success.
  BarelyMusicianHandle musician = nullptr;
  EXPECT_TRUE(BarelyMusician_Create(1, &musician));
  EXPECT_TRUE(BarelyMusician_Destroy(musician));
}

TEST(BarelyMusicianTest, CreateDestroyInstrument) {
  BarelyMusicianHandle musician = nullptr;
  ASSERT_TRUE(BarelyMusician_Create(1, &musician));

  // Failures.
  EXPECT_FALSE(BarelyMusician_AddInstrument(musician, nullptr));
  EXPECT_FALSE(BarelyMusician_RemoveInstrument(musician, nullptr));

  // Success.
  BarelyInstrumentHandle instrument = nullptr;
  EXPECT_TRUE(BarelyMusician_AddInstrument(musician, &instrument));
  EXPECT_NE(instrument, nullptr);

  EXPECT_TRUE(BarelyMusician_RemoveInstrument(musician, instrument));
  EXPECT_TRUE(BarelyMusician_Destroy(musician));
}

TEST(BarelyMusicianTest, CreateDestroyPerformer) {
  BarelyMusicianHandle musician = nullptr;
  ASSERT_TRUE(BarelyMusician_Create(1, &musician));

  // Failures.
  EXPECT_FALSE(BarelyMusician_AddPerformer(musician, 0, nullptr));
  EXPECT_FALSE(BarelyMusician_RemovePerformer(musician, nullptr));

  // Success.
  BarelyPerformerHandle performer = nullptr;
  EXPECT_TRUE(BarelyMusician_AddPerformer(musician, 0, &performer));
  EXPECT_NE(performer, nullptr);

  EXPECT_TRUE(BarelyMusician_RemovePerformer(musician, performer));
  EXPECT_TRUE(BarelyMusician_Destroy(musician));
}

TEST(MusicianTest, CreateDestroyMusician) { [[maybe_unused]] const Musician musician(1); }

TEST(MusicianTest, CreateDestroyInstrument) {
  Musician musician(1);
  auto instrument = musician.AddInstrument();
  musician.RemoveInstrument(instrument);
}

TEST(MusicianTest, CreateDestroyPerformer) {
  Musician musician(1);
  auto performer = musician.AddPerformer();
  musician.RemovePerformer(performer);
}

}  // namespace
}  // namespace barely
