#ifndef BARELYMUSICIAN_CONDUCTOR_TRANSPORT_H_
#define BARELYMUSICIAN_CONDUCTOR_TRANSPORT_H_

#include <functional>

#include "barelymusician/api/conductor.h"

namespace barelyapi {

/// Class that wraps playback transport.
class Transport {
 public:
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
  /// @param beat_callback Beat callback.
  void SetBeatCallback(barely::Conductor::BeatCallback beat_callback) noexcept;

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

  /// Sets update callback.
  ///
  /// @param update_callback Update callback.
  void SetUpdateCallback(
      barely::Conductor::UpdateCallback update_callback) noexcept;

  /// Starts playback.
  void Start() noexcept;

  /// Stops playback.
  void Stop() noexcept;

  /// Updates transport at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void Update(double timestamp) noexcept;

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
  barely::Conductor::BeatCallback beat_callback_;

  // Update callback.
  barely::Conductor::UpdateCallback update_callback_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_CONDUCTOR_TRANSPORT_H_
