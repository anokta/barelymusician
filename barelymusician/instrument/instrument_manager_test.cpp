#include "barelymusician/instrument/instrument_manager.h"

#include <algorithm>
#include <vector>

#include "barelymusician/base/status.h"
#include "barelymusician/base/types.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

constexpr int kNumChannels = 2;
constexpr int kNumFrames = 12;

// Returns test instrument definition that produces constant output that is set.
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
          },
      .set_param_fn =
          [](InstrumentState* state, int id, float value) {
            *reinterpret_cast<float*>(*state) = static_cast<float>(id) * value;
          }};
}

// Returns test instrument parameter definition.
InstrumentParamDefinitions GetTestInstrumentParamDefinitions() {
  return InstrumentParamDefinitions{InstrumentParamDefinition{1, 0.0f}};
}

// Tests that the manager creates and destroy instruments as expected.
TEST(InstrumentManagerTest, CreateDestroy) {
  const int64 kTimestamp = 10;
  const float kNotePitch = 1.25f;
  const float kNoteIntensity = 0.75f;

  InstrumentManager manager;

  // Create instrument and set note on.
  const int64 instrument_id =
      manager.Create(GetTestInstrumentDefinition(),
                     GetTestInstrumentParamDefinitions(), kTimestamp);
  EXPECT_TRUE(IsOk(manager.SetNoteOn(instrument_id, kTimestamp, kNotePitch,
                                     kNoteIntensity)));

  std::vector<float> buffer(kNumChannels * kNumFrames);
  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(IsOk(manager.Process(instrument_id, kTimestamp, buffer.data(),
                                   kNumChannels, kNumFrames)));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel],
                      kNotePitch * kNoteIntensity);
    }
  }

  // Destroy instrument.
  EXPECT_TRUE(IsOk(manager.Destroy(instrument_id, kTimestamp)));

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_FALSE(IsOk(manager.Process(instrument_id, kTimestamp + kNumFrames,
                                    buffer.data(), kNumChannels, kNumFrames)));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

// Tests that setting a single note produces the expected output.
TEST(InstrumentManagerTest, SetSingleNote) {
  const int64 kTimestamp = 20;
  const float kNotePitch = 32.0f;
  const float kNoteIntensity = 0.5f;

  InstrumentManager manager;

  // Create instrument.
  const int64 instrument_id =
      manager.Create(GetTestInstrumentDefinition(),
                     GetTestInstrumentParamDefinitions(), kTimestamp);

  std::vector<float> buffer(kNumChannels * kNumFrames);
  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(IsOk(manager.Process(instrument_id, kTimestamp, buffer.data(),
                                   kNumChannels, kNumFrames)));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Set note on.
  manager.SetNoteOn(instrument_id, kTimestamp, kNotePitch, kNoteIntensity);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(IsOk(manager.Process(instrument_id, kTimestamp, buffer.data(),
                                   kNumChannels, kNumFrames)));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel],
                      kNotePitch * kNoteIntensity);
    }
  }

  // Set note off.
  manager.SetNoteOff(instrument_id, kTimestamp + kNumFrames, kNotePitch);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(IsOk(manager.Process(instrument_id, kTimestamp + kNumFrames,
                                   buffer.data(), kNumChannels, kNumFrames)));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

// Tests that setting multiple notes produces the expected output.
TEST(InstrumentManagerTest, SetMultipleNotes) {
  const float kNoteIntensity = 1.0f;

  InstrumentManager manager;

  // Create instrument.
  const int64 instrument_id = manager.Create(
      GetTestInstrumentDefinition(), GetTestInstrumentParamDefinitions(), 0);

  std::vector<float> buffer(kNumChannels * kNumFrames);
  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(IsOk(manager.Process(instrument_id, 0, buffer.data(),
                                   kNumChannels, kNumFrames)));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Start new note per each sample in the buffer.
  for (int i = 0; i < kNumFrames; ++i) {
    manager.SetNoteOn(instrument_id, i, static_cast<float>(i), kNoteIntensity);
  }

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(IsOk(manager.Process(instrument_id, 0, buffer.data(),
                                   kNumChannels, kNumFrames)));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    const float expected = static_cast<float>(frame) * kNoteIntensity;
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], expected);
    }
  }

  // Stop all notes.
  manager.SetAllNotesOff(instrument_id, kNumFrames);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(IsOk(manager.Process(instrument_id, kNumFrames, buffer.data(),
                                   kNumChannels, kNumFrames)));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

// Tests that playing notes triggers the corresponding callbacks as expected.
TEST(InstrumentManagerTest, SetNoteCallbacks) {
  const int64 kTimestamp = 50;
  const float kNotePitch = 4.0f;
  const float kNoteIntensity = 0.25f;

  InstrumentManager manager;

  // Create instrument.
  const int64 instrument_id = manager.Create(
      GetTestInstrumentDefinition(), GetTestInstrumentParamDefinitions(), 0);

  // Trigger note on callback.
  int64 note_on_instrument_id = -1;
  int64 note_on_timestamp = -1;
  float note_on_pitch = 0.0f;
  float note_on_intensity = 0.0f;
  manager.SetNoteOnCallback([&](int64 instrument_id, int64 timestamp,
                                float note_pitch, float note_intensity) {
    note_on_instrument_id = instrument_id;
    note_on_timestamp = timestamp;
    note_on_pitch = note_pitch;
    note_on_intensity = note_intensity;
  });
  EXPECT_NE(note_on_instrument_id, instrument_id);
  EXPECT_NE(note_on_timestamp, kTimestamp);
  EXPECT_NE(note_on_pitch, kNotePitch);
  EXPECT_NE(note_on_intensity, kNoteIntensity);

  EXPECT_TRUE(IsOk(manager.SetNoteOn(instrument_id, kTimestamp, kNotePitch,
                                     kNoteIntensity)));
  EXPECT_EQ(note_on_instrument_id, instrument_id);
  EXPECT_EQ(note_on_timestamp, kTimestamp);
  EXPECT_FLOAT_EQ(note_on_pitch, kNotePitch);
  EXPECT_FLOAT_EQ(note_on_intensity, kNoteIntensity);

  // This should not trigger the callback since the note is already on.
  EXPECT_FALSE(
      IsOk(manager.SetNoteOn(instrument_id, 2, kNotePitch, kNoteIntensity)));
  EXPECT_NE(note_on_timestamp, 2);

  // Trigger note on callback again with another note.
  EXPECT_TRUE(IsOk(manager.SetNoteOn(instrument_id, 2, 2.0f, kNoteIntensity)));
  EXPECT_EQ(note_on_timestamp, 2);
  EXPECT_FLOAT_EQ(note_on_pitch, 2.0f);

  // Trigger note off callback.
  int64 note_off_instrument_id = -1;
  int64 note_off_timestamp = -1;
  float note_off_pitch = 0.0f;
  manager.SetNoteOffCallback(
      [&](int64 instrument_id, int64 timestamp, float note_pitch) {
        note_off_instrument_id = instrument_id;
        note_off_timestamp = timestamp;
        note_off_pitch = note_pitch;
      });
  EXPECT_NE(note_off_instrument_id, instrument_id);
  EXPECT_NE(note_off_timestamp, kTimestamp);
  EXPECT_NE(note_off_pitch, kNotePitch);

  EXPECT_TRUE(IsOk(manager.SetNoteOff(instrument_id, kTimestamp, kNotePitch)));
  EXPECT_EQ(note_off_instrument_id, instrument_id);
  EXPECT_EQ(note_off_timestamp, kTimestamp);
  EXPECT_FLOAT_EQ(note_off_pitch, kNotePitch);

  // This should not trigger the callback since the note is already off.
  EXPECT_FALSE(IsOk(manager.SetNoteOff(instrument_id, 2, kNotePitch)));
  EXPECT_NE(note_off_timestamp, 2);

  // Destroying will trigger the note off callback with the remaining note.
  EXPECT_TRUE(IsOk(manager.Destroy(instrument_id, 2)));
  EXPECT_EQ(note_off_timestamp, 2);
  EXPECT_FLOAT_EQ(note_off_pitch, 2.0f);
}

}  // namespace
}  // namespace barelyapi
