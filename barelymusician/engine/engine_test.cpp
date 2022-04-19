#include "barelymusician/engine/engine.h"

#include <algorithm>
#include <vector>

#include "barelymusician/engine/id.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/engine/note.h"
#include "barelymusician/engine/parameter.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

using ::testing::AllOf;
using ::testing::ElementsAre;
using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::Pointee;
using ::testing::UnorderedElementsAre;

constexpr int kFrameRate = 48000;
constexpr int kNumChannels = 2;
constexpr int kNumFrames = 8;

// Returns test instrument definition that produces constant output that is set.
Instrument::Definition GetTestInstrumentDefinition() {
  static const std::vector<Parameter::Definition> parameter_definitions = {
      Parameter::Definition{0.0, -10.0, 10.0},
  };
  return Instrument::Definition(
      [](void** state, int /*frame_rate*/) {
        *state = reinterpret_cast<void*>(new double{0.0});
      },
      [](void** state) { delete static_cast<double*>(*state); },
      [](void** state, double* output, int num_output_channels,
         int num_output_frames) {
        std::fill_n(output, num_output_channels * num_output_frames,
                    *reinterpret_cast<double*>(*state));
      },
      [](void** /*state*/, const void* /*data*/, int /*size*/) {},
      [](void** state, double /*pitch*/) {
        *reinterpret_cast<double*>(*state) = 0.0;
      },
      [](void** state, double pitch, double intensity) {
        *reinterpret_cast<double*>(*state) = pitch * intensity;
      },
      [](void** state, int index, double value, double /*slope*/) {
        *reinterpret_cast<double*>(*state) =
            static_cast<double>(index + 1) * value;
      },
      parameter_definitions);
}

// Tests that single instrument is created and destroyed as expected.
TEST(EngineTest, CreateDestroySingleInstrument) {
  const Id kId = 1;
  const double kPitch = -1.25;
  const double kIntensity = 0.75;

  Engine engine;
  std::vector<double> buffer(kNumChannels * kNumFrames);
  EXPECT_THAT(engine.GetInstrument(kId), IsNull());

  // Create instrument.
  EXPECT_TRUE(
      engine.CreateInstrument(kId, GetTestInstrumentDefinition(), kFrameRate));

  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(engine.ProcessInstrument(kId, buffer.data(), kNumChannels,
                                       kNumFrames, 0.0));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kNumChannels * frame + channel], 0.0);
    }
  }

  // Get instrument.
  auto* instrument = engine.GetInstrument(kId);
  EXPECT_THAT(instrument, NotNull());

  // Set note callbacks.
  double note_on_pitch = 0.0;
  double note_on_intensity = 0.0;
  double note_on_timestamp = 0.0;
  instrument->SetNoteOnCallback(
      [&](double pitch, double intensity, double timestamp) {
        note_on_pitch = pitch;
        note_on_intensity = intensity;
        note_on_timestamp = timestamp;
      });
  EXPECT_NE(note_on_pitch, kPitch);
  EXPECT_NE(note_on_intensity, kIntensity);
  EXPECT_DOUBLE_EQ(note_on_timestamp, 0.0);

  double note_off_pitch = 0.0;
  double note_off_timestamp = 0.0;
  instrument->SetNoteOffCallback([&](double pitch, double timestamp) {
    note_off_pitch = pitch;
    note_off_timestamp = timestamp;
  });
  EXPECT_NE(note_off_pitch, kPitch);
  EXPECT_DOUBLE_EQ(note_off_timestamp, 0.0);

  // Start note.
  instrument->StartNote(kPitch, kIntensity, 0.0);
  EXPECT_TRUE(instrument->IsNoteOn(kPitch));

  EXPECT_DOUBLE_EQ(note_on_pitch, kPitch);
  EXPECT_DOUBLE_EQ(note_on_intensity, kIntensity);
  EXPECT_DOUBLE_EQ(note_on_timestamp, 0.0);

  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(engine.ProcessInstrument(kId, buffer.data(), kNumChannels,
                                       kNumFrames, 0.0));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kNumChannels * frame + channel],
                       kPitch * kIntensity);
    }
  }

  // Update timestamp.
  engine.Update(5.0);

  // Destroy instrument, which should also trigger note off callback.
  EXPECT_TRUE(engine.DestroyInstrument(kId));
  EXPECT_THAT(engine.GetInstrument(kId), IsNull());

  EXPECT_DOUBLE_EQ(note_off_pitch, kPitch);
  EXPECT_DOUBLE_EQ(note_off_timestamp, 5.0);

  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_FALSE(engine.ProcessInstrument(kId, buffer.data(), kNumChannels,
                                        kNumFrames, 0.0));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kNumChannels * frame + channel], 0.0);
    }
  }
}

// Tests that multiple instruments are created and destroyed as expected.
TEST(EngineTest, CreateDestroyMultipleInstruments) {
  std::vector<double> note_off_pitches;

  {
    Engine engine;

    // Create instruments with note off callback.
    for (int i = 0; i < 3; ++i) {
      const Id instrument_id = i + 1;
      EXPECT_THAT(engine.GetInstrument(instrument_id), IsNull());
      EXPECT_TRUE(engine.CreateInstrument(
          instrument_id, GetTestInstrumentDefinition(), kFrameRate));
      EXPECT_THAT(engine.GetInstrument(instrument_id), NotNull());
      engine.GetInstrument(instrument_id)
          ->SetNoteOffCallback([&](double pitch, double /*timestamp*/) {
            note_off_pitches.push_back(pitch);
          });
    }

    // Start multiple notes, then immediately stop some of them.
    for (int i = 0; i < 3; ++i) {
      engine.GetInstrument(i + 1)->StartNote(static_cast<double>(i + 1), 1.0,
                                             0.0);
      engine.GetInstrument(i + 1)->StartNote(static_cast<double>(-i - 1), 1.0,
                                             0.0);
      engine.GetInstrument(i + 1)->StopNote(static_cast<double>(i + 1), 0.0);
    }
    EXPECT_THAT(note_off_pitches, ElementsAre(1.0, 2.0, 3.0));
  }

  // Remaining active notes should be stopped once the engine goes out of scope.
  EXPECT_THAT(note_off_pitches,
              UnorderedElementsAre(-3.0, -2.0, -1.0, 1.0, 2.0, 3.0));
}

// Tests that engine plays sequence as expected.
TEST(EngineTest, PlaySequence) {
  const Id kInstrumentId = 1;
  const Id kSequenceId = 2;
  const Id kNoteId = 3;

  const double kRootNote = 0.5;

  Engine engine;
  EXPECT_THAT(engine.GetInstrument(kInstrumentId), IsNull());
  EXPECT_THAT(engine.GetSequence(kSequenceId), IsNull());

  // Create instrument.
  EXPECT_TRUE(engine.CreateInstrument(
      kInstrumentId, GetTestInstrumentDefinition(), kFrameRate));
  auto* instrument = engine.GetInstrument(kInstrumentId);
  EXPECT_THAT(instrument, NotNull());

  // Create sequence.
  EXPECT_TRUE(engine.CreateSequence(kSequenceId));
  auto* sequence = engine.GetSequence(kSequenceId);
  EXPECT_THAT(sequence, NotNull());

  // Set sequence instrument.
  EXPECT_TRUE(engine.SetSequenceInstrumentId(kSequenceId, kInstrumentId));
  EXPECT_THAT(engine.GetSequenceInstrumentId(kSequenceId),
              AllOf(NotNull(), Pointee(kInstrumentId)));

  // Create note.
  EXPECT_TRUE(sequence->CreateNote(kNoteId, 1.0, 5.0, kRootNote + 1.0, 1.0));

  EXPECT_FALSE(instrument->IsNoteOn(kRootNote + 1.0));

  // Start playback with one beat per second tempo.
  engine.GetTransport().SetTempo(60.0);
  EXPECT_DOUBLE_EQ(engine.GetTransport().GetTempo(), 60.0);

  EXPECT_FALSE(engine.GetTransport().IsPlaying());
  engine.Start();
  EXPECT_TRUE(engine.GetTransport().IsPlaying());

  // Update timestamp just before the start position of the note.
  engine.Update(1.0);
  EXPECT_DOUBLE_EQ(engine.GetTransport().GetPosition(), 1.0);
  EXPECT_FALSE(instrument->IsNoteOn(kRootNote + 1.0));

  // Update timestamp past the start position, but before the note ends.
  engine.Update(1.5);
  EXPECT_DOUBLE_EQ(engine.GetTransport().GetPosition(), 1.5);
  EXPECT_TRUE(instrument->IsNoteOn(kRootNote + 1.0));

  // Start another note manually.
  instrument->StartNote(kRootNote, 1.0, 1.5);
  EXPECT_TRUE(instrument->IsNoteOn(kRootNote));

  // Stop playback, which should only stop the sequence note.
  engine.Stop();
  EXPECT_FALSE(instrument->IsNoteOn(kRootNote + 1.0));
  EXPECT_TRUE(instrument->IsNoteOn(kRootNote));

  // Destroy sequence, which should not affect the instrument.
  EXPECT_TRUE(engine.DestroySequence(kSequenceId));
  EXPECT_THAT(engine.GetSequence(kSequenceId), IsNull());
  EXPECT_TRUE(instrument->IsNoteOn(kRootNote));

  // Destroy instrument.
  EXPECT_TRUE(engine.DestroyInstrument(kInstrumentId));
  EXPECT_THAT(engine.GetInstrument(kInstrumentId), IsNull());
}

// Tests that engine starts and stops playback as expected.
TEST(EngineTest, StartStopPlayback) {
  Engine engine;
  EXPECT_EQ(engine.GetTransport().GetPosition(), 0.0);
  EXPECT_FALSE(engine.GetTransport().IsPlaying());

  engine.Update(10.0);
  EXPECT_EQ(engine.GetTransport().GetPosition(), 0.0);
  EXPECT_FALSE(engine.GetTransport().IsPlaying());

  engine.Start();
  EXPECT_EQ(engine.GetTransport().GetPosition(), 0.0);
  EXPECT_TRUE(engine.GetTransport().IsPlaying());

  engine.Update(20.0);
  EXPECT_EQ(engine.GetTransport().GetPosition(), 20.0);
  EXPECT_TRUE(engine.GetTransport().IsPlaying());

  engine.Stop();
  EXPECT_EQ(engine.GetTransport().GetPosition(), 20.0);
  EXPECT_FALSE(engine.GetTransport().IsPlaying());

  engine.Update(30.0);
  EXPECT_EQ(engine.GetTransport().GetPosition(), 20.0);
  EXPECT_FALSE(engine.GetTransport().IsPlaying());
}

}  // namespace
}  // namespace barelyapi
