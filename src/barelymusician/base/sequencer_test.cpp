#include "barelymusician/base/sequencer.h"

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Sampling rate.
const int kSampleRate = 48000;

// Tempo and time signature.
const float kBpm = 120.0f;
const int kNumBeatsPerBar = 4;
const Sequencer::NoteValue kBeatLength = Sequencer::NoteValue::kQuarterNote;

const int kBeatsPerSecond = static_cast<int>(kBpm / kSecondsFromMinutes);

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
  const int kNumUpdateSamples = kSampleRate + 1;

  Sequencer sequencer(kSampleRate);
  sequencer.SetBpm(kBpm);
  sequencer.SetTimeSignature(0, kBeatLength);
  // Test beat count.
  sequencer.Update(kNumUpdateSamples);
  EXPECT_EQ(0, sequencer.GetCurrentSection());
  EXPECT_EQ(0, sequencer.GetCurrentBar());
  EXPECT_EQ(kBeatsPerSecond, sequencer.GetCurrentBeat());
  // Test bar count.
  sequencer.Reset();
  sequencer.SetTimeSignature(1, kBeatLength);
  sequencer.Update(kNumUpdateSamples);
  EXPECT_EQ(0, sequencer.GetCurrentSection());
  EXPECT_EQ(kBeatsPerSecond, sequencer.GetCurrentBar());
  EXPECT_EQ(0, sequencer.GetCurrentBeat());
  // Test section count.
  sequencer.Reset();
  sequencer.SetNumBarsPerSection(1);
  sequencer.Update(kNumUpdateSamples);
  EXPECT_EQ(kBeatsPerSecond, sequencer.GetCurrentSection());
  EXPECT_EQ(0, sequencer.GetCurrentBar());
  EXPECT_EQ(0, sequencer.GetCurrentBeat());
}

// Tests that the sequencer triggers the registered beat callback as expected.
TEST(SequencerTest, RegisterBeatCallback) {
  const int kNumBarsPerSection = 2;
  const int kNumSeconds = 8;

  Sequencer sequencer(kSampleRate);

  int current_section = sequencer.GetCurrentSection();
  int current_bar = sequencer.GetCurrentBar();
  int current_beat = sequencer.GetCurrentBeat();
  const auto beat_callback = [&current_section, &current_bar, &current_beat](
                                 int section, int bar, int beat,
                                 int offset_samples) {
    LOG(INFO) << section << "." << bar << "." << beat << ":" << offset_samples;
    current_section = section;
    current_bar = bar;
    current_beat = beat;
  };
  sequencer.RegisterBeatCallback(beat_callback);

  sequencer.SetBpm(kBpm);
  sequencer.SetTimeSignature(kNumBeatsPerBar, kBeatLength);
  sequencer.SetNumBarsPerSection(kNumBarsPerSection);

  for (int i = 0; i < kNumSeconds; ++i) {
    sequencer.Update(kSampleRate);
    EXPECT_EQ(sequencer.GetCurrentSection(), current_section);
    EXPECT_EQ(sequencer.GetCurrentBar(), current_bar);
    EXPECT_EQ(sequencer.GetCurrentBeat(), current_beat);
  }
}

// Tests that the sequencer successfully resets its current state.
TEST(SequencerTest, Reset) {
  Sequencer sequencer(kSampleRate);
  sequencer.SetBpm(kBpm);
  sequencer.SetTimeSignature(kNumBeatsPerBar, kBeatLength);

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
