#include "barelymusician/composition/performer.h"

#include <vector>

#include "barelymusician/base/logging.h"
#include "barelymusician/base/note.h"
#include "barelymusician/composition/message.h"
#include "barelymusician/composition/message_utils.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Default number of samples to perform.
const int kNumSamples = 16;

// Test note intensity.
const float kNoteIntensity = 1.0f;

// Fake voice that produces constant note index as output for testing.
class FakeInstrument : public Instrument {
 public:
  FakeInstrument() : output_(0.0f) {}

  // Implements |UnitGenerator|.
  float Next() override { return output_; }
  void Reset() override { output_ = 0.0f; }

  // Implements |Instrument|.
  void NoteOn(float index, float intensity) override {
    LOG(INFO) << "NoteOn(" << index << ", " << intensity << ")";
    output_ = index;
  }
  void NoteOff(float index) override {
    LOG(INFO) << "NoteOff(" << index << ")";
    output_ = 0.0f;
  }
  bool SetFloatParam(ParamId id, float value) override {
    LOG(INFO) << "SetFloatParam(" << id << ", " << value << ")";
    return false;
  }

 private:
  float output_;
};

// Tests that performing a single note produces the expected output.
TEST(PerformerTest, PerformSingleNote) {
  const float kNoteIndex = 32.0f;

  FakeInstrument instrument;
  Performer performer(&instrument);

  std::vector<float> output(kNumSamples, 0.0f);
  performer.Process(0, kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(0.0f, output[i]);
  }

  // Perform note on.
  Message note_on_message;
  note_on_message.type = MessageType::kNoteOn;
  WriteMessageData<Note>({kNoteIndex, kNoteIntensity}, note_on_message.data);
  note_on_message.timestamp = 0;
  performer.Perform(note_on_message);

  output.assign(kNumSamples, 0.0f);
  performer.Process(0, kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(kNoteIndex, output[i]);
  }

  // Perform note off.
  Message note_off_message;
  note_off_message.type = MessageType::kNoteOff;
  WriteMessageData<Note>({kNoteIndex, kNoteIntensity}, note_off_message.data);
  note_off_message.timestamp = kNumSamples;
  performer.Perform(note_off_message);

  output.assign(kNumSamples, 0.0f);
  performer.Process(kNumSamples, kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(0.0f, output[i]);
  }
}

// Tests that performing multiple notes produces the expected output.
TEST(PerformerTest, PerformMultipleNotes) {
  FakeInstrument instrument;
  Performer performer(&instrument);

  std::vector<float> output(kNumSamples, 0.0f);
  performer.Process(0, kNumSamples, output.data());
  for (const auto& sample : output) {
    EXPECT_FLOAT_EQ(0.0f, sample);
  }

  // Perform a new note per each sample in the buffer.
  Message note_on_message;
  note_on_message.type = MessageType::kNoteOn;
  for (int i = 0; i < kNumSamples; ++i) {
    WriteMessageData<Note>({static_cast<float>(i), kNoteIntensity},
                           note_on_message.data);
    note_on_message.timestamp = i;
    performer.Perform(note_on_message);
  }

  output.assign(kNumSamples, 0.0f);
  performer.Process(0, kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(static_cast<float>(i), output[i]);
  }

  // Perform note off.
  Message note_off_message;
  note_off_message.type = MessageType::kNoteOff;
  WriteMessageData<Note>({0.0f, kNoteIntensity}, note_off_message.data);
  note_off_message.timestamp = kNumSamples;
  performer.Perform(note_off_message);

  output.assign(kNumSamples, 0.0f);
  performer.Process(kNumSamples, kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(0.0f, output[i]);
  }
}

// Tests that resetting the performer clears out the output buffer as expected.
TEST(PerformerTest, Reset) {
  const float kNoteIndex = 1.0f;

  FakeInstrument instrument;
  Performer performer(&instrument);

  // Perform note on, then reset.
  Message note_on_message;
  note_on_message.type = MessageType::kNoteOn;
  WriteMessageData<Note>({kNoteIndex, kNoteIntensity}, note_on_message.data);
  note_on_message.timestamp = 0;
  performer.Perform(note_on_message);
  performer.Reset();

  std::vector<float> output(kNumSamples, 0.0f);
  performer.Process(0, kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(0.0f, output[i]);
  }
}

}  // namespace
}  // namespace barelyapi
