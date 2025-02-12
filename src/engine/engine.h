#ifndef BARELYMUSICIAN_ENGINE_ENGINE_H_
#define BARELYMUSICIAN_ENGINE_ENGINE_H_

#include <cmath>
#include <memory>
#include <unordered_set>
#include <utility>

#include "barelymusician.h"
#include "engine/instrument.h"
#include "engine/performer.h"

namespace barely::internal {

/// Class that wraps an engine.
class Engine {
 public:
  /// Constructs a new `Engine`.
  ///
  /// @param sample_rate Sampling rate in hertz.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  explicit Engine(int sample_rate) noexcept;

  /// Creates a new instrument.
  ///
  /// @return Pointer to instrument.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  Instrument* CreateInstrument() noexcept;

  /// Creates a new performer.
  ///
  /// @return Pointer to performer.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  Performer* CreatePerformer() noexcept;

  /// Destroys an instrument.
  ///
  /// @param instrument Pointer to instrument.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void DestroyInstrument(Instrument* instrument) noexcept;

  /// Destroys a performer.
  ///
  /// @param performer Pointer to performer.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void DestroyPerformer(Performer* performer) noexcept;

  /// Returns the corresponding number of beats for a given number of seconds.
  ///
  /// @param seconds Number of seconds.
  /// @return Number of beats.
  [[nodiscard]] double GetBeatsFromSeconds(double seconds) const noexcept;

  /// Returns reference frequency.
  ///
  /// @return Reference frequency in hertz.
  [[nodiscard]] float GetReferenceFrequency() const noexcept;

  /// Returns the corresponding number of samples for a given number of seconds.
  ///
  /// @param seconds Number of seconds.
  /// @return Number of samples.
  [[nodiscard]] int64_t GetSamplesFromSeconds(double seconds) const noexcept;

  /// Returns the corresponding number of seconds for a given number of beats.
  ///
  /// @param beats Number of beats.
  /// @return Number of seconds.
  [[nodiscard]] double GetSecondsFromBeats(double beats) const noexcept;

  /// Returns tempo.
  ///
  /// @return Tempo in beats per minute.
  [[nodiscard]] double GetTempo() const noexcept;

  /// Returns timestamp.
  ///
  /// @return Timestamp in seconds.
  [[nodiscard]] double GetTimestamp() const noexcept;

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

 private:
  // Map of instruments by their pointers.
  std::unordered_map<Instrument*, std::unique_ptr<Instrument>> instruments_;

  // Map of performers by their pointers.
  std::unordered_map<Performer*, std::unique_ptr<Performer>> performers_;

  // Sampling rate in hertz.
  const int sample_rate_ = 0;

  // Reference frequency at zero pitch (C4 by default).
  float reference_frequency_ = 440.0f * std::pow(2.0f, -9.0f / 12.0f);

  // Tempo in beats per minute.
  double tempo_ = 120.0;

  // Timestamp in seconds.
  double timestamp_ = 0.0;
};

}  // namespace barely::internal

struct BarelyEngine : public barely::internal::Engine {
 public:
  explicit BarelyEngine(int32_t sample_rate) noexcept : Engine(sample_rate) {}
  ~BarelyEngine() = default;

  // Non-copyable and non-movable.
  BarelyEngine(const BarelyEngine& other) noexcept = delete;
  BarelyEngine& operator=(const BarelyEngine& other) noexcept = delete;
  BarelyEngine(BarelyEngine&& other) noexcept = delete;
  BarelyEngine& operator=(BarelyEngine&& other) noexcept = delete;
};
static_assert(sizeof(BarelyEngine) == sizeof(barely::internal::Engine));

#endif  // BARELYMUSICIAN_ENGINE_ENGINE_H_
