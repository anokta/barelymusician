#include "dsp/instrument_processor.h"

#include <barelymusician.h>

#include <array>

#include "common/constants.h"
#include "common/rng.h"
#include "dsp/sample_data.h"
#include "dsp/voice_pool.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

constexpr int kSampleRate = 1;

constexpr int kVoiceCount = 4;
constexpr std::array<float, kVoiceCount> kSamples = {0.5f};
constexpr std::array<BarelySlice, kVoiceCount> kSlices = {
    BarelySlice{0.0, kSampleRate, kSamples.data(), kVoiceCount},
    BarelySlice{1.0, kSampleRate, kSamples.data(), kVoiceCount},
    BarelySlice{2.0, kSampleRate, kSamples.data(), kVoiceCount},
    BarelySlice{3.0, kSampleRate, kSamples.data(), kVoiceCount},
};
constexpr std::array<float, BarelyNoteControlType_kCount> kNoteControls = {1.0f, 0.0f};

// Tests that playing a single voice produces the expected output.
TEST(InstrumentProcessorTest, SingleVoice) {
  AudioRng rng;
  VoicePool voice_pool;
  InstrumentProcessor processor({}, {}, rng, voice_pool, kSampleRate);
  processor.SetControl(InstrumentControlType::kVoiceCount, kVoiceCount);
  processor.SetControl(InstrumentControlType::kSliceMode, static_cast<float>(SliceMode::kLoop));

  SampleData sample_data(kSlices);
  processor.SetSampleData(sample_data);

  std::array<float, kStereoChannelCount> delay;
  std::array<float, kStereoChannelCount> sidechain;
  std::array<float, kStereoChannelCount> output;

  output.fill(0.0f);
  voice_pool.Process<true>(delay.data(), sidechain.data(), output.data());
  for (int channel = 0; channel < kStereoChannelCount; ++channel) {
    EXPECT_FLOAT_EQ(output[channel], 0.0f);
  }

  processor.SetNoteOn(0.0f, kNoteControls);

  output.fill(0.0f);
  voice_pool.Process(delay.data(), sidechain.data(), output.data());
  for (int channel = 0; channel < kStereoChannelCount; ++channel) {
    EXPECT_FLOAT_EQ(output[channel], 0.5f * kSamples[0]);
  }

  processor.SetNoteOff(0.0f);

  output.fill(0.0f);
  voice_pool.Process(delay.data(), sidechain.data(), output.data());
  for (int channel = 0; channel < kStereoChannelCount; ++channel) {
    EXPECT_FLOAT_EQ(output[channel], 0.0f);
  }
}

// Tests that playing voices are capped at maximum allowed number of voices.
TEST(InstrumentProcessorTest, MaxVoices) {
  AudioRng rng;
  VoicePool voice_pool;
  InstrumentProcessor processor({}, {}, rng, voice_pool, kSampleRate);
  processor.SetControl(InstrumentControlType::kVoiceCount, kVoiceCount);
  processor.SetControl(InstrumentControlType::kSliceMode, static_cast<float>(SliceMode::kLoop));

  SampleData sample_data(kSlices);
  processor.SetSampleData(sample_data);

  std::array<float, kStereoChannelCount> delay;
  std::array<float, kStereoChannelCount> sidechain;
  std::array<float, kStereoChannelCount> output;

  output.fill(0.0f);
  voice_pool.Process(delay.data(), sidechain.data(), output.data());
  for (int channel = 0; channel < kStereoChannelCount; ++channel) {
    EXPECT_FLOAT_EQ(output[channel], 0.0f);
  }

  float expected_output = 0.0f;
  for (int i = 0; i < kVoiceCount; ++i) {
    processor.SetNoteOn(static_cast<float>(i), kNoteControls);

    expected_output += kSamples[i];

    output.fill(0.0f);
    voice_pool.Process(delay.data(), sidechain.data(), output.data());
    for (int channel = 0; channel < kStereoChannelCount; ++channel) {
      EXPECT_FLOAT_EQ(output[channel], 0.5f * expected_output) << i;
    }
  }

  for (int i = 0; i < kSampleRate; ++i) {
    processor.SetNoteOn(static_cast<float>(kVoiceCount), kNoteControls);

    output.fill(0.0f);
    voice_pool.Process(delay.data(), sidechain.data(), output.data());
    for (int channel = 0; channel < kStereoChannelCount; ++channel) {
      EXPECT_FLOAT_EQ(output[channel], 0.5f * expected_output) << i;
    }
  }
}

}  // namespace
}  // namespace barely
