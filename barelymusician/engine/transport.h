#ifndef BARELYMUSICIAN_ENGINE_TRANSPORT_H_
#define BARELYMUSICIAN_ENGINE_TRANSPORT_H_

#include <functional>

namespace barelyapi {

/// Transport that controls playback.
class Transport {
 public:
  /// Beat callback signature.
  ///
  /// @param position Beat position in beats.
  /// @param position Beat timestamp in seconds.
  using BeatCallback = std::function<void(double position, double timestamp)>;

  /// Update callback signature.
  ///
  /// @param begin_position Begin position in beats (inclusive).
  /// @param end_position End position in beats (exclusive).
  using UpdateCallback =
      std::function<void(double begin_position, double end_position)>;

  /// Constructs new |Transport|.
  Transport() noexcept;

  /// Returns the current position.
  ///
  /// @return Position in beats.
  double GetPosition() const noexcept;

  /// Returns the tempo.
  ///
  /// @return Tempo in beats per second.
  double GetTempo() const noexcept;

  /// Returns the current timestamp.
  ///
  /// @return Timestamp in seconds.
  double GetTimestamp() const noexcept;

  /// Returns the current timestamp at position.
  ///
  /// @return Timestamp in seconds.
  double GetTimestamp(double position) const noexcept;

  /// Returns whether the transport is currently playing or not.
  ///
  /// @return True if playing, false otherwise.
  bool IsPlaying() const noexcept;

  /// Sets the beat callback.
  ///
  /// @param beat_callback Beat callback.
  void SetBeatCallback(BeatCallback beat_callback) noexcept;

  /// Sets the current position.
  ///
  /// @param position Position in beats.
  void SetPosition(double position) noexcept;

  /// Sets the tempo.
  ///
  /// @param tempo Tempo in beats per second.
  void SetTempo(double tempo) noexcept;

  /// Sets the update callback.
  ///
  /// @param update_callback Update callback.
  void SetUpdateCallback(UpdateCallback update_callback) noexcept;

  /// Starts the playback.
  void Start() noexcept;

  /// Stops the playback.
  void Stop() noexcept;

  /// Updates the transport at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void Update(double timestamp) noexcept;

 private:
  // Denotes whether the transport is playing or not.
  bool is_playing_;

  // Next beat position in beats.
  double next_beat_position_;

  // Next beat timestamp in seconds.
  double next_beat_timestamp_;

  // Position in beats.
  double position_;

  // Tempo in beats per second.
  double tempo_;

  // Timestamp in seconds.
  double timestamp_;

  // Beat callback.
  BeatCallback beat_callback_;

  // Update callback.
  UpdateCallback update_callback_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_TRANSPORT_H_
