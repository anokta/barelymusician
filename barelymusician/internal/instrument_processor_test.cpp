#include "barelymusician/internal/instrument_processor.h"

#include <array>

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/sample_data.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

constexpr int kFrameRate = 1;
constexpr int kReferenceFrequency = 1;

constexpr int kVoiceCount = 4;
constexpr std::array<double, kVoiceCount> kSamples = {0.5};
constexpr std::array<SampleDataDefinition, kVoiceCount> kDefinitions = {
    SampleDataDefinition(0.0, kFrameRate, kSamples),
    SampleDataDefinition(1.0, kFrameRate, kSamples),
    SampleDataDefinition(2.0, kFrameRate, kSamples),
    SampleDataDefinition(3.0, kFrameRate, kSamples),
};

// Tests that playing a single voice produces the expected output.
TEST(InstrumentProcessorTest, SingleVoice) {
  InstrumentProcessor processor(kFrameRate, kReferenceFrequency);
  processor.SetControl(InstrumentControlType::kVoiceCount, kVoiceCount);
  processor.SetControl(InstrumentControlType::kSamplePlayerLoop, true);

  SampleData sample_data(kDefinitions);
  processor.SetSampleData(sample_data);

  double output = 0.0;
  processor.Process(&output, 1, 1);
  EXPECT_DOUBLE_EQ(output, 0.0);

  processor.SetNoteOn(0.0, 1.0);

  for (int i = 0; i < kFrameRate; ++i) {
    output = 0.0;
    processor.Process(&output, 1, 1);
    EXPECT_DOUBLE_EQ(output, kSamples[0]);
  }

  processor.SetNoteOff(0.0);

  output = 0.0;
  processor.Process(&output, 1, 1);
  EXPECT_DOUBLE_EQ(output, 0.0);
}

// Tests that playing voices are capped at maximum allowed number of voices.
TEST(InstrumentProcessorTest, MaxVoices) {
  InstrumentProcessor processor(kFrameRate, kReferenceFrequency);
  processor.SetControl(InstrumentControlType::kVoiceCount, kVoiceCount);
  processor.SetControl(InstrumentControlType::kSamplePlayerLoop, true);

  SampleData sample_data(kDefinitions);
  processor.SetSampleData(sample_data);

  double output = 0.0;
  processor.Process(&output, 1, 1);
  EXPECT_DOUBLE_EQ(output, 0.0);

  double expected_output = 0.0;
  for (int i = 0; i < kVoiceCount; ++i) {
    processor.SetNoteOn(static_cast<double>(i), 1.0);

    output = 0.0;
    processor.Process(&output, 1, 1);

    expected_output += kSamples[i];
    EXPECT_DOUBLE_EQ(output, expected_output) << i;
  }

  for (int i = 0; i < kFrameRate; ++i) {
    processor.SetNoteOn(static_cast<double>(kVoiceCount), 1.0);

    output = 0.0;
    processor.Process(&output, 1, 1);
    EXPECT_DOUBLE_EQ(output, expected_output);
  }
}

// Tests that the processor processor produces silence when there are no available voices set.
TEST(InstrumentProcessorTest, NoVoice) {
  InstrumentProcessor processor(kFrameRate, kReferenceFrequency);
  processor.SetControl(InstrumentControlType::kVoiceCount, 0);

  SampleData sample_data(kDefinitions);
  processor.SetSampleData(sample_data);

  double output = 0.0;
  processor.Process(&output, 1, 1);
  EXPECT_DOUBLE_EQ(output, 0.0);

  processor.SetNoteOn(0.0, 1.0);

  output = 0.0;
  processor.Process(&output, 1, 1);
  EXPECT_DOUBLE_EQ(output, 0.0);
}

}  // namespace
}  // namespace barely
