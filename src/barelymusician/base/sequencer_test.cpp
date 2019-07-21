#include "barelymusician/base/sequencer.h"

#include "barelymusician/base/constants.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Sampling rate.
const int kSampleRate = 48000;

// Test sequencer properties.
const float kTempo = 120.0f;
const int kNumBars = 2;
const int kNumBeats = 4;

const int kBeatsPerSecond = static_cast<int>(kTempo / kSecondsFromMinutes);

// Tests that the sequencer resets its transport position as expected.
TEST(SequencerTest, Reset) {
  const int kUpdateSeconds = 12;

  Sequencer sequencer(kSampleRate);
  sequencer.SetTempo(kTempo);
  sequencer.SetNumBars(kNumBars);
  sequencer.SetNumBeats(kNumBeats);
  EXPECT_NE(sequencer.GetTransport().num_samples, 0);

  sequencer.Update(kUpdateSeconds * kSampleRate);
  EXPECT_GE(sequencer.GetTransport().section, 0);
  EXPECT_GE(sequencer.GetTransport().bar, 0);
  EXPECT_GE(sequencer.GetTransport().beat, 0);
  EXPECT_GE(sequencer.GetTransport().sample, 0);

  sequencer.Reset();
  EXPECT_EQ(sequencer.GetTransport().section, 0);
  EXPECT_EQ(sequencer.GetTransport().bar, 0);
  EXPECT_EQ(sequencer.GetTransport().beat, 0);
  EXPECT_EQ(sequencer.GetTransport().sample, 0);
}

// Tests that the sequencer triggers the registered beat callback as expected.
TEST(SequencerTest, SetBeatCallback) {
  const int kNumSeconds = 8;

  Sequencer sequencer(kSampleRate);
  sequencer.SetTempo(kTempo);

  int beat = 0;
  const auto beat_callback = [&beat](const Transport& transport) {
    EXPECT_EQ(transport.beat, beat);
    ++beat;
  };
  sequencer.SetBeatCallback(beat_callback);

  for (int i = 0; i < kNumSeconds; ++i) {
    sequencer.Update(kSampleRate);
  }
  EXPECT_NE(beat, 0);
}

// Tests that transport parameters of the sequencer get set as expected.
TEST(SequencerTest, SetTransport) {
  const int kSection = 2;
  const int kBar = 1;
  const int kBeat = 3;

  Sequencer sequencer(kSampleRate);

  // Set number of bars.
  sequencer.SetNumBars(kNumBars);
  EXPECT_EQ(sequencer.GetTransport().num_bars, kNumBars);

  // Set number of beats.
  sequencer.SetNumBeats(kNumBeats);
  EXPECT_EQ(sequencer.GetTransport().num_beats, kNumBeats);

  // Set tempo.
  sequencer.SetTempo(kTempo);
  EXPECT_FLOAT_EQ(sequencer.GetTransport().tempo, kTempo);
}

// Tests that the sequencer updates its transport position as expected.
TEST(SequencerTest, Update) {
  Sequencer sequencer(kSampleRate);
  sequencer.SetTempo(kTempo);
  sequencer.SetNumBeats(0);
  EXPECT_NE(sequencer.GetTransport().num_samples, 0);

  // Test beat count.
  sequencer.Update(kSampleRate);
  EXPECT_EQ(sequencer.GetTransport().section, 0);
  EXPECT_EQ(sequencer.GetTransport().bar, 0);
  EXPECT_EQ(sequencer.GetTransport().beat, kBeatsPerSecond);
  EXPECT_EQ(sequencer.GetTransport().sample, 0);
  // Test bar count.
  sequencer.Reset();
  sequencer.SetNumBeats(1);
  sequencer.Update(kSampleRate);
  EXPECT_EQ(sequencer.GetTransport().section, 0);
  EXPECT_EQ(sequencer.GetTransport().bar, kBeatsPerSecond);
  EXPECT_EQ(sequencer.GetTransport().beat, 0);
  EXPECT_EQ(sequencer.GetTransport().sample, 0);
  // Test section count.
  sequencer.Reset();
  sequencer.SetNumBars(1);
  sequencer.Update(kSampleRate);
  EXPECT_EQ(sequencer.GetTransport().section, kBeatsPerSecond);
  EXPECT_EQ(sequencer.GetTransport().bar, 0);
  EXPECT_EQ(sequencer.GetTransport().beat, 0);
  EXPECT_EQ(sequencer.GetTransport().sample, 0);
}

// Tests that the sequencer updates its transport position as expected when
// initialized with the default constructor.
TEST(SequencerTest, UpdateDefault) {
  Sequencer sequencer(kSampleRate);

  EXPECT_EQ(sequencer.GetTransport().section, 0);
  EXPECT_EQ(sequencer.GetTransport().bar, 0);
  EXPECT_EQ(sequencer.GetTransport().beat, 0);
  EXPECT_EQ(sequencer.GetTransport().sample, 0);
  EXPECT_EQ(sequencer.GetTransport().num_samples, 0);

  sequencer.Update(kSampleRate);
  EXPECT_EQ(sequencer.GetTransport().section, 0);
  EXPECT_EQ(sequencer.GetTransport().bar, 0);
  EXPECT_EQ(sequencer.GetTransport().beat, 0);
  EXPECT_EQ(sequencer.GetTransport().sample, 0);
  EXPECT_EQ(sequencer.GetTransport().num_samples, 0);
}

}  // namespace
}  // namespace barelyapi
