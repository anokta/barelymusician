#ifndef BARELYMUSICIAN_INTERNAL_MUSICIAN_H_
#define BARELYMUSICIAN_INTERNAL_MUSICIAN_H_

#include <memory>
#include <unordered_map>

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
  /// @return Pointer to performer.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  Performer* CreatePerformer() noexcept;

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
  [[nodiscard]] double GetBeatsFromSeconds(double seconds) noexcept;

  /// Returns the corresponding number of seconds for a given number of beats.
  ///
  /// @param beats Number of beats.
  /// @return Number of seconds.
  [[nodiscard]] double GetSecondsFromBeats(double beats) noexcept;

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
  // Pointer map alias.
  template <typename PointerType>
  using PointerMap = std::unordered_map<PointerType*, std::unique_ptr<PointerType>>;

  // Map of pointers to effects.
  PointerMap<Effect> effects_;

  // Map of pointers to instruments.
  PointerMap<Instrument> instruments_;

  // Map of pointers to performers.
  PointerMap<Performer> performers_;

  // Tempo in beats per minute.
  double tempo_ = 120.0;

  // Timestamp in seconds.
  double timestamp_ = 0.0;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_MUSICIAN_H_
