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
  EXPECT_EQ(0, sequencer.current_beat());
  EXPECT_EQ(0, sequencer.current_bar());
  EXPECT_EQ(0, sequencer.current_section());
  EXPECT_EQ(0, sequencer.sample_offset());

  sequencer.Update(kSampleRate);
  EXPECT_EQ(0, sequencer.current_beat());
  EXPECT_EQ(0, sequencer.current_bar());
  EXPECT_EQ(0, sequencer.current_section());
  EXPECT_EQ(0, sequencer.sample_offset());
}

// Tests that the sequencer updates its current state as expected.
TEST(SequencerTest, Process) {
  Sequencer sequencer(kSampleRate);
  sequencer.SetBpm(kBpm);
  sequencer.SetTimeSignature(0, kBeatLength);
  // Test beat count.
  sequencer.Update(kSampleRate);
  EXPECT_EQ(static_cast<int>(kBeatsPerSecond), sequencer.current_beat());
  EXPECT_EQ(0, sequencer.current_bar());
  EXPECT_EQ(0, sequencer.current_section());
  EXPECT_EQ(0, sequencer.sample_offset());
  // Test bar count.
  sequencer.Reset();
  sequencer.SetTimeSignature(1, kBeatLength);
  sequencer.Update(kSampleRate);
  EXPECT_EQ(0, sequencer.current_beat());
  EXPECT_EQ(static_cast<int>(kBeatsPerSecond), sequencer.current_bar());
  EXPECT_EQ(0, sequencer.current_section());
  EXPECT_EQ(0, sequencer.sample_offset());
  // Test section count.
  sequencer.Reset();
  sequencer.SetNumBarsPerSection(1);
  sequencer.Update(kSampleRate);
  EXPECT_EQ(0, sequencer.current_beat());
  EXPECT_EQ(0, sequencer.current_bar());
  EXPECT_EQ(static_cast<int>(kBeatsPerSecond), sequencer.current_section());
  EXPECT_EQ(0, sequencer.sample_offset());
}

// Tests that the sequencer successfully resets its current state.
TEST(SequencerTest, Reset) {
  Sequencer sequencer(kSampleRate);
  sequencer.SetBpm(kBpm);
  sequencer.SetTimeSignature(kNumBeatsPerBar, kBeatLength);

  sequencer.Update(kSampleRate);
  EXPECT_GE(sequencer.current_beat(), 0);
  EXPECT_GE(sequencer.current_bar(), 0);
  EXPECT_GE(sequencer.current_section(), 0);
  EXPECT_GE(sequencer.sample_offset(), 0);

  sequencer.Reset();
  EXPECT_EQ(0, sequencer.current_beat());
  EXPECT_EQ(0, sequencer.current_bar());
  EXPECT_EQ(0, sequencer.current_section());
  EXPECT_EQ(0, sequencer.sample_offset());
}

}  // namespace
}  // namespace barelyapi
