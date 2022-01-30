#include "barelymusician/engine/instrument.h"

#include <algorithm>
#include <memory>
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
      .create_fn =
          [](void** state, int /*sample_rate*/) {
            *state = reinterpret_cast<void*>(new float{0.0f});
          },
      .destroy_fn =
          [](void** state) { delete reinterpret_cast<float*>(*state); },
      .process_fn =
          [](void** state, float* output, int num_channels, int num_frames) {
            std::fill_n(output, num_channels * num_frames,
                        *reinterpret_cast<float*>(*state));
          },
      .set_data_fn =
          [](void** state, void* data) {
            *reinterpret_cast<float*>(*state) = *reinterpret_cast<float*>(data);
          },
      .set_note_off_fn =
          [](void** state, float /*pitch*/) {
            *reinterpret_cast<float*>(*state) = 0.0f;
          },
      .set_note_on_fn =
          [](void** state, float pitch, float intensity) {
            *reinterpret_cast<float*>(*state) = pitch * intensity;
          },
      .set_param_fn =
          [](void** state, int index, float value) {
            *reinterpret_cast<float*>(*state) =
                static_cast<float>(index) * value;
          }};
}

// Tests that the instrument processes its buffer as expected.
TEST(InstrumentTest, Process) {
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

  // Set data.
  auto data = std::make_unique<float>(-5.0f);
  instrument.SetData(reinterpret_cast<void*>(data.get()));

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
TEST(InstrumentTest, ProcessEmptyDefinition) {
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

  // Set data.
  auto data = std::make_unique<float>(-5.0f);
  instrument.SetData(reinterpret_cast<void*>(data.get()));

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
