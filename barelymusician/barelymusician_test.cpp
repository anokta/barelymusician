#include "barelymusician/barelymusician.h"

#include "gtest/gtest.h"

namespace barely {
namespace {

TEST(BarelyMusicianTest, CreateDestroyMusician) {
  // Failures.
  EXPECT_EQ(BarelyMusician_Create(nullptr), BarelyStatus_kInvalidArgument);
  EXPECT_EQ(BarelyMusician_Destroy(nullptr), BarelyStatus_kNotFound);

  // Success.
  BarelyMusicianHandle handle = nullptr;
  ASSERT_EQ(BarelyMusician_Create(&handle), BarelyStatus_kOk);
  EXPECT_EQ(BarelyMusician_Destroy(handle), BarelyStatus_kOk);
}

TEST(BarelyMusicianTest, CreateDestroyInstrument) {
  BarelyMusicianHandle handle = nullptr;
  ASSERT_EQ(BarelyMusician_Create(&handle), BarelyStatus_kOk);

  // Failures.
  EXPECT_EQ(BarelyInstrument_Create(handle, {}, 1, nullptr),
            BarelyStatus_kInvalidArgument);
  EXPECT_EQ(BarelyInstrument_Destroy(handle, BarelyId_kInvalid),
            BarelyStatus_kInvalidArgument);

  BarelyId instrument_id = BarelyId_kInvalid;
  EXPECT_EQ(BarelyInstrument_Create(handle, {}, 0, &instrument_id),
            BarelyStatus_kInvalidArgument);

  // Success.
  EXPECT_EQ(BarelyInstrument_Create(handle, {}, 1, &instrument_id),
            BarelyStatus_kOk);
  EXPECT_NE(instrument_id, BarelyId_kInvalid);

  EXPECT_EQ(BarelyInstrument_Destroy(handle, instrument_id), BarelyStatus_kOk);
  EXPECT_EQ(BarelyMusician_Destroy(handle), BarelyStatus_kOk);
}

TEST(BarelyMusicianTest, CreateDestroyPerformer) {
  BarelyMusicianHandle handle = nullptr;
  ASSERT_EQ(BarelyMusician_Create(&handle), BarelyStatus_kOk);

  // Failures.
  EXPECT_EQ(BarelyPerformer_Create(handle, nullptr),
            BarelyStatus_kInvalidArgument);
  EXPECT_EQ(BarelyPerformer_Destroy(handle, BarelyId_kInvalid),
            BarelyStatus_kInvalidArgument);

  // Success.
  BarelyId performer_id = BarelyId_kInvalid;
  EXPECT_EQ(BarelyPerformer_Create(handle, &performer_id), BarelyStatus_kOk);
  EXPECT_NE(performer_id, BarelyId_kInvalid);

  EXPECT_EQ(BarelyPerformer_Destroy(handle, performer_id), BarelyStatus_kOk);
  EXPECT_EQ(BarelyMusician_Destroy(handle), BarelyStatus_kOk);
}

TEST(MusicianTest, CreateDestroyMusician) { Musician musician; }

TEST(MusicianTest, CreateDestroyInstrument) {
  Musician musician;
  InstrumentRef instrument_ref = musician.CreateInstrument({{}}, 1);
  EXPECT_EQ(musician.DestroyInstrument(instrument_ref), Status::kOk);
}

TEST(MusicianTest, CreateDestroyPerformer) {
  Musician musician;
  PerformerRef performer_ref = musician.CreatePerformer();
  EXPECT_EQ(musician.DestroyPerformer(performer_ref), Status::kOk);
}

}  // namespace
}  // namespace barely
