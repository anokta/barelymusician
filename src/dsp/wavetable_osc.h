#ifndef BARELYMUSICIAN_DSP_WAVETABLE_OSC_H_
#define BARELYMUSICIAN_DSP_WAVETABLE_OSC_H_

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <numbers>

namespace barely {

/// Wavetable oscillator that generates interpolated output samples of basic waveforms.
class WavetableOsc {
 public:
  WavetableOsc() noexcept
      : wavetables_(kShapeCount,
                    BandlimitedWavetables(kTableCount, Wavetable(kSampleCount, 0.0f))) {
    // Generate the wavetables.
    for (int i = 0; i < kTableCount; ++i) {
      FillSineWavetable(wavetables_[0][i]);
      FillSquareWavetable(wavetables_[1][i], i);
      FillTriangleWavetable(wavetables_[2][i], i);
      FillSawtoothWavetable(wavetables_[3][i], i);
    }
  }

  /// Generates the next output sample.
  ///
  /// @param shape Normalized shape value.
  /// @return Next output sample.
  [[nodiscard]] float Next(float shape) noexcept {
    // Select the wavetables.
    const float shape_interp = static_cast<float>(kShapeCount - 1) * std::clamp(shape, 0.0f, 1.0f);
    const int shape_low = static_cast<int>(shape_interp);
    const int shape_hi = std::min(kShapeCount - 1, shape_low + 1);

    const int sample_index = static_cast<int>(phase_);
    const float sample_fractional = phase_ - static_cast<float>(sample_index);
    const float output = (1.0f - shape_interp) * GetSample(wavetables_[shape_low][table_index_],
                                                           sample_index, sample_fractional) +
                         shape_interp * GetSample(wavetables_[shape_hi][table_index_], sample_index,
                                                  sample_fractional);

    // Update the phase.
    phase_ += increment_;
    if (phase_ >= static_cast<float>(kSampleCount)) {
      phase_ -= static_cast<float>(kSampleCount);
    }

    return output;
  }

  /// Resets the phase.
  void Reset() noexcept { phase_ = 0.0f; }

  /// Sets the increment per sample.
  ///
  /// @param pitch Note pitch.
  /// @param reference_frequency Reference frequency in hertz.
  /// @param sample_interval Sample interval in seconds.
  void SetIncrement(float pitch, float reference_frequency, float sample_interval) noexcept {
    assert(reference_frequency >= 0.0f);
    assert(sample_interval >= 0.0f);

    const float frequency = std::pow(2.0f, pitch) * reference_frequency;
    // TODO: Revisit this
    const float max_harmonics = (1.0f / sample_interval) / (2.0f * frequency);
    table_index_ = std::clamp(static_cast<int>(std::log2(max_harmonics)), 0, kTableCount - 1);
    increment_ = frequency * sample_interval * static_cast<float>(kSampleCount);
  }

 private:
  // Number of band-limited tables.
  static inline constexpr int kTableCount = 8;

  // Number of samples in each table.
  static inline constexpr int kSampleCount = 2048;

  // Number of wave shapes.
  static inline constexpr int kShapeCount = 4;

  // Array of band-limited wavetables of each wave shape.
  // TODO: Get rid of global heap allocation here.
  using Wavetable = std::vector<float>;
  using BandlimitedWavetables = std::vector<Wavetable>;
  std::vector<BandlimitedWavetables> wavetables_ = {};

  static void FillSineWavetable(Wavetable& sine_wavetable) noexcept {
    for (int i = 0; i < kSampleCount; ++i) {
      sine_wavetable[i] = std::sin(2.0f * std::numbers::pi_v<float> * static_cast<float>(i) /
                                   static_cast<float>(kSampleCount));
    }
  }

  static void FillSquareWavetable(Wavetable& square_wavetable, int harmonic_limit) noexcept {
    for (int harmonic = 1; harmonic <= (1 << harmonic_limit); harmonic += 2) {
      for (int i = 0; i < kSampleCount; ++i) {
        square_wavetable[i] += (4.0f / std::numbers::pi_v<float>)*std::sin(
                                   2.0f * std::numbers::pi_v<float> *
                                   static_cast<float>(harmonic * i) / kSampleCount) /
                               static_cast<float>(harmonic);
      }
    }
  }

  static void FillTriangleWavetable(Wavetable& triangle_wavetable, int harmonic_limit) noexcept {
    for (int harmonic = 1; harmonic <= (1 << harmonic_limit); harmonic += 2) {
      for (int i = 0; i < kSampleCount; ++i) {
        triangle_wavetable[i] += (8.0f / (std::numbers::pi_v<float> * std::numbers::pi_v<float>)) *
                                 std::pow(-1.0f, 0.5f * (harmonic - 1)) *
                                 std::sin(2.0f * std::numbers::pi_v<float> *
                                          static_cast<float>(harmonic * i) / kSampleCount) /
                                 static_cast<float>(harmonic * harmonic);
      }
    }
  }

  static void FillSawtoothWavetable(Wavetable& sawtooth_wavetable, int harmonic_limit) noexcept {
    for (int harmonic = 1; harmonic <= (1 << harmonic_limit); harmonic += 2) {
      for (int i = 0; i < kSampleCount; ++i) {
        sawtooth_wavetable[i] += (2.0f / (harmonic * std::numbers::pi_v<float>)) *
                                 std::sin(2.0f * std::numbers::pi_v<float> *
                                          static_cast<float>(harmonic * i) / kSampleCount);
      }
    }
  }

  static float GetSample(const Wavetable& wavetable, int index, float fractional) noexcept {
    return (1.0f - fractional) * wavetable[index] +
           fractional * wavetable[(index + 1) % kSampleCount];
  }

  // Selected table index.
  int table_index_ = 0;

  // Increment per sample.
  float increment_ = 0.0f;

  // Internal clock.
  float phase_ = 0.0f;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_WAVETABLE_OSC_H_
