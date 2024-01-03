#ifndef BARELYMUSICIAN_INTERNAL_MUSICIAN_H_
#define BARELYMUSICIAN_INTERNAL_MUSICIAN_H_

#include <functional>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/instrument.h"
#include "barelymusician/internal/mutable.h"
#include "barelymusician/internal/observable.h"
#include "barelymusician/internal/performer.h"

namespace barely::internal {

/// Class that wraps a musician.
class Musician {
 public:
  /// Creates a new instrument.
  ///
  /// @param definition Instrument definition.
  /// @param frame_rate Frame rate in hertz.
  /// @return Instrument.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  Observer<Instrument> CreateInstrument(InstrumentDefinition definition, int frame_rate) noexcept;

  /// Creates a new performer.
  ///
  /// @return Performer.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  Observer<Performer> CreatePerformer() noexcept;

  /// Destroys instrument.
  ///
  /// @param instrument Instrument.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void DestroyInstrument(Instrument& instrument) noexcept;

  /// Destroys performer.
  ///
  /// @param performer Performer.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void DestroyPerformer(Performer& performer) noexcept;

  /// Returns tempo.
  ///
  /// @return Tempo in beats per minute.
  [[nodiscard]] double GetTempo() const noexcept;

  /// Returns timestamp.
  ///
  /// @return Timestamp in seconds.
  [[nodiscard]] double GetTimestamp() const noexcept;

  /// Sets the tempo.
  ///
  /// @param tempo Tempo in beats per minute.
  void SetTempo(double tempo) noexcept;

  /// Updates the musician at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Update(double timestamp) noexcept;

  // Map of instruments by pointers.
  std::unordered_map<Instrument*, Observable<Instrument>> instruments_;

  // Map of performers by pointers.
  std::unordered_map<Performer*, Observable<Performer>> performers_;

  // Tempo in beats per minute.
  double tempo_ = 120.0;

  // Timestamp in seconds.
  double timestamp_ = 0.0;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_MUSICIAN_H_
