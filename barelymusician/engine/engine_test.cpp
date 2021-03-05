#include "barelymusician/engine/engine.h"

#include <algorithm>
#include <any>
#include <vector>

#include "barelymusician/engine/instrument_data.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

using ::testing::UnorderedElementsAre;

constexpr int kSampleRate = 48000;
constexpr int kNumChannels = 2;
constexpr int kNumFrames = 12;

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

// Tests that engine creates and destroy instruments as expected.
TEST(EngineTest, CreateDestroy) {
  const float kNotePitch = 1.25f;
  const float kNoteIntensity = 0.75f;

  Engine engine(kSampleRate);
  std::vector<float> buffer(kNumChannels * kNumFrames);

  // Create instrument.
  const int instrument_id = engine.CreateInstrument(
      GetTestInstrumentDefinition(), GetTestInstrumentParamDefinitions());

  EXPECT_TRUE(engine.GetAllInstrumentNotes(instrument_id).empty());
  EXPECT_THAT(engine.GetAllInstrumentParams(instrument_id),
              UnorderedElementsAre(Param{1, 0.0f}));

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(engine.ProcessInstrument(instrument_id, 0.0, buffer.data(),
                                       kNumChannels, kNumFrames));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Set note on.
  EXPECT_TRUE(
      engine.SetInstrumentNoteOn(instrument_id, kNotePitch, kNoteIntensity));

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(engine.ProcessInstrument(instrument_id, 0.0, buffer.data(),
                                       kNumChannels, kNumFrames));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel],
                      kNotePitch * kNoteIntensity);
    }
  }

  // Destroy instrument.
  EXPECT_TRUE(engine.DestroyInstrument(instrument_id));

  EXPECT_TRUE(engine.GetAllInstrumentNotes(instrument_id).empty());
  EXPECT_TRUE(engine.GetAllInstrumentParams(instrument_id).empty());

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_FALSE(engine.ProcessInstrument(instrument_id, 0.0, buffer.data(),
                                        kNumChannels, kNumFrames));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

// Tests that scheduling multiple instrument notes produces the expected output.
TEST(EngineTest, ScheduleInstrumentNotes) {
  const float kNoteIntensity = 1.0f;

  Engine engine(1);
  std::vector<float> buffer(kNumChannels * kNumFrames);

  engine.StartPlayback();

  // Create instrument.
  const int instrument_id = engine.CreateInstrument(
      GetTestInstrumentDefinition(), GetTestInstrumentParamDefinitions());

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(engine.ProcessInstrument(instrument_id, 0.0, buffer.data(),
                                       kNumChannels, kNumFrames));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Schedule new note per each sample in the buffer.
  for (int i = 0; i < kNumFrames; ++i) {
    engine.ScheduleInstrumentNote(instrument_id, static_cast<double>(i),
                                  static_cast<double>(i + kNumFrames),
                                  static_cast<float>(i), kNoteIntensity);
  }
  engine.Update(static_cast<double>(kNumFrames));
  EXPECT_EQ(engine.GetAllInstrumentNotes(instrument_id).size(), kNumFrames);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(engine.ProcessInstrument(instrument_id, 0.0, buffer.data(),
                                       kNumChannels, kNumFrames));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    const float expected = static_cast<float>(frame) * kNoteIntensity;
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], expected);
    }
  }

  engine.Update(2.0 * static_cast<double>(kNumFrames));
  EXPECT_TRUE(engine.GetAllInstrumentNotes(instrument_id).empty());

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(
      engine.ProcessInstrument(instrument_id, static_cast<double>(kNumFrames),
                               buffer.data(), kNumChannels, kNumFrames));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

// Tests that setting a single instrument note produces the expected output.
TEST(EngineTest, SetInstrumentNote) {
  const double kTimestamp = 20.0;
  const float kNotePitch = 32.0f;
  const float kNoteIntensity = 0.5f;

  Engine engine(kSampleRate);
  std::vector<float> buffer(kNumChannels * kNumFrames);

  engine.Update(kTimestamp);

  // Create instrument.
  const int instrument_id = engine.CreateInstrument(
      GetTestInstrumentDefinition(), GetTestInstrumentParamDefinitions());

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(engine.ProcessInstrument(instrument_id, kTimestamp, buffer.data(),
                                       kNumChannels, kNumFrames));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Set note on.
  EXPECT_TRUE(
      engine.SetInstrumentNoteOn(instrument_id, kNotePitch, kNoteIntensity));
  EXPECT_TRUE(engine.IsInstrumentNoteOn(instrument_id, kNotePitch));

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(engine.ProcessInstrument(instrument_id, kTimestamp, buffer.data(),
                                       kNumChannels, kNumFrames));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel],
                      kNotePitch * kNoteIntensity);
    }
  }

  // Set note off.
  EXPECT_TRUE(engine.SetInstrumentNoteOff(instrument_id, kNotePitch));
  EXPECT_FALSE(engine.IsInstrumentNoteOn(instrument_id, kNotePitch));

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(engine.ProcessInstrument(instrument_id, kTimestamp, buffer.data(),
                                       kNumChannels, kNumFrames));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

// Tests that playing notes triggers the corresponding callbacks as expected.
TEST(EngineTest, SetInstrumentNoteCallbacks) {
  const float kNotePitch = 4.0f;
  const float kNoteIntensity = 0.25f;

  Engine engine(1);
  engine.Update(1.0);

  // Create instrument.
  const int instrument_id = engine.CreateInstrument(
      GetTestInstrumentDefinition(), GetTestInstrumentParamDefinitions());

  // Trigger note on callback.
  int note_on_instrument_id = 0;
  float note_on_pitch = 0.0f;
  float note_on_intensity = 0.0f;
  engine.SetNoteOnCallback([&](int instrument_id, double timestamp,
                               float note_pitch, float note_intensity) {
    note_on_instrument_id = instrument_id;
    note_on_pitch = note_pitch;
    note_on_intensity = note_intensity;
  });
  EXPECT_NE(note_on_instrument_id, instrument_id);
  EXPECT_NE(note_on_pitch, kNotePitch);
  EXPECT_NE(note_on_intensity, kNoteIntensity);

  EXPECT_TRUE(
      engine.SetInstrumentNoteOn(instrument_id, kNotePitch, kNoteIntensity));
  EXPECT_EQ(note_on_instrument_id, instrument_id);
  EXPECT_FLOAT_EQ(note_on_pitch, kNotePitch);
  EXPECT_FLOAT_EQ(note_on_intensity, kNoteIntensity);

  // This should not trigger the callback since the note is already on.
  engine.Update(2.0);
  EXPECT_FALSE(
      engine.SetInstrumentNoteOn(instrument_id, kNotePitch, kNoteIntensity));

  // Trigger note on callback again with another note.
  EXPECT_TRUE(engine.SetInstrumentNoteOn(instrument_id, 2.0f, kNoteIntensity));
  EXPECT_FLOAT_EQ(note_on_pitch, 2.0f);

  // Trigger note off callback.
  int note_off_instrument_id = 0;
  double note_off_timestamp = 0.0;
  float note_off_pitch = 0.0f;
  engine.SetNoteOffCallback(
      [&](int instrument_id, double timestamp, float note_pitch) {
        note_off_instrument_id = instrument_id;
        note_off_pitch = note_pitch;
      });
  EXPECT_NE(note_off_instrument_id, instrument_id);
  EXPECT_NE(note_off_pitch, kNotePitch);

  EXPECT_TRUE(engine.SetInstrumentNoteOff(instrument_id, kNotePitch));
  EXPECT_EQ(note_off_instrument_id, instrument_id);
  EXPECT_FLOAT_EQ(note_off_pitch, kNotePitch);

  // This should not trigger the callback since the note is already off.
  engine.Update(3.0);
  EXPECT_FALSE(engine.SetInstrumentNoteOff(instrument_id, kNotePitch));

  // Trigger both callbacks with a scheduled note.
  engine.StartPlayback();
  engine.ScheduleInstrumentNote(instrument_id, 1.0, 1.5, 10.0f, 1.0f);

  engine.Update(4.0);
  EXPECT_NE(note_on_pitch, 10.0f);
  EXPECT_NE(note_on_intensity, 1.0f);
  EXPECT_NE(note_off_timestamp, 4.5);
  EXPECT_NE(note_off_pitch, 10.0f);

  engine.Update(5.0);
  EXPECT_FLOAT_EQ(note_on_pitch, 10.0f);
  EXPECT_FLOAT_EQ(note_on_intensity, 1.0f);
  EXPECT_DOUBLE_EQ(note_off_timestamp, 4.5);
  EXPECT_FLOAT_EQ(note_off_pitch, 10.0f);

  // Finally, destroy to trigger the note off callback with the remaining note.
  EXPECT_TRUE(engine.DestroyInstrument(instrument_id));
  EXPECT_FLOAT_EQ(note_off_pitch, 2.0f);
}

// Tests that engine resets all parameters of multiple instruments as expected.
TEST(EngineTest, ResetAllParams) {
  const int kNumInstruments = 2;

  Engine engine(kSampleRate);
  std::vector<float> buffer(kNumChannels * kNumFrames);

  // Create instruments.
  std::vector<int> instrument_ids(kNumInstruments);
  for (int i = 0; i < kNumInstruments; ++i) {
    instrument_ids[i] = engine.CreateInstrument(
        GetTestInstrumentDefinition(), GetTestInstrumentParamDefinitions());
  }

  // Set parameter values.
  for (int i = 0; i < kNumInstruments; ++i) {
    std::fill(buffer.begin(), buffer.end(), 0.0f);
    EXPECT_TRUE(engine.ProcessInstrument(instrument_ids[i], 0.0, buffer.data(),
                                         kNumChannels, kNumFrames));
    for (int frame = 0; frame < kNumFrames; ++frame) {
      for (int channel = 0; channel < kNumChannels; ++channel) {
        EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
      }
    }

    engine.SetInstrumentParam(instrument_ids[i], 1, static_cast<float>(i));
    EXPECT_THAT(engine.GetAllInstrumentParams(instrument_ids[i]),
                UnorderedElementsAre(Param{1, static_cast<float>(i)}));

    std::fill(buffer.begin(), buffer.end(), 0.0f);
    EXPECT_TRUE(engine.ProcessInstrument(instrument_ids[i], 0.0, buffer.data(),
                                         kNumChannels, kNumFrames));
    const float expected = static_cast<float>(i);
    for (int frame = 0; frame < kNumFrames; ++frame) {
      for (int channel = 0; channel < kNumChannels; ++channel) {
        EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], expected);
      }
    }
  }

  // Reset all parameters.
  engine.ResetAllInstrumentParams();

  for (int i = 0; i < kNumInstruments; ++i) {
    EXPECT_THAT(engine.GetAllInstrumentParams(instrument_ids[i]),
                UnorderedElementsAre(Param{1, 0.0f}));

    std::fill(buffer.begin(), buffer.end(), 0.0f);
    EXPECT_TRUE(engine.ProcessInstrument(instrument_ids[i], 0.0, buffer.data(),
                                         kNumChannels, kNumFrames));
    for (int frame = 0; frame < kNumFrames; ++frame) {
      for (int channel = 0; channel < kNumChannels; ++channel) {
        EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
      }
    }
  }
}

// Tests that engine set all notes of multiple instruments off as expected.
TEST(EngineTest, SetAllInstrumentNotesOff) {
  const int kNumInstruments = 3;
  const float kNoteIntensity = 0.1f;

  Engine engine(kSampleRate);
  std::vector<float> buffer(kNumChannels * kNumFrames);

  // Create instruments.
  std::vector<int> instrument_ids(kNumInstruments);
  for (int i = 0; i < kNumInstruments; ++i) {
    instrument_ids[i] = engine.CreateInstrument(
        GetTestInstrumentDefinition(), GetTestInstrumentParamDefinitions());
  }

  // Set notes on.
  for (int i = 0; i < kNumInstruments; ++i) {
    std::fill(buffer.begin(), buffer.end(), 0.0f);
    EXPECT_TRUE(engine.ProcessInstrument(instrument_ids[i], 0.0, buffer.data(),
                                         kNumChannels, kNumFrames));
    for (int frame = 0; frame < kNumFrames; ++frame) {
      for (int channel = 0; channel < kNumChannels; ++channel) {
        EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
      }
    }

    EXPECT_TRUE(engine.SetInstrumentNoteOn(
        instrument_ids[i], static_cast<float>(i), kNoteIntensity));
    EXPECT_THAT(engine.GetAllInstrumentNotes(instrument_ids[i]),
                UnorderedElementsAre(static_cast<float>(i)));

    std::fill(buffer.begin(), buffer.end(), 0.0f);
    const float expected = static_cast<float>(i) * kNoteIntensity;
    EXPECT_TRUE(engine.ProcessInstrument(instrument_ids[i], 0.0, buffer.data(),
                                         kNumChannels, kNumFrames));
    for (int frame = 0; frame < kNumFrames; ++frame) {
      for (int channel = 0; channel < kNumChannels; ++channel) {
        EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], expected);
      }
    }
  }

  // Set all notes off.
  engine.SetAllInstrumentNotesOff();

  for (int i = 0; i < kNumInstruments; ++i) {
    EXPECT_TRUE(engine.GetAllInstrumentNotes(instrument_ids[i]).empty());

    std::fill(buffer.begin(), buffer.end(), 0.0f);
    EXPECT_TRUE(engine.ProcessInstrument(instrument_ids[i], 0.0, buffer.data(),
                                         kNumChannels, kNumFrames));
    for (int frame = 0; frame < kNumFrames; ++frame) {
      for (int channel = 0; channel < kNumChannels; ++channel) {
        EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
      }
    }
  }
}

}  // namespace
}  // namespace barelyapi
