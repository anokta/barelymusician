#include "barelymusician/engine/instrument_processor.h"

#include <algorithm>
#include <any>
#include <cstdint>
#include <vector>

#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_event.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

constexpr int kNumChannels = 1;
constexpr int kNumFrames = 16;

// Returns test instrument definition that produces constant output per note.
InstrumentDefinition GetTestInstrumentDefinition() {
  return InstrumentDefinition{
      .create_fn = [](InstrumentState* state,
                      int /*sample_rate*/) { state->emplace<float>(0.0f); },
      .destroy_fn = [](InstrumentState* state) { state->reset(); },
      .process_fn =
          [](InstrumentState* state, float* output, int num_channels,
             int num_frames) {
            std::fill_n(output, num_channels * num_frames,
                        *std::any_cast<float>(state));
          },
      .set_note_off_fn =
          [](InstrumentState* state, float /*pitch*/) {
            *std::any_cast<float>(state) = 0.0f;
          },
      .set_note_on_fn =
          [](InstrumentState* state, float pitch, float intensity) {
            *std::any_cast<float>(state) = pitch * intensity;
          }};
}

// Tests that processing a single note produces the expected output.
TEST(InstrumentProcessorTest, ProcessSingleNote) {
  const int kSampleRate = 48000;
  const double kTimestamp = 2.0;
  const float kPitch = 32.0f;
  const float kIntensity = 0.5f;

  InstrumentProcessor processor(kSampleRate, GetTestInstrumentDefinition());
  std::vector<float> buffer(kNumChannels * kNumFrames);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  processor.Process(buffer.data(), kNumChannels, kNumFrames, kTimestamp);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Start note.
  processor.ScheduleEvent(NoteOn{kPitch, kIntensity}, kTimestamp);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  processor.Process(buffer.data(), kNumChannels, kNumFrames, kTimestamp);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel],
                      kPitch * kIntensity);
    }
  }

  // Stop note.
  processor.ScheduleEvent(NoteOff{kPitch}, kTimestamp);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  processor.Process(buffer.data(), kNumChannels, kNumFrames, kTimestamp);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

// Tests that processing multiple notes produces the expected output.
TEST(InstrumentProcessorTest, ProcessMultipleNotes) {
  const float kIntensity = 1.0f;

  InstrumentProcessor processor(1, GetTestInstrumentDefinition());
  std::vector<float> buffer(kNumChannels * kNumFrames);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  processor.Process(buffer.data(), kNumChannels, kNumFrames, 0.0);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Start new note per each sample in the buffer.
  for (int i = 0; i < kNumFrames; ++i) {
    processor.ScheduleEvent(NoteOn{static_cast<float>(i), kIntensity},
                            static_cast<double>(i));
  }

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  processor.Process(buffer.data(), kNumChannels, kNumFrames, 0.0);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    const float expected = static_cast<float>(frame) * kIntensity;
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], expected);
    }
  }

  // Stop all notes.
  for (int i = 0; i < kNumFrames; ++i) {
    processor.ScheduleEvent(NoteOff{static_cast<float>(i)}, 0.0);
  }

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  processor.Process(buffer.data(), kNumChannels, kNumFrames, 0.0);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

// Tests that instrument gets reset as expected.
TEST(InstrumentProcessorTest, Reset) {
  auto definition = GetTestInstrumentDefinition();
  definition.create_fn = [](InstrumentState* state, int sample_rate) {
    state->emplace<float>(static_cast<float>(sample_rate));
  };
  InstrumentProcessor processor(1000, std::move(definition));
  std::vector<float> buffer(kNumChannels * kNumFrames);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  processor.Process(buffer.data(), kNumChannels, kNumFrames, 0.0);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 1000.0f);
    }
  }

  processor.Reset(2000);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  processor.Process(buffer.data(), kNumChannels, kNumFrames, 0.0);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 2000.0f);
    }
  }
}

// Tests that scheduling multiple events at once gets processed as expected.
TEST(InstrumentProcessorTest, ScheduleEvents) {
  const float kNoteOnPitch = 3.0f;
  const float kNoteOffPitch = -2.0f;
  const float kParamValue = 0.5f;
  const std::multimap<double, InstrumentEvent> kEvents = {
      {0.0, NoteOn{kNoteOnPitch, 1.0f}},
      {1.5, NoteOff{kNoteOffPitch}},
      {2.0, Param{1, kParamValue}}};

  float note_on_pitch = 0.0f;
  float note_off_pitch = 0.0f;
  float param_value = 0.0f;
  InstrumentDefinition definition = {
      .set_note_off_fn = [&](InstrumentState* /*state*/,
                             float pitch) { note_off_pitch = pitch; },
      .set_note_on_fn = [&](InstrumentState* /*state*/, float pitch,
                            float /*intensity*/) { note_on_pitch = pitch; },
      .set_param_fn = [&](InstrumentState* /*state*/, int /*id*/,
                          float value) { param_value = value; }};

  InstrumentProcessor processor(1, std::move(definition));
  std::vector<float> buffer(1);

  processor.ScheduleEvents(kEvents);
  EXPECT_NE(note_on_pitch, kNoteOnPitch);
  EXPECT_NE(note_off_pitch, kNoteOffPitch);
  EXPECT_NE(param_value, kParamValue);

  processor.Process(buffer.data(), 1, 1, 0.0);
  EXPECT_FLOAT_EQ(note_on_pitch, kNoteOnPitch);
  EXPECT_NE(note_off_pitch, kNoteOffPitch);
  EXPECT_NE(param_value, kParamValue);

  processor.Process(buffer.data(), 1, 1, 1.0);
  EXPECT_FLOAT_EQ(note_on_pitch, kNoteOnPitch);
  EXPECT_FLOAT_EQ(note_off_pitch, kNoteOffPitch);
  EXPECT_NE(param_value, kParamValue);

  processor.Process(buffer.data(), 1, 1, 2.0);
  EXPECT_FLOAT_EQ(note_on_pitch, kNoteOnPitch);
  EXPECT_FLOAT_EQ(note_off_pitch, kNoteOffPitch);
  EXPECT_FLOAT_EQ(param_value, kParamValue);
}

}  // namespace
}  // namespace barelyapi
