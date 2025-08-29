#include <barelymusician.h>

#include <array>

#include "benchmark/benchmark.h"

namespace barely {
namespace {

using ::benchmark::State;

constexpr int kSampleRate = 48000;
constexpr int kChannelCount = 2;
constexpr int kFrameCount = 1024;

void BM_BarelyEngine_AddRemoveInstrument(State& state) {
  Engine engine(kSampleRate, kFrameCount);

  for (auto _ : state) {  // NOLINT(clang-analyzer-deadcode.DeadStores)
    [[maybe_unused]] const auto instrument = engine.CreateInstrument();
  }
}
BENCHMARK(BM_BarelyEngine_AddRemoveInstrument);

void BM_BarelyEngine_AddRemovePerformer(State& state) {
  Engine engine(kSampleRate, kFrameCount);

  for (auto _ : state) {  // NOLINT(clang-analyzer-deadcode.DeadStores)
    [[maybe_unused]] const auto performer = engine.CreatePerformer();
  }
}
BENCHMARK(BM_BarelyEngine_AddRemovePerformer);

void BM_BarelyEngine_ProcessEmpty(State& state) {
  Engine engine(kSampleRate, kFrameCount);

  std::array<float, kChannelCount * kFrameCount> output_samples;

  for (auto _ : state) {  // NOLINT(clang-analyzer-deadcode.DeadStores)
    engine.Process(output_samples.data(), kChannelCount, kFrameCount, 0.0);
  }
}
BENCHMARK(BM_BarelyEngine_ProcessEmpty);

void BM_BarelyEngine_ProcessFrequentUpdates(State& state) {
  Engine engine(kSampleRate, kFrameCount);

  auto instrument = engine.CreateInstrument();
  instrument.SetControl(ControlType::kOscMode, OscMode::kMix);
  instrument.SetControl(ControlType::kOscShape, 0.0f);

  std::array<float, kChannelCount * kFrameCount> output_samples;

  double timestamp = 0.0;

  constexpr int kUpdateCount = 20;
  constexpr double kTimestampIncrement =
      static_cast<double>(kFrameCount) / static_cast<double>(kSampleRate);

  for (auto _ : state) {  // NOLINT(clang-analyzer-deadcode.DeadStores)
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
    engine.Process(output_samples.data(), kChannelCount, kFrameCount, timestamp);
    timestamp += kTimestampIncrement;
  }
}
BENCHMARK(BM_BarelyEngine_ProcessFrequentUpdates);

void BM_BarelyInstrument_PlaySingleNoteWithLoopingSample(State& state) {
  constexpr std::array<float, 5> kSamples = {-0.5f, -0.25f, 0.0f, 0.25f, 1.0f};
  const std::array<Slice, 1> kSlices = {Slice(0.0, kSampleRate, kSamples)};

  Engine engine(kSampleRate, kFrameCount);

  auto instrument = engine.CreateInstrument();
  instrument.SetControl(ControlType::kSliceMode, SliceMode::kLoop);
  instrument.SetSampleData(kSlices);
  instrument.SetNoteOn(1.0);

  std::array<float, kChannelCount * kFrameCount> output_samples;

  for (auto _ : state) {  // NOLINT(clang-analyzer-deadcode.DeadStores)
    engine.Process(output_samples.data(), kChannelCount, kFrameCount, 0.0);
  }
}
BENCHMARK(BM_BarelyInstrument_PlaySingleNoteWithLoopingSample);

void BM_BarelyInstrument_PlaySingleoteWithSineOsc(State& state) {
  Engine engine(kSampleRate, kFrameCount);

  auto instrument = engine.CreateInstrument();
  instrument.SetControl(ControlType::kOscMode, OscMode::kMix);
  instrument.SetControl(ControlType::kOscShape, 0.0f);
  instrument.SetNoteOn(0.0);

  std::array<float, kChannelCount * kFrameCount> output_samples;

  for (auto _ : state) {  // NOLINT(clang-analyzer-deadcode.DeadStores)
    engine.Process(output_samples.data(), kChannelCount, kFrameCount, 0.0);
  }
}
BENCHMARK(BM_BarelyInstrument_PlaySingleoteWithSineOsc);

void BM_BarelyInstrument_PlayMultipleNotesWithSineOsc(State& state) {
  Engine engine(kSampleRate, kFrameCount);

  auto instrument = engine.CreateInstrument();
  instrument.SetControl(ControlType::kOscMode, OscMode::kMix);
  instrument.SetControl(ControlType::kOscShape, 0.0f);

  const int voice_count = instrument.GetControl<int>(ControlType::kVoiceCount);
  for (int i = 0; i < voice_count; ++i) {
    instrument.SetNoteOn(static_cast<float>(i));
  }

  std::array<float, kChannelCount * kFrameCount> output_samples;

  for (auto _ : state) {  // NOLINT(clang-analyzer-deadcode.DeadStores)
    engine.Process(output_samples.data(), kChannelCount, kFrameCount, 0.0);
  }
}
BENCHMARK(BM_BarelyInstrument_PlayMultipleNotesWithSineOsc);

void BM_BarelyInstrument_SetMultipleControls(State& state) {
  Engine engine(kSampleRate, kFrameCount);

  auto instrument = engine.CreateInstrument();
  int i = 0;

  for (auto _ : state) {  // NOLINT(clang-analyzer-deadcode.DeadStores)
    state.PauseTiming();
    const ControlType type = static_cast<ControlType>(i % BarelyControlType_kCount);
    const float value = static_cast<float>(i++);
    state.ResumeTiming();
    instrument.SetControl(type, value);
  }
}
BENCHMARK(BM_BarelyInstrument_SetMultipleControls);

BENCHMARK_MAIN();

}  // namespace
}  // namespace barely
