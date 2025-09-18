#include <barelymusician.h>

#include <array>
#include <vector>

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

template <int kUpdateCount>
void BM_BarelyEngine_ProcessInstrumentUpdates(State& state) {
  Engine engine(kSampleRate, kFrameCount);

  auto instrument = engine.CreateInstrument();
  instrument.SetControl(ControlType::kOscMode, OscMode::kMix);
  instrument.SetControl(ControlType::kOscShape, 0.0f);

  std::array<float, kChannelCount * kFrameCount> output_samples;

  double timestamp = 0.0;

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
BENCHMARK(BM_BarelyEngine_ProcessInstrumentUpdates<1>);
BENCHMARK(BM_BarelyEngine_ProcessInstrumentUpdates<10>);
BENCHMARK(BM_BarelyEngine_ProcessInstrumentUpdates<100>);

template <int kInstrumentCount>
void BM_BarelyEngine_ProcessMultipleInstruments(State& state) {
  Engine engine(kSampleRate, kFrameCount);

  std::vector<Instrument> instruments;
  for (int i = 0; i < kInstrumentCount; ++i) {
    instruments.push_back(engine.CreateInstrument());
    instruments.back().SetControl(ControlType::kOscMode, OscMode::kMix);
    instruments.back().SetControl(ControlType::kOscShape, 0.0f);
    const int voice_count = instruments.back().GetControl<int>(ControlType::kVoiceCount);
    for (int voice_index = 0; voice_index < voice_count; ++voice_index) {
      instruments.back().SetNoteOn(static_cast<float>(i * voice_index) / 12.0f);
    }
  }

  std::array<float, kChannelCount * kFrameCount> output_samples;
  engine.Process(output_samples.data(), kChannelCount, kFrameCount, 0.0);  // start voices

  for (auto _ : state) {  // NOLINT(clang-analyzer-deadcode.DeadStores)
    engine.Process(output_samples.data(), kChannelCount, kFrameCount, 0.0);
  }
}
BENCHMARK(BM_BarelyEngine_ProcessMultipleInstruments<5>);
BENCHMARK(BM_BarelyEngine_ProcessMultipleInstruments<20>);
BENCHMARK(BM_BarelyEngine_ProcessMultipleInstruments<50>);

template <FilterType kFilterType>
void BM_BarelyInstrument_PlaySingleNoteWithLoopingSample(State& state) {
  constexpr std::array<float, 5> kSamples = {-0.5f, -0.25f, 0.0f, 0.25f, 1.0f};
  const std::array<Slice, 1> kSlices = {Slice(0.0, kSampleRate, kSamples)};

  Engine engine(kSampleRate, kFrameCount);

  auto instrument = engine.CreateInstrument();
  instrument.SetControl(ControlType::kSliceMode, SliceMode::kLoop);
  instrument.SetSampleData(kSlices);
  instrument.SetControl(ControlType::kFilterType, kFilterType);
  instrument.SetControl(ControlType::kFilterFrequency, 1000.0f);
  instrument.SetNoteOn(1.0);

  std::array<float, kChannelCount * kFrameCount> output_samples;
  engine.Process(output_samples.data(), kChannelCount, kFrameCount, 0.0);  // start voices

  for (auto _ : state) {  // NOLINT(clang-analyzer-deadcode.DeadStores)
    engine.Process(output_samples.data(), kChannelCount, kFrameCount, 0.0);
  }
}
BENCHMARK(BM_BarelyInstrument_PlaySingleNoteWithLoopingSample<FilterType::kNone>);
BENCHMARK(BM_BarelyInstrument_PlaySingleNoteWithLoopingSample<FilterType::kLowPass>);

template <float kOscShape, FilterType kFilterType>
void BM_BarelyInstrument_PlaySingleNoteWithOsc(State& state) {
  Engine engine(kSampleRate, kFrameCount);

  auto instrument = engine.CreateInstrument();
  instrument.SetControl(ControlType::kOscMode, OscMode::kMix);
  instrument.SetControl(ControlType::kOscShape, 0.0f);
  instrument.SetControl(ControlType::kFilterType, kFilterType);
  instrument.SetControl(ControlType::kFilterFrequency, 1000.0f);
  instrument.SetNoteOn(0.0);

  std::array<float, kChannelCount * kFrameCount> output_samples;
  engine.Process(output_samples.data(), kChannelCount, kFrameCount, 0.0);  // start voices

  for (auto _ : state) {  // NOLINT(clang-analyzer-deadcode.DeadStores)
    engine.Process(output_samples.data(), kChannelCount, kFrameCount, 0.0);
  }
}
BENCHMARK(BM_BarelyInstrument_PlaySingleNoteWithOsc<0.0f, FilterType::kNone>);
BENCHMARK(BM_BarelyInstrument_PlaySingleNoteWithOsc<0.0f, FilterType::kLowPass>);
BENCHMARK(BM_BarelyInstrument_PlaySingleNoteWithOsc<1.0f, FilterType::kNone>);
BENCHMARK(BM_BarelyInstrument_PlaySingleNoteWithOsc<1.0f, FilterType::kLowPass>);

template <float kOscShape, FilterType kFilterType>
void BM_BarelyInstrument_PlayMultipleNotesWithOsc(State& state) {
  Engine engine(kSampleRate, kFrameCount);

  auto instrument = engine.CreateInstrument();
  instrument.SetControl(ControlType::kOscMode, OscMode::kMix);
  instrument.SetControl(ControlType::kOscShape, kOscShape);
  instrument.SetControl(ControlType::kFilterType, kFilterType);
  instrument.SetControl(ControlType::kFilterFrequency, 1000.0f);
  const int voice_count = instrument.GetControl<int>(ControlType::kVoiceCount);
  for (int i = 0; i < voice_count; ++i) {
    instrument.SetNoteOn(static_cast<float>(i));
  }

  std::array<float, kChannelCount * kFrameCount> output_samples;
  engine.Process(output_samples.data(), kChannelCount, kFrameCount, 0.0);  // start voices

  for (auto _ : state) {  // NOLINT(clang-analyzer-deadcode.DeadStores)
    engine.Process(output_samples.data(), kChannelCount, kFrameCount, 0.0);
  }
}
BENCHMARK(BM_BarelyInstrument_PlayMultipleNotesWithOsc<0.0f, FilterType::kNone>);
BENCHMARK(BM_BarelyInstrument_PlayMultipleNotesWithOsc<0.0f, FilterType::kLowPass>);
BENCHMARK(BM_BarelyInstrument_PlayMultipleNotesWithOsc<1.0f, FilterType::kNone>);
BENCHMARK(BM_BarelyInstrument_PlayMultipleNotesWithOsc<1.0f, FilterType::kLowPass>);

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
