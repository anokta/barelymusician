#include "barelymusician/engine/engine.h"

#include <algorithm>
#include <vector>

#include "barelymusician/engine/control.h"
#include "barelymusician/engine/id.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/engine/number.h"
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

constexpr Integer kFrameRate = 48000;
constexpr Integer kChannelCount = 2;
constexpr Integer kFrameCount = 8;

// Returns test instrument definition that produces constant output that is set.
InstrumentDefinition GetTestInstrumentDefinition() {
  static const std::vector<ControlDefinition> control_definitions = {
      ControlDefinition{0.0, -10.0, 10.0},
  };
  return InstrumentDefinition(
      [](void** state, Integer /*frame_rate*/) {
        *state = reinterpret_cast<void*>(new Real{0.0});
      },
      [](void** state) { delete static_cast<Real*>(*state); },
      [](void** state, Real* output_samples, Integer output_channel_count,
         Integer output_frame_count) {
        std::fill_n(output_samples, output_channel_count * output_frame_count,
                    *reinterpret_cast<Real*>(*state));
      },
      [](void** state, Integer index, Real value, Real /*slope_per_frame*/) {
        *reinterpret_cast<Real*>(*state) = static_cast<Real>(index + 1) * value;
      },
      [](void** /*state*/, const void* /*data*/, Integer /*size*/) {},
      [](void** state, Real /*pitch*/) {
        *reinterpret_cast<Real*>(*state) = 0.0;
      },
      [](void** state, Real pitch, Real intensity) {
        *reinterpret_cast<Real*>(*state) = pitch * intensity;
      },
      control_definitions);
}

// Tests that single instrument is created and destroyed as expected.
TEST(EngineTest, CreateDestroySingleInstrument) {
  const Id kId = 1;
  const Real kPitch = -1.25;
  const Real kIntensity = 0.75;

  Engine engine;
  std::vector<Real> buffer(kChannelCount * kFrameCount);
  EXPECT_THAT(engine.GetInstrument(kId), IsNull());

  // Create instrument.
  EXPECT_TRUE(
      engine.CreateInstrument(kId, GetTestInstrumentDefinition(), kFrameRate));

  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(engine.ProcessInstrument(kId, buffer.data(), kChannelCount,
                                       kFrameCount, 0.0));
  for (Integer frame = 0; frame < kFrameCount; ++frame) {
    for (Integer channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 0.0);
    }
  }

  // Get instrument.
  auto* instrument = engine.GetInstrument(kId);
  EXPECT_THAT(instrument, NotNull());

  // Set note callbacks.
  Real note_on_pitch = 0.0;
  Real note_on_intensity = 0.0;
  instrument->SetNoteOnEventCallback([&](Real pitch, Real intensity) {
    note_on_pitch = pitch;
    note_on_intensity = intensity;
  });
  EXPECT_DOUBLE_EQ(note_on_pitch, 0.0);
  EXPECT_DOUBLE_EQ(note_on_intensity, 0.0);

  Real note_off_pitch = 0.0;
  instrument->SetNoteOffEventCallback(
      [&](Real pitch) { note_off_pitch = pitch; });
  EXPECT_DOUBLE_EQ(note_off_pitch, 0.0);

  // Start note.
  instrument->SetNoteOn(kPitch, kIntensity, 0.0);
  EXPECT_TRUE(instrument->IsNoteOn(kPitch));

  EXPECT_DOUBLE_EQ(note_on_pitch, kPitch);
  EXPECT_DOUBLE_EQ(note_on_intensity, kIntensity);

  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(engine.ProcessInstrument(kId, buffer.data(), kChannelCount,
                                       kFrameCount, 0.0));
  for (Integer frame = 0; frame < kFrameCount; ++frame) {
    for (Integer channel = 0; channel < kChannelCount; ++channel) {
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
  for (Integer frame = 0; frame < kFrameCount; ++frame) {
    for (Integer channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 0.0);
    }
  }
}

// Tests that multiple instruments are created and destroyed as expected.
TEST(EngineTest, CreateDestroyMultipleInstruments) {
  std::vector<Real> note_off_pitches;

  {
    Engine engine;

    // Create instruments with note off callback.
    for (Integer i = 0; i < 3; ++i) {
      const Id instrument_id = i + 1;
      EXPECT_THAT(engine.GetInstrument(instrument_id), IsNull());
      EXPECT_TRUE(engine.CreateInstrument(
          instrument_id, GetTestInstrumentDefinition(), kFrameRate));
      EXPECT_THAT(engine.GetInstrument(instrument_id), NotNull());
      engine.GetInstrument(instrument_id)
          ->SetNoteOffEventCallback(
              [&](Real pitch) { note_off_pitches.push_back(pitch); });
    }

    // Start multiple notes, then immediately stop some of them.
    for (Integer i = 0; i < 3; ++i) {
      engine.GetInstrument(i + 1)->SetNoteOn(static_cast<Real>(i + 1), 1.0,
                                             0.0);
      engine.GetInstrument(i + 1)->SetNoteOn(static_cast<Real>(-i - 1), 1.0,
                                             0.0);
      engine.GetInstrument(i + 1)->SetNoteOff(static_cast<Real>(i + 1), 0.0);
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
  const Id kTaskId = 2;
  const Integer kPriority = 0;

  Engine engine;
  EXPECT_THAT(engine.GetPerformer(kPerformerId), IsNull());

  // Create performer.
  EXPECT_TRUE(engine.CreatePerformer(kPerformerId, kPriority));
  auto* performer = engine.GetPerformer(kPerformerId);
  EXPECT_THAT(performer, NotNull());

  // Add task.
  Real task_position = 0.0;
  EXPECT_TRUE(performer->AddTask(
      kTaskId, 1.0, [&]() { task_position = performer->GetPosition(); }));
  EXPECT_DOUBLE_EQ(task_position, 0.0);

  // Start playback with one beat per second tempo.
  engine.SetTempo(60.0);
  EXPECT_DOUBLE_EQ(engine.GetTempo(), 60.0);

  EXPECT_FALSE(performer->IsPlaying());
  performer->Start();
  EXPECT_TRUE(performer->IsPlaying());

  // Update timestamp just before the task, which should not be triggered yet.
  EXPECT_DOUBLE_EQ(performer->GetDurationToNextTask(), 1.0);
  engine.Update(1.0);
  EXPECT_DOUBLE_EQ(performer->GetDurationToNextTask(), 0.0);
  EXPECT_DOUBLE_EQ(performer->GetPosition(), 1.0);
  EXPECT_DOUBLE_EQ(task_position, 0.0);

  // Update timestamp past the task, which should be triggered now.
  EXPECT_DOUBLE_EQ(performer->GetDurationToNextTask(), 0.0);
  engine.Update(1.5);
  EXPECT_DOUBLE_EQ(performer->GetDurationToNextTask(),
                   std::numeric_limits<Real>::max());
  EXPECT_DOUBLE_EQ(performer->GetPosition(), 1.5);
  EXPECT_DOUBLE_EQ(task_position, 1.0);

  // Destroy performer.
  EXPECT_TRUE(engine.DestroyPerformer(kPerformerId));
  EXPECT_THAT(engine.GetPerformer(kPerformerId), IsNull());
}

// TODO(#108): Add `CreateDestroyMultiplePerformers` using differing priorities.

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
