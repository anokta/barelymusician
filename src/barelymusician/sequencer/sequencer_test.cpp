#include "barelymusician/sequencer/sequencer.h"

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

// Tests that the sequencer updates its transport position as expected when
// initialized with the default constructor.
TEST(SequencerTest, ProcessDefault) {
  Sequencer sequencer(kSampleRate);

  EXPECT_EQ(0, sequencer.GetTransport().section);
  EXPECT_EQ(0, sequencer.GetTransport().bar);
  EXPECT_EQ(0, sequencer.GetTransport().beat);
  EXPECT_FLOAT_EQ(0.0f, sequencer.GetTransport().offset_beats);

  sequencer.Update(kSampleRate);
  EXPECT_EQ(0, sequencer.GetTransport().section);
  EXPECT_EQ(0, sequencer.GetTransport().bar);
  EXPECT_EQ(0, sequencer.GetTransport().beat);
  EXPECT_FLOAT_EQ(0.0f, sequencer.GetTransport().offset_beats);
}

// Tests that the sequencer updates its transport position as expected.
TEST(SequencerTest, Process) {
  Sequencer sequencer(kSampleRate);
  sequencer.SetTempo(kTempo);
  sequencer.SetNumBeats(0);

  const auto& transport = sequencer.GetTransport();
  // Test beat count.
  sequencer.Update(kSampleRate);
  EXPECT_EQ(0, transport.section);
  EXPECT_EQ(0, transport.bar);
  EXPECT_EQ(kBeatsPerSecond, transport.beat);
  EXPECT_FLOAT_EQ(0.0f, transport.offset_beats);
  // Test bar count.
  sequencer.Reset();
  sequencer.SetNumBeats(1);
  sequencer.Update(kSampleRate);
  EXPECT_EQ(0, transport.section);
  EXPECT_EQ(kBeatsPerSecond, transport.bar);
  EXPECT_EQ(0, transport.beat);
  EXPECT_FLOAT_EQ(0.0f, transport.offset_beats);
  // Test section count.
  sequencer.Reset();
  sequencer.SetNumBars(1);
  sequencer.Update(kSampleRate);
  EXPECT_EQ(kBeatsPerSecond, transport.section);
  EXPECT_EQ(0, transport.bar);
  EXPECT_EQ(0, transport.beat);
  EXPECT_FLOAT_EQ(0.0f, transport.offset_beats);
}

// Tests that the sequencer triggers the registered beat callback as expected.
TEST(SequencerTest, RegisterBeatCallback) {
  const int kNumSeconds = 8;

  Sequencer sequencer(kSampleRate);
  sequencer.SetTempo(kTempo);

  int beat = 0;
  const auto beat_callback = [&beat](const Transport& transport,
                                     int start_sample) {
    EXPECT_EQ(beat, transport.beat);
    EXPECT_EQ(start_sample,
              (beat % kBeatsPerSecond) * kSampleRate / kBeatsPerSecond);
    ++beat;
  };
  sequencer.RegisterBeatCallback(beat_callback);

  for (int i = 0; i < kNumSeconds; ++i) {
    sequencer.Update(kSampleRate);
    EXPECT_FLOAT_EQ(0.0f, sequencer.GetTransport().offset_beats);
  }
}

// Tests that the sequencer successfully resets its transport position.
TEST(SequencerTest, Reset) {
  Sequencer sequencer(kSampleRate);
  sequencer.SetTempo(kTempo);
  sequencer.SetNumBeats(kNumBeats);

  sequencer.Update(kSampleRate);
  EXPECT_GE(sequencer.GetTransport().section, 0);
  EXPECT_GE(sequencer.GetTransport().bar, 0);
  EXPECT_GE(sequencer.GetTransport().beat, 0);
  EXPECT_GE(sequencer.GetTransport().offset_beats, 0.0f);

  sequencer.Reset();
  EXPECT_EQ(0, sequencer.GetTransport().section);
  EXPECT_EQ(0, sequencer.GetTransport().bar);
  EXPECT_EQ(0, sequencer.GetTransport().beat);
  EXPECT_FLOAT_EQ(0.0f, sequencer.GetTransport().offset_beats);
}

// Tests that transport parameters of the sequencer get set as expected.
TEST(SequencerTest, SetTransport) {
  const int kSection = 2;
  const int kBar = 1;
  const int kBeat = 3;

  Sequencer sequencer(kSampleRate);

  // Set number of bars.
  sequencer.SetNumBars(kNumBars);
  EXPECT_EQ(kNumBars, sequencer.GetTransport().num_bars);

  // Set number of beats.
  sequencer.SetNumBeats(kNumBeats);
  EXPECT_EQ(kNumBeats, sequencer.GetTransport().num_beats);

  // Set position.
  sequencer.SetPosition(kSection, kBar, kBeat);
  EXPECT_EQ(kSection, sequencer.GetTransport().section);
  EXPECT_EQ(kBar, sequencer.GetTransport().bar);
  EXPECT_EQ(kBeat, sequencer.GetTransport().beat);
  EXPECT_FLOAT_EQ(0.0f, sequencer.GetTransport().offset_beats);

  // Set tempo.
  sequencer.SetTempo(kTempo);
  EXPECT_FLOAT_EQ(kTempo, sequencer.GetTransport().tempo);
}

}  // namespace
}  // namespace barelyapi
