#include "internal/instrument_processor.h"

#include <array>

#include "barelymusician.h"
#include "gtest/gtest.h"
#include "internal/sample_data.h"

namespace barely::internal {
namespace {

constexpr int kSampleRate = 1;
constexpr int kReferenceFrequency = 1;

constexpr int kVoiceCount = 4;
constexpr std::array<double, kVoiceCount> kSamples = {0.5};
constexpr std::array<SampleDataSlice, kVoiceCount> kSlices = {
    SampleDataSlice(0.0, kSampleRate, kSamples),
    SampleDataSlice(1.0, kSampleRate, kSamples),
    SampleDataSlice(2.0, kSampleRate, kSamples),
    SampleDataSlice(3.0, kSampleRate, kSamples),
};

// Tests that playing a single voice produces the expected output.
TEST(InstrumentProcessorTest, SingleVoice) {
  InstrumentProcessor processor(kSampleRate, kReferenceFrequency);
  processor.SetControl(ControlType::kVoiceCount, kVoiceCount);
  processor.SetControl(ControlType::kSamplePlaybackMode,
                       static_cast<double>(SamplePlaybackMode::kLoop));

  SampleData sample_data(kSlices);
  processor.SetSampleData(sample_data);

  double output = 0.0;
  processor.Process(&output, 1);
  EXPECT_DOUBLE_EQ(output, 0.0);

  processor.SetNoteOn(0.0, 1.0);

  for (int i = 0; i < kSampleRate; ++i) {
    output = 0.0;
    processor.Process(&output, 1);
    EXPECT_DOUBLE_EQ(output, kSamples[0]);
  }

  processor.SetNoteOff(0.0);

  output = 0.0;
  processor.Process(&output, 1);
  EXPECT_DOUBLE_EQ(output, 0.0);
}

// Tests that playing voices are capped at maximum allowed number of voices.
TEST(InstrumentProcessorTest, MaxVoices) {
  InstrumentProcessor processor(kSampleRate, kReferenceFrequency);
  processor.SetControl(ControlType::kVoiceCount, kVoiceCount);
  processor.SetControl(ControlType::kSamplePlaybackMode,
                       static_cast<double>(SamplePlaybackMode::kLoop));

  SampleData sample_data(kSlices);
  processor.SetSampleData(sample_data);

  double output = 0.0;
  processor.Process(&output, 1);
  EXPECT_DOUBLE_EQ(output, 0.0);

  double expected_output = 0.0;
  for (int i = 0; i < kVoiceCount; ++i) {
    processor.SetNoteOn(static_cast<double>(i), 1.0);

    output = 0.0;
    processor.Process(&output, 1);

    expected_output += kSamples[i];
    EXPECT_DOUBLE_EQ(output, expected_output) << i;
  }

  for (int i = 0; i < kSampleRate; ++i) {
    processor.SetNoteOn(static_cast<double>(kVoiceCount), 1.0);

    output = 0.0;
    processor.Process(&output, 1);
    EXPECT_DOUBLE_EQ(output, expected_output);
  }
}

// Tests that the processor processor produces silence when there are no available voices set.
TEST(InstrumentProcessorTest, NoVoice) {
  InstrumentProcessor processor(kSampleRate, kReferenceFrequency);
  processor.SetControl(ControlType::kVoiceCount, 0);

  SampleData sample_data(kSlices);
  processor.SetSampleData(sample_data);

  double output = 0.0;
  processor.Process(&output, 1);
  EXPECT_DOUBLE_EQ(output, 0.0);

  processor.SetNoteOn(0.0, 1.0);

  output = 0.0;
  processor.Process(&output, 1);
  EXPECT_DOUBLE_EQ(output, 0.0);
}

}  // namespace
}  // namespace barely::internal
