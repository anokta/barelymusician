#include "barelymusician/engine/instrument_processor.h"

#include <algorithm>
#include <any>
#include <utility>
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
  processor.Process(kTimestamp, buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Start note.
  processor.Schedule({{kTimestamp, SetNoteOn{kPitch, kIntensity}}});

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  processor.Process(kTimestamp, buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel],
                      kPitch * kIntensity);
    }
  }

  // Stop note.
  processor.Schedule({{kTimestamp, SetNoteOff{kPitch}}});

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  processor.Process(kTimestamp, buffer.data(), kNumChannels, kNumFrames);
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
  processor.Process(0.0, buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Start new note per each sample in the buffer.
  InstrumentProcessorEvents note_on_events;
  for (int i = 0; i < kNumFrames; ++i) {
    note_on_events.emplace(static_cast<double>(i),
                           SetNoteOn{static_cast<float>(i), kIntensity});
  }
  processor.Schedule(std::move(note_on_events));

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  processor.Process(0.0, buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    const float expected = static_cast<float>(frame) * kIntensity;
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], expected);
    }
  }

  // Stop all notes.
  InstrumentProcessorEvents note_off_events;
  for (int i = 0; i < kNumFrames; ++i) {
    note_off_events.emplace(0.0, SetNoteOff{static_cast<float>(i)});
  }
  processor.Schedule(std::move(note_off_events));

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  processor.Process(0.0, buffer.data(), kNumChannels, kNumFrames);
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
  processor.Process(0.0, buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 1000.0f);
    }
  }

  processor.Reset(2000);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  processor.Process(0.0, buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 2000.0f);
    }
  }
}

// Tests that scheduled instrument processor events gets processed as expected.
TEST(InstrumentProcessorTest, Schedule) {
  const float kNoteOnPitch = 3.0f;
  const float kNoteOffPitch = -2.0f;
  const float kParamValue = 0.5f;
  const InstrumentProcessorEvents kEvents = {
      {0.0, SetNoteOn{kNoteOnPitch, 1.0f}},
      {1.5, SetNoteOff{kNoteOffPitch}},
      {2.0, SetParam{1, kParamValue}}};

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

  processor.Schedule(kEvents);
  EXPECT_NE(note_on_pitch, kNoteOnPitch);
  EXPECT_NE(note_off_pitch, kNoteOffPitch);
  EXPECT_NE(param_value, kParamValue);

  processor.Process(0.0, buffer.data(), 1, 1);
  EXPECT_FLOAT_EQ(note_on_pitch, kNoteOnPitch);
  EXPECT_NE(note_off_pitch, kNoteOffPitch);
  EXPECT_NE(param_value, kParamValue);

  processor.Process(1.0, buffer.data(), 1, 1);
  EXPECT_FLOAT_EQ(note_on_pitch, kNoteOnPitch);
  EXPECT_FLOAT_EQ(note_off_pitch, kNoteOffPitch);
  EXPECT_NE(param_value, kParamValue);

  processor.Process(2.0, buffer.data(), 1, 1);
  EXPECT_FLOAT_EQ(note_on_pitch, kNoteOnPitch);
  EXPECT_FLOAT_EQ(note_off_pitch, kNoteOffPitch);
  EXPECT_FLOAT_EQ(param_value, kParamValue);
}

}  // namespace
}  // namespace barelyapi
