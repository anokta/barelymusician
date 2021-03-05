#ifndef BARELYMUSICIAN_ENGINE_CLOCK_H_
#define BARELYMUSICIAN_ENGINE_CLOCK_H_

namespace barelyapi {

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
  /// @return Tempo in beats per second.
  double GetTempo() const;

  /// Returns the current timestamp.
  ///
  /// @return Timestamp in seconds.
  double GetTimestamp() const;

  /// Returns the timestamp at position.
  /// @note Assumes the playback is active with a constant valid tempo.
  ///
  /// @param position Position in beats.
  /// @return Timestamp in seconds.
  double GetTimestampAtPosition(double position) const;

  /// Returns whether the playback is currently active or not.
  ///
  /// @return True if playing, false otherwise.
  bool IsPlaying() const;

  /// Sets the current position.
  ///
  /// @param position Position in beats.
  void SetPosition(double position);

  /// Sets the tempo.
  ///
  /// @param tempo Tempo in beats per second.
  void SetTempo(double tempo);

  /// Starts playback.
  void Start();

  /// Stops playback.
  void Stop();

  /// Updates the current state.
  ///
  /// @param timestamp Timestamp in seconds.
  void Update(double timestamp);

 private:
  // Denotes whether the playback is active or not.
  double is_playing_;

  // Position in beats.
  double position_;

  // Tempo in beats per second.
  double tempo_;

  // Last updated timestamp in seconds.
  double timestamp_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_CLOCK_H_
