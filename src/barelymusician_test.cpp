#include <barelymusician.h>

#include <array>
#include <cstdint>
#include <vector>

#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

using ::testing::ElementsAre;
using ::testing::UnorderedElementsAre;

constexpr int kSampleRate = 48000;

TEST(BarelyEngineTest, CreateDestroyEngine) {
  // Failures.
  EXPECT_FALSE(BarelyEngine_Create(0, nullptr));
  EXPECT_FALSE(BarelyEngine_Create(kSampleRate, nullptr));
  EXPECT_FALSE(BarelyEngine_Destroy(nullptr));

  // Success.
  BarelyEngine* engine = nullptr;
  EXPECT_TRUE(BarelyEngine_Create(kSampleRate, &engine));
  EXPECT_TRUE(BarelyEngine_Destroy(engine));
}

TEST(BarelyEngineTest, CreateDestroyInstrument) {
  BarelyEngine* engine = nullptr;
  ASSERT_TRUE(BarelyEngine_Create(kSampleRate, &engine));

  // Failures.
  EXPECT_FALSE(BarelyEngine_CreateInstrument(engine, nullptr));
  EXPECT_FALSE(BarelyEngine_DestroyInstrument(nullptr, {}));

  // Success.
  uint32_t instrument_id = 0;
  EXPECT_TRUE(BarelyEngine_CreateInstrument(engine, &instrument_id));

  EXPECT_TRUE(BarelyEngine_DestroyInstrument(engine, instrument_id));
  EXPECT_TRUE(BarelyEngine_Destroy(engine));
}

TEST(BarelyEngineTest, CreateDestroyPerformer) {
  BarelyEngine* engine = nullptr;
  ASSERT_TRUE(BarelyEngine_Create(kSampleRate, &engine));

  // Failures.
  EXPECT_FALSE(BarelyEngine_CreatePerformer(engine, nullptr));
  EXPECT_FALSE(BarelyEngine_DestroyPerformer(nullptr, {}));

  // Success.
  uint32_t performer_id = 0;
  EXPECT_TRUE(BarelyEngine_CreatePerformer(engine, &performer_id));

  EXPECT_TRUE(BarelyEngine_DestroyPerformer(engine, performer_id));
  EXPECT_TRUE(BarelyEngine_Destroy(engine));
}

TEST(EngineTest, CreateDestroyEngine) { [[maybe_unused]] const Engine engine(kSampleRate); }

TEST(EngineTest, CreateDestroyInstrument) {
  Engine engine(kSampleRate);
  [[maybe_unused]] const auto instrument = engine.CreateInstrument();
}

TEST(EngineTest, CreateDestroyPerformer) {
  Engine engine(kSampleRate);
  engine.DestroyPerformer(engine.CreatePerformer());
}

TEST(EngineTest, CreateDestroySingleInstrument) {
  constexpr float kPitch = 0.5;

  Engine engine(kSampleRate);

  float note_off_pitch = 0.0f;
  float note_on_pitch = 0.0f;

  // Create an instrument.
  Instrument instrument_ref = engine.CreateInstrument();

  // Set the note callbacks.
  instrument_ref.SetNoteEventCallback([&](NoteEventType type, float pitch) {
    (type == NoteEventType::kBegin ? note_on_pitch : note_off_pitch) = pitch;
  });
  EXPECT_FLOAT_EQ(note_on_pitch, 0.0f);
  EXPECT_FLOAT_EQ(note_off_pitch, 0.0f);

  // Set a note on.
  instrument_ref.SetNoteOn(kPitch);
  EXPECT_TRUE(instrument_ref.IsNoteOn(kPitch));
  EXPECT_FLOAT_EQ(note_on_pitch, kPitch);

  engine.DestroyInstrument(instrument_ref);

  // Note should be stopped once the instrument goes out of scope.
  EXPECT_FLOAT_EQ(note_off_pitch, kPitch);
}

TEST(EngineTest, CreateDestroyMultipleInstruments) {
  std::vector<float> note_off_pitches;

  {
    Engine engine(kSampleRate);

    // Create instruments with note off callbacks.
    std::vector<Instrument> instruments;
    for (int i = 0; i < 3; ++i) {
      instruments.push_back(engine.CreateInstrument());
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

TEST(EngineTest, GenerateRandomNumber) {
  constexpr int kValueCount = 1000;
  constexpr int kMin = -7;
  constexpr int kMax = 35;

  Engine engine(1);
  for (int i = 0; i < kValueCount; ++i) {
    const int value = engine.GenerateRandomNumber(kMin, kMax);
    EXPECT_GE(value, kMin);
    EXPECT_LT(value, kMax);
  }
}

TEST(EngineTest, ResetSeed) {
  constexpr int kSeed = 1;
  constexpr int kValueCount = 10;

  Engine engine(1);
  engine.ResetSeed(kSeed);

  // Generate some random values.
  std::array<double, kValueCount> values;
  for (int i = 0; i < kValueCount; ++i) {
    values[i] = engine.GenerateRandomNumber();
  }

  // Reset the seed with the same value.
  engine.ResetSeed(kSeed);
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
