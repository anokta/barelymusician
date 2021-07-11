#include "barelymusician/engine/instrument.h"

#include <algorithm>
#include <any>
#include <vector>

#include "barelymusician/engine/instrument_definition.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

constexpr int kSampleRate = 8000;
constexpr int kNumChannels = 1;
constexpr int kNumFrames = 4;

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
      .set_custom_data_fn =
          [](InstrumentState* state, std::any data) {
            *std::any_cast<float>(state) = *std::any_cast<float>(&data);
          },
      .set_note_off_fn =
          [](InstrumentState* state, float /*pitch*/) {
            *std::any_cast<float>(state) = 0.0f;
          },
      .set_note_on_fn =
          [](InstrumentState* state, float pitch, float intensity) {
            *std::any_cast<float>(state) = pitch * intensity;
          },
      .set_param_fn =
          [](InstrumentState* state, int id, float value) {
            *std::any_cast<float>(state) = static_cast<float>(id) * value;
          }};
}

// Tests that the instrument processes its buffer as expected.
TEST(InstrumentProcessorTest, Process) {
  Instrument instrument(kSampleRate, GetTestInstrumentDefinition());
  std::vector<float> buffer(kNumChannels * kNumFrames);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument.Process(buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Set note on.
  instrument.SetNoteOn(2.0f, 0.25f);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument.Process(buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 2.0f * 0.25f);
    }
  }

  // Set note off.
  instrument.SetNoteOff(2.0f);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument.Process(buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Set parameter.
  instrument.SetParam(1, 0.4f);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument.Process(buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.4f);
    }
  }

  // Set custom data.
  instrument.SetCustomData(-5.0f);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument.Process(buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], -5.0f);
    }
  }
}

// Tests that the instrument processes its buffer as expected when its
// definition is empty.
TEST(InstrumentProcessorTest, ProcessEmptyDefinition) {
  Instrument instrument(kSampleRate, InstrumentDefinition{});
  std::vector<float> buffer(kNumChannels * kNumFrames);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument.Process(buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Set note on.
  instrument.SetNoteOn(2.0f, 0.25f);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument.Process(buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Set note off.
  instrument.SetNoteOff(2.0f);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument.Process(buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Set parameter.
  instrument.SetParam(1, 0.4f);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument.Process(buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Set custom data.
  instrument.SetCustomData(-5.0f);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument.Process(buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

}  // namespace
}  // namespace barelyapi
