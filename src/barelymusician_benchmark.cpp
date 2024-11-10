#include <array>

#include "barelymusician.h"
#include "benchmark/benchmark.h"

namespace barely {
namespace {

using ::benchmark::State;

constexpr int kSampleRate = 48000;
constexpr int kSampleCount = 1024;

void BM_BarelyInstrument_Process_Empty(State& state) {
  Musician musician(kSampleRate);

  auto instrument = musician.AddInstrument();

  std::array<double, kSampleCount> output_samples;

  for (auto _ : state) {
    instrument.Process(output_samples, 0.0);
  }
}
BENCHMARK(BM_BarelyInstrument_Process_Empty);

void BM_BarelyInstrument_Process_SingleNoteWithLoopingSample(State& state) {
  constexpr std::array<double, 5> kSamples = {-0.5, -0.25, 0.0, 0.25, 1.0};
  const std::array<SampleDataSlice, 1> kSlices = {SampleDataSlice(0.0, kSampleRate, kSamples)};

  Musician musician(kSampleRate);

  auto instrument = musician.AddInstrument();
  instrument.SetControl(ControlType::kSamplePlaybackMode, SamplePlaybackMode::kLoop);
  instrument.SetSampleData(kSlices);
  instrument.SetNoteOn(1.0);

  std::array<double, kSampleCount> output_samples;

  for (auto _ : state) {
    instrument.Process(output_samples, 0.0);
  }
}
BENCHMARK(BM_BarelyInstrument_Process_SingleNoteWithLoopingSample);

void BM_BarelyInstrument_Process_SingleNoteWithSineOscillator(State& state) {
  Musician musician(kSampleRate);

  auto instrument = musician.AddInstrument();
  instrument.SetControl(ControlType::kOscillatorShape, OscillatorShape::kSine);
  instrument.SetNoteOn(0.0);

  std::array<double, kSampleCount> output_samples;

  for (auto _ : state) {
    instrument.Process(output_samples, 0.0);
  }
}
BENCHMARK(BM_BarelyInstrument_Process_SingleNoteWithSineOscillator);

void BM_BarelyInstrument_Process_MultipleNotesWithSineOscillator(State& state) {
  Musician musician(kSampleRate);

  auto instrument = musician.AddInstrument();
  instrument.SetControl(ControlType::kOscillatorShape, OscillatorShape::kSine);

  const int voice_count = instrument.GetControl<int>(ControlType::kVoiceCount);
  for (int i = 0; i < voice_count; ++i) {
    instrument.SetNoteOn(static_cast<double>(i));
  }

  std::array<double, kSampleCount> output_samples;

  for (auto _ : state) {
    instrument.Process(output_samples, 0.0);
  }
}
BENCHMARK(BM_BarelyInstrument_Process_MultipleNotesWithSineOscillator);

void BM_BarelyInstrument_Process_FrequentUpdates(State& state) {
  Musician musician(kSampleRate);

  auto instrument = musician.AddInstrument();
  instrument.SetControl(ControlType::kOscillatorShape, OscillatorShape::kSine);

  std::array<double, kSampleCount> output_samples;
  double timestamp = 0.0;

  constexpr int kUpdateCount = 20;
  constexpr double kTimestampIncrement =
      static_cast<double>(kSampleCount) / static_cast<double>(kSampleRate);

  for (auto _ : state) {
    state.PauseTiming();
    timestamp += kTimestampIncrement;
    for (int i = 0; i < kUpdateCount; ++i) {
      instrument.SetControl(ControlType::kAttack, 0.001 * static_cast<double>(i));
      const double pitch = static_cast<double>(i) / static_cast<double>(kUpdateCount);
      instrument.SetNoteOn(pitch);
      instrument.SetNoteControl(pitch, NoteControlType::kPitchShift, static_cast<double>(i));
      musician.Update(timestamp + kTimestampIncrement * static_cast<double>(i) /
                                      static_cast<double>(kUpdateCount));
      instrument.SetNoteOff(pitch);
      instrument.SetControl(ControlType::kAttack, 0.01 * static_cast<double>(i));
    }
    state.ResumeTiming();
    instrument.Process(output_samples, timestamp);
  }
}
BENCHMARK(BM_BarelyInstrument_Process_FrequentUpdates);

void BM_BarelyInstrument_SetControl(State& state) {
  Musician musician(kSampleRate);

  auto instrument = musician.AddInstrument();
  int i = 0;

  for (auto _ : state) {
    state.PauseTiming();
    const ControlType type = static_cast<ControlType>(i % BarelyControlType_kCount);
    const double value = i++;
    state.ResumeTiming();
    instrument.SetControl(type, value);
  }
}
BENCHMARK(BM_BarelyInstrument_SetControl);

void BM_BarelyMusician_AddRemoveInstrument(State& state) {
  Musician musician(kSampleRate);

  for (auto _ : state) {
    musician.RemoveInstrument(musician.AddInstrument());
  }
}
BENCHMARK(BM_BarelyMusician_AddRemoveInstrument);

void BM_BarelyMusician_AddRemovePerformer(State& state) {
  Musician musician(kSampleRate);

  for (auto _ : state) {
    musician.RemovePerformer(musician.AddPerformer());
  }
}
BENCHMARK(BM_BarelyMusician_AddRemovePerformer);

BENCHMARK_MAIN();

}  // namespace
}  // namespace barely
