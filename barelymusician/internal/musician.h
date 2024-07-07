#ifndef BARELYMUSICIAN_INTERNAL_MUSICIAN_H_
#define BARELYMUSICIAN_INTERNAL_MUSICIAN_H_

#include <map>
#include <memory>
#include <unordered_map>
#include <utility>

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/effect.h"
#include "barelymusician/internal/instrument.h"
#include "barelymusician/internal/performer.h"

namespace barely::internal {

/// Class that wraps a musician.
class Musician {
 public:
  /// Creates an effect.
  ///
  /// @return Pointer to effect.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  Effect* CreateEffect(EffectDefinition definition, int frame_rate) noexcept;

  /// Creates an instrument.
  ///
  /// @return Pointer to instrument.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  Instrument* CreateInstrument(InstrumentDefinition definition, int frame_rate) noexcept;

  /// Creates a performer.
  ///
  /// @param process_order Process order.
  /// @return Pointer to performer.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  Performer* CreatePerformer(int process_order) noexcept;

  /// Destroys effect.
  ///
  /// @param effect Effect.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  bool DestroyEffect(Effect* effect) noexcept;

  /// Destroys instrument.
  ///
  /// @param instrument Instrument.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  bool DestroyInstrument(Instrument* instrument) noexcept;

  /// Destroys a performer.
  ///
  /// @param performer Performer.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  bool DestroyPerformer(Performer* performer) noexcept;

  /// Returns the corresponding number of beats for a given number of seconds.
  ///
  /// @param seconds Number of seconds.
  /// @return Number of beats.
  [[nodiscard]] double GetBeatsFromSeconds(double seconds) const noexcept;

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
  // Map of pointers to effects.
  std::unordered_map<Effect*, std::unique_ptr<Effect>> effects_;

  // Map of pointers to instruments.
  std::unordered_map<Instrument*, std::unique_ptr<Instrument>> instruments_;

  // Map of process order-pointer pairs to performers.
  std::map<std::pair<int, Performer*>, std::unique_ptr<Performer>> performers_;

  // Tempo in beats per minute.
  double tempo_ = 120.0;

  // Timestamp in seconds.
  double timestamp_ = 0.0;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_MUSICIAN_H_
