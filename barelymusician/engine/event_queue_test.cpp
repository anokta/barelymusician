#include "barelymusician/engine/event_queue.h"

#include "barelymusician/engine/event.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::AllOf;
using ::testing::Field;
using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::Pair;
using ::testing::Pointee;
using ::testing::VariantWith;

namespace barelyapi {
namespace {

// Tests that adding a single event is queued as expected.
TEST(EventQueueTest, AddSingleEvent) {
  EventQueue events;
  EXPECT_THAT(events.GetNext(0.0), IsNull());
  EXPECT_THAT(events.GetNext(1.0), IsNull());
  EXPECT_THAT(events.GetNext(10.0), IsNull());

  events.Add(1.0, SetNoteOffEvent{5.0});
  EXPECT_THAT(events.GetNext(0.0), IsNull());
  EXPECT_THAT(events.GetNext(1.0), IsNull());
  EXPECT_THAT(
      events.GetNext(10.0),
      AllOf(NotNull(), Pointee(Pair(1.0, VariantWith<SetNoteOffEvent>(Field(
                                             &SetNoteOffEvent::pitch, 5.0))))));

  // Event is already returned.
  EXPECT_THAT(events.GetNext(10.0), IsNull());
}

// Tests that adding multiple events are queued as expected.
TEST(EventQueueTest, AddMultipleEvents) {
  EventQueue events;
  EXPECT_THAT(events.GetNext(10.0), IsNull());

  for (double i = 0.0; i < 10.0; ++i) {
    events.Add(i, SetNoteOffEvent{i});
  }
  for (double i = 0.0; i < 10.0; ++i) {
    EXPECT_THAT(
        events.GetNext(10.0),
        AllOf(NotNull(), Pointee(Pair(i, VariantWith<SetNoteOffEvent>(Field(
                                             &SetNoteOffEvent::pitch, i))))));
  }

  // All events are already returned.
  EXPECT_THAT(events.GetNext(10.0), IsNull());
}

}  // namespace
}  // namespace barelyapi
