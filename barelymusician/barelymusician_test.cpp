#include "barelymusician/barelymusician.h"

#include <array>

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

TEST(ScaleTest, GetPitch) {
  constexpr int kPitchCount = 5;
  const std::array<double, kPitchCount> kPitches = {0.0, 0.2, 0.35, 0.5, 0.95};
  constexpr double kRootPitch = 1.75;
  constexpr int kMode = 1;

  const Scale scale = {kPitches, kRootPitch, kMode};

  constexpr int kOctaveRange = 2;
  for (int octave = -kOctaveRange; octave <= kOctaveRange; ++octave) {
    for (int i = 0; i < kPitchCount; ++i) {
      const int degree = octave * kPitchCount + i;
      // NOLINTNEXTLINE(bugprone-integer-division)
      const double expected_pitch = kRootPitch + octave + (i + kMode) / kPitchCount +
                                    kPitches[(i + kMode) % kPitchCount] - kPitches[kMode];
      EXPECT_EQ(scale.GetPitch(degree), expected_pitch) << degree;
    }
  }
}

}  // namespace
}  // namespace barely
