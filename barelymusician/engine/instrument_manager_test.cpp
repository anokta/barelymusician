#include "barelymusician/engine/instrument_manager.h"

#include <algorithm>
#include <vector>

#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/param.h"
#include "barelymusician/engine/param_definition.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

using ::testing::Property;

constexpr int kSampleRate = 48000;
constexpr int kNumChannels = 2;
constexpr int kNumFrames = 12;

constexpr Id kInstrumentId = 1;

// Returns test instrument definition that produces constant output that is set.
InstrumentDefinition GetTestInstrumentDefinition() {
  return InstrumentDefinition{
      .create_fn = [](void** state,
                      int /*sample_rate*/) { *state = new float{0.0f}; },
      .destroy_fn = [](void** state) { delete *state; },
      .process_fn =
          [](void** state, float* output, int num_channels, int num_frames) {
            std::fill_n(output, num_channels * num_frames,
                        *reinterpret_cast<float*>(*state));
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
                static_cast<float>(index + 1) * value;
          },
      .param_definitions = {ParamDefinition{0.0f}}};
}

TEST(InstrumentManagerTest, GetParam) {
  InstrumentManager instrument_manager;
  EXPECT_TRUE(IsOk(instrument_manager.Create(
      kInstrumentId, 0.0, GetTestInstrumentDefinition(), kSampleRate)));
  EXPECT_TRUE(IsOk(instrument_manager.GetParam(kInstrumentId, 0)));
}

// Tests that instruments are added and removed as expected.
TEST(InstrumentManagerTest, CreateDestroy) {
  const float kNotePitch = 1.25f;
  const float kNoteIntensity = 0.75f;

  InstrumentManager instrument_manager;
  std::vector<float> buffer(kNumChannels * kNumFrames);

  // Create instrument.
  EXPECT_FALSE(instrument_manager.IsValid(kInstrumentId));
  EXPECT_TRUE(IsOk(instrument_manager.Create(
      kInstrumentId, 0.0, GetTestInstrumentDefinition(), kSampleRate)));
  EXPECT_TRUE(instrument_manager.IsValid(kInstrumentId));

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument_manager.Process(kInstrumentId, 0.0, buffer.data(), kNumChannels,
                             kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Set note on.
  EXPECT_TRUE(IsOk(instrument_manager.SetNoteOn(kInstrumentId, 0.0, kNotePitch,
                                                kNoteIntensity)));
  EXPECT_TRUE(
      GetStatusOrValue(instrument_manager.IsNoteOn(kInstrumentId, kNotePitch)));

  instrument_manager.Update();

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument_manager.Process(kInstrumentId, 0.0, buffer.data(), kNumChannels,
                             kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel],
                      kNotePitch * kNoteIntensity);
    }
  }

  // Destroy instrument.
  EXPECT_TRUE(instrument_manager.IsValid(kInstrumentId));
  EXPECT_TRUE(IsOk(instrument_manager.Destroy(kInstrumentId, 0.0)));
  EXPECT_FALSE(instrument_manager.IsValid(kInstrumentId));

  instrument_manager.Update();

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument_manager.Process(kInstrumentId, 0.0, buffer.data(), kNumChannels,
                             kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

// Tests that instrument events are processed as expected.
TEST(InstrumentManagerTest, ProcessEvents) {
  const double kTimestamp = 16.0;

  InstrumentManager instrument_manager;

  EXPECT_TRUE(IsOk(instrument_manager.Create(
      kInstrumentId, kTimestamp, GetTestInstrumentDefinition(), kSampleRate)));

  // Set note on.
  instrument_manager.ProcessEvent(kInstrumentId, kTimestamp,
                                  StartNoteEvent{-0.5f, 0.25f});
  EXPECT_TRUE(
      GetStatusOrValue(instrument_manager.IsNoteOn(kInstrumentId, -0.5f)));

  // Set note off.
  instrument_manager.ProcessEvent(kInstrumentId, kTimestamp,
                                  StopNoteEvent{-0.5f});
  EXPECT_FALSE(
      GetStatusOrValue(instrument_manager.IsNoteOn(kInstrumentId, -0.5f)));

  // Set parameter value.
  instrument_manager.ProcessEvent(kInstrumentId, kTimestamp,
                                  SetParamEvent{0, 5.0f});
  EXPECT_FLOAT_EQ(
      GetStatusOrValue(instrument_manager.GetParam(kInstrumentId, 0))
          .GetValue(),
      5.0f);
}

// Tests that setting a single instrument note produces the expected output.
TEST(InstrumentManagerTest, SetNote) {
  const double kTimestamp = 20.0;
  const float kNotePitch = 32.0f;
  const float kNoteIntensity = 0.5f;

  InstrumentManager instrument_manager;
  std::vector<float> buffer(kNumChannels * kNumFrames);

  // Create instrument.
  EXPECT_TRUE(IsOk(instrument_manager.Create(
      kInstrumentId, kTimestamp, GetTestInstrumentDefinition(), kSampleRate)));

  instrument_manager.Update();

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument_manager.Process(kInstrumentId, kTimestamp, buffer.data(),
                             kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Set note on.
  EXPECT_FALSE(
      GetStatusOrValue(instrument_manager.IsNoteOn(kInstrumentId, kNotePitch)));
  EXPECT_TRUE(IsOk(instrument_manager.SetNoteOn(kInstrumentId, kTimestamp,
                                                kNotePitch, kNoteIntensity)));
  EXPECT_TRUE(
      GetStatusOrValue(instrument_manager.IsNoteOn(kInstrumentId, kNotePitch)));

  instrument_manager.Update();

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument_manager.Process(kInstrumentId, kTimestamp, buffer.data(),
                             kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel],
                      kNotePitch * kNoteIntensity);
    }
  }

  // Set note off.
  EXPECT_TRUE(
      GetStatusOrValue(instrument_manager.IsNoteOn(kInstrumentId, kNotePitch)));
  EXPECT_TRUE(IsOk(
      instrument_manager.SetNoteOff(kInstrumentId, kTimestamp, kNotePitch)));
  EXPECT_FALSE(
      GetStatusOrValue(instrument_manager.IsNoteOn(kInstrumentId, kNotePitch)));

  instrument_manager.Update();

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument_manager.Process(kInstrumentId, kTimestamp, buffer.data(),
                             kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

// Tests that setting multiple instrument notes produces the expected output.
TEST(InstrumentManagerTest, SetNotes) {
  const float kNoteIntensity = 1.0f;

  InstrumentManager instrument_manager;
  std::vector<float> buffer(kNumChannels * kNumFrames);

  // Create instrument.
  EXPECT_TRUE(IsOk(instrument_manager.Create(
      kInstrumentId, 0.0, GetTestInstrumentDefinition(), 1)));

  instrument_manager.Update();

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument_manager.Process(kInstrumentId, 0.0, buffer.data(), kNumChannels,
                             kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Set new note per each sample in the buffer.
  for (int i = 0; i < kNumFrames; ++i) {
    EXPECT_TRUE(IsOk(
        instrument_manager.SetNoteOn(kInstrumentId, static_cast<double>(i),
                                     static_cast<float>(i), kNoteIntensity)));
    EXPECT_TRUE(IsOk(instrument_manager.SetNoteOff(
        kInstrumentId, static_cast<double>(i + kNumFrames),
        static_cast<float>(i))));
  }

  // Verify that the instrument processor does not receive the note events
  // before the update call.
  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument_manager.Process(kInstrumentId, static_cast<double>(kNumFrames),
                             buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  instrument_manager.Update();

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument_manager.Process(kInstrumentId, 0.0, buffer.data(), kNumChannels,
                             kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    const float expected = static_cast<float>(frame) * kNoteIntensity;
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], expected);
    }
  }

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument_manager.Process(kInstrumentId, static_cast<double>(kNumFrames),
                             buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

// Tests that all instrument notes are set off as expected.
TEST(InstrumentManagerTest, SetAllNotesOff) {
  const int kNumInstruments = 3;
  const float kNoteIntensity = 0.1f;

  InstrumentManager instrument_manager;
  std::vector<float> buffer(kNumChannels * kNumFrames);

  // Create instruments.
  for (int i = 0; i < kNumInstruments; ++i) {
    const Id instrument_id = static_cast<Id>(i);

    EXPECT_FALSE(instrument_manager.IsValid(instrument_id));
    EXPECT_TRUE(IsOk(instrument_manager.Create(
        instrument_id, 0.0, GetTestInstrumentDefinition(), kSampleRate)));
    EXPECT_TRUE(instrument_manager.IsValid(instrument_id));

    instrument_manager.Update();

    std::fill(buffer.begin(), buffer.end(), 0.0f);
    instrument_manager.Process(instrument_id, 0.0, buffer.data(), kNumChannels,
                               kNumFrames);
    for (int frame = 0; frame < kNumFrames; ++frame) {
      for (int channel = 0; channel < kNumChannels; ++channel) {
        EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
      }
    }
  }

  // Set notes on.
  for (int i = 0; i < kNumInstruments; ++i) {
    const Id instrument_id = static_cast<Id>(i);
    const float note_pitch = static_cast<float>(i);

    EXPECT_TRUE(IsOk(instrument_manager.SetNoteOn(instrument_id, 0.0,
                                                  note_pitch, kNoteIntensity)));
    EXPECT_TRUE(GetStatusOrValue(
        instrument_manager.IsNoteOn(instrument_id, note_pitch)));

    instrument_manager.Update();

    std::fill(buffer.begin(), buffer.end(), 0.0f);
    const float expected = note_pitch * kNoteIntensity;
    instrument_manager.Process(instrument_id, 0.0, buffer.data(), kNumChannels,
                               kNumFrames);
    for (int frame = 0; frame < kNumFrames; ++frame) {
      for (int channel = 0; channel < kNumChannels; ++channel) {
        EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], expected);
      }
    }
  }

  // Set all notes off.
  instrument_manager.SetAllNotesOff(0.0);

  instrument_manager.Update();

  for (int i = 0; i < kNumInstruments; ++i) {
    const Id instrument_id = static_cast<Id>(i);
    const float note_pitch = static_cast<float>(i);

    EXPECT_FALSE(GetStatusOrValue(
        instrument_manager.IsNoteOn(instrument_id, note_pitch)));

    std::fill(buffer.begin(), buffer.end(), 0.0f);
    instrument_manager.Process(instrument_id, 0.0, buffer.data(), kNumChannels,
                               kNumFrames);
    for (int frame = 0; frame < kNumFrames; ++frame) {
      for (int channel = 0; channel < kNumChannels; ++channel) {
        EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
      }
    }
  }
}

// Tests that instrument parameters are reset as expected.
TEST(InstrumentManagerTest, SetAllParamsToDefault) {
  const int kNumInstruments = 2;

  InstrumentManager instrument_manager;
  std::vector<float> buffer(kNumChannels * kNumFrames);

  // Create instruments.
  for (int i = 0; i < kNumInstruments; ++i) {
    const Id instrument_id = static_cast<Id>(i);

    EXPECT_FALSE(instrument_manager.IsValid(instrument_id));
    EXPECT_TRUE(IsOk(instrument_manager.Create(
        instrument_id, 0.0, GetTestInstrumentDefinition(), kSampleRate)));
    EXPECT_TRUE(instrument_manager.IsValid(instrument_id));
    EXPECT_THAT(GetStatusOrValue(instrument_manager.GetParam(instrument_id, 0)),
                Property(&Param::GetValue, 0.0f));

    instrument_manager.Update();

    std::fill(buffer.begin(), buffer.end(), 0.0f);
    instrument_manager.Process(instrument_id, 0.0, buffer.data(), kNumChannels,
                               kNumFrames);
    for (int frame = 0; frame < kNumFrames; ++frame) {
      for (int channel = 0; channel < kNumChannels; ++channel) {
        EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
      }
    }
  }

  // Set parameter values.
  for (int i = 0; i < kNumInstruments; ++i) {
    const Id instrument_id = static_cast<Id>(i);
    const float param_value = static_cast<float>(i + 1);

    EXPECT_TRUE(
        IsOk(instrument_manager.SetParam(instrument_id, 0.0, 0, param_value)));
    EXPECT_THAT(GetStatusOrValue(instrument_manager.GetParam(instrument_id, 0)),
                Property(&Param::GetValue, param_value));

    instrument_manager.Update();

    std::fill(buffer.begin(), buffer.end(), 0.0f);
    instrument_manager.Process(instrument_id, 0.0, buffer.data(), kNumChannels,
                               kNumFrames);
    for (int frame = 0; frame < kNumFrames; ++frame) {
      for (int channel = 0; channel < kNumChannels; ++channel) {
        EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], param_value);
      }
    }
  }

  // Reset all parameters.
  instrument_manager.SetAllParamsToDefault(0.0);

  instrument_manager.Update();

  for (int i = 0; i < kNumInstruments; ++i) {
    const Id instrument_id = static_cast<Id>(i);

    EXPECT_THAT(GetStatusOrValue(instrument_manager.GetParam(instrument_id, 0)),
                Property(&Param::GetValue, 0.0f));

    std::fill(buffer.begin(), buffer.end(), 0.0f);
    instrument_manager.Process(instrument_id, 0.0, buffer.data(), kNumChannels,
                               kNumFrames);
    for (int frame = 0; frame < kNumFrames; ++frame) {
      for (int channel = 0; channel < kNumChannels; ++channel) {
        EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
      }
    }
  }
}

// Tests that setting notes triggers the corresponding callbacks as expected.
TEST(InstrumentManagerTest, SetNoteCallbacks) {
  const float kNotePitch = 4.0f;
  const float kNoteIntensity = 0.25f;

  InstrumentManager instrument_manager;

  // Create instrument.
  EXPECT_TRUE(IsOk(instrument_manager.Create(
      kInstrumentId, 0.0, GetTestInstrumentDefinition(), 1)));

  // Trigger note on callback.
  float note_on_pitch = 0.0f;
  float note_on_intensity = 0.0f;
  double note_on_timestamp = 0.0;
  instrument_manager.SetNoteOnCallback(
      kInstrumentId,
      [&](float note_pitch, float note_intensity, double timestamp) {
        note_on_pitch = note_pitch;
        note_on_intensity = note_intensity;
        note_on_timestamp = timestamp;
      });
  EXPECT_FLOAT_EQ(note_on_pitch, 0.0f);
  EXPECT_FLOAT_EQ(note_on_intensity, 0.0f);
  EXPECT_DOUBLE_EQ(note_on_timestamp, 0.0);

  EXPECT_TRUE(IsOk(instrument_manager.SetNoteOn(kInstrumentId, 10.0, kNotePitch,
                                                kNoteIntensity)));

  EXPECT_FLOAT_EQ(note_on_pitch, kNotePitch);
  EXPECT_FLOAT_EQ(note_on_intensity, kNoteIntensity);
  EXPECT_DOUBLE_EQ(note_on_timestamp, 10.0);

  // This should not trigger the callback since the note is already on.
  EXPECT_TRUE(IsOk(instrument_manager.SetNoteOn(kInstrumentId, 15.0, kNotePitch,
                                                kNoteIntensity)));

  EXPECT_FLOAT_EQ(note_on_pitch, kNotePitch);
  EXPECT_FLOAT_EQ(note_on_intensity, kNoteIntensity);
  EXPECT_DOUBLE_EQ(note_on_timestamp, 10.0);

  // Trigger note on callback again with another note.
  EXPECT_TRUE(IsOk(instrument_manager.SetNoteOn(
      kInstrumentId, 15.0, kNotePitch + 2.0f, kNoteIntensity)));

  EXPECT_FLOAT_EQ(note_on_pitch, kNotePitch + 2.0f);
  EXPECT_FLOAT_EQ(note_on_intensity, kNoteIntensity);
  EXPECT_DOUBLE_EQ(note_on_timestamp, 15.0);

  // Trigger note off callback.
  float note_off_pitch = 0.0f;
  double note_off_timestamp = 0.0;
  instrument_manager.SetNoteOffCallback(
      kInstrumentId, [&](float note_pitch, double timestamp) {
        note_off_pitch = note_pitch;
        note_off_timestamp = timestamp;
      });
  EXPECT_FLOAT_EQ(note_off_pitch, 0.0f);
  EXPECT_DOUBLE_EQ(note_off_timestamp, 0.0);

  EXPECT_TRUE(
      IsOk(instrument_manager.SetNoteOff(kInstrumentId, 20.0, kNotePitch)));

  EXPECT_FLOAT_EQ(note_off_pitch, kNotePitch);
  EXPECT_DOUBLE_EQ(note_off_timestamp, 20.0);

  // This should not trigger the callback since the note is already off.
  EXPECT_TRUE(
      IsOk(instrument_manager.SetNoteOff(kInstrumentId, 25.0, kNotePitch)));

  EXPECT_FLOAT_EQ(note_off_pitch, kNotePitch);
  EXPECT_DOUBLE_EQ(note_off_timestamp, 20.0);

  // Finally, remove to trigger the note off callback with the remaining note.
  EXPECT_TRUE(IsOk(instrument_manager.Destroy(kInstrumentId, 30.0)));

  EXPECT_FLOAT_EQ(note_off_pitch, kNotePitch + 2.0f);
  EXPECT_DOUBLE_EQ(note_off_timestamp, 30.0);
}

}  // namespace
}  // namespace barelyapi
