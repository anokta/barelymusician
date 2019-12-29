#include "barelymusician/base/sequencer.h"

#include "barelymusician/base/constants.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Sampling rate.
const int kSampleRate = 48000;

// Sequencer tempo.
const float kTempo = 120.0f;

// Tests that the sequencer triggers the registered beat callback as expected.
TEST(SequencerTest, SetBeatCallback) {
  const int kNumSeconds = 8;

  Sequencer sequencer(kSampleRate);
  sequencer.SetTempo(kTempo);

  int expected_beat = 0;
  const auto beat_callback = [&expected_beat](int beat, int) {
    EXPECT_EQ(beat, expected_beat);
    ++expected_beat;
  };
  sequencer.SetBeatCallback(beat_callback);

  for (int i = 0; i < kNumSeconds; ++i) {
    sequencer.Update(kSampleRate);
  }
  EXPECT_NE(expected_beat, 0);
}

}  // namespace
}  // namespace barelyapi
