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
  processor.Process(kTimestamp, buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Start note.
  processor.ScheduleEvent(NoteOn{kPitch, kIntensity}, kTimestamp);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  processor.Process(kTimestamp, buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel],
                      kPitch * kIntensity);
    }
  }

  // Stop note.
  processor.ScheduleEvent(NoteOff{kPitch}, kTimestamp);

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
  for (int i = 0; i < kNumFrames; ++i) {
    processor.ScheduleEvent(NoteOn{static_cast<float>(i), kIntensity},
                            static_cast<double>(i));
  }

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  processor.Process(0.0, buffer.data(), kNumChannels, kNumFrames);
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

}  // namespace
}  // namespace barelyapi
