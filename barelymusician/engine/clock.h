#ifndef BARELYMUSICIAN_ENGINE_CLOCK_H_
#define BARELYMUSICIAN_ENGINE_CLOCK_H_

#include <functional>

namespace barelyapi {

/// Clock that keeps track of position in beats.
class Clock {
 public:
  /// Beat callback signature.
  ///
  /// @param position Beat position in beats.
  /// @param timestamp Beat timestamp in seconds.
  using BeatCallback = std::function<void(double position, double timestamp)>;

  /// Update callback signature.
  ///
  /// @param begin_position Begin position in beats (inclusive).
  /// @param end_position End position in beats (exclusive).
  using UpdateCallback =
      std::function<void(double begin_position, double end_position)>;

  /// Constructs new |Clock|.
  Clock();

  /// Returns the current position.
  ///
  /// @return Position in beats.
  double GetPosition() const;

  /// Returns the position at next beat.
  ///
  /// @return Position in beats.
  double GetPositionAtNextBeat() const;

  /// Returns the tempo.
  ///
  /// @return Tempo in beats per second.
  double GetTempo() const;

  /// Returns the current timestamp.
  ///
  /// @return Timestamp in seconds.
  double GetTimestamp() const;

  /// Returns the timestamp at position.
  ///
  /// @param position Position in beats.
  /// @return Timestamp in seconds.
  double GetTimestampAtPosition(double position) const;

  /// Sets the beat callback.
  ///
  /// @param beat_callback Beat callback.
  void SetBeatCallback(BeatCallback beat_callback);

  /// Sets the current position.
  ///
  /// @param position Position in beats.
  void SetPosition(double position);

  /// Sets the tempo.
  ///
  /// @param tempo Tempo in beats per second.
  void SetTempo(double tempo);

  /// Sets the update callback.
  ///
  /// @param update_callback Update callback.
  void SetUpdateCallback(UpdateCallback update_callback);

  /// Updates the current position.
  ///
  /// @param timestamp Timestamp in seconds.
  void UpdatePosition(double timestamp);

 private:
  // Position in beats.
  double position_;

  // Tempo in beats per second.
  double tempo_;

  // Last updated timestamp in seconds.
  double timestamp_;

  // Beat callback.
  BeatCallback beat_callback_;

  // Update callback.
  UpdateCallback update_callback_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_CLOCK_H_
