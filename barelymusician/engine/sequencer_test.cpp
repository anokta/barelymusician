#include "barelymusician/engine/sequencer.h"

#include <limits>
#include <vector>

#include "barelymusician/engine/id.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

// Tests that sequencer triggers multiple events as expected.
TEST(SequencerTest, TriggerMultipleEvents) {
  Sequencer sequencer;

  EXPECT_FALSE(sequencer.IsPlaying());
  EXPECT_DOUBLE_EQ(sequencer.GetPosition(), 0.0);
  EXPECT_DOUBLE_EQ(sequencer.GetDurationToNextEvent(),
                   std::numeric_limits<double>::max());

  // Add events.
  std::vector<double> positions;
  for (int i = 1; i <= 4; ++i) {
    sequencer.AddEvent(Id{i}, static_cast<double>(i), [&, i](double position) {
      EXPECT_DOUBLE_EQ(position, static_cast<double>(i));
      positions.push_back(position);
    });
  }
  EXPECT_FALSE(sequencer.IsPlaying());
  EXPECT_DOUBLE_EQ(sequencer.GetPosition(), 0.0);
  EXPECT_DOUBLE_EQ(sequencer.GetDurationToNextEvent(),
                   std::numeric_limits<double>::max());
  EXPECT_TRUE(positions.empty());

  // Start playback.
  sequencer.Start();
  EXPECT_TRUE(sequencer.IsPlaying());
  EXPECT_DOUBLE_EQ(sequencer.GetPosition(), 0.0);
  EXPECT_DOUBLE_EQ(sequencer.GetDurationToNextEvent(), 1.0);
  EXPECT_TRUE(positions.empty());

  // Trigger events.
  std::vector<double> expected_positions;
  for (int i = 1; i <= 4; ++i) {
    const double expected_position = static_cast<double>(i);
    expected_positions.push_back(expected_position);
    EXPECT_DOUBLE_EQ(sequencer.GetDurationToNextEvent(), 1.0);

    sequencer.Update(sequencer.GetDurationToNextEvent());
    EXPECT_DOUBLE_EQ(sequencer.GetPosition(), expected_position);

    sequencer.TriggerAllEventsAtCurrentPosition();
    EXPECT_EQ(positions, expected_positions);
  }

  EXPECT_TRUE(sequencer.IsPlaying());
  EXPECT_DOUBLE_EQ(sequencer.GetPosition(), 4.0);
  EXPECT_DOUBLE_EQ(sequencer.GetDurationToNextEvent(),
                   std::numeric_limits<double>::max());
}

// Tests that sequencer triggers a single event as expected.
TEST(SequencerTest, TriggerSingleEvent) {
  Sequencer sequencer;

  EXPECT_FALSE(sequencer.IsPlaying());
  EXPECT_DOUBLE_EQ(sequencer.GetPosition(), 0.0);
  EXPECT_DOUBLE_EQ(sequencer.GetDurationToNextEvent(),
                   std::numeric_limits<double>::max());

  // Add event.
  int num_event_triggers = 0;
  EXPECT_TRUE(sequencer.AddEvent(Id{1}, 0.25, [&](double position) {
    EXPECT_DOUBLE_EQ(position, 0.25);
    ++num_event_triggers;
  }));
  EXPECT_FALSE(sequencer.IsPlaying());
  EXPECT_DOUBLE_EQ(sequencer.GetPosition(), 0.0);
  EXPECT_DOUBLE_EQ(sequencer.GetDurationToNextEvent(),
                   std::numeric_limits<double>::max());
  EXPECT_EQ(num_event_triggers, 0);

  // Start playback.
  sequencer.Start();
  EXPECT_TRUE(sequencer.IsPlaying());
  EXPECT_DOUBLE_EQ(sequencer.GetPosition(), 0.0);
  EXPECT_DOUBLE_EQ(sequencer.GetDurationToNextEvent(), 0.25);
  EXPECT_EQ(num_event_triggers, 0);

  // Trigger event.
  sequencer.Update(0.25);
  EXPECT_TRUE(sequencer.IsPlaying());
  EXPECT_DOUBLE_EQ(sequencer.GetPosition(), 0.25);
  EXPECT_DOUBLE_EQ(sequencer.GetDurationToNextEvent(), 0.0);
  EXPECT_EQ(num_event_triggers, 0);

  sequencer.TriggerAllEventsAtCurrentPosition();
  EXPECT_TRUE(sequencer.IsPlaying());
  EXPECT_DOUBLE_EQ(sequencer.GetPosition(), 0.25);
  EXPECT_DOUBLE_EQ(sequencer.GetDurationToNextEvent(),
                   std::numeric_limits<double>::max());
  EXPECT_EQ(num_event_triggers, 1);

  // Set looping on.
  sequencer.SetLooping(true);
  EXPECT_DOUBLE_EQ(sequencer.GetDurationToNextEvent(), 1.0);

  // Trigger next event with a loop back.
  sequencer.Update(1.0);
  EXPECT_TRUE(sequencer.IsPlaying());
  EXPECT_DOUBLE_EQ(sequencer.GetPosition(), 0.25);
  EXPECT_DOUBLE_EQ(sequencer.GetDurationToNextEvent(), 0.0);
  EXPECT_EQ(num_event_triggers, 1);

  sequencer.TriggerAllEventsAtCurrentPosition();
  EXPECT_TRUE(sequencer.IsPlaying());
  EXPECT_DOUBLE_EQ(sequencer.GetPosition(), 0.25);
  EXPECT_DOUBLE_EQ(sequencer.GetDurationToNextEvent(), 1.0);
  EXPECT_EQ(num_event_triggers, 2);

  // Update event position and callback.
  EXPECT_TRUE(sequencer.SetEventPosition(Id{1}, 0.75));
  EXPECT_TRUE(sequencer.SetEventCallback(Id{1}, [&](double position) {
    EXPECT_DOUBLE_EQ(position, 0.75);
    --num_event_triggers;
  }));
  EXPECT_TRUE(sequencer.IsPlaying());
  EXPECT_DOUBLE_EQ(sequencer.GetPosition(), 0.25);
  EXPECT_DOUBLE_EQ(sequencer.GetDurationToNextEvent(), 0.5);
  EXPECT_EQ(num_event_triggers, 2);

  // Trigger event with the updated position and callback.
  sequencer.Update(0.5);
  EXPECT_TRUE(sequencer.IsPlaying());
  EXPECT_DOUBLE_EQ(sequencer.GetPosition(), 0.75);
  EXPECT_DOUBLE_EQ(sequencer.GetDurationToNextEvent(), 0.0);
  EXPECT_EQ(num_event_triggers, 2);

  sequencer.TriggerAllEventsAtCurrentPosition();
  EXPECT_TRUE(sequencer.IsPlaying());
  EXPECT_DOUBLE_EQ(sequencer.GetPosition(), 0.75);
  EXPECT_DOUBLE_EQ(sequencer.GetDurationToNextEvent(), 1.0);
  EXPECT_EQ(num_event_triggers, 1);

  // Stop playback.
  sequencer.Stop();
  EXPECT_FALSE(sequencer.IsPlaying());
  EXPECT_DOUBLE_EQ(sequencer.GetPosition(), 0.75);
  EXPECT_DOUBLE_EQ(sequencer.GetDurationToNextEvent(),
                   std::numeric_limits<double>::max());
  EXPECT_EQ(num_event_triggers, 1);
}

// TODO(#108): Add `TriggerOneOffEvents` test (at minimum).

// Tests that sequencer sets its current position as expected.
TEST(SequencerTest, SetPosition) {
  Sequencer sequencer;
  EXPECT_EQ(sequencer.GetPosition(), 0.0);

  sequencer.SetPosition(2.75);
  EXPECT_DOUBLE_EQ(sequencer.GetPosition(), 2.75);

  sequencer.SetPosition(1.25);
  EXPECT_DOUBLE_EQ(sequencer.GetPosition(), 1.25);

  // Set looping on which should wrap the current position back.
  sequencer.SetLooping(true);
  EXPECT_DOUBLE_EQ(sequencer.GetPosition(), 0.25);

  sequencer.SetPosition(3.5);
  EXPECT_DOUBLE_EQ(sequencer.GetPosition(), 0.5);

  // Set loop begin position.
  sequencer.SetLoopBeginPosition(0.75);
  EXPECT_DOUBLE_EQ(sequencer.GetPosition(), 0.5);

  // Set loop length.
  sequencer.SetLoopLength(2.0);
  EXPECT_DOUBLE_EQ(sequencer.GetPosition(), 0.5);

  sequencer.SetPosition(4.0f);
  EXPECT_DOUBLE_EQ(sequencer.GetPosition(), 2.0);

  // Resetting back position before the loop should still be okay.
  sequencer.SetPosition(0.25);
  EXPECT_DOUBLE_EQ(sequencer.GetPosition(), 0.25);
}

}  // namespace
}  // namespace barely::internal
