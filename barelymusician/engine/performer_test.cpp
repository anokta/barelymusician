#include "barelymusician/engine/performer.h"

#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/engine/conductor.h"
#include "barelymusician/engine/instrument_event.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

using ::testing::ElementsAre;
using ::testing::Pair;

// Tests that performed instrument events are returned as expected.
TEST(PerformerTest, Perform) {
  Performer performer;

  for (int i = 0; i < 4; ++i) {
    EXPECT_TRUE(IsOk(performer.GetMutableSequence()->AddNote(
        Id{i}, static_cast<double>(i),
        Note{.pitch = static_cast<float>(i), .duration = 1.0})));
  }

  Conductor conductor;
  EXPECT_TRUE(performer.Perform(0.0, 4.0, conductor).empty());

  // Add instruments.
  EXPECT_TRUE(IsOk(performer.AddInstrument(Id{1})));
  EXPECT_TRUE(IsOk(performer.AddInstrument(Id{2})));
  EXPECT_TRUE(IsOk(performer.AddInstrument(Id{3})));

  EXPECT_EQ(performer.Perform(0.0, 4.0, conductor).size(), 21);

  // Remove instrument.
  EXPECT_EQ(GetStatusOrValue(performer.RemoveInstrument(Id{2})).size(), 1);

  EXPECT_EQ(performer.Perform(0.0, 4.0, conductor).size(), 16);

  // Set begin and end positions.
  performer.SetSequenceBeginPosition(2.0);
  EXPECT_EQ(performer.GetSequenceBeginPosition(), 2.0);

  performer.SetSequenceEndPosition(3.0);
  EXPECT_EQ(performer.GetSequenceEndPosition(), 3.0);

  EXPECT_EQ(performer.Perform(0.0, 4.0, conductor).size(), 4);

  // Remove all instruments.
  EXPECT_EQ(performer.RemoveAllInstruments().size(), 2);
}

}  // namespace
}  // namespace barely
