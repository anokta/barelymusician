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
  EXPECT_FALSE(BarelyInstrument_Create(musician, {}, 1, nullptr));
  EXPECT_FALSE(BarelyInstrument_Destroy(nullptr));

  BarelyInstrumentHandle instrument = nullptr;
  EXPECT_FALSE(BarelyInstrument_Create(musician, {}, 0, &instrument));

  // Success.
  EXPECT_TRUE(BarelyInstrument_Create(musician, {}, 1, &instrument));
  EXPECT_NE(instrument, nullptr);

  EXPECT_TRUE(BarelyInstrument_Destroy(instrument));
  EXPECT_TRUE(BarelyMusician_Destroy(musician));
}

TEST(BarelyMusicianTest, CreateDestroyPerformer) {
  BarelyMusicianHandle musician = nullptr;
  ASSERT_TRUE(BarelyMusician_Create(&musician));

  // Failures.
  EXPECT_FALSE(BarelyPerformer_Create(musician, nullptr));
  EXPECT_FALSE(BarelyPerformer_Destroy(nullptr));

  // Success.
  BarelyPerformerHandle performer = nullptr;
  EXPECT_TRUE(BarelyPerformer_Create(musician, &performer));
  EXPECT_NE(performer, nullptr);

  EXPECT_TRUE(BarelyPerformer_Destroy(performer));
  EXPECT_TRUE(BarelyMusician_Destroy(musician));
}

TEST(MusicianTest, CreateDestroyMusician) { [[maybe_unused]] const Musician musician; }

TEST(MusicianTest, CreateDestroyInstrument) {
  Musician musician;
  [[maybe_unused]] const Instrument instrument = musician.CreateInstrument({{}}, 1);
}

TEST(MusicianTest, CreateDestroyPerformer) {
  Musician musician;
  [[maybe_unused]] const Performer performer = musician.CreatePerformer();
}

}  // namespace
}  // namespace barely
