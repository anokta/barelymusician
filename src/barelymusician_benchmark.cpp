#include <array>

#include "barelymusician.h"
#include "benchmark/benchmark.h"

namespace barely {
namespace {

using ::benchmark::State;

constexpr int kFrameRate = 48000;
constexpr int kChannelCount = 2;
constexpr int kFrameCount = 1024;

void BM_BarelyInstrument_Process_Empty(State& state) {
  Musician musician(kFrameRate);

  auto instrument = musician.AddInstrument();

  std::array<double, kChannelCount * kFrameCount> output;

  for (auto _ : state) {
    instrument.Process(output.data(), kChannelCount, kFrameCount, 0.0);
  }
}
BENCHMARK(BM_BarelyInstrument_Process_Empty);

void BM_BarelyInstrument_Process_SingleNote(State& state) {
  Musician musician(kFrameRate);

  auto instrument = musician.AddInstrument();
  instrument.SetControl(ControlType::kOscillatorShape, OscillatorShape::kSine);
  instrument.SetNoteOn(0.0);

  std::array<double, kChannelCount * kFrameCount> output;

  for (auto _ : state) {
    instrument.Process(output.data(), kChannelCount, kFrameCount, 0.0);
  }
}
BENCHMARK(BM_BarelyInstrument_Process_SingleNote);

void BM_BarelyInstrument_Process_MultipleNotes(State& state) {
  Musician musician(kFrameRate);

  auto instrument = musician.AddInstrument();
  instrument.SetControl(ControlType::kOscillatorShape, OscillatorShape::kSine);

  const int voice_count = instrument.GetControl<int>(ControlType::kVoiceCount);
  for (int i = 0; i < voice_count; ++i) {
    instrument.SetNoteOn(static_cast<double>(i));
  }

  std::array<double, kChannelCount * kFrameCount> output;

  for (auto _ : state) {
    instrument.Process(output.data(), kChannelCount, kFrameCount, 0.0);
  }
}
BENCHMARK(BM_BarelyInstrument_Process_MultipleNotes);

void BM_BarelyInstrument_Process_FrequentUpdates(State& state) {
  Musician musician(kFrameRate);

  auto instrument = musician.AddInstrument();
  instrument.SetControl(ControlType::kOscillatorShape, OscillatorShape::kSine);

  std::array<double, kChannelCount * kFrameCount> output;
  double timestamp = 0.0;

  constexpr int kUpdateCount = 10;
  constexpr double kTimestampIncrement =
      static_cast<double>(kFrameCount) / static_cast<double>(kFrameRate);

  for (auto _ : state) {
    state.PauseTiming();
    timestamp += kTimestampIncrement;
    for (int i = 0; i < kUpdateCount; ++i) {
      const double pitch = static_cast<double>(i) / static_cast<double>(kUpdateCount);
      instrument.SetNoteOn(pitch);
      instrument.SetNoteControl(pitch, NoteControlType::kPitchShift, static_cast<double>(i));
      musician.Update(timestamp + kTimestampIncrement * static_cast<double>(i) /
                                      static_cast<double>(kUpdateCount));
      instrument.SetNoteOff(pitch);
    }
    state.ResumeTiming();
    instrument.Process(output.data(), kChannelCount, kFrameCount, timestamp);
  }
}
BENCHMARK(BM_BarelyInstrument_Process_FrequentUpdates);

void BM_BarelyInstrument_SetControl(State& state) {
  Musician musician(kFrameRate);

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
  Musician musician(kFrameRate);

  for (auto _ : state) {
    musician.RemoveInstrument(musician.AddInstrument());
  }
}
BENCHMARK(BM_BarelyMusician_AddRemoveInstrument);

void BM_BarelyMusician_AddRemovePerformer(State& state) {
  Musician musician(kFrameRate);

  for (auto _ : state) {
    musician.RemovePerformer(musician.AddPerformer());
  }
}
BENCHMARK(BM_BarelyMusician_AddRemovePerformer);

BENCHMARK_MAIN();

}  // namespace
}  // namespace barely
