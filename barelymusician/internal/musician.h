#ifndef BARELYMUSICIAN_INTERNAL_MUSICIAN_H_
#define BARELYMUSICIAN_INTERNAL_MUSICIAN_H_

#include <unordered_set>

#include "barelymusician/internal/instrument.h"
#include "barelymusician/internal/mutable.h"
#include "barelymusician/internal/performer.h"

namespace barely::internal {

/// Class that wraps a musician.
class Musician {
 public:
  /// Adds an instrument.
  ///
  /// @param instrument Instrument.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void AddInstrument(Instrument& instrument) noexcept;

  /// Adds a performer.
  ///
  /// @param performer Performer.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void AddPerformer(Performer& performer) noexcept;

  /// Returns tempo.
  ///
  /// @return Tempo in beats per minute.
  [[nodiscard]] double GetTempo() const noexcept;

  /// Returns timestamp.
  ///
  /// @return Timestamp in seconds.
  [[nodiscard]] double GetTimestamp() const noexcept;

  /// Destroys instrument.
  ///
  /// @param instrument Instrument.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void RemoveInstrument(Instrument& instrument) noexcept;

  /// Destroys a performer.
  ///
  /// @param performer Performer.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void RemovePerformer(Performer& performer) noexcept;

  /// Sets the tempo.
  ///
  /// @param tempo Tempo in beats per minute.
  void SetTempo(double tempo) noexcept;

  /// Updates the musician at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Update(double timestamp) noexcept;

  // Set of pointers to instruments.
  std::unordered_set<Instrument*> instruments_;

  // Set of pointers to performers.
  std::unordered_set<Performer*> performers_;

  // Tempo in beats per minute.
  double tempo_ = 120.0;

  // Timestamp in seconds.
  double timestamp_ = 0.0;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_MUSICIAN_H_
