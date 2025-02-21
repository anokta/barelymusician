#include <array>

#include "barelymusician.h"
#include "benchmark/benchmark.h"

namespace barely {
namespace {

using ::benchmark::State;

constexpr int kSampleRate = 48000;
constexpr int kSampleCount = 1024;

void BM_BarelyEngine_AddRemoveInstrument(State& state) {
  Engine engine(kSampleRate);

  for (auto _ : state) {
    [[maybe_unused]] const auto instrument = engine.CreateInstrument();
  }
}
BENCHMARK(BM_BarelyEngine_AddRemoveInstrument);

void BM_BarelyEngine_AddRemovePerformer(State& state) {
  Engine engine(kSampleRate);

  for (auto _ : state) {
    [[maybe_unused]] const auto performer = engine.CreatePerformer();
  }
}
BENCHMARK(BM_BarelyEngine_AddRemovePerformer);

void BM_BarelyInstrument_Process_Empty(State& state) {
  Engine engine(kSampleRate);

  auto instrument = engine.CreateInstrument();

  std::array<float, kSampleCount> output_samples;

  for (auto _ : state) {
    instrument.Process(output_samples, 0.0);
  }
}
BENCHMARK(BM_BarelyInstrument_Process_Empty);

void BM_BarelyInstrument_Process_SingleNoteWithLoopingSample(State& state) {
  constexpr std::array<float, 5> kSamples = {-0.5f, -0.25f, 0.0f, 0.25f, 1.0f};
  const std::array<SampleDataSlice, 1> kSlices = {SampleDataSlice(0.0, kSampleRate, kSamples)};

  Engine engine(kSampleRate);

  auto instrument = engine.CreateInstrument();
  instrument.SetControl(ControlType::kSamplePlaybackMode, SamplePlaybackMode::kLoop);
  instrument.SetSampleData(kSlices);
  instrument.SetNoteOn(1.0);

  std::array<float, kSampleCount> output_samples;

  for (auto _ : state) {
    instrument.Process(output_samples, 0.0);
  }
}
BENCHMARK(BM_BarelyInstrument_Process_SingleNoteWithLoopingSample);

void BM_BarelyInstrument_Process_SingleNoteWithSineOsc(State& state) {
  Engine engine(kSampleRate);

  auto instrument = engine.CreateInstrument();
  instrument.SetControl(ControlType::kOscShape, 0.0f);
  instrument.SetNoteOn(0.0);

  std::array<float, kSampleCount> output_samples;

  for (auto _ : state) {
    instrument.Process(output_samples, 0.0);
  }
}
BENCHMARK(BM_BarelyInstrument_Process_SingleNoteWithSineOsc);

void BM_BarelyInstrument_Process_MultipleNotesWithSineOsc(State& state) {
  Engine engine(kSampleRate);

  auto instrument = engine.CreateInstrument();
  instrument.SetControl(ControlType::kOscShape, 0.0f);

  const int voice_count = instrument.GetControl<int>(ControlType::kVoiceCount);
  for (int i = 0; i < voice_count; ++i) {
    instrument.SetNoteOn(static_cast<float>(i));
  }

  std::array<float, kSampleCount> output_samples;

  for (auto _ : state) {
    instrument.Process(output_samples, 0.0);
  }
}
BENCHMARK(BM_BarelyInstrument_Process_MultipleNotesWithSineOsc);

void BM_BarelyInstrument_Process_FrequentUpdates(State& state) {
  Engine engine(kSampleRate);

  auto instrument = engine.CreateInstrument();
  instrument.SetControl(ControlType::kOscShape, 0.0f);

  std::array<float, kSampleCount> output_samples;
  double timestamp = 0.0;

  constexpr int kUpdateCount = 20;
  constexpr double kTimestampIncrement =
      static_cast<double>(kSampleCount) / static_cast<double>(kSampleRate);

  for (auto _ : state) {
    state.PauseTiming();
    for (int i = 0; i < kUpdateCount; ++i) {
      engine.Update(timestamp);
      instrument.SetControl(ControlType::kAttack, 0.001f * static_cast<float>(i));
      const float pitch = static_cast<float>(i) / static_cast<float>(kUpdateCount);
      instrument.SetNoteOn(pitch);
      instrument.SetNoteControl(pitch, NoteControlType::kPitchShift, static_cast<float>(i));
      engine.Update(timestamp + kTimestampIncrement * static_cast<double>(i) /
                                    static_cast<double>(kUpdateCount));
      instrument.SetNoteOff(pitch);
      instrument.SetControl(ControlType::kAttack, 0.01f * static_cast<float>(i));
    }
    state.ResumeTiming();
    instrument.Process(output_samples, timestamp);
    timestamp += kTimestampIncrement;
  }
}
BENCHMARK(BM_BarelyInstrument_Process_FrequentUpdates);

void BM_BarelyInstrument_SetControl(State& state) {
  Engine engine(kSampleRate);

  auto instrument = engine.CreateInstrument();
  int i = 0;

  for (auto _ : state) {
    state.PauseTiming();
    const ControlType type = static_cast<ControlType>(i % BarelyControlType_kCount);
    const float value = static_cast<float>(i++);
    state.ResumeTiming();
    instrument.SetControl(type, value);
  }
}
BENCHMARK(BM_BarelyInstrument_SetControl);

BENCHMARK_MAIN();

}  // namespace
}  // namespace barely
