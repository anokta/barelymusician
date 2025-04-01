#ifndef BARELYMUSICIAN_API_ENGINE_H_
#define BARELYMUSICIAN_API_ENGINE_H_

#include <barelymusician.h>

#include <cmath>
#include <unordered_set>

#include "api/instrument.h"
#include "api/performer.h"
#include "common/rng.h"

/// Implementation of an engine.
struct BarelyEngine {
 public:
  /// Constructs a new `BarelyEngine`.
  ///
  /// @param sample_rate Sampling rate in hertz.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  explicit BarelyEngine(int sample_rate) noexcept;

  /// Adds a new instrument.
  ///
  /// @param instrument Pointer to instrument.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void AddInstrument(BarelyInstrument* instrument) noexcept;

  /// Adds a new performer.
  ///
  /// @return Pointer to performer.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void AddPerformer(BarelyPerformer* performer) noexcept;

  /// Returns reference frequency.
  ///
  /// @return Reference frequency in hertz.
  [[nodiscard]] float GetReferenceFrequency() const noexcept { return reference_frequency_; }

  /// Returns sampling rate.
  ///
  /// @return Sampling rate in hertz.
  [[nodiscard]] int GetSampleRate() const noexcept { return sample_rate_; }

  /// Returns tempo.
  ///
  /// @return Tempo in beats per minute.
  [[nodiscard]] double GetTempo() const noexcept { return tempo_; }

  /// Returns timestamp.
  ///
  /// @return Timestamp in seconds.
  [[nodiscard]] double GetTimestamp() const noexcept { return timestamp_; }

  /// Removes an instrument.
  ///
  /// @param instrument Pointer to instrument.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void RemoveInstrument(BarelyInstrument* instrument) noexcept;

  /// Removes a performer.
  ///
  /// @param performer Pointer to performer.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void RemovePerformer(BarelyPerformer* performer) noexcept;

  /// Sets the reference frequency.
  ///
  /// @param reference_frequency Reference frequency in hertz.
  void SetReferenceFrequency(float reference_frequency) noexcept;

  /// Sets the tempo.
  ///
  /// @param tempo Tempo in beats per minute.
  void SetTempo(double tempo) noexcept;

  /// Updates the engine at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Update(double timestamp) noexcept;

  barely::AudioRng& audio_rng() noexcept { return audio_rng_; }
  barely::MainRng& main_rng() noexcept { return main_rng_; }

 private:
  // Sampling rate in hertz.
  int sample_rate_ = 0;

  // Random number generator for the audio thread.
  barely::AudioRng audio_rng_;

  // Random number generator for the main thread.
  barely::MainRng main_rng_;

  // Set of pointers to instruments.
  std::unordered_set<BarelyInstrument*> instruments_;

  // Set of pointers to performers.
  std::unordered_set<BarelyPerformer*> performers_;

  // Reference frequency at zero pitch (C4 by default).
  float reference_frequency_ = 440.0f * std::pow(2.0f, -9.0f / 12.0f);

  // Tempo in beats per minute.
  double tempo_ = 120.0;

  // Timestamp in seconds.
  double timestamp_ = 0.0;
};

#endif  // BARELYMUSICIAN_API_ENGINE_H_
