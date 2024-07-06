#include "barelymusician/barelymusician.h"

#include "gtest/gtest.h"

namespace barely {
namespace {

TEST(BarelyMusicianTest, CreateDestroyMusician) {
  // Failures.
  EXPECT_FALSE(BarelyMusician_Create(nullptr));
  EXPECT_FALSE(BarelyMusician_Destroy(nullptr));

  // Success.
  BarelyMusicianHandle musician = nullptr;
  EXPECT_TRUE(BarelyMusician_Create(&musician));
  EXPECT_TRUE(BarelyMusician_Destroy(musician));
}

TEST(BarelyMusicianTest, CreateDestroyInstrument) {
  BarelyMusicianHandle musician = nullptr;
  ASSERT_TRUE(BarelyMusician_Create(&musician));

  // Failures.
  EXPECT_FALSE(BarelyMusician_CreateInstrument(musician, {}, 1, nullptr));
  EXPECT_FALSE(BarelyMusician_DestroyInstrument(musician, nullptr));

  BarelyInstrumentHandle instrument = nullptr;
  EXPECT_FALSE(BarelyMusician_CreateInstrument(musician, {}, 0, &instrument));

  // Success.
  EXPECT_TRUE(BarelyMusician_CreateInstrument(musician, {}, 1, &instrument));
  EXPECT_NE(instrument, nullptr);

  EXPECT_TRUE(BarelyMusician_DestroyInstrument(musician, instrument));
  EXPECT_TRUE(BarelyMusician_Destroy(musician));
}

TEST(BarelyMusicianTest, CreateDestroyPerformer) {
  BarelyMusicianHandle musician = nullptr;
  ASSERT_TRUE(BarelyMusician_Create(&musician));

  // Failures.
  EXPECT_FALSE(BarelyMusician_CreatePerformer(musician, nullptr));
  EXPECT_FALSE(BarelyMusician_DestroyPerformer(musician, nullptr));

  // Success.
  BarelyPerformerHandle performer = nullptr;
  EXPECT_TRUE(BarelyMusician_CreatePerformer(musician, &performer));
  EXPECT_NE(performer, nullptr);

  EXPECT_TRUE(BarelyMusician_DestroyPerformer(musician, performer));
  EXPECT_TRUE(BarelyMusician_Destroy(musician));
}

TEST(MusicianTest, CreateDestroyMusician) {
  const Musician musician = Musician::Create();
  Musician::Destroy(musician);
}

TEST(MusicianTest, CreateDestroyInstrument) {
  ScopedMusician musician;
  [[maybe_unused]] const Instrument instrument = musician.CreateInstrument({{}}, 1);
}

TEST(MusicianTest, CreateDestroyPerformer) {
  ScopedMusician musician;
  [[maybe_unused]] const Performer performer = musician.CreatePerformer();
}

}  // namespace
}  // namespace barely
