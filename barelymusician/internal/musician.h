#ifndef BARELYMUSICIAN_INTERNAL_MUSICIAN_H_
#define BARELYMUSICIAN_INTERNAL_MUSICIAN_H_

#include <cmath>
#include <map>
#include <memory>
#include <unordered_map>
#include <utility>

#include "barelymusician/internal/instrument_controller.h"
#include "barelymusician/internal/performer.h"

namespace barely::internal {

/// Class that wraps a musician.
class Musician {
 public:
  /// Constructs a new `Musician`.
  ///
  /// @param frame_rate Frame rate in hertz.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  explicit Musician(int frame_rate) noexcept;

  /// Adds an instrument.
  ///
  /// @return Pointer to instrument.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  InstrumentController* AddInstrument() noexcept;

  /// Adds a performer.
  ///
  /// @param process_order Process order.
  /// @return Pointer to performer.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  Performer* AddPerformer(int process_order) noexcept;

  /// Returns the corresponding number of beats for a given number of seconds.
  ///
  /// @param seconds Number of seconds.
  /// @return Number of beats.
  [[nodiscard]] double GetBeatsFromSeconds(double seconds) const noexcept;

  /// Returns reference frequency.
  ///
  /// @return Reference frequency in hertz.
  [[nodiscard]] double GetReferenceFrequency() const noexcept;

  /// Returns the corresponding number of frames for a given number of seconds.
  ///
  /// @param seconds Number of seconds.
  /// @return Number of frames.
  [[nodiscard]] int64_t GetFramesFromSeconds(double seconds) const noexcept;

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

  /// Removes instrument.
  ///
  /// @param instrument Instrument.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void RemoveInstrument(InstrumentController* instrument) noexcept;

  /// Removes a performer.
  ///
  /// @param performer Performer.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void RemovePerformer(Performer* performer) noexcept;

  /// Sets the reference frequency.
  ///
  /// @param reference_frequency Reference frequency in hertz.
  void SetReferenceFrequency(double reference_frequency) noexcept;

  /// Sets the tempo.
  ///
  /// @param tempo Tempo in beats per minute.
  void SetTempo(double tempo) noexcept;

  /// Updates the musician at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Update(double timestamp) noexcept;

 private:
  // Map of pointers to instruments.
  // TODO(#126): Replace these by memory pools.
  std::unordered_map<InstrumentController*, std::unique_ptr<InstrumentController>> instruments_;

  // Map of process order-pointer pairs to performers.
  std::map<Performer*, std::pair<int, std::unique_ptr<Performer>>> performers_;

  // Frame rate in hertz.
  const int frame_rate_ = 0;

  // Reference frequency at zero pitch (C4 by default).
  double reference_frequency_ = 440.0 * std::pow(2.0, -9.0 / 12.0);

  // Tempo in beats per minute.
  double tempo_ = 120.0;

  // Timestamp in seconds.
  double timestamp_ = 0.0;

  // Update frame.
  int64_t update_frame_ = 0;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_MUSICIAN_H_
