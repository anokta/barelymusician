#include "engine/engine_processor.h"

#include <barelymusician.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>

#include "core/arena.h"
#include "core/constants.h"
#include "dsp/envelope.h"
#include "dsp/tone_filter.h"
#include "engine/cmd.h"
#include "engine/engine_state.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

constexpr uint32_t kInstrumentIndex = 1;
constexpr int kSampleRate = 1000;
constexpr int kSampleCount = 4;
constexpr std::array<double, kSampleCount> kSamples = {1.0, 2.0, 3.0, 4.0};

constexpr double kEpsilon = 1e-5;

TEST(EngineProcessorTest, PlayNote) {
  constexpr int kFrameCount = 5;
  constexpr double kPitch = 1.0;
  constexpr std::array<BarelySlice, 1> kSlices = {
      BarelySlice{kPitch, kSamples.data(), kSampleCount, kSampleRate},
  };

  const auto size = GetAllocSize<EngineState>(EngineConfig(kSampleRate));
  auto data = std::make_unique<std::byte[]>(size);
  Arena arena(data.get(), size);
  EngineState engine(arena, EngineConfig(kSampleRate));

  const uint32_t slice_index =
      engine.slice_pool.Acquire(kSlices.data(), static_cast<uint32_t>(kSlices.size()));

  EngineProcessor processor(engine);
  engine.ScheduleCmd(InstrumentCreateCmd{kInstrumentIndex});
  engine.ScheduleCmd(SampleDataCmd{kInstrumentIndex, slice_index});

  Envelope envelope;
  Envelope::Adsr adsr;
  adsr.SetRelease(kSampleRate, 0.0);

  engine.ScheduleCmd(
      InstrumentControlCmd{0.0, kInstrumentIndex, BarelyInstrumentControlType_kRelease});

  ToneFilter filters[kStereoChannelCount];
  ToneFilterParams filter_params;
  filter_params.SetCutoff(kSampleRate, 1.0);

  std::array<double, kStereoChannelCount * kFrameCount> samples;

  // Control is set to its default value.
  samples.fill(0.0);
  processor.Process(samples.data(), kStereoChannelCount, kFrameCount, 0.0);
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kStereoChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(samples[frame * kStereoChannelCount + channel], 0.0);
    }
  }

  // Set a note on.
  engine.ScheduleCmd(NoteOnCmd{kPitch, kInstrumentIndex});
  envelope.Start(adsr);

  samples.fill(0.0);
  processor.Process(samples.data(), kStereoChannelCount, kFrameCount, 0.0);
  for (int frame = 0; frame < kFrameCount; ++frame) {
    const double envelope_output = envelope.Next();
    for (int channel = 0; channel < kStereoChannelCount; ++channel) {
      EXPECT_NEAR(
          samples[frame * kStereoChannelCount + channel],
          (envelope_output > 0.0)
              ? (0.5 * filters[channel].Next(
                           (frame < kSampleCount) ? (envelope_output * kSamples[frame]) : 0.0,
                           filter_params))
              : 0.0,
          kEpsilon);
    }
  }

  // Set the note off.
  engine.ScheduleCmd(NoteOffCmd{kPitch, kInstrumentIndex});
  envelope.Stop();

  samples.fill(0.0);
  processor.Process(samples.data(), kStereoChannelCount, kFrameCount, 0.0);
  for (int frame = 0; frame < kFrameCount; ++frame) {
    const bool is_envelope_active = envelope.IsActive();
    if (is_envelope_active) {
      envelope.Next();
    }
    for (int channel = 0; channel < kStereoChannelCount; ++channel) {
      EXPECT_NEAR(samples[frame * kStereoChannelCount + channel],
                  is_envelope_active ? 0.5 * filters[channel].Next(0.0, filter_params) : 0.0,
                  kEpsilon);
    }
  }
}

}  // namespace
}  // namespace barely
