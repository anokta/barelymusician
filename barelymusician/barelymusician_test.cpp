#include "barelymusician/barelymusician.h"

#include "gtest/gtest.h"

namespace barely {
namespace {

TEST(BarelyMusicianTest, CreateDestroyMusician) {
  // Failures.
  EXPECT_EQ(BarelyMusician_Create(nullptr), BarelyStatus_kInvalidArgument);
  EXPECT_EQ(BarelyMusician_Destroy(nullptr), BarelyStatus_kNotFound);

  // Success.
  BarelyMusicianHandle musician = nullptr;
  ASSERT_EQ(BarelyMusician_Create(&musician), BarelyStatus_kOk);
  EXPECT_EQ(BarelyMusician_Destroy(musician), BarelyStatus_kOk);
}

TEST(BarelyMusicianTest, CreateDestroyInstrument) {
  BarelyMusicianHandle musician = nullptr;
  ASSERT_EQ(BarelyMusician_Create(&musician), BarelyStatus_kOk);

  // Failures.
  EXPECT_EQ(BarelyInstrument_Create(musician, {}, 1, nullptr),
            BarelyStatus_kInvalidArgument);
  EXPECT_EQ(BarelyInstrument_Destroy(nullptr), BarelyStatus_kNotFound);

  BarelyInstrumentHandle instrument = nullptr;
  EXPECT_EQ(BarelyInstrument_Create(musician, {}, 0, &instrument),
            BarelyStatus_kInvalidArgument);

  // Success.
  EXPECT_EQ(BarelyInstrument_Create(musician, {}, 1, &instrument),
            BarelyStatus_kOk);
  EXPECT_NE(instrument, nullptr);

  EXPECT_EQ(BarelyInstrument_Destroy(instrument), BarelyStatus_kOk);
  EXPECT_EQ(BarelyMusician_Destroy(musician), BarelyStatus_kOk);
}

TEST(BarelyMusicianTest, CreateDestroyPerformer) {
  BarelyMusicianHandle musician = nullptr;
  ASSERT_EQ(BarelyMusician_Create(&musician), BarelyStatus_kOk);

  // Failures.
  EXPECT_EQ(BarelyPerformer_Create(musician, nullptr),
            BarelyStatus_kInvalidArgument);
  EXPECT_EQ(BarelyPerformer_Destroy(nullptr), BarelyStatus_kNotFound);

  // Success.
  BarelyPerformerHandle performer = nullptr;
  EXPECT_EQ(BarelyPerformer_Create(musician, &performer), BarelyStatus_kOk);
  EXPECT_NE(performer, nullptr);

  EXPECT_EQ(BarelyPerformer_Destroy(performer), BarelyStatus_kOk);
  EXPECT_EQ(BarelyMusician_Destroy(musician), BarelyStatus_kOk);
}

TEST(MusicianTest, CreateDestroyMusician) { Musician musician; }

TEST(MusicianTest, CreateDestroyInstrument) {
  Musician musician;
  Instrument instrument = musician.CreateInstrument({{}}, 1);
}

TEST(MusicianTest, CreateDestroyPerformer) {
  Musician musician;
  Performer performer = musician.CreatePerformer();
}

}  // namespace
}  // namespace barely
