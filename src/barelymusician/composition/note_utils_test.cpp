#include "barelymusician/composition/note_utils.h"

#include <utility>
#include <vector>

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that note indices are transformed correctly given an arbitrary scale.
TEST(NoteUtilsTest, GetScaledNoteIndex) {
  const std::vector<float> kMajorScale = {0.0f, 2.0f, 4.0f, 5.0f,
                                          7.0f, 9.0f, 11.0f};
  const std::vector<std::pair<float, float>> kRawScaledNoteIndexPairs = {
      {-28.0f, -48.0f}, {-15.0f, -25.0f}, {-14.0f, -24.0f}, {-9.0f, -15.0f},
      {-6.0f, -10.0f},  {-1.0f, -1.0f},   {0.0f, 0.0f},     {1.0f, 2.0f},
      {3.0f, 5.0f},     {6.0f, 11.0f},    {7.0f, 12.0f},    {8.0f, 14.0f},
      {14.0f, 24.0f},   {15.0f, 26.0f},   {28.0f, 48.0f}};

  for (const auto& note_index_pair : kRawScaledNoteIndexPairs) {
    EXPECT_FLOAT_EQ(note_index_pair.second,
                    GetScaledNoteIndex(note_index_pair.first, kMajorScale));
  }
}

}  // namespace
}  // namespace barelyapi
