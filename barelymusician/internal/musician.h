#ifndef BARELYMUSICIAN_INTERNAL_MUSICIAN_H_
#define BARELYMUSICIAN_INTERNAL_MUSICIAN_H_

#include <set>
#include <unordered_set>
#include <utility>

#include "barelymusician/internal/instrument.h"
#include "barelymusician/internal/performer.h"

namespace barely::internal {

/// Class that wraps a musician.
class Musician {
 public:
  /// Constructs a new `Musician`.
  ///
  /// @param frame_rate Frame rate in hertz.
  explicit Musician(int frame_rate) noexcept;

  /// Adds an instrument.
  ///
  /// @param instrument Pointer to instrument.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void AddInstrument(Instrument* instrument) noexcept;

  /// Adds a performer.
  ///
  /// @param process_order Process order.
  /// @param performer Pointer to performer.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void AddPerformer(Performer* performer) noexcept;

  /// Returns the corresponding number of beats for a given number of seconds.
  ///
  /// @param seconds Number of seconds.
  /// @return Number of beats.
  [[nodiscard]] double GetBeatsFromSeconds(double seconds) const noexcept;

  /// Returns frame rate.
  ///
  /// @return Frame rate in hertz.
  [[nodiscard]] int GetFrameRate() const noexcept;

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

  /// Returns update frame.
  ///
  /// @return Update frame.
  [[nodiscard]] int64_t GetUpdateFrame() const noexcept;

  /// Removes instrument.
  ///
  /// @param instrument Instrument.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void RemoveInstrument(Instrument* instrument) noexcept;

  /// Removes a performer.
  ///
  /// @param performer Performer.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void RemovePerformer(Performer* performer) noexcept;

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
  // Set of pointers to instruments.
  std::unordered_set<Instrument*> instruments_;

  // Set of process order-pointer pairs to performers.
  std::set<std::pair<int, Performer*>> performers_;

  // Frame rate in hertz.
  const int frame_rate_ = 0;

  // Tempo in beats per minute.
  double tempo_ = 120.0;

  // Timestamp in seconds.
  double timestamp_ = 0.0;

  // Update frame.
  int64_t update_frame_ = 0;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_MUSICIAN_H_
