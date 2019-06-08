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

// Tests that the sequencer triggers the registered beat callback as expected.
TEST(SequencerTest, RegisterBeatCallback) {
  const int kNumSeconds = 8;

  Sequencer sequencer(kSampleRate);
  sequencer.SetTempo(kTempo);
  sequencer.Start();

  int beat = 0;
  const auto beat_callback = [&beat](const Transport& transport,
                                     int start_sample,
                                     int num_samples_per_beat) {
    EXPECT_EQ(transport.beat, beat);
    EXPECT_EQ((beat % kBeatsPerSecond) * num_samples_per_beat, start_sample);
    ++beat;
  };
  sequencer.RegisterBeatCallback(beat_callback);

  for (int i = 0; i < kNumSeconds; ++i) {
    sequencer.Update(kSampleRate);
  }
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

  // Set position.
  sequencer.SetPosition(kSection, kBar, kBeat);
  EXPECT_EQ(sequencer.GetTransport().section, kSection);
  EXPECT_EQ(sequencer.GetTransport().bar, kBar);
  EXPECT_EQ(sequencer.GetTransport().beat, kBeat);

  // Set tempo.
  sequencer.SetTempo(kTempo);
  EXPECT_FLOAT_EQ(sequencer.GetTransport().tempo, kTempo);
}

// Tests that the sequencer updates its transport as expected when it's started
// and stopped respectively.
TEST(SequencerTest, StartStop) {
  Sequencer sequencer(kSampleRate);
  sequencer.SetTempo(kTempo);
  sequencer.SetNumBeats(0);

  sequencer.Update(kSampleRate);
  EXPECT_EQ(sequencer.GetTransport().section, 0);
  EXPECT_EQ(sequencer.GetTransport().bar, 0);
  EXPECT_EQ(sequencer.GetTransport().beat, 0);

  // Start the playback.
  sequencer.Start();
  sequencer.Update(kSampleRate);
  EXPECT_EQ(sequencer.GetTransport().section, 0);
  EXPECT_EQ(sequencer.GetTransport().bar, 0);
  EXPECT_EQ(sequencer.GetTransport().beat, kBeatsPerSecond);

  // Stop the playback.
  sequencer.Stop();
  sequencer.Update(kSampleRate);
  EXPECT_EQ(sequencer.GetTransport().section, 0);
  EXPECT_EQ(sequencer.GetTransport().bar, 0);
  EXPECT_EQ(sequencer.GetTransport().beat, kBeatsPerSecond);
}

// Tests that the sequencer updates its transport position as expected.
TEST(SequencerTest, Update) {
  Sequencer sequencer(kSampleRate);
  sequencer.SetTempo(kTempo);
  sequencer.SetNumBeats(0);
  sequencer.Start();

  // Test beat count.
  sequencer.Update(kSampleRate);
  EXPECT_EQ(sequencer.GetTransport().section, 0);
  EXPECT_EQ(sequencer.GetTransport().bar, 0);
  EXPECT_EQ(sequencer.GetTransport().beat, kBeatsPerSecond);
  // Test bar count.
  sequencer.SetPosition(0, 0, 0);
  sequencer.SetNumBeats(1);
  sequencer.Update(kSampleRate);
  EXPECT_EQ(sequencer.GetTransport().section, 0);
  EXPECT_EQ(sequencer.GetTransport().bar, kBeatsPerSecond);
  EXPECT_EQ(sequencer.GetTransport().beat, 0);
  // Test section count.
  sequencer.SetPosition(0, 0, 0);
  sequencer.SetNumBars(1);
  sequencer.Update(kSampleRate);
  EXPECT_EQ(sequencer.GetTransport().section, kBeatsPerSecond);
  EXPECT_EQ(sequencer.GetTransport().bar, 0);
  EXPECT_EQ(sequencer.GetTransport().beat, 0);
}

// Tests that the sequencer updates its transport position as expected when
// initialized with the default constructor.
TEST(SequencerTest, UpdateDefault) {
  Sequencer sequencer(kSampleRate);

  EXPECT_EQ(sequencer.GetTransport().section, 0);
  EXPECT_EQ(sequencer.GetTransport().bar, 0);
  EXPECT_EQ(sequencer.GetTransport().beat, 0);

  sequencer.Update(kSampleRate);
  EXPECT_EQ(sequencer.GetTransport().section, 0);
  EXPECT_EQ(sequencer.GetTransport().bar, 0);
  EXPECT_EQ(sequencer.GetTransport().beat, 0);
}

}  // namespace
}  // namespace barelyapi
