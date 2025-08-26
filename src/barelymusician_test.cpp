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
constexpr float kReferenceFrequency = kDefaultReferenceFrequency;

TEST(BarelyEngineTest, CreateDestroyEngine) {
  // Failures.
  EXPECT_FALSE(BarelyEngine_Create(0, 0, 0.0f, nullptr));
  EXPECT_FALSE(BarelyEngine_Create(kSampleRate, 0, 0.0f, nullptr));
  EXPECT_FALSE(BarelyEngine_Create(kSampleRate, kMaxFrameCount, 0.0f, nullptr));
  EXPECT_FALSE(BarelyEngine_Create(kSampleRate, kMaxFrameCount, kReferenceFrequency, nullptr));
  EXPECT_FALSE(BarelyEngine_Destroy(nullptr));

  // Success.
  BarelyEngineHandle engine = nullptr;
  EXPECT_TRUE(BarelyEngine_Create(kSampleRate, kMaxFrameCount, kReferenceFrequency, &engine));
  EXPECT_TRUE(BarelyEngine_Destroy(engine));
}

TEST(BarelyEngineTest, CreateDestroyInstrument) {
  BarelyEngineHandle engine = nullptr;
  ASSERT_TRUE(BarelyEngine_Create(kSampleRate, kMaxFrameCount, kReferenceFrequency, &engine));

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
  ASSERT_TRUE(BarelyEngine_Create(kSampleRate, kMaxFrameCount, kReferenceFrequency, &engine));

  // Failures.
  EXPECT_FALSE(BarelyPerformer_Create(engine, nullptr));
  EXPECT_FALSE(BarelyPerformer_Destroy(nullptr));

  // Success.
  BarelyPerformerHandle performer = nullptr;
  EXPECT_TRUE(BarelyPerformer_Create(engine, &performer));
  EXPECT_NE(performer, nullptr);

  EXPECT_TRUE(BarelyPerformer_Destroy(performer));
  EXPECT_TRUE(BarelyEngine_Destroy(engine));
}

// Tests that amplitude/decibels conversion returns expected results.
TEST(DecibelsTest, AmplitudeDecibelsConversion) {
  constexpr float kEpsilon = 5e-2f;

  constexpr int kValueCount = 4;
  constexpr std::array<float, kValueCount> kAmplitudes = {0.0f, 0.1f, 1.0f, 2.0f};
  constexpr std::array<float, kValueCount> kDecibels = {-80.0f, -20.0f, 0.0f, 6.0f};

  for (int i = 0; i < kValueCount; ++i) {
    EXPECT_NEAR(AmplitudeToDecibels(kAmplitudes[i]), kDecibels[i], kEpsilon);
    EXPECT_NEAR(DecibelsToAmplitude(kDecibels[i]), kAmplitudes[i], kEpsilon);

    // Verify that the back and forth conversions do not mutate the value.
    EXPECT_FLOAT_EQ(AmplitudeToDecibels(DecibelsToAmplitude(kDecibels[i])), kDecibels[i]);
    EXPECT_FLOAT_EQ(DecibelsToAmplitude(AmplitudeToDecibels(kAmplitudes[i])), kAmplitudes[i]);
  }
}

// Tests that amplitude/decibels conversion snaps to `kMinDecibels` threshold.
TEST(DecibelsTest, AmplitudeDecibelsMinThreshold) {
  EXPECT_FLOAT_EQ(AmplitudeToDecibels(0.0f), kMinDecibels);
  EXPECT_FLOAT_EQ(DecibelsToAmplitude(kMinDecibels), 0.0f);
}

TEST(EngineTest, CreateDestroyEngine) {
  [[maybe_unused]] const Engine engine(kSampleRate, kMaxFrameCount);
}

TEST(EngineTest, CreateDestroyInstrument) {
  Engine engine(kSampleRate, kMaxFrameCount, kReferenceFrequency);
  [[maybe_unused]] const auto instrument = engine.CreateInstrument();
}

TEST(EngineTest, CreateDestroyPerformer) {
  Engine engine(kSampleRate, kMaxFrameCount, kReferenceFrequency);
  [[maybe_unused]] const auto performer = engine.CreatePerformer();
}

// Tests that a single instrument is created and destroyed as expected.
TEST(EngineTest, CreateDestroySingleInstrument) {
  constexpr float kPitch = 0.5;

  Engine engine(kSampleRate, kMaxFrameCount, kReferenceFrequency);

  float note_off_pitch = 0.0f;
  float note_on_pitch = 0.0f;
  {
    // Create an instrument.
    Instrument instrument = engine.CreateInstrument({});

    // Set the note callbacks.
    instrument.SetNoteEventCallback([&](NoteEventType type, float pitch) {
      (type == NoteEventType::kOn ? note_on_pitch : note_off_pitch) = pitch;
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
    Engine engine(kSampleRate, kMaxFrameCount, kReferenceFrequency);

    // Create instruments with note off callbacks.
    std::vector<Instrument> instruments;
    for (int i = 0; i < 3; ++i) {
      instruments.push_back(engine.CreateInstrument({}));
      instruments[i].SetNoteEventCallback([&](NoteEventType type, float pitch) {
        if (type == NoteEventType::kOff) {
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

  Engine engine(1, 1);
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

  Engine engine(1, 1);
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
      EXPECT_FLOAT_EQ(scale.GetPitch(degree), expected_pitch) << degree;
    }
  }
}

}  // namespace
}  // namespace barely
