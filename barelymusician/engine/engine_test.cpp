#include "barelymusician/engine/engine.h"

#include <algorithm>
#include <vector>

#include "barelymusician/engine/id.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/engine/parameter.h"
#include "barelymusician/engine/performer.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

using ::testing::AllOf;
using ::testing::ElementsAre;
using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::Pointee;
using ::testing::UnorderedElementsAre;

constexpr int kFrameRate = 48000;
constexpr int kChannelCount = 2;
constexpr int kFrameCount = 8;

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
      [](void** state, double* output_samples, int output_channel_count,
         int output_frame_count) {
        std::fill_n(output_samples, output_channel_count * output_frame_count,
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
  std::vector<double> buffer(kChannelCount * kFrameCount);
  EXPECT_THAT(engine.GetInstrument(kId), IsNull());

  // Create instrument.
  EXPECT_TRUE(
      engine.CreateInstrument(kId, GetTestInstrumentDefinition(), kFrameRate));

  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(engine.ProcessInstrument(kId, buffer.data(), kChannelCount,
                                       kFrameCount, 0.0));
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 0.0);
    }
  }

  // Get instrument.
  auto* instrument = engine.GetInstrument(kId);
  EXPECT_THAT(instrument, NotNull());

  // Set note callbacks.
  double note_on_pitch = 0.0;
  double note_on_intensity = 0.0;
  instrument->SetNoteOnCallback([&](double pitch, double intensity) {
    note_on_pitch = pitch;
    note_on_intensity = intensity;
  });
  EXPECT_DOUBLE_EQ(note_on_pitch, 0.0);
  EXPECT_DOUBLE_EQ(note_on_intensity, 0.0);

  double note_off_pitch = 0.0;
  instrument->SetNoteOffCallback([&](double pitch) { note_off_pitch = pitch; });
  EXPECT_DOUBLE_EQ(note_off_pitch, 0.0);

  // Start note.
  instrument->StartNote(kPitch, kIntensity, 0.0);
  EXPECT_TRUE(instrument->IsNoteOn(kPitch));

  EXPECT_DOUBLE_EQ(note_on_pitch, kPitch);
  EXPECT_DOUBLE_EQ(note_on_intensity, kIntensity);

  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(engine.ProcessInstrument(kId, buffer.data(), kChannelCount,
                                       kFrameCount, 0.0));
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel],
                       kPitch * kIntensity);
    }
  }

  // Update timestamp.
  engine.Update(5.0);

  // Destroy instrument, which should also trigger note off callback.
  note_off_pitch = 0.0;
  EXPECT_TRUE(engine.DestroyInstrument(kId));
  EXPECT_THAT(engine.GetInstrument(kId), IsNull());
  EXPECT_DOUBLE_EQ(note_off_pitch, kPitch);

  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_FALSE(engine.ProcessInstrument(kId, buffer.data(), kChannelCount,
                                        kFrameCount, 0.0));
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 0.0);
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
          ->SetNoteOffCallback(
              [&](double pitch) { note_off_pitches.push_back(pitch); });
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

// Tests that single performer is created and destroyed as expected.
TEST(EngineTest, CreateDestroySinglePerformer) {
  const Id kPerformerId = 1;
  const Id kEventId = 2;
  const int kPriority = 0;

  Engine engine;
  EXPECT_THAT(engine.GetPerformer(kPerformerId), IsNull());

  // Create performer.
  EXPECT_TRUE(engine.CreatePerformer(kPerformerId, kPriority));
  auto* performer = engine.GetPerformer(kPerformerId);
  EXPECT_THAT(performer, NotNull());

  // Add event.
  double event_position = 0.0;
  EXPECT_TRUE(performer->AddEvent(
      kEventId, 1.0, [&]() { event_position = performer->GetPosition(); }));
  EXPECT_DOUBLE_EQ(event_position, 0.0);

  // Start playback with one beat per second tempo.
  engine.SetTempo(60.0);
  EXPECT_DOUBLE_EQ(engine.GetTempo(), 60.0);

  EXPECT_FALSE(performer->IsPlaying());
  performer->Start();
  EXPECT_TRUE(performer->IsPlaying());

  // Update timestamp just before the event, which should not be triggered yet.
  EXPECT_DOUBLE_EQ(performer->GetDurationToNextEvent(), 1.0);
  engine.Update(1.0);
  EXPECT_DOUBLE_EQ(performer->GetDurationToNextEvent(), 0.0);
  EXPECT_DOUBLE_EQ(performer->GetPosition(), 1.0);
  EXPECT_DOUBLE_EQ(event_position, 0.0);

  // Update timestamp past the event, which should be triggered now.
  EXPECT_DOUBLE_EQ(performer->GetDurationToNextEvent(), 0.0);
  engine.Update(1.5);
  EXPECT_DOUBLE_EQ(performer->GetDurationToNextEvent(),
                   std::numeric_limits<double>::max());
  EXPECT_DOUBLE_EQ(performer->GetPosition(), 1.5);
  EXPECT_DOUBLE_EQ(event_position, 1.0);

  // Destroy performer.
  EXPECT_TRUE(engine.DestroyPerformer(kPerformerId));
  EXPECT_THAT(engine.GetPerformer(kPerformerId), IsNull());
}

// TODO(#108): Add `CreateDestroyMultiplePerformers` using differing priorities.

// Tests that engine returns beats and seconds as expected.
TEST(EngineTest, GetBeatsSeconds) {
  Engine engine;
  EXPECT_DOUBLE_EQ(engine.GetTempo(), 120.0);

  EXPECT_DOUBLE_EQ(engine.GetBeats(0.0), 0.0);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(0.0), 0.0);
  EXPECT_DOUBLE_EQ(engine.GetBeats(engine.GetSeconds(0.0)), 0.0);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(engine.GetBeats(0.0)), 0.0);

  EXPECT_DOUBLE_EQ(engine.GetBeats(1.0), 2.0);
  EXPECT_DOUBLE_EQ(engine.GetBeats(engine.GetSeconds(1.0)), 1.0);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(1.0), 0.5);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(engine.GetBeats(1.0)), 1.0);

  EXPECT_DOUBLE_EQ(engine.GetBeats(-1.0), -2.0);
  EXPECT_DOUBLE_EQ(engine.GetBeats(engine.GetSeconds(-1.0)), -1.0);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(-1.0), -0.5);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(engine.GetBeats(-1.0)), -1.0);

  EXPECT_DOUBLE_EQ(engine.GetBeats(2.5), 5.0);
  EXPECT_DOUBLE_EQ(engine.GetBeats(engine.GetSeconds(2.5)), 2.5);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(2.5), 1.25);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(engine.GetBeats(2.5)), 2.5);

  EXPECT_DOUBLE_EQ(engine.GetBeats(10.0), 20.0);
  EXPECT_DOUBLE_EQ(engine.GetBeats(engine.GetSeconds(10.0)), 10.0);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(10.0), 5.0);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(engine.GetBeats(10.0)), 10.0);

  // Increase tempo.
  engine.SetTempo(150.0);

  EXPECT_DOUBLE_EQ(engine.GetBeats(0.0), 0.0);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(0.0), 0.0);
  EXPECT_DOUBLE_EQ(engine.GetBeats(engine.GetSeconds(0.0)), 0.0);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(engine.GetBeats(0.0)), 0.0);

  EXPECT_DOUBLE_EQ(engine.GetBeats(1.0), 2.5);
  EXPECT_DOUBLE_EQ(engine.GetBeats(engine.GetSeconds(1.0)), 1.0);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(1.0), 0.4);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(engine.GetBeats(1.0)), 1.0);

  EXPECT_DOUBLE_EQ(engine.GetBeats(-1.0), -2.5);
  EXPECT_DOUBLE_EQ(engine.GetBeats(engine.GetSeconds(-1.0)), -1.0);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(-1.0), -0.4);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(engine.GetBeats(-1.0)), -1.0);

  EXPECT_DOUBLE_EQ(engine.GetBeats(2.5), 6.25);
  EXPECT_DOUBLE_EQ(engine.GetBeats(engine.GetSeconds(2.5)), 2.5);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(2.5), 1.0);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(engine.GetBeats(2.5)), 2.5);

  EXPECT_DOUBLE_EQ(engine.GetBeats(10.0), 25.0);
  EXPECT_DOUBLE_EQ(engine.GetBeats(engine.GetSeconds(10.0)), 10.0);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(10.0), 4.0);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(engine.GetBeats(10.0)), 10.0);

  // Decrease tempo.
  engine.SetTempo(60.0);

  EXPECT_DOUBLE_EQ(engine.GetBeats(0.0), 0.0);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(0.0), 0.0);
  EXPECT_DOUBLE_EQ(engine.GetBeats(engine.GetSeconds(0.0)), 0.0);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(engine.GetBeats(0.0)), 0.0);

  EXPECT_DOUBLE_EQ(engine.GetBeats(1.0), 1.0);
  EXPECT_DOUBLE_EQ(engine.GetBeats(engine.GetSeconds(1.0)), 1.0);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(1.0), 1.0);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(engine.GetBeats(1.0)), 1.0);

  EXPECT_DOUBLE_EQ(engine.GetBeats(-1.0), -1.0);
  EXPECT_DOUBLE_EQ(engine.GetBeats(engine.GetSeconds(-1.0)), -1.0);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(-1.0), -1.0);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(engine.GetBeats(-1.0)), -1.0);

  EXPECT_DOUBLE_EQ(engine.GetBeats(2.5), 2.5);
  EXPECT_DOUBLE_EQ(engine.GetBeats(engine.GetSeconds(2.5)), 2.5);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(2.5), 2.5);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(engine.GetBeats(2.5)), 2.5);

  EXPECT_DOUBLE_EQ(engine.GetBeats(10.0), 10.0);
  EXPECT_DOUBLE_EQ(engine.GetBeats(engine.GetSeconds(10.0)), 10.0);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(10.0), 10.0);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(engine.GetBeats(10.0)), 10.0);

  // Set tempo to zero.
  engine.SetTempo(0.0);

  EXPECT_DOUBLE_EQ(engine.GetBeats(0.0), 0.0);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(0.0), 0.0);
  EXPECT_DOUBLE_EQ(engine.GetBeats(engine.GetSeconds(0.0)), 0.0);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(engine.GetBeats(0.0)), 0.0);

  EXPECT_DOUBLE_EQ(engine.GetBeats(1.0), 0.0);
  EXPECT_DOUBLE_EQ(engine.GetBeats(engine.GetSeconds(1.0)), 0.0);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(1.0), std::numeric_limits<double>::max());
  EXPECT_DOUBLE_EQ(engine.GetSeconds(engine.GetBeats(1.0)), 0.0);

  EXPECT_DOUBLE_EQ(engine.GetBeats(-1.0), 0.0);
  EXPECT_DOUBLE_EQ(engine.GetBeats(engine.GetSeconds(-1.0)), 0.0);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(-1.0),
                   std::numeric_limits<double>::lowest());
  EXPECT_DOUBLE_EQ(engine.GetSeconds(engine.GetBeats(-1.0)), 0.0);

  EXPECT_DOUBLE_EQ(engine.GetBeats(2.5), 0.0);
  EXPECT_DOUBLE_EQ(engine.GetBeats(engine.GetSeconds(2.5)), 0.0);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(2.5), std::numeric_limits<double>::max());
  EXPECT_DOUBLE_EQ(engine.GetSeconds(engine.GetBeats(2.5)), 0.0);

  EXPECT_DOUBLE_EQ(engine.GetBeats(10.0), 0.0);
  EXPECT_DOUBLE_EQ(engine.GetBeats(engine.GetSeconds(10.0)), 0.0);
  EXPECT_DOUBLE_EQ(engine.GetSeconds(10.0), std::numeric_limits<double>::max());
  EXPECT_DOUBLE_EQ(engine.GetSeconds(engine.GetBeats(10.0)), 0.0);
}

// Tests that engine sets its tempo as expected.
TEST(EngineTest, SetTempo) {
  Engine engine;
  EXPECT_DOUBLE_EQ(engine.GetTempo(), 120.0);

  engine.SetTempo(200.0);
  EXPECT_DOUBLE_EQ(engine.GetTempo(), 200.0);

  engine.SetTempo(0.0);
  EXPECT_DOUBLE_EQ(engine.GetTempo(), 0.0);
}

}  // namespace
}  // namespace barely::internal
