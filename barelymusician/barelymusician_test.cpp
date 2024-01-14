#include "barelymusician/barelymusician.h"

#include "gtest/gtest.h"

namespace barely {
namespace {

TEST(BarelyMusicianTest, CreateDestroyMusician) {
  // Failures.
  EXPECT_FALSE(BarelyMusician_Create(0, nullptr));
  EXPECT_FALSE(BarelyMusician_Create(48000, nullptr));
  EXPECT_FALSE(BarelyMusician_Destroy(nullptr));

  // Success.
  BarelyMusicianHandle musician = nullptr;
  EXPECT_TRUE(BarelyMusician_Create(48000, &musician));
  EXPECT_TRUE(BarelyMusician_Destroy(musician));
}

TEST(BarelyMusicianTest, CreateDestroyInstrument) {
  BarelyMusicianHandle musician = nullptr;
  ASSERT_TRUE(BarelyMusician_Create(48000, &musician));

  // Failures.
  EXPECT_FALSE(BarelyInstrument_Create(musician, {}, nullptr));
  EXPECT_FALSE(BarelyInstrument_Destroy(nullptr));

  // Success.
  BarelyInstrumentHandle instrument = nullptr;
  EXPECT_TRUE(BarelyInstrument_Create(musician, {}, &instrument));
  EXPECT_NE(instrument, nullptr);

  EXPECT_TRUE(BarelyInstrument_Destroy(instrument));
  EXPECT_TRUE(BarelyMusician_Destroy(musician));
}

TEST(BarelyMusicianTest, CreateDestroyPerformer) {
  BarelyMusicianHandle musician = nullptr;
  ASSERT_TRUE(BarelyMusician_Create(48000, &musician));

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

TEST(MusicianTest, CreateDestroyMusician) { [[maybe_unused]] const Musician musician(48000); }

TEST(MusicianTest, CreateDestroyInstrument) {
  Musician musician(48000);
  [[maybe_unused]] const Instrument instrument = musician.CreateInstrument({{}});
}

TEST(MusicianTest, CreateDestroyPerformer) {
  Musician musician(48000);
  [[maybe_unused]] const Performer performer = musician.CreatePerformer();
}

}  // namespace
}  // namespace barely
