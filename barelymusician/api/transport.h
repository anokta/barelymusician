#ifndef BARELYMUSICIAN_API_TRANSPORT_H_
#define BARELYMUSICIAN_API_TRANSPORT_H_

// NOLINTBEGIN
#include <stdbool.h>

#include "barelymusician/api/status.h"
#include "barelymusician/api/visibility.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Transport alias.
typedef struct BarelyTransport* BarelyTransportHandle;

/// Beat callback signature.
///
/// @param position Beat position in beats.
/// @param timestamp Beat timestamp in seconds.
/// @param user_data User data.
typedef void (*BarelyTransport_BeatCallback)(double position, double timestamp,
                                             void* user_data);

/// Creates new transport.
///
/// @param out_handle Output transport handle.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyTransport_Create(BarelyTransportHandle* out_handle);

/// Destroys transport.
///
/// @param handle Transport handle.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyTransport_Destroy(BarelyTransportHandle handle);

/// Gets position.
///
/// @param handle Transport handle.
/// @param out_position Output position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyTransport_GetPosition(BarelyTransportHandle handle, double* out_position);

/// Gets tempo.
///
/// @param handle Transport handle.
/// @param out_tempo Output tempo in bpm.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyTransport_GetTempo(BarelyTransportHandle handle, double* out_tempo);

/// Gets timestamp.
///
/// @param handle Transport handle.
/// @param out_timestamp Output timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyTransport_GetTimestamp(
    BarelyTransportHandle handle, double* out_timestamp);

/// Gets timestamp at position.
///
/// @param handle Transport handle.
/// @param position Position in beats.
/// @param out_timestamp Output timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyTransport_GetTimestampAtPosition(
    BarelyTransportHandle handle, double position, double* out_timestamp);

/// Gets whether transport is playing or not.
///
/// @param handle Transport handle.
/// @param out_is_playing Output true if playing, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyTransport_IsPlaying(BarelyTransportHandle handle, bool* out_is_playing);

/// Sets beat callback.
///
/// @param handle Transport handle.
/// @param beat_callback Beat callback.
/// @param user_data User data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyTransport_SetBeatCallback(
    BarelyTransportHandle handle, BarelyTransport_BeatCallback beat_callback,
    void* user_data);

/// Sets position.
///
/// @param handle Transport handle.
/// @param position Position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyTransport_SetPosition(BarelyTransportHandle handle, double position);

/// Sets tempo.
///
/// @param handle Transport handle.
/// @param tempo Tempo in bpm.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyTransport_SetTempo(BarelyTransportHandle handle, double tempo);

/// Sets timestamp.
///
/// @param handle Transport handle.
/// @param timestamp Timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyTransport_SetTimestamp(BarelyTransportHandle handle, double timestamp);

/// Starts playback.
///
/// @param handle Transport handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyTransport_Start(BarelyTransportHandle handle);

/// Stops playback.
///
/// @param handle Transport handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyTransport_Stop(BarelyTransportHandle handle);

/// Updates transport at timestamp.
///
/// @param handle Transport handle.
/// @param timestamp Timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyTransport_Update(BarelyTransportHandle handle,
                                                  double timestamp);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
// NOLINTEND

#ifdef __cplusplus
#include <functional>

namespace barely {

/// Transport.
class Transport {
 public:
  /// Beat callback signature.
  ///
  /// @param position Beat position in beats.
  /// @param timestamp Beat timestamp in seconds.
  using BeatCallback = std::function<void(double position, double timestamp)>;

  /// Constructs new `Transport`.
  Transport() {
    const auto status = BarelyTransport_Create(&handle_);
    assert(IsOk(static_cast<Status>(status)));
  }

  /// Destroys `Transport`.
  ~Transport() {
    if (handle_) {
      const auto status = BarelyTransport_Destroy(handle_);
      assert(IsOk(static_cast<Status>(status)));
      handle_ = nullptr;
    }
  }

  /// Non-copyable.
  Transport(const Transport& other) = delete;
  Transport& operator=(const Transport& other) = delete;

  /// Constructs new `Transport` via move.
  ///
  /// @param other Other transport.
  Transport(Transport&& other) noexcept
      : handle_(std::exchange(other.handle_, nullptr)) {
    SetBeatCallback(std::exchange(other.beat_callback_, nullptr));
  }

  /// Assigns `Transport` via move.
  ///
  /// @param other Other transport.
  Transport& operator=(Transport&& other) noexcept {
    if (this != &other) {
      if (handle_) {
        const auto status = BarelyTransport_Destroy(handle_);
        assert(IsOk(static_cast<Status>(status)));
      }
      handle_ = std::exchange(other.handle_, nullptr);
      SetBeatCallback(std::exchange(other.beat_callback_, nullptr));
    }
    return *this;
  }

  /// Returns position.
  ///
  /// @return Position in beats.
  [[nodiscard]] double GetPosition() const {
    double position = 0.0;
    if (handle_) {
      const auto status = BarelyTransport_GetPosition(handle_, &position);
      assert(IsOk(static_cast<Status>(status)));
    }
    return position;
  }

  /// Returns tempo.
  ///
  /// @return Tempo in bpm.
  [[nodiscard]] double GetTempo() const {
    double tempo = 0.0;
    const auto status = BarelyTransport_GetTempo(handle_, &tempo);
    assert(IsOk(static_cast<Status>(status)));
    return tempo;
  }

  /// Returns timestamp.
  ///
  /// @return Timestamp in seconds.
  [[nodiscard]] double GetTimestamp() const {
    double timestamp = 0.0;
    const auto status = BarelyTransport_GetTimestamp(handle_, &timestamp);
    assert(IsOk(static_cast<Status>(status)));
    return timestamp;
  }

  /// Returns timestamp at position.
  ///
  /// @param position Position in beats.
  /// @return Timestamp in seconds.
  [[nodiscard]] double GetTimestampAtPosition(double position) const {
    double timestamp = 0.0;
    const auto status =
        BarelyTransport_GetTimestampAtPosition(handle_, position, &timestamp);
    assert(IsOk(static_cast<Status>(status)));
    return timestamp;
  }

  /// Returns whether transport is playing or not.
  ///
  /// @return True if playing, false otherwise.
  [[nodiscard]] bool IsPlaying() const {
    bool is_playing = false;
    const auto status = BarelyTransport_IsPlaying(handle_, &is_playing);
    assert(status == BarelyStatus_kOk);
    return is_playing;
  }

  /// Sets beat callback.
  ///
  /// @param beat_callback Beat callback.
  /// @return Status.
  Status SetBeatCallback(BeatCallback beat_callback) {
    if (beat_callback) {
      beat_callback_ = std::move(beat_callback);
      return static_cast<Status>(BarelyTransport_SetBeatCallback(
          handle_,
          [](double beat, double timestamp, void* user_data) {
            (*static_cast<BeatCallback*>(user_data))(beat, timestamp);
          },
          static_cast<void*>(&beat_callback_)));
    }
    return static_cast<Status>(
        BarelyTransport_SetBeatCallback(handle_, nullptr, nullptr));
  }

  /// Sets position.
  ///
  /// @param position Position in beats.
  /// @return Status.
  Status SetPosition(double position) {
    return static_cast<Status>(BarelyTransport_SetPosition(handle_, position));
  }

  /// Sets tempo.
  ///
  /// @param tempo Tempo in bpm.
  /// @return Status.
  Status SetTempo(double tempo) {
    return static_cast<Status>(BarelyTransport_SetTempo(handle_, tempo));
  }

  /// Sets timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status SetTimestamp(double timestamp) {
    return static_cast<Status>(
        BarelyTransport_SetTimestamp(handle_, timestamp));
  }

  /// Starts playback.
  ///
  /// @return Status.
  Status Start() { return static_cast<Status>(BarelyTransport_Start(handle_)); }

  /// Stops playback.
  ///
  /// @return Status.
  Status Stop() { return static_cast<Status>(BarelyTransport_Stop(handle_)); }

  /// Updates internal state at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status Update(double timestamp) {
    return static_cast<Status>(BarelyTransport_Update(handle_, timestamp));
  }

 private:
  // Internal handle.
  BarelyTransportHandle handle_ = nullptr;

  // Beat callback.
  BeatCallback beat_callback_;
};

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_API_TRANSPORT_H_
