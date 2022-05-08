#ifndef BARELYMUSICIAN_ENGINE_CLOCK_H_
#define BARELYMUSICIAN_ENGINE_CLOCK_H_

namespace barely::internal {

/// Beat clock.
class Clock {
 public:
  /// Returns beats from seconds.
  ///
  /// @param seconds Number of seconds.
  /// @return Number of beats.
  [[nodiscard]] double GetBeats(double seconds) const noexcept;

  /// Returns seconds from beats.
  ///
  /// @param beats Number of beats.
  /// @return Number of seconds.
  [[nodiscard]] double GetSeconds(double beats) const noexcept;

  /// Returns tempo.
  ///
  /// @return Tempo in beats per minute.
  [[nodiscard]] double GetTempo() const noexcept;

  /// Sets tempo.
  ///
  /// @param tempo Tempo in beats per minute.
  void SetTempo(double tempo) noexcept;

 private:
  // Tempo in beats per minute.
  double tempo_ = 120.0;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_ENGINE_CLOCK_H_
