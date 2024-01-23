#include "barelymusician/barelymusician.h"

#include <sstream>

#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

using ::testing::AllOf;
using ::testing::Field;

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

// Tests that arbitrary arithmetic operations for rational numbers are computed as expected.
TEST(MusicianTest, RationalArithmetic) {
  // Addition.
  Rational value(1);
  EXPECT_EQ(value + Rational(2), Rational(3));
  value += Rational(2);
  EXPECT_EQ(value, Rational(3));

  value = 1;
  EXPECT_EQ(value + 2, 3);
  value += 2;
  EXPECT_EQ(value, 3);

  value = Rational(-45, 7);
  EXPECT_EQ(value + Rational(32, -14), Rational(-61, 7));
  value += Rational(32, -14);
  EXPECT_EQ(value, Rational(-61, 7));

  value = Rational(3, 5);
  EXPECT_EQ(value + Rational(12, 5), 3);
  value += Rational(12, 5);
  EXPECT_EQ(value, 3);

  // Subtraction.
  value = Rational();
  EXPECT_EQ(value - Rational(8), Rational(-8));
  value -= Rational(8);
  EXPECT_EQ(value, Rational(-8));

  value = 0;
  EXPECT_EQ(value - (-8), 8);
  value -= -8;
  EXPECT_EQ(value, 8);

  value = Rational(4, 9);
  EXPECT_EQ(value - Rational(5, 11), Rational(-1, 99));
  value -= Rational(5, 11);
  EXPECT_EQ(value, Rational(-1, 99));

  value = Rational(1, 6);
  EXPECT_EQ(value - Rational(19, 6), -3);
  value -= Rational(19, 6);
  EXPECT_EQ(value, -3);

  // Multiplication.
  value = Rational();
  EXPECT_EQ(value * Rational(234, 567), Rational());
  value *= Rational(234, 567);
  EXPECT_EQ(value, Rational());

  value = 0;
  EXPECT_EQ(value * 20, 0);
  value *= 20;
  EXPECT_EQ(value, 0);

  value = Rational(-4, 5);
  EXPECT_EQ(value * Rational(-75, 4), Rational(15));
  value *= Rational(-75, 4);
  EXPECT_EQ(value, Rational(15));

  // Division.
  value = Rational(-10);
  EXPECT_EQ(value / Rational(-10), Rational(1));
  value /= Rational(-10);
  EXPECT_EQ(value, Rational(1));

  value = -10;
  EXPECT_EQ(value / (-10), 1);
  value /= -10;
  EXPECT_EQ(value, 1);

  value = Rational(7, 55);
  EXPECT_EQ(value / Rational(4, -7), Rational(-49, 220));
  value /= Rational(4, -7);
  EXPECT_EQ(value, Rational(-49, 220));
}

// Tests that arbitrary rational numbers are compared as expected.
TEST(MusicianTest, RationalComparison) {
  // Equal.
  EXPECT_EQ(Rational(), Rational(0));
  EXPECT_LE(Rational(), Rational(0));
  EXPECT_GE(Rational(), Rational(0));

  EXPECT_EQ(Rational(), Rational(0, 5));
  EXPECT_LE(Rational(), Rational(0, 5));
  EXPECT_GE(Rational(), Rational(0, 5));

  EXPECT_EQ(Rational(-333, 5), Rational(-333, 5));
  EXPECT_LE(Rational(-333, 5), Rational(-333, 5));
  EXPECT_GE(Rational(-333, 5), Rational(-333, 5));

  EXPECT_EQ(Rational(-3, 7), Rational(9, -21));
  EXPECT_LE(Rational(-3, 7), Rational(9, -21));
  EXPECT_GE(Rational(-3, 7), Rational(9, -21));

  // Inequal.
  EXPECT_NE(Rational(), Rational(1));
  EXPECT_NE(Rational(-10, 7), Rational(-6));

  // Less.
  EXPECT_LT(Rational(), Rational(1));
  EXPECT_NE(Rational(), Rational(1));

  EXPECT_LT(Rational(5, 7), Rational(2));
  EXPECT_NE(Rational(5, 7), Rational(2));

  EXPECT_LT(Rational(-15, 6), Rational(-3, 2));
  EXPECT_NE(Rational(-15, 6), Rational(-3, 2));

  EXPECT_LT(Rational(3, 44), Rational(9, 44));
  EXPECT_NE(Rational(3, 44), Rational(9, 44));

  // Greater.
  EXPECT_GT(Rational(), Rational(-122));
  EXPECT_NE(Rational(), Rational(-122));

  EXPECT_GT(Rational(5, 7), Rational(-2));
  EXPECT_NE(Rational(5, 7), Rational(-2));

  EXPECT_GT(Rational(15, 6), Rational(3, 2));
  EXPECT_NE(Rational(15, 6), Rational(3, 2));

  EXPECT_GT(Rational(15, 4), Rational(9, 4));
  EXPECT_NE(Rational(15, 4), Rational(9, 4));
}

// Tests that arbitrary rational numbers are converted to strings as expected.
TEST(MusicianTest, RationalToString) {
  std::stringstream stream;
  stream << Rational();
  EXPECT_EQ(stream.str(), "0");

  stream.str("");
  stream.clear();
  stream << Rational(3, 4);
  EXPECT_EQ(stream.str(), "3/4");

  stream.str("");
  stream.clear();
  stream << Rational(-5);
  EXPECT_EQ(stream.str(), "-5");

  stream.str("");
  stream.clear();
  stream << Rational(77, -68);
  EXPECT_EQ(stream.str(), "-77/68");

  stream.str("");
  stream.clear();
  stream << Rational(-10, -2001);
  EXPECT_EQ(stream.str(), "10/2001");
}

// Tests that arbitrary rational numbers are normalized as expected.
TEST(MusicianTest, RationalNormalized) {
  EXPECT_THAT(Rational::Normalized(1, 3),
              AllOf(Field(&Rational::numerator, 1), Field(&Rational::denominator, 3)));
  EXPECT_THAT(Rational::Normalized(-1, -3),
              AllOf(Field(&Rational::numerator, 1), Field(&Rational::denominator, 3)));
  EXPECT_THAT(Rational::Normalized(2, 6),
              AllOf(Field(&Rational::numerator, 1), Field(&Rational::denominator, 3)));
  EXPECT_THAT(Rational::Normalized(-2, -6),
              AllOf(Field(&Rational::numerator, 1), Field(&Rational::denominator, 3)));
  EXPECT_THAT(Rational::Normalized(22, 66),
              AllOf(Field(&Rational::numerator, 1), Field(&Rational::denominator, 3)));
  EXPECT_THAT(Rational::Normalized(-22, -66),
              AllOf(Field(&Rational::numerator, 1), Field(&Rational::denominator, 3)));

  EXPECT_THAT(Rational::Normalized(-77, 1),
              AllOf(Field(&Rational::numerator, -77), Field(&Rational::denominator, 1)));
  EXPECT_THAT(Rational::Normalized(77, -1),
              AllOf(Field(&Rational::numerator, -77), Field(&Rational::denominator, 1)));
  EXPECT_THAT(Rational::Normalized(-154, 2),
              AllOf(Field(&Rational::numerator, -77), Field(&Rational::denominator, 1)));
  EXPECT_THAT(Rational::Normalized(154, -2),
              AllOf(Field(&Rational::numerator, -77), Field(&Rational::denominator, 1)));
  EXPECT_THAT(Rational::Normalized(-7700000, 100000),
              AllOf(Field(&Rational::numerator, -77), Field(&Rational::denominator, 1)));
  EXPECT_THAT(Rational::Normalized(7700000, -100000),
              AllOf(Field(&Rational::numerator, -77), Field(&Rational::denominator, 1)));

  EXPECT_THAT(Rational::Normalized(0, 1),
              AllOf(Field(&Rational::numerator, 0), Field(&Rational::denominator, 1)));
  EXPECT_THAT(Rational::Normalized(0, -1),
              AllOf(Field(&Rational::numerator, 0), Field(&Rational::denominator, 1)));
  EXPECT_THAT(Rational::Normalized(0, 1234),
              AllOf(Field(&Rational::numerator, 0), Field(&Rational::denominator, 1)));
  EXPECT_THAT(Rational::Normalized(0, -1234),
              AllOf(Field(&Rational::numerator, 0), Field(&Rational::denominator, 1)));
}

}  // namespace
}  // namespace barely
