#ifndef BARELYMUSICIAN_ENGINE_CLOCK_H_
#define BARELYMUSICIAN_ENGINE_CLOCK_H_

namespace barelyapi {

/// Converts minutes to seconds.
inline constexpr double kSecondsFromMinutes = 60.0;

/// Clock that keeps track of position in beats.
class Clock {
 public:
  /// Constructs new |Clock|.
  Clock();

  /// Returns the current position.
  ///
  /// @return Position in beats.
  double GetPosition() const;

  /// Returns the tempo.
  ///
  /// @return Tempo in BPM.
  double GetTempo() const;

  /// Sets the current position.
  ///
  /// @param position Position in beats.
  void SetPosition(double position);

  /// Sets the tempo.
  ///
  /// @param tempo Tempo in BPM.
  void SetTempo(double tempo);

  /// Updates the current position.
  ///
  /// @param seconds Number of seconds to iterate.
  void UpdatePosition(double seconds);

 private:
  // Position in beats.
  double position_;

  // Tempo in BPM.
  double tempo_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_CLOCK_H_
