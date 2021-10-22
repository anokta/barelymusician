#include "barelymusician/engine/instrument_manager.h"

#include <algorithm>
#include <any>
#include <vector>

#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_param.h"
#include "barelymusician/engine/instrument_param_definition.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

using ::testing::AllOf;
using ::testing::Property;
using ::testing::UnorderedElementsAre;

constexpr int kSampleRate = 48000;
constexpr int kNumChannels = 2;
constexpr int kNumFrames = 12;

constexpr Id kInstrumentId = 1;

// Returns test instrument definition that produces constant output that is set.
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
          },
      .set_param_fn =
          [](InstrumentState* state, int id, float value) {
            *std::any_cast<float>(state) = static_cast<float>(id) * value;
          }};
}

// Returns test instrument parameter definition.
InstrumentParamDefinitions GetTestInstrumentParamDefinitions() {
  return InstrumentParamDefinitions{InstrumentParamDefinition{1, 0.0f}};
}

// Tests that instruments are created and destroyed as expected.
TEST(InstrumentManagerTest, CreateDestroy) {
  const float kNotePitch = 1.25f;
  const float kNoteIntensity = 0.75f;

  InstrumentManager instrument_manager(kSampleRate);
  std::vector<float> buffer(kNumChannels * kNumFrames);

  // Create instrument.
  EXPECT_TRUE(IsOk(instrument_manager.Create(
      kInstrumentId, 0.0, GetTestInstrumentDefinition(),
      GetTestInstrumentParamDefinitions())));

  EXPECT_THAT(GetStatusOrValue(instrument_manager.GetAllNotes(kInstrumentId)),
              UnorderedElementsAre());
  EXPECT_THAT(
      GetStatusOrValue(instrument_manager.GetAllParams(kInstrumentId)),
      UnorderedElementsAre(AllOf(Property(&InstrumentParam::GetId, 1),
                                 Property(&InstrumentParam::GetValue, 0.0f))));

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

  EXPECT_THAT(GetStatusOrValue(instrument_manager.GetAllNotes(kInstrumentId)),
              UnorderedElementsAre(kNotePitch));

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
  EXPECT_TRUE(IsOk(instrument_manager.Destroy(kInstrumentId, 0.0)));

  EXPECT_EQ(GetStatusOrStatus(instrument_manager.GetAllNotes(kInstrumentId)),
            Status::kNotFound);
  EXPECT_EQ(GetStatusOrStatus(instrument_manager.GetAllParams(kInstrumentId)),
            Status::kNotFound);

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

  InstrumentManager instrument_manager(kSampleRate);

  EXPECT_TRUE(IsOk(instrument_manager.Create(
      kInstrumentId, kTimestamp, GetTestInstrumentDefinition(),
      GetTestInstrumentParamDefinitions())));

  // Set note on.
  instrument_manager.ProcessEvent(kInstrumentId, kTimestamp,
                                  SetNoteOnEvent{-0.5f, 0.25f});
  EXPECT_TRUE(
      GetStatusOrValue(instrument_manager.IsNoteOn(kInstrumentId, -0.5f)));

  // Set note off.
  instrument_manager.ProcessEvent(kInstrumentId, kTimestamp,
                                  SetNoteOffEvent{-0.5f});
  EXPECT_FALSE(
      GetStatusOrValue(instrument_manager.IsNoteOn(kInstrumentId, -0.5f)));

  // Set multiple notes on at once.
  instrument_manager.ProcessEvents(InstrumentControllerEvents{
      {kTimestamp, {kInstrumentId, SetNoteOnEvent{1.0f, 1.0f}}},
      {kTimestamp, {kInstrumentId, SetNoteOnEvent{2.0f, 1.0f}}},
      {kTimestamp, {kInstrumentId, SetNoteOnEvent{3.0f, 1.0f}}}});
  EXPECT_THAT(GetStatusOrValue(instrument_manager.GetAllNotes(kInstrumentId)),
              UnorderedElementsAre(1.0f, 2.0f, 3.0f));

  // Set all notes off.
  instrument_manager.ProcessEvent(kInstrumentId, kTimestamp,
                                  SetAllNotesOffEvent{});
  EXPECT_THAT(GetStatusOrValue(instrument_manager.GetAllNotes(kInstrumentId)),
              UnorderedElementsAre());

  // Set parameter value.
  instrument_manager.ProcessEvent(kInstrumentId, kTimestamp,
                                  SetParamEvent{1, 5.0f});
  EXPECT_FLOAT_EQ(
      GetStatusOrValue(instrument_manager.GetParam(kInstrumentId, 1))
          .GetValue(),
      5.0f);

  // Set all parameters to default value.
  instrument_manager.ProcessEvent(kInstrumentId, kTimestamp,
                                  SetAllParamsToDefaultEvent{});
  EXPECT_FLOAT_EQ(
      GetStatusOrValue(instrument_manager.GetParam(kInstrumentId, 1))
          .GetValue(),
      0.0f);
}

// Tests that setting a single instrument note produces the expected output.
TEST(InstrumentManagerTest, SetNote) {
  const double kTimestamp = 20.0;
  const float kNotePitch = 32.0f;
  const float kNoteIntensity = 0.5f;

  InstrumentManager instrument_manager(kSampleRate);
  std::vector<float> buffer(kNumChannels * kNumFrames);

  // Create instrument.
  EXPECT_TRUE(IsOk(instrument_manager.Create(
      kInstrumentId, kTimestamp, GetTestInstrumentDefinition(),
      GetTestInstrumentParamDefinitions())));

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

  InstrumentManager instrument_manager(1);
  std::vector<float> buffer(kNumChannels * kNumFrames);

  // Create instrument.
  EXPECT_TRUE(IsOk(instrument_manager.Create(
      kInstrumentId, 0.0, GetTestInstrumentDefinition(),
      GetTestInstrumentParamDefinitions())));

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

  InstrumentManager instrument_manager(kSampleRate);
  std::vector<float> buffer(kNumChannels * kNumFrames);

  // Create instruments.
  for (int i = 0; i < kNumInstruments; ++i) {
    const Id instrument_id = static_cast<Id>(i);

    EXPECT_TRUE(IsOk(instrument_manager.Create(
        instrument_id, 0.0, GetTestInstrumentDefinition(),
        GetTestInstrumentParamDefinitions())));

    EXPECT_THAT(GetStatusOrValue(instrument_manager.GetAllNotes(instrument_id)),
                UnorderedElementsAre());

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

    EXPECT_THAT(GetStatusOrValue(instrument_manager.GetAllNotes(instrument_id)),
                UnorderedElementsAre(note_pitch));

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

    EXPECT_THAT(GetStatusOrValue(instrument_manager.GetAllNotes(instrument_id)),
                UnorderedElementsAre());

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

  InstrumentManager instrument_manager(kSampleRate);
  std::vector<float> buffer(kNumChannels * kNumFrames);

  // Create instruments.
  for (int i = 0; i < kNumInstruments; ++i) {
    const Id instrument_id = static_cast<Id>(i);

    EXPECT_TRUE(IsOk(instrument_manager.Create(
        instrument_id, 0.0, GetTestInstrumentDefinition(),
        GetTestInstrumentParamDefinitions())));

    EXPECT_THAT(
        GetStatusOrValue(instrument_manager.GetAllParams(instrument_id)),
        UnorderedElementsAre(
            AllOf(Property(&InstrumentParam::GetId, 1),
                  Property(&InstrumentParam::GetValue, 0.0f))));

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
        IsOk(instrument_manager.SetParam(instrument_id, 0.0, 1, param_value)));

    EXPECT_THAT(
        GetStatusOrValue(instrument_manager.GetAllParams(instrument_id)),
        UnorderedElementsAre(
            AllOf(Property(&InstrumentParam::GetId, 1),
                  Property(&InstrumentParam::GetValue, param_value))));

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

    EXPECT_THAT(
        GetStatusOrValue(instrument_manager.GetAllParams(instrument_id)),
        UnorderedElementsAre(
            AllOf(Property(&InstrumentParam::GetId, 1),
                  Property(&InstrumentParam::GetValue, 0.0f))));

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

  InstrumentManager instrument_manager(1);

  // Create instrument.
  EXPECT_TRUE(IsOk(instrument_manager.Create(
      kInstrumentId, 0.0, GetTestInstrumentDefinition(),
      GetTestInstrumentParamDefinitions())));

  // Trigger note on callback.
  Id note_on_instrument_id = 0;
  double note_on_timestamp = 0.0;
  float note_on_pitch = 0.0f;
  float note_on_intensity = 0.0f;
  instrument_manager.SetNoteOnCallback([&](Id instrument_id, double timestamp,
                                           float note_pitch,
                                           float note_intensity) {
    note_on_instrument_id = instrument_id;
    note_on_timestamp = timestamp;
    note_on_pitch = note_pitch;
    note_on_intensity = note_intensity;
  });
  EXPECT_NE(note_on_instrument_id, kInstrumentId);
  EXPECT_NE(note_on_timestamp, 1.0);
  EXPECT_NE(note_on_pitch, kNotePitch);
  EXPECT_NE(note_on_intensity, kNoteIntensity);

  EXPECT_TRUE(IsOk(instrument_manager.SetNoteOn(kInstrumentId, 10.0, kNotePitch,
                                                kNoteIntensity)));

  EXPECT_EQ(note_on_instrument_id, kInstrumentId);
  EXPECT_DOUBLE_EQ(note_on_timestamp, 10.0);
  EXPECT_FLOAT_EQ(note_on_pitch, kNotePitch);
  EXPECT_FLOAT_EQ(note_on_intensity, kNoteIntensity);

  // This should not trigger the callback since the note is already on.
  EXPECT_EQ(instrument_manager.SetNoteOn(kInstrumentId, 15.0, kNotePitch,
                                         kNoteIntensity),
            Status::kFailedPrecondition);

  EXPECT_DOUBLE_EQ(note_on_timestamp, 10.0);
  EXPECT_FLOAT_EQ(note_on_pitch, kNotePitch);

  // Trigger note on callback again with another note.
  EXPECT_TRUE(IsOk(instrument_manager.SetNoteOn(
      kInstrumentId, 15.0, kNotePitch + 2.0f, kNoteIntensity)));

  EXPECT_DOUBLE_EQ(note_on_timestamp, 15.0);
  EXPECT_FLOAT_EQ(note_on_pitch, kNotePitch + 2.0f);

  // Trigger note off callback.
  Id note_off_instrument_id = 0;
  double note_off_timestamp = 0.0;
  float note_off_pitch = 0.0f;
  instrument_manager.SetNoteOffCallback(
      [&](Id instrument_id, double timestamp, float note_pitch) {
        note_off_instrument_id = instrument_id;
        note_off_timestamp = timestamp;
        note_off_pitch = note_pitch;
      });
  EXPECT_NE(note_off_instrument_id, kInstrumentId);
  EXPECT_NE(note_off_timestamp, 20.0);
  EXPECT_NE(note_off_pitch, kNotePitch);

  EXPECT_TRUE(
      IsOk(instrument_manager.SetNoteOff(kInstrumentId, 20.0, kNotePitch)));

  EXPECT_EQ(note_off_instrument_id, kInstrumentId);
  EXPECT_DOUBLE_EQ(note_off_timestamp, 20.0);
  EXPECT_FLOAT_EQ(note_off_pitch, kNotePitch);

  // This should not trigger the callback since the note is already off.
  EXPECT_EQ(instrument_manager.SetNoteOff(kInstrumentId, 25.0, kNotePitch),
            Status::kFailedPrecondition);

  EXPECT_DOUBLE_EQ(note_off_timestamp, 20.0);
  EXPECT_FLOAT_EQ(note_off_pitch, kNotePitch);

  // Finally, destroy to trigger the note off callback with the remaining note.
  EXPECT_TRUE(IsOk(instrument_manager.Destroy(kInstrumentId, 30.0)));

  EXPECT_DOUBLE_EQ(note_off_timestamp, 30.0);
  EXPECT_FLOAT_EQ(note_off_pitch, kNotePitch + 2.0f);
}

// Tests that the instrument manager updates its sampling rate as expected.
TEST(InstrumentManagerTest, SetSampleRate) {
  const float kNotePitch = 3.0f;
  const float kNoteIntensity = 1.0f;
  const float kParamValue = -0.25f;

  InstrumentManager instrument_manager(1);
  std::vector<float> buffer(kNumChannels * kNumFrames);

  // Create instrument.
  EXPECT_TRUE(IsOk(instrument_manager.Create(
      kInstrumentId, 0.0, GetTestInstrumentDefinition(),
      GetTestInstrumentParamDefinitions())));

  EXPECT_THAT(GetStatusOrValue(instrument_manager.GetAllNotes(kInstrumentId)),
              UnorderedElementsAre());
  EXPECT_THAT(
      GetStatusOrValue(instrument_manager.GetAllParams(kInstrumentId)),
      UnorderedElementsAre(AllOf(Property(&InstrumentParam::GetId, 1),
                                 Property(&InstrumentParam::GetValue, 0.0f))));

  instrument_manager.Update();

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

  EXPECT_THAT(GetStatusOrValue(instrument_manager.GetAllNotes(kInstrumentId)),
              UnorderedElementsAre(kNotePitch));

  // Set parameter.
  EXPECT_TRUE(
      IsOk(instrument_manager.SetParam(kInstrumentId, 0.0, 1, kParamValue)));

  EXPECT_THAT(GetStatusOrValue(instrument_manager.GetAllParams(kInstrumentId)),
              UnorderedElementsAre(
                  AllOf(Property(&InstrumentParam::GetId, 1),
                        Property(&InstrumentParam::GetValue, kParamValue))));

  instrument_manager.Update();

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument_manager.Process(kInstrumentId, 0.0, buffer.data(), kNumChannels,
                             kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], kParamValue);
    }
  }

  // Update sampling rate.
  instrument_manager.SetSampleRate(0.0, 2);

  EXPECT_THAT(GetStatusOrValue(instrument_manager.GetAllNotes(kInstrumentId)),
              UnorderedElementsAre());
  EXPECT_THAT(GetStatusOrValue(instrument_manager.GetAllParams(kInstrumentId)),
              UnorderedElementsAre(
                  AllOf(Property(&InstrumentParam::GetId, 1),
                        Property(&InstrumentParam::GetValue, kParamValue))));

  instrument_manager.Update();

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument_manager.Process(kInstrumentId, 0.0, buffer.data(), kNumChannels,
                             kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], kParamValue);
    }
  }
}

}  // namespace
}  // namespace barelyapi
