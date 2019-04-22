#include "barelymusician/base/sequencer.h"

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Sampling rate.
const int kSampleRate = 48000;

// Test sequencer properties.
const float kTempo = 120.0f;
const int kNumBeatsPerBar = 4;

const int kBeatsPerSecond = static_cast<int>(kTempo / kSecondsFromMinutes);

// Tests that the sequencer updates its current state as expected when
// initialized with the default constructor.
TEST(SequencerTest, ProcessDefault) {
  Sequencer sequencer(kSampleRate);
  EXPECT_EQ(0, sequencer.GetCurrentSection());
  EXPECT_EQ(0, sequencer.GetCurrentBar());
  EXPECT_EQ(0, sequencer.GetCurrentBeat());

  sequencer.Update(kSampleRate);
  EXPECT_EQ(0, sequencer.GetCurrentSection());
  EXPECT_EQ(0, sequencer.GetCurrentBar());
  EXPECT_EQ(0, sequencer.GetCurrentBeat());
}

// Tests that the sequencer updates its current state as expected.
TEST(SequencerTest, Process) {
  Sequencer sequencer(kSampleRate);
  sequencer.SetTempo(kTempo);
  sequencer.SetNumBeatsPerBar(0);
  // Test beat count.
  sequencer.Update(kSampleRate);
  EXPECT_EQ(0, sequencer.GetCurrentSection());
  EXPECT_EQ(0, sequencer.GetCurrentBar());
  EXPECT_EQ(kBeatsPerSecond, sequencer.GetCurrentBeat());
  // Test bar count.
  sequencer.Reset();
  sequencer.SetNumBeatsPerBar(1);
  sequencer.Update(kSampleRate);
  EXPECT_EQ(0, sequencer.GetCurrentSection());
  EXPECT_EQ(kBeatsPerSecond, sequencer.GetCurrentBar());
  EXPECT_EQ(0, sequencer.GetCurrentBeat());
  // Test section count.
  sequencer.Reset();
  sequencer.SetNumBarsPerSection(1);
  sequencer.Update(kSampleRate);
  EXPECT_EQ(kBeatsPerSecond, sequencer.GetCurrentSection());
  EXPECT_EQ(0, sequencer.GetCurrentBar());
  EXPECT_EQ(0, sequencer.GetCurrentBeat());
}

// Tests that the sequencer successfully resets its current state.
TEST(SequencerTest, Reset) {
  Sequencer sequencer(kSampleRate);
  sequencer.SetTempo(kTempo);
  sequencer.SetNumBeatsPerBar(kNumBeatsPerBar);

  sequencer.Update(kSampleRate);
  EXPECT_GE(sequencer.GetCurrentSection(), 0);
  EXPECT_GE(sequencer.GetCurrentBar(), 0);
  EXPECT_GE(sequencer.GetCurrentBeat(), 0);

  sequencer.Reset();
  EXPECT_EQ(0, sequencer.GetCurrentSection());
  EXPECT_EQ(0, sequencer.GetCurrentBar());
  EXPECT_EQ(0, sequencer.GetCurrentBeat());
}

}  // namespace
}  // namespace barelyapi
