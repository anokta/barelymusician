#include "barelymusician/engine/engine.h"

#include <algorithm>
#include <tuple>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
#include "barelymusician/engine/parameter.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

// TODO: Expand engine tests to cover sequence & transport functionality.
namespace barelyapi {
namespace {

using ::testing::Property;

constexpr int kSampleRate = 48000;
constexpr int kNumChannels = 2;
constexpr int kNumFrames = 12;

// Returns test instrument definition that produces constant output that is set.
BarelyInstrumentDefinition GetTestInstrumentDefinition() {
  static std::vector<BarelyParameterDefinition> parameter_definitions = {
      BarelyParameterDefinition{0.0f, -10.0f, 10.0f}};
  return {
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
      .set_data_fn = [](void** /*state*/, void* /*data*/) {},
      .set_note_off_fn =
          [](void** state, float /*pitch*/) {
            *reinterpret_cast<float*>(*state) = 0.0f;
          },
      .set_note_on_fn =
          [](void** state, float pitch, float intensity) {
            *reinterpret_cast<float*>(*state) = pitch * intensity;
          },
      .set_parameter_fn =
          [](void** state, int index, float value) {
            *reinterpret_cast<float*>(*state) =
                static_cast<float>(index + 1) * value;
          },
      .parameter_definitions = parameter_definitions.data(),
      .num_parameter_definitions =
          static_cast<int>(parameter_definitions.size())};
}

// Tests that instruments are added and removed as expected.
TEST(EngineTest, CreateDestroy) {
  const float kNotePitch = 1.25f;
  const float kNoteIntensity = 0.75f;

  Engine engine;
  std::vector<float> buffer(kNumChannels * kNumFrames);

  // Create instrument.
  const Id instrument_id =
      engine.CreateInstrument(GetTestInstrumentDefinition(), kSampleRate);
  EXPECT_NE(instrument_id, kInvalidId);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(IsOk(engine.ProcessInstrument(instrument_id, 0.0, buffer.data(),
                                            kNumChannels, kNumFrames)));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Start note.
  EXPECT_TRUE(IsOk(
      engine.StartInstrumentNote(instrument_id, kNotePitch, kNoteIntensity)));
  EXPECT_TRUE(
      GetStatusOrValue(engine.IsInstrumentNoteOn(instrument_id, kNotePitch)));

  engine.Update(0.0);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(IsOk(engine.ProcessInstrument(instrument_id, 0.0, buffer.data(),
                                            kNumChannels, kNumFrames)));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel],
                      kNotePitch * kNoteIntensity);
    }
  }

  // Destroy instrument.
  EXPECT_TRUE(IsOk(engine.DestroyInstrument(instrument_id)));

  engine.Update(0.0);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_FALSE(IsOk(engine.ProcessInstrument(instrument_id, 0.0, buffer.data(),
                                             kNumChannels, kNumFrames)));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

// Tests that instrument parameter is returned as expected.
TEST(EngineTest, GetInstrumentParameter) {
  Engine engine;
  const Id instrument_id =
      engine.CreateInstrument(GetTestInstrumentDefinition(), kSampleRate);
  EXPECT_NE(instrument_id, kInvalidId);
  EXPECT_TRUE(IsOk(engine.GetInstrumentParameter(instrument_id, 0)));
}

// Tests that playing a single instrument note produces the expected output.
TEST(EngineTest, PlayNote) {
  const float kNotePitch = 32.0f;
  const float kNoteIntensity = 0.5f;

  Engine engine;
  std::vector<float> buffer(kNumChannels * kNumFrames);

  // Create instrument.
  const Id instrument_id =
      engine.CreateInstrument(GetTestInstrumentDefinition(), kSampleRate);
  EXPECT_NE(instrument_id, kInvalidId);

  engine.Update(0.0);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(IsOk(engine.ProcessInstrument(instrument_id, 0.0, buffer.data(),
                                            kNumChannels, kNumFrames)));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Start note.
  EXPECT_FALSE(
      GetStatusOrValue(engine.IsInstrumentNoteOn(instrument_id, kNotePitch)));
  EXPECT_TRUE(IsOk(
      engine.StartInstrumentNote(instrument_id, kNotePitch, kNoteIntensity)));
  EXPECT_TRUE(
      GetStatusOrValue(engine.IsInstrumentNoteOn(instrument_id, kNotePitch)));

  engine.Update(0.0);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(IsOk(engine.ProcessInstrument(instrument_id, 0.0, buffer.data(),
                                            kNumChannels, kNumFrames)));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel],
                      kNotePitch * kNoteIntensity);
    }
  }

  // Stop note.
  EXPECT_TRUE(
      GetStatusOrValue(engine.IsInstrumentNoteOn(instrument_id, kNotePitch)));
  EXPECT_TRUE(IsOk(engine.StopInstrumentNote(instrument_id, kNotePitch)));
  EXPECT_FALSE(
      GetStatusOrValue(engine.IsInstrumentNoteOn(instrument_id, kNotePitch)));

  engine.Update(0.0);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(IsOk(engine.ProcessInstrument(instrument_id, 0.0, buffer.data(),
                                            kNumChannels, kNumFrames)));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

// Tests that playing multiple instrument notes produces the expected output.
TEST(EngineTest, PlayNotes) {
  const float kNoteIntensity = 1.0f;

  Engine engine;
  std::vector<float> buffer(kNumChannels * kNumFrames);

  // Create instrument.
  const Id instrument_id =
      engine.CreateInstrument(GetTestInstrumentDefinition(), 1);
  EXPECT_NE(instrument_id, kInvalidId);

  engine.Update(0.0);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(IsOk(engine.ProcessInstrument(instrument_id, 0.0, buffer.data(),
                                            kNumChannels, kNumFrames)));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Start new note per each sample in the buffer.
  for (int i = 0; i < kNumFrames; ++i) {
    EXPECT_TRUE(IsOk(engine.StartInstrumentNote(
        instrument_id, static_cast<float>(i), kNoteIntensity)));
    engine.Update(static_cast<double>(i + 1));
  }

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(IsOk(engine.ProcessInstrument(instrument_id, 0.0, buffer.data(),
                                            kNumChannels, kNumFrames)));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    const float expected = static_cast<float>(frame) * kNoteIntensity;
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], expected);
    }
  }

  // Stop each note in each sample accordingly.
  for (int i = 0; i < kNumFrames; ++i) {
    EXPECT_TRUE(
        IsOk(engine.StopInstrumentNote(instrument_id, static_cast<float>(i))));
    engine.Update(static_cast<double>(i + kNumFrames + 1));
  }

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(IsOk(
      engine.ProcessInstrument(instrument_id, static_cast<double>(kNumFrames),
                               buffer.data(), kNumChannels, kNumFrames)));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

// Tests that all instrument notes are stopped as expected.
TEST(EngineTest, StopAllNotes) {
  const int kNumInstruments = 3;
  const float kNoteIntensity = 0.1f;

  Engine engine;
  std::vector<float> buffer(kNumChannels * kNumFrames);

  // Create instruments.
  std::vector<Id> instrument_ids(kNumInstruments);
  for (int i = 0; i < kNumInstruments; ++i) {
    instrument_ids[i] =
        engine.CreateInstrument(GetTestInstrumentDefinition(), kSampleRate);
    EXPECT_NE(instrument_ids[i], kInvalidId);

    engine.Update(0.0);

    std::fill(buffer.begin(), buffer.end(), 0.0f);
    EXPECT_TRUE(IsOk(engine.ProcessInstrument(
        instrument_ids[i], 0.0, buffer.data(), kNumChannels, kNumFrames)));
    for (int frame = 0; frame < kNumFrames; ++frame) {
      for (int channel = 0; channel < kNumChannels; ++channel) {
        EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
      }
    }
  }

  // Start notes.
  for (int i = 0; i < kNumInstruments; ++i) {
    const float pitch = static_cast<float>(i);

    EXPECT_TRUE(IsOk(
        engine.StartInstrumentNote(instrument_ids[i], pitch, kNoteIntensity)));
    EXPECT_TRUE(
        GetStatusOrValue(engine.IsInstrumentNoteOn(instrument_ids[i], pitch)));

    engine.Update(0.0);

    std::fill(buffer.begin(), buffer.end(), 0.0f);
    const float expected = pitch * kNoteIntensity;
    EXPECT_TRUE(IsOk(engine.ProcessInstrument(
        instrument_ids[i], 0.0, buffer.data(), kNumChannels, kNumFrames)));
    for (int frame = 0; frame < kNumFrames; ++frame) {
      for (int channel = 0; channel < kNumChannels; ++channel) {
        EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], expected);
      }
    }
  }

  // Stop all notes.
  engine.StopPlayback();

  engine.Update(0.0);

  for (int i = 0; i < kNumInstruments; ++i) {
    const float pitch = static_cast<float>(i);

    EXPECT_FALSE(
        GetStatusOrValue(engine.IsInstrumentNoteOn(instrument_ids[i], pitch)));

    std::fill(buffer.begin(), buffer.end(), 0.0f);
    EXPECT_TRUE(IsOk(engine.ProcessInstrument(
        instrument_ids[i], 0.0, buffer.data(), kNumChannels, kNumFrames)));
    for (int frame = 0; frame < kNumFrames; ++frame) {
      for (int channel = 0; channel < kNumChannels; ++channel) {
        EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
      }
    }
  }
}

// Tests that instrument parameters are reset as expected.
TEST(EngineTest, ResetAllParameters) {
  const float kParameterValue = 4.0f;

  Engine engine;
  std::vector<float> buffer(kNumChannels * kNumFrames);

  // Create instrument.
  const Id instrument_id =
      engine.CreateInstrument(GetTestInstrumentDefinition(), kSampleRate);
  EXPECT_NE(instrument_id, kInvalidId);
  EXPECT_THAT(GetStatusOrValue(engine.GetInstrumentParameter(instrument_id, 0)),
              Property(&Parameter::GetValue, 0.0f));

  engine.Update(0.0);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(IsOk(engine.ProcessInstrument(instrument_id, 0.0, buffer.data(),
                                            kNumChannels, kNumFrames)));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Set parameter value.
  EXPECT_TRUE(
      IsOk(engine.SetInstrumentParameter(instrument_id, 0, kParameterValue)));
  EXPECT_THAT(GetStatusOrValue(engine.GetInstrumentParameter(instrument_id, 0)),
              Property(&Parameter::GetValue, kParameterValue));

  engine.Update(0.0);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(IsOk(engine.ProcessInstrument(instrument_id, 0.0, buffer.data(),
                                            kNumChannels, kNumFrames)));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], kParameterValue);
    }
  }

  // Reset all parameters.
  EXPECT_TRUE(IsOk(engine.ResetAllInstrumentParameters(instrument_id)));
  EXPECT_THAT(GetStatusOrValue(engine.GetInstrumentParameter(instrument_id, 0)),
              Property(&Parameter::GetValue, 0.0f));

  engine.Update(0.0);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(IsOk(engine.ProcessInstrument(instrument_id, 0.0, buffer.data(),
                                            kNumChannels, kNumFrames)));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

// Tests that setting notes triggers the corresponding callbacks as expected.
TEST(EngineTest, SetNoteCallbacks) {
  const float kNotePitch = 4.0f;
  const float kNoteIntensity = 0.25f;

  Engine engine;

  // Create instrument.
  const Id instrument_id =
      engine.CreateInstrument(GetTestInstrumentDefinition(), 1);
  EXPECT_NE(instrument_id, kInvalidId);

  // Trigger note on callback.
  std::tuple<float, float, double> note_on_data;
  // float note_on_pitch = 0.0f;
  // float note_on_intensity = 0.0f;
  // double note_on_timestamp = 0.0;
  engine.SetInstrumentNoteOnCallback(
      instrument_id,
      [](float pitch, float intensity, double timestamp, void* user_data) {
        *reinterpret_cast<std::tuple<float, float, double>*>(user_data) = {
            pitch, intensity, timestamp};
      },
      reinterpret_cast<void*>(&note_on_data));
  EXPECT_EQ(note_on_data, std::tuple(0.0f, 0.0f, 0.0));

  engine.Update(10.0);

  EXPECT_TRUE(IsOk(
      engine.StartInstrumentNote(instrument_id, kNotePitch, kNoteIntensity)));
  EXPECT_EQ(note_on_data, std::tuple(kNotePitch, kNoteIntensity, 10.0));

  engine.Update(15.0);

  // This should not trigger the callback since the note is already on.
  EXPECT_TRUE(IsOk(
      engine.StartInstrumentNote(instrument_id, kNotePitch, kNoteIntensity)));
  EXPECT_EQ(note_on_data, std::tuple(kNotePitch, kNoteIntensity, 10.0));

  // Trigger note on callback again with another note.
  EXPECT_TRUE(IsOk(engine.StartInstrumentNote(instrument_id, kNotePitch + 2.0f,
                                              kNoteIntensity)));
  EXPECT_EQ(note_on_data, std::tuple(kNotePitch + 2.0f, kNoteIntensity, 15.0));

  // Trigger note off callback.
  std::tuple<float, double> note_off_data;
  engine.SetInstrumentNoteOffCallback(
      instrument_id,
      [](float pitch, double timestamp, void* user_data) {
        *reinterpret_cast<std::tuple<float, double>*>(user_data) = {pitch,
                                                                    timestamp};
      },
      reinterpret_cast<void*>(&note_off_data));
  EXPECT_EQ(note_off_data, std::tuple(0.0f, 0.0));

  engine.Update(20.0);

  EXPECT_TRUE(IsOk(engine.StopInstrumentNote(instrument_id, kNotePitch)));
  EXPECT_EQ(note_off_data, std::tuple(kNotePitch, 20.0));

  engine.Update(25.0);

  // This should not trigger the callback since the note is already off.
  EXPECT_TRUE(IsOk(engine.StopInstrumentNote(instrument_id, kNotePitch)));
  EXPECT_EQ(note_off_data, std::tuple(kNotePitch, 20.0));

  engine.Update(30.0);

  // This should trigger the note off callback with the remaining note.
  EXPECT_TRUE(IsOk(engine.StopAllInstrumentNotes(instrument_id)));
  EXPECT_EQ(note_off_data, std::tuple(kNotePitch + 2.0f, 30.0));
}

}  // namespace
}  // namespace barelyapi
