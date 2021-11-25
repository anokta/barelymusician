#ifndef BARELYMUSICIAN_ENGINE_TRANSPORT_H_
#define BARELYMUSICIAN_ENGINE_TRANSPORT_H_

#include <functional>

namespace barely {

/// Transport that controls playback.
class Transport {
 public:
  /// Beat callback signature.
  ///
  /// @param position Beat position in beats.
  using BeatCallback = std::function<void(double position)>;

  /// Get timestamp function signature.
  ///
  /// @param position Position in beats.
  /// @return Timestamp in seconds.
  using GetTimestampFn = std::function<double(double position)>;

  /// Update callback signature.
  ///
  /// @param begin_position Begin position in beats (inclusive).
  /// @param end_position End position in beats (exclusive).
  /// @param get_timestamp_fn Get timestamp function.
  using UpdateCallback =
      std::function<void(double begin_position, double end_position,
                         const GetTimestampFn& get_timestamp_fn)>;

  /// Constructs new |Transport|.
  Transport();

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

  /// Returns whether the transport is currently playing or not.
  ///
  /// @return True if playing, false otherwise.
  bool IsPlaying() const;

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

  /// Starts the playback.
  void Start();

  /// Stops the playback.
  void Stop();

  /// Updates the transport at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void Update(double timestamp);

 private:
  // Denotes whether the transport is playing or not.
  bool is_playing_;

  // Position in beats.
  double position_;

  // Tempo in beats per second.
  double tempo_;

  // Last updated timestamp in seconds.
  double timestamp_;

  // Beat callback.
  BeatCallback beat_callback_;

  // Get timestamp function.
  GetTimestampFn get_timestamp_fn_;

  // Update callback.
  UpdateCallback update_callback_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_TRANSPORT_H_
