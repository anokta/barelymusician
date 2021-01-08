#include "barelymusician/instrument/instrument_processor.h"

#include <algorithm>
#include <cstdint>
#include <vector>

#include "barelymusician/instrument/instrument_data.h"
#include "barelymusician/instrument/instrument_definition.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

constexpr int kNumChannels = 1;
constexpr int kNumFrames = 16;

// Returns test instrument definition that produces constant output per note.
InstrumentDefinition GetTestInstrumentDefinition() {
  return InstrumentDefinition{
      .create_fn = [](InstrumentState* state) { *state = new float(0.0f); },
      .destroy_fn =
          [](InstrumentState* state) {
            float* sample = reinterpret_cast<float*>(*state);
            delete sample;
          },
      .process_fn =
          [](InstrumentState* state, float* output, int num_channels,
             int num_frames) {
            std::fill_n(output, num_channels * num_frames,
                        *reinterpret_cast<float*>(*state));
          },
      .set_note_off_fn =
          [](InstrumentState* state, float /*pitch*/) {
            *reinterpret_cast<float*>(*state) = 0.0f;
          },
      .set_note_on_fn =
          [](InstrumentState* state, float pitch, float intensity) {
            *reinterpret_cast<float*>(*state) = pitch * intensity;
          }};
}

// Tests that processing a single note produces the expected output.
TEST(InstrumentProcessorTest, ProcessSingleNote) {
  const std::int64_t kTimestamp = 20;
  const float kPitch = 32.0f;
  const float kIntensity = 0.5f;

  InstrumentProcessor processor(GetTestInstrumentDefinition());
  std::vector<float> buffer(kNumChannels * kNumFrames);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  processor.Process(kTimestamp, buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Start note.
  processor.SetData(kTimestamp, NoteOn{kPitch, kIntensity});

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  processor.Process(kTimestamp, buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel],
                      kPitch * kIntensity);
    }
  }

  // Stop note.
  processor.SetData(kTimestamp, NoteOff{kPitch});

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

  InstrumentProcessor processor(GetTestInstrumentDefinition());
  std::vector<float> buffer(kNumChannels * kNumFrames);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  processor.Process(0, buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Start new note per each sample in the buffer.
  for (int i = 0; i < kNumFrames; ++i) {
    processor.SetData(i, NoteOn{static_cast<float>(i), kIntensity});
  }

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  processor.Process(0, buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    const float expected = static_cast<float>(frame) * kIntensity;
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], expected);
    }
  }

  // Stop all notes.
  for (int i = 0; i < kNumFrames; ++i) {
    processor.SetData(0, NoteOff{static_cast<float>(i)});
  }

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  processor.Process(0, buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

}  // namespace
}  // namespace barelyapi
