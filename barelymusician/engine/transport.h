#ifndef BARELYMUSICIAN_ENGINE_TRANSPORT_H_
#define BARELYMUSICIAN_ENGINE_TRANSPORT_H_

#include <functional>

#include "barelymusician/barelymusician.h"

namespace barelyapi {

/// Class that wraps playback transport.
class Transport {
 public:
  /// Beat callback alias.
  using BeatCallback = barely::Musician::BeatCallback;

  /// Update callback signature.
  ///
  /// @param begin_position Begin position in beats.
  /// @param end_position End position in beats.
  using UpdateCallback =
      std::function<void(double begin_position, double end_position)>;

  /// Returns current position.
  ///
  /// @return Position in beats.
  [[nodiscard]] double GetPosition() const noexcept;

  /// Returns tempo.
  ///
  /// @return Tempo in beats per second.
  [[nodiscard]] double GetTempo() const noexcept;

  /// Returns current timestamp.
  ///
  /// @return Timestamp in seconds.
  [[nodiscard]] double GetTimestamp() const noexcept;

  /// Returns current timestamp at position.
  ///
  /// @return Timestamp in seconds.
  [[nodiscard]] double GetTimestamp(double position) const noexcept;

  /// Returns whether transport is currently playing or not.
  ///
  /// @return True if playing, false otherwise.
  [[nodiscard]] bool IsPlaying() const noexcept;

  /// Sets beat callback.
  ///
  /// @param callback Beat callback.
  void SetBeatCallback(BeatCallback callback) noexcept;

  /// Sets current position.
  ///
  /// @param position Position in beats.
  void SetPosition(double position) noexcept;

  /// Sets tempo.
  ///
  /// @param tempo Tempo in beats per second.
  void SetTempo(double tempo) noexcept;

  /// Sets timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void SetTimestamp(double timestamp) noexcept;

  /// Starts playback.
  void Start() noexcept;

  /// Stops playback.
  void Stop() noexcept;

  /// Updates transport at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  /// @param callback Update callback.
  void Update(double timestamp, const UpdateCallback& callback) noexcept;

 private:
  // Denotes whether transport is playing or not.
  bool is_playing_ = false;

  // Next beat position in beats.
  double next_beat_position_ = 0.0;

  // Next beat timestamp in seconds.
  double next_beat_timestamp_ = 0.0;

  // Position in beats.
  double position_ = 0.0;

  // Tempo in beats per second.
  double tempo_ = 1.0;

  // Timestamp in seconds.
  double timestamp_ = 0.0;

  // Beat callback.
  BeatCallback beat_callback_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_TRANSPORT_H_
