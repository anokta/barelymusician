#include "engine/engine_processor.h"

#include <barelymusician.h>

#include <array>
#include <cstdint>
#include <memory>

#include "core/constants.h"
#include "engine/engine_state.h"
#include "engine/instrument_state.h"
#include "engine/message.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

constexpr uint32_t kInstrumentIndex = 1;
constexpr uint32_t kNoteIndex = 2;
constexpr int kSampleRate = 4;
constexpr std::array<float, kSampleRate> kSamples = {1.0f, 2.0f, 3.0f, 4.0f};

// Tests that an instrument plays a single note as expected.
TEST(EngineProcessorTest, PlaySingleNote) {
  constexpr int kFrameCount = 5;
  constexpr float kPitch = 1.0f;
  constexpr std::array<BarelySlice, 1> kSlices = {
      BarelySlice{kSamples.data(), kSampleRate, kSampleRate, kPitch},
  };

  auto engine = std::make_unique<EngineState>();
  engine->sample_rate = static_cast<float>(kSampleRate);

  const uint32_t slice_index =
      engine->slice_pool.Acquire(kSlices.data(), static_cast<uint32_t>(kSlices.size()));

  EngineProcessor processor(*engine);
  const auto controls = BuildControlArray(nullptr, 0);
  for (int i = 0; i < BarelyInstrumentControlType_kCount; ++i) {
    engine->ScheduleMessage(InstrumentControlMessage{
        kInstrumentIndex, static_cast<BarelyInstrumentControlType>(i), controls[i].value});
  }
  engine->ScheduleMessage(SampleDataMessage{kInstrumentIndex, slice_index});

  std::array<float, kStereoChannelCount * kFrameCount> samples;

  // Control is set to its default value.
  samples.fill(0.0f);
  processor.Process(samples.data(), kStereoChannelCount, kFrameCount, 0.0);
  for (int i = 0; i < kStereoChannelCount * kFrameCount; ++i) {
    EXPECT_FLOAT_EQ(samples[i], 0.0f);
  }

  // Set a note on.
  engine->ScheduleMessage(NoteOnMessage{kNoteIndex, kInstrumentIndex, kPitch, {1.0f, 0.0f}});

  samples.fill(0.0f);
  processor.Process(samples.data(), kStereoChannelCount, kFrameCount, 0.0);
  for (int i = 0; i < kStereoChannelCount * kFrameCount; ++i) {
    EXPECT_FLOAT_EQ(samples[i], (i / kStereoChannelCount < kSampleRate)
                                    ? 0.5f * kSamples[i / kStereoChannelCount]
                                    : 0.0f);
  }

  // Set the note off.
  engine->ScheduleMessage(NoteOffMessage{kNoteIndex});

  samples.fill(0.0f);
  processor.Process(samples.data(), kStereoChannelCount, kFrameCount, 0.0);
  for (int i = 0; i < kStereoChannelCount * kFrameCount; ++i) {
    EXPECT_FLOAT_EQ(samples[i], 0.0f);
  }
}

// Tests that an instrument plays multiple notes as expected.
TEST(EngineProcessorTest, PlayMultipleNotes) {
  constexpr std::array<BarelySlice, kSampleRate> kSlices = {
      BarelySlice{kSamples.data(), 1, kSampleRate, 0.0f},
      BarelySlice{kSamples.data() + 1, 1, kSampleRate, 1.0f},
      BarelySlice{kSamples.data() + 2, 1, kSampleRate, 2.0f},
      BarelySlice{kSamples.data() + 3, 1, kSampleRate, 3.0f},
  };

  auto engine = std::make_unique<EngineState>();
  engine->sample_rate = static_cast<float>(kSampleRate);

  const uint32_t slice_index =
      engine->slice_pool.Acquire(kSlices.data(), static_cast<uint32_t>(kSlices.size()));

  EngineProcessor processor(*engine);
  engine->ScheduleMessage(InstrumentCreateMessage{kInstrumentIndex});
  const auto controls = BuildControlArray(nullptr, 0);
  for (int i = 0; i < BarelyInstrumentControlType_kCount; ++i) {
    engine->ScheduleMessage(InstrumentControlMessage{
        kInstrumentIndex, static_cast<BarelyInstrumentControlType>(i), controls[i].value});
  }
  engine->ScheduleMessage(SampleDataMessage{kInstrumentIndex, slice_index});

  std::array<float, kStereoChannelCount * kSampleRate> samples;

  // Control is set to its default value.
  samples.fill(0.0f);
  processor.Process(samples.data(), kStereoChannelCount, kSampleRate, 0.0);
  for (int i = 0; i < kStereoChannelCount * kSampleRate; ++i) {
    EXPECT_FLOAT_EQ(samples[i], 0.0f);
  }

  // Start a new note per each i in the samples.
  for (int i = 0; i < kSampleRate; ++i) {
    engine->ScheduleMessage(
        NoteOnMessage{kNoteIndex, kInstrumentIndex, static_cast<float>(i), {1.0f, 0.0f}});
    engine->timestamp = static_cast<double>(i + 1) / static_cast<double>(kSampleRate);
    engine->ScheduleMessage(NoteOffMessage{kNoteIndex});
  }

  samples.fill(0.0f);
  processor.Process(samples.data(), kStereoChannelCount, kSampleRate, 0.0);
  for (int i = 0; i < kStereoChannelCount * kSampleRate; ++i) {
    EXPECT_FLOAT_EQ(samples[i], 0.5f * kSamples[i / kStereoChannelCount]);
  }

  samples.fill(0.0f);
  processor.Process(samples.data(), kStereoChannelCount, kSampleRate, engine->timestamp);
  for (int i = 0; i < kStereoChannelCount * kSampleRate; ++i) {
    EXPECT_FLOAT_EQ(samples[i], 0.0f);
  }
}

}  // namespace
}  // namespace barely
