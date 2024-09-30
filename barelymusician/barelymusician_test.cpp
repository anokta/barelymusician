#include "barelymusician/barelymusician.h"

#include <array>
#include <cmath>

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

TEST(TuningDefinitionTest, GetFrequency) {
  constexpr int kPitchCount = 5;
  constexpr std::array<double, kPitchCount> kRatios = {1.25, 1.5, 1.75, 2.0, 3.0};
  constexpr double kRootFrequency = 100.0;
  constexpr int kRootPitch = 25;

  const TuningDefinition definition = {kRatios, kRootFrequency, kRootPitch};

  constexpr int kOctaveRange = 2;
  for (int octave = -kOctaveRange; octave <= kOctaveRange; ++octave) {
    for (int i = 0; i < kPitchCount; ++i) {
      const int pitch = kRootPitch + octave * kPitchCount + i;
      const double expected_frequency = kRootFrequency *
                                        std::pow(kRatios[kPitchCount - 1], octave) *
                                        (i > 0 ? kRatios[i - 1] : 1.0);
      EXPECT_DOUBLE_EQ(definition.GetFrequency(pitch), expected_frequency);
    }
  }
}

}  // namespace
}  // namespace barely
