#include <barelymusician.h>

#include <array>
#include <vector>

#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

using ::testing::ElementsAre;
using ::testing::UnorderedElementsAre;

constexpr int kSampleRate = 48000;
constexpr int kMaxFrameCount = 512;

TEST(BarelyEngineTest, CreateDestroyEngine) {
  // Failures.
  EXPECT_FALSE(BarelyEngine_Create(0, 0, nullptr));
  EXPECT_FALSE(BarelyEngine_Create(kSampleRate, 0, nullptr));
  EXPECT_FALSE(BarelyEngine_Create(kSampleRate, kMaxFrameCount, nullptr));
  EXPECT_FALSE(BarelyEngine_Destroy(nullptr));

  // Success.
  BarelyEngineHandle engine = nullptr;
  EXPECT_TRUE(BarelyEngine_Create(kSampleRate, kMaxFrameCount, &engine));
  EXPECT_TRUE(BarelyEngine_Destroy(engine));
}

TEST(BarelyEngineTest, CreateDestroyInstrument) {
  BarelyEngineHandle engine = nullptr;
  ASSERT_TRUE(BarelyEngine_Create(kSampleRate, kMaxFrameCount, &engine));

  // Failures.
  EXPECT_FALSE(BarelyInstrument_Create(engine, nullptr, 0, nullptr));
  EXPECT_FALSE(BarelyInstrument_Destroy(nullptr));

  // Success.
  BarelyInstrumentHandle instrument = nullptr;
  EXPECT_TRUE(BarelyInstrument_Create(engine, nullptr, 0, &instrument));
  EXPECT_NE(instrument, nullptr);

  EXPECT_TRUE(BarelyInstrument_Destroy(instrument));
  EXPECT_TRUE(BarelyEngine_Destroy(engine));
}

TEST(BarelyEngineTest, CreateDestroyPerformer) {
  BarelyEngineHandle engine = nullptr;
  ASSERT_TRUE(BarelyEngine_Create(kSampleRate, kMaxFrameCount, &engine));

  // Failures.
  EXPECT_FALSE(BarelyEngine_CreatePerformer(engine, nullptr));
  EXPECT_FALSE(BarelyEngine_DestroyPerformer(nullptr, 0));

  // Success.
  BarelyPerformerRef performer_ref;
  EXPECT_TRUE(BarelyEngine_CreatePerformer(engine, &performer_ref));

  EXPECT_TRUE(BarelyEngine_DestroyPerformer(engine, performer_ref));
  EXPECT_TRUE(BarelyEngine_Destroy(engine));
}

TEST(EngineTest, CreateDestroyEngine) {
  [[maybe_unused]] const Engine engine(kSampleRate, kMaxFrameCount);
}

TEST(EngineTest, CreateDestroyInstrument) {
  Engine engine(kSampleRate, kMaxFrameCount);
  [[maybe_unused]] const auto instrument = engine.CreateInstrument();
}

TEST(EngineTest, CreateDestroyPerformer) {
  Engine engine(kSampleRate, kMaxFrameCount);
  engine.DestroyPerformer(engine.CreatePerformer());
}

// Tests that a single instrument is created and destroyed as expected.
TEST(EngineTest, CreateDestroySingleInstrument) {
  constexpr float kPitch = 0.5;

  Engine engine(kSampleRate, kMaxFrameCount);

  float note_off_pitch = 0.0f;
  float note_on_pitch = 0.0f;
  {
    // Create an instrument.
    Instrument instrument = engine.CreateInstrument({});

    // Set the note callbacks.
    instrument.SetNoteEventCallback([&](NoteEventType type, float pitch) {
      (type == NoteEventType::kBegin ? note_on_pitch : note_off_pitch) = pitch;
    });
    EXPECT_FLOAT_EQ(note_on_pitch, 0.0f);
    EXPECT_FLOAT_EQ(note_off_pitch, 0.0f);

    // Set a note on.
    instrument.SetNoteOn(kPitch);
    EXPECT_TRUE(instrument.IsNoteOn(kPitch));
    EXPECT_FLOAT_EQ(note_on_pitch, kPitch);
  }

  // Note should be stopped once the instrument goes out of scope.
  EXPECT_FLOAT_EQ(note_off_pitch, kPitch);
}

// Tests that multiple instruments are created and destroyed as expected.
TEST(EngineTest, CreateDestroyMultipleInstruments) {
  std::vector<float> note_off_pitches;

  {
    Engine engine(kSampleRate, kMaxFrameCount);

    // Create instruments with note off callbacks.
    std::vector<Instrument> instruments;
    for (int i = 0; i < 3; ++i) {
      instruments.push_back(engine.CreateInstrument({}));
      instruments[i].SetNoteEventCallback([&](NoteEventType type, float pitch) {
        if (type == NoteEventType::kEnd) {
          note_off_pitches.push_back(pitch);
        }
      });
    }

    // Start multiple notes, then immediately stop some of them.
    for (int i = 0; i < 3; ++i) {
      instruments[i].SetNoteOn(static_cast<float>(i + 1));
      instruments[i].SetNoteOn(static_cast<float>(-i - 1));
      instruments[i].SetNoteOff(static_cast<float>(i + 1));
    }
    EXPECT_THAT(note_off_pitches, ElementsAre(1, 2, 3));
  }

  // Remaining active notes should be stopped once the engine goes out of scope.
  EXPECT_THAT(note_off_pitches, UnorderedElementsAre(-3.0f, -2.0f, -1.0f, 1.0f, 2.0f, 3.0f));
}

// Tests that the engine generates uniform numbers that are always within a given range.
TEST(EngineTest, GenerateRandomNumber) {
  constexpr int kValueCount = 1000;
  constexpr int kMin = -7;
  constexpr int kMax = 35;

  Engine engine(1, kSampleRate);
  for (int i = 0; i < kValueCount; ++i) {
    const int value = engine.GenerateRandomNumber(kMin, kMax);
    EXPECT_GE(value, kMin);
    EXPECT_LT(value, kMax);
  }
}

// Tests that the engine generates the same values the seed is reset with the same value.
TEST(EngineTest, SetSeed) {
  constexpr int kSeed = 1;
  constexpr int kValueCount = 10;

  Engine engine(1, kSampleRate);
  engine.SetSeed(kSeed);

  // Generate some random values.
  std::array<double, kValueCount> values;
  for (int i = 0; i < kValueCount; ++i) {
    values[i] = engine.GenerateRandomNumber();
  }

  // Reset the seed with the same value.
  engine.SetSeed(kSeed);
  // Validate that the same numbers are generated for the next `kValueCount`.
  for (int i = 0; i < kValueCount; ++i) {
    EXPECT_DOUBLE_EQ(engine.GenerateRandomNumber(), values[i]);
  }
}

TEST(ScaleTest, GetPitch) {
  constexpr float kEpsilon = 1e-5f;

  constexpr int kPitchCount = 5;
  const std::array<float, kPitchCount> kPitches = {0.0f, 0.2f, 0.35f, 0.5f, 0.95f};
  constexpr float kRootPitch = 1.75f;
  constexpr int kMode = 1;

  const Scale scale = {kPitches, kRootPitch, kMode};

  constexpr int kOctaveRange = 2;
  for (int octave = -kOctaveRange; octave <= kOctaveRange; ++octave) {
    for (int i = 0; i < kPitchCount; ++i) {
      const int degree = octave * kPitchCount + i;
      const float expected_pitch = kRootPitch +
                                   // NOLINTNEXTLINE(bugprone-integer-division)
                                   static_cast<float>(octave + (i + kMode) / kPitchCount) +
                                   kPitches[(i + kMode) % kPitchCount] - kPitches[kMode];
      EXPECT_NEAR(scale.GetPitch(degree), expected_pitch, kEpsilon) << degree;
    }
  }
}

}  // namespace
}  // namespace barely
