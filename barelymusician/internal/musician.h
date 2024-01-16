#ifndef BARELYMUSICIAN_INTERNAL_MUSICIAN_H_
#define BARELYMUSICIAN_INTERNAL_MUSICIAN_H_

#include <cstdint>
#include <unordered_set>

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/instrument.h"
#include "barelymusician/internal/mutable.h"
#include "barelymusician/internal/performer.h"

namespace barely::internal {

/// Class that wraps a musician.
class Musician {
 public:
  /// Constructs a new `Musician`.
  ///
  /// @param frame_rate Frame rate in hz.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  explicit Musician(int frame_rate) noexcept;

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

  [[nodiscard]] Rational FramesFromBeats(Rational beats) noexcept;

  /// Returns frame rate.
  ///
  /// @return Frame rate in hz.
  [[nodiscard]] int GetFrameRate() const noexcept;

  /// Returns tempo.
  ///
  /// @return Tempo in beats per minute.
  [[nodiscard]] int GetTempo() const noexcept;

  /// Returns timestamp.
  ///
  /// @return Timestamp in frames.
  [[nodiscard]] std::int64_t GetTimestamp() const noexcept;

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
  void SetTempo(int tempo) noexcept;

  /// Updates the musician at timestamp.
  ///
  /// @param timestamp Timestamp in frames.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Update(std::int64_t timestamp) noexcept;

 private:
  // Set of pointers to instruments.
  std::unordered_set<Instrument*> instruments_;

  // Set of pointers to performers.
  std::unordered_set<Performer*> performers_;

  // Frame rate in hz.
  int frame_rate_ = 48000;

  // Tempo in beats per minute.
  int tempo_ = 120;

  // Timestamp in frames.
  std::int64_t timestamp_ = 0;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_MUSICIAN_H_
