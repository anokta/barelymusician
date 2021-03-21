#include "barelymusician/engine/instrument_manager.h"

#include <algorithm>
#include <any>
#include <vector>

#include "barelymusician/common/id_generator.h"
#include "barelymusician/engine/instrument_event.h"
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

// Tests that instrument_manager creates and destroy instruments as expected.
TEST(InstrumentManagerTest, CreateDestroy) {
  const float kNotePitch = 1.25f;
  const float kNoteIntensity = 0.75f;

  IdGenerator id_generator;
  InstrumentManager instrument_manager(kSampleRate, &id_generator);
  std::vector<float> buffer(kNumChannels * kNumFrames);

  // Create instrument.
  const int instrument_id = instrument_manager.Create(
      GetTestInstrumentDefinition(), GetTestInstrumentParamDefinitions());

  EXPECT_TRUE(instrument_manager.GetAllNotes(instrument_id).empty());
  EXPECT_THAT(instrument_manager.GetAllParams(instrument_id),
              UnorderedElementsAre(Param{1, 0.0f}));

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(instrument_manager.Process(instrument_id, buffer.data(),
                                         kNumChannels, kNumFrames));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Set note on.
  EXPECT_TRUE(
      instrument_manager.SetNoteOn(instrument_id, kNotePitch, kNoteIntensity));
  instrument_manager.Update();

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(instrument_manager.Process(instrument_id, buffer.data(),
                                         kNumChannels, kNumFrames));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel],
                      kNotePitch * kNoteIntensity);
    }
  }

  // Destroy instrument.
  EXPECT_TRUE(instrument_manager.Destroy(instrument_id));
  instrument_manager.Update();

  EXPECT_TRUE(instrument_manager.GetAllNotes(instrument_id).empty());
  EXPECT_TRUE(instrument_manager.GetAllParams(instrument_id).empty());

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_FALSE(instrument_manager.Process(instrument_id, buffer.data(),
                                          kNumChannels, kNumFrames));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

// Tests that setting multiple instrument notes produces the expected output.
TEST(InstrumentManagerTest, SetNotes) {
  const float kNoteIntensity = 1.0f;

  IdGenerator id_generator;
  InstrumentManager instrument_manager(1, &id_generator);
  std::vector<float> buffer(kNumChannels * kNumFrames);

  // Create instrument.
  const int instrument_id = instrument_manager.Create(
      GetTestInstrumentDefinition(), GetTestInstrumentParamDefinitions());

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(instrument_manager.Process(instrument_id, buffer.data(),
                                         kNumChannels, kNumFrames, 0.0));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Set new note per each sample in the buffer.
  std::multimap<double, InstrumentEvent> events;
  for (int i = 0; i < kNumFrames; ++i) {
    events.emplace(static_cast<double>(i),
                   NoteOn{static_cast<float>(i), kNoteIntensity});
    events.emplace(static_cast<double>(i + kNumFrames),
                   NoteOff{static_cast<float>(i)});
  }
  instrument_manager.SetEvents(instrument_id, std::move(events));
  instrument_manager.Update(static_cast<double>(kNumFrames));

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(instrument_manager.Process(instrument_id, buffer.data(),
                                         kNumChannels, kNumFrames, 0.0));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    const float expected = static_cast<float>(frame) * kNoteIntensity;
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], expected);
    }
  }

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(instrument_manager.Process(instrument_id, buffer.data(),
                                         kNumChannels, kNumFrames,
                                         static_cast<double>(kNumFrames)));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

// Tests that setting a single instrument note produces the expected output.
TEST(InstrumentManagerTest, SetNote) {
  const double kTimestamp = 20.0;
  const float kNotePitch = 32.0f;
  const float kNoteIntensity = 0.5f;

  IdGenerator id_generator;
  InstrumentManager instrument_manager(kSampleRate, &id_generator);
  std::vector<float> buffer(kNumChannels * kNumFrames);

  // Create instrument.
  const int instrument_id = instrument_manager.Create(
      GetTestInstrumentDefinition(), GetTestInstrumentParamDefinitions());

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(instrument_manager.Process(instrument_id, buffer.data(),
                                         kNumChannels, kNumFrames, kTimestamp));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Set note on.
  EXPECT_TRUE(instrument_manager.SetNoteOn(instrument_id, kNotePitch,
                                           kNoteIntensity, kTimestamp));
  instrument_manager.Update(kTimestamp);
  EXPECT_TRUE(instrument_manager.IsNoteOn(instrument_id, kNotePitch));

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(instrument_manager.Process(instrument_id, buffer.data(),
                                         kNumChannels, kNumFrames, kTimestamp));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel],
                      kNotePitch * kNoteIntensity);
    }
  }

  // Set note off.
  EXPECT_TRUE(
      instrument_manager.SetNoteOff(instrument_id, kNotePitch, kTimestamp));
  instrument_manager.Update(kTimestamp);
  EXPECT_FALSE(instrument_manager.IsNoteOn(instrument_id, kNotePitch));

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(instrument_manager.Process(instrument_id, buffer.data(),
                                         kNumChannels, kNumFrames, kTimestamp));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

// Tests that playing notes triggers the corresponding callbacks as expected.
TEST(InstrumentManagerTest, SetNoteCallbacks) {
  const float kNotePitch = 4.0f;
  const float kNoteIntensity = 0.25f;

  IdGenerator id_generator;
  InstrumentManager instrument_manager(1, &id_generator);

  // Create instrument.
  const int instrument_id = instrument_manager.Create(
      GetTestInstrumentDefinition(), GetTestInstrumentParamDefinitions());

  // Trigger note on callback.
  int note_on_instrument_id = 0;
  float note_on_pitch = 0.0f;
  float note_on_intensity = 0.0f;
  instrument_manager.SetNoteOnCallback(
      [&](int instrument_id, float note_pitch, float note_intensity) {
        note_on_instrument_id = instrument_id;
        note_on_pitch = note_pitch;
        note_on_intensity = note_intensity;
      });
  EXPECT_NE(note_on_instrument_id, instrument_id);
  EXPECT_NE(note_on_pitch, kNotePitch);
  EXPECT_NE(note_on_intensity, kNoteIntensity);

  EXPECT_TRUE(
      instrument_manager.SetNoteOn(instrument_id, kNotePitch, kNoteIntensity));
  instrument_manager.Update();
  EXPECT_EQ(note_on_instrument_id, instrument_id);
  EXPECT_FLOAT_EQ(note_on_pitch, kNotePitch);
  EXPECT_FLOAT_EQ(note_on_intensity, kNoteIntensity);

  // This should not trigger the callback since the note is already on.
  EXPECT_TRUE(
      instrument_manager.SetNoteOn(instrument_id, kNotePitch, kNoteIntensity));
  instrument_manager.Update();
  EXPECT_FLOAT_EQ(note_on_pitch, kNotePitch);

  // Trigger note on callback again with another note.
  EXPECT_TRUE(
      instrument_manager.SetNoteOn(instrument_id, 2.0f, kNoteIntensity));
  instrument_manager.Update();
  EXPECT_FLOAT_EQ(note_on_pitch, 2.0f);

  // Trigger note off callback.
  int note_off_instrument_id = 0;
  float note_off_pitch = 0.0f;
  instrument_manager.SetNoteOffCallback(
      [&](int instrument_id, float note_pitch) {
        note_off_instrument_id = instrument_id;
        note_off_pitch = note_pitch;
      });
  EXPECT_NE(note_off_instrument_id, instrument_id);
  EXPECT_NE(note_off_pitch, kNotePitch);

  EXPECT_TRUE(instrument_manager.SetNoteOff(instrument_id, kNotePitch));
  instrument_manager.Update();
  EXPECT_EQ(note_off_instrument_id, instrument_id);
  EXPECT_FLOAT_EQ(note_off_pitch, kNotePitch);

  // This should not trigger the callback since the note is already off.
  EXPECT_TRUE(instrument_manager.SetNoteOff(instrument_id, kNotePitch));
  instrument_manager.Update();
  EXPECT_FLOAT_EQ(note_off_pitch, kNotePitch);

  // Finally, destroy to trigger the note off callback with the remaining note.
  EXPECT_TRUE(instrument_manager.Destroy(instrument_id));
  EXPECT_FLOAT_EQ(note_off_pitch, 2.0f);
}

// Tests that instrument_manager resets all parameters of multiple instruments
// as expected.
TEST(InstrumentManagerTest, ResetAllParams) {
  const int kNumInstruments = 2;

  IdGenerator id_generator;
  InstrumentManager instrument_manager(kSampleRate, &id_generator);
  std::vector<float> buffer(kNumChannels * kNumFrames);

  // Create instruments.
  std::vector<int> instrument_ids(kNumInstruments);
  for (int i = 0; i < kNumInstruments; ++i) {
    instrument_ids[i] = instrument_manager.Create(
        GetTestInstrumentDefinition(), GetTestInstrumentParamDefinitions());
  }

  // Set parameter values.
  for (int i = 0; i < kNumInstruments; ++i) {
    std::fill(buffer.begin(), buffer.end(), 0.0f);
    EXPECT_TRUE(instrument_manager.Process(instrument_ids[i], buffer.data(),
                                           kNumChannels, kNumFrames));
    for (int frame = 0; frame < kNumFrames; ++frame) {
      for (int channel = 0; channel < kNumChannels; ++channel) {
        EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
      }
    }

    instrument_manager.SetParam(instrument_ids[i], 1, static_cast<float>(i));
    instrument_manager.Update();
    EXPECT_THAT(instrument_manager.GetAllParams(instrument_ids[i]),
                UnorderedElementsAre(Param{1, static_cast<float>(i)}));

    std::fill(buffer.begin(), buffer.end(), 0.0f);
    EXPECT_TRUE(instrument_manager.Process(instrument_ids[i], buffer.data(),
                                           kNumChannels, kNumFrames));
    const float expected = static_cast<float>(i);
    for (int frame = 0; frame < kNumFrames; ++frame) {
      for (int channel = 0; channel < kNumChannels; ++channel) {
        EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], expected);
      }
    }
  }

  // Reset all parameters.
  instrument_manager.ResetAllParams();
  instrument_manager.Update();

  for (int i = 0; i < kNumInstruments; ++i) {
    EXPECT_THAT(instrument_manager.GetAllParams(instrument_ids[i]),
                UnorderedElementsAre(Param{1, 0.0f}));

    std::fill(buffer.begin(), buffer.end(), 0.0f);
    EXPECT_TRUE(instrument_manager.Process(instrument_ids[i], buffer.data(),
                                           kNumChannels, kNumFrames));
    for (int frame = 0; frame < kNumFrames; ++frame) {
      for (int channel = 0; channel < kNumChannels; ++channel) {
        EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
      }
    }
  }
}

// Tests that instrument_manager set all notes of multiple instruments off as
// expected.
TEST(InstrumentManagerTest, SetAllNotesOff) {
  const int kNumInstruments = 3;
  const float kNoteIntensity = 0.1f;

  IdGenerator id_generator;
  InstrumentManager instrument_manager(kSampleRate, &id_generator);
  std::vector<float> buffer(kNumChannels * kNumFrames);

  // Create instruments.
  std::vector<int> instrument_ids(kNumInstruments);
  for (int i = 0; i < kNumInstruments; ++i) {
    instrument_ids[i] = instrument_manager.Create(
        GetTestInstrumentDefinition(), GetTestInstrumentParamDefinitions());
  }

  // Set notes on.
  for (int i = 0; i < kNumInstruments; ++i) {
    std::fill(buffer.begin(), buffer.end(), 0.0f);
    EXPECT_TRUE(instrument_manager.Process(instrument_ids[i], buffer.data(),
                                           kNumChannels, kNumFrames));
    for (int frame = 0; frame < kNumFrames; ++frame) {
      for (int channel = 0; channel < kNumChannels; ++channel) {
        EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
      }
    }

    EXPECT_TRUE(instrument_manager.SetNoteOn(
        instrument_ids[i], static_cast<float>(i), kNoteIntensity));
    instrument_manager.Update();
    EXPECT_THAT(instrument_manager.GetAllNotes(instrument_ids[i]),
                UnorderedElementsAre(static_cast<float>(i)));

    std::fill(buffer.begin(), buffer.end(), 0.0f);
    const float expected = static_cast<float>(i) * kNoteIntensity;
    EXPECT_TRUE(instrument_manager.Process(instrument_ids[i], buffer.data(),
                                           kNumChannels, kNumFrames));
    for (int frame = 0; frame < kNumFrames; ++frame) {
      for (int channel = 0; channel < kNumChannels; ++channel) {
        EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], expected);
      }
    }
  }

  // Set all notes off.
  instrument_manager.SetAllNotesOff();
  instrument_manager.Update();

  for (int i = 0; i < kNumInstruments; ++i) {
    EXPECT_TRUE(instrument_manager.GetAllNotes(instrument_ids[i]).empty());

    std::fill(buffer.begin(), buffer.end(), 0.0f);
    EXPECT_TRUE(instrument_manager.Process(instrument_ids[i], buffer.data(),
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
