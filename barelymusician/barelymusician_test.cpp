#include "barelymusician/barelymusician.h"

#include "gtest/gtest.h"

namespace barely {
namespace {

TEST(BarelyMusicianTest, CreateDestroyMusician) {
  // Failures.
  EXPECT_FALSE(BarelyMusician_Create(nullptr));
  EXPECT_FALSE(BarelyMusician_Destroy(nullptr));

  // Success.
  BarelyMusician* musician = nullptr;
  EXPECT_TRUE(BarelyMusician_Create(&musician));
  EXPECT_TRUE(BarelyMusician_Destroy(musician));
}

TEST(BarelyMusicianTest, CreateDestroyInstrument) {
  BarelyMusician* musician = nullptr;
  ASSERT_TRUE(BarelyMusician_Create(&musician));

  // Failures.
  EXPECT_FALSE(BarelyMusician_CreateInstrument(musician, {}, 1, nullptr));
  EXPECT_FALSE(BarelyMusician_DestroyInstrument(musician, nullptr));

  BarelyInstrument* instrument = nullptr;
  EXPECT_FALSE(BarelyMusician_CreateInstrument(musician, {}, 0, &instrument));

  // Success.
  EXPECT_TRUE(BarelyMusician_CreateInstrument(musician, {}, 1, &instrument));
  EXPECT_NE(instrument, nullptr);

  EXPECT_TRUE(BarelyMusician_DestroyInstrument(musician, instrument));
  EXPECT_TRUE(BarelyMusician_Destroy(musician));
}

TEST(BarelyMusicianTest, CreateDestroyPerformer) {
  BarelyMusician* musician = nullptr;
  ASSERT_TRUE(BarelyMusician_Create(&musician));

  // Failures.
  EXPECT_FALSE(BarelyMusician_CreatePerformer(musician, 0, nullptr));
  EXPECT_FALSE(BarelyMusician_DestroyPerformer(musician, nullptr));

  // Success.
  BarelyPerformer* performer = nullptr;
  EXPECT_TRUE(BarelyMusician_CreatePerformer(musician, 0, &performer));
  EXPECT_NE(performer, nullptr);

  EXPECT_TRUE(BarelyMusician_DestroyPerformer(musician, performer));
  EXPECT_TRUE(BarelyMusician_Destroy(musician));
}

TEST(MusicianTest, CreateDestroyMusician) {
  const Musician musician = Musician::Create();
  Musician::Destroy(musician);
}

TEST(MusicianTest, CreateDestroyInstrument) {
  Scoped<Musician> musician;
  [[maybe_unused]] const Instrument instrument = musician.CreateInstrument({{}}, 1);
}

TEST(MusicianTest, CreateDestroyPerformer) {
  Scoped<Musician> musician;
  [[maybe_unused]] const Performer performer = musician.CreatePerformer();
}

}  // namespace
}  // namespace barely
