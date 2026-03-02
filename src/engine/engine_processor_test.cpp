#include "engine/engine_processor.h"

#include <barelymusician.h>

#include <array>
#include <cstdint>
#include <memory>

#include "core/constants.h"
#include "core/control.h"
#include "dsp/envelope.h"
#include "dsp/tone_filter.h"
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

TEST(EngineProcessorTest, PlayNote) {
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
  engine->ScheduleMessage(InstrumentCreateMessage{kInstrumentIndex});
  engine->ScheduleMessage(SampleDataMessage{kInstrumentIndex, slice_index});

  Envelope envelope;
  Envelope::Adsr adsr;
  adsr.SetAttack(kSampleRate, 0.0f);
  adsr.SetRelease(kSampleRate, 0.0f);

  ToneFilter filters[kStereoChannelCount];
  ToneFilterParams filter_params;
  filter_params.SetCutoff(kSampleRate, 1.0f);

  std::array<float, kStereoChannelCount * kFrameCount> samples;

  // Control is set to its default value.
  samples.fill(0.0f);
  processor.Process(samples.data(), kStereoChannelCount, kFrameCount, 0.0);
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kStereoChannelCount; ++channel) {
      EXPECT_FLOAT_EQ(samples[frame * kStereoChannelCount + channel], 0.0f);
    }
  }

  // Set a note on.
  engine->ScheduleMessage(NoteOnMessage{kNoteIndex, kInstrumentIndex, kPitch});
  envelope.Start(adsr);

  samples.fill(0.0f);
  processor.Process(samples.data(), kStereoChannelCount, kFrameCount, 0.0);
  for (int frame = 0; frame < kFrameCount; ++frame) {
    const float envelope_output = envelope.IsActive() ? envelope.Next() : 0.0f;
    for (int channel = 0; channel < kStereoChannelCount; ++channel) {
      EXPECT_FLOAT_EQ(
          samples[frame * kStereoChannelCount + channel],
          (envelope_output > 0.0f)
              ? (0.5f * filters[channel].Next(
                            (frame < kSampleRate) ? (envelope_output * kSamples[frame]) : 0.0f,
                            filter_params))
              : 0.0f);
    }
  }

  // Set the note off.
  engine->ScheduleMessage(NoteOffMessage{kNoteIndex});
  envelope.Stop();

  samples.fill(0.0f);
  processor.Process(samples.data(), kStereoChannelCount, kFrameCount, 0.0);
  for (int frame = 0; frame < kFrameCount; ++frame) {
    const float envelope_output = envelope.IsActive() ? envelope.Next() : 0.0f;
    for (int channel = 0; channel < kStereoChannelCount; ++channel) {
      EXPECT_FLOAT_EQ(
          samples[frame * kStereoChannelCount + channel],
          (envelope_output > 0.0f) ? (0.5f * filters[channel].Next(0.0f, filter_params)) : 0.0f);
    }
  }
}

}  // namespace
}  // namespace barely
