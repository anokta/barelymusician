#include "barelymusician/base/sequencer.h"

#include "barelymusician/base/constants.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Sampling rate.
const int kSampleRate = 48000;

// Tempo and time signature.
const float kBpm = 120.0f;
const int kNumBeatsPerBar = 4;
const Sequencer::NoteValue kBeatLength = Sequencer::NoteValue::kQuarterNote;

const float kBeatsPerSecond = kBpm / kSecondsFromMinutes;

// Tests that the sequencer updates its current state as expected when
// initialized with the default constructor.
TEST(SequencerTest, ProcessDefault) {
  Sequencer sequencer(kSampleRate);
  EXPECT_EQ(0, sequencer.GetCurrentBeat());
  EXPECT_EQ(0, sequencer.GetCurrentBar());
  EXPECT_EQ(0, sequencer.GetCurrentSection());
  EXPECT_EQ(0, sequencer.GetCurrentSampleOffset());

  sequencer.Update(kSampleRate);
  EXPECT_EQ(0, sequencer.GetCurrentBeat());
  EXPECT_EQ(0, sequencer.GetCurrentBar());
  EXPECT_EQ(0, sequencer.GetCurrentSection());
  EXPECT_EQ(0, sequencer.GetCurrentSampleOffset());
}

// Tests that the sequencer updates its current state as expected.
TEST(SequencerTest, Process) {
  Sequencer sequencer(kSampleRate);
  sequencer.SetBpm(kBpm);
  sequencer.SetNumBeatsPerBar(1);
  sequencer.SetNumBarsPerSection(1);
  sequencer.SetBeatLength(Sequencer::NoteValue::kWholeNote);

  sequencer.Update(kSampleRate);
  EXPECT_EQ(0, sequencer.GetCurrentBeat());
  EXPECT_EQ(0, sequencer.GetCurrentBar());
  EXPECT_EQ(kBeatsPerSecond, sequencer.GetCurrentSection());
  EXPECT_EQ(0, sequencer.GetCurrentSampleOffset());
}

// Tests that the sequencer successfully resets its current state.
TEST(SequencerTest, Reset) {
  Sequencer sequencer(kSampleRate);
  sequencer.SetBpm(kBpm);
  sequencer.SetNumBeatsPerBar(kNumBeatsPerBar);
  sequencer.SetBeatLength(kBeatLength);

  sequencer.Update(kSampleRate);
  EXPECT_GE(sequencer.GetCurrentBeat(), 0);
  EXPECT_GE(sequencer.GetCurrentBar(), 0);
  EXPECT_GE(sequencer.GetCurrentSection(), 0);
  EXPECT_GE(sequencer.GetCurrentSampleOffset(), 0);

  sequencer.Reset();
  EXPECT_EQ(0, sequencer.GetCurrentBeat());
  EXPECT_EQ(0, sequencer.GetCurrentBar());
  EXPECT_EQ(0, sequencer.GetCurrentSection());
  EXPECT_EQ(0, sequencer.GetCurrentSampleOffset());
}

}  // namespace
}  // namespace barelyapi
