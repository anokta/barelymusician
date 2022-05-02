#ifndef BARELYMUSICIAN_ENGINE_CLOCK_H_
#define BARELYMUSICIAN_ENGINE_CLOCK_H_

namespace barelyapi {

/// Beat clock that controls playback.
class Clock {
 public:
  /// Returns duration to timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  /// @return Duration in seconds.
  [[nodiscard]] double GetDuration(double timestamp) const noexcept;

  /// Returns tempo.
  ///
  /// @return Tempo in beats per minute.
  [[nodiscard]] double GetTempo() const noexcept;

  /// Returns current timestamp.
  ///
  /// @return Timestamp in seconds.
  [[nodiscard]] double GetTimestamp() const noexcept;

  /// Returns timestamp after duration.
  ///
  /// @param duration Duration in beats.
  /// @return Timestamp in seconds.
  [[nodiscard]] double GetTimestamp(double duration) const noexcept;

  /// Sets tempo.
  ///
  /// @param tempo Tempo in beats per minute.
  void SetTempo(double tempo) noexcept;

  /// Sets timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void SetTimestamp(double timestamp) noexcept;

 private:
  // Tempo in beats per minute.
  double tempo_ = 120.0;

  // Timestamp in seconds.
  double timestamp_ = 0.0;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_CLOCK_H_
