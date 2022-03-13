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

/// Gets playback position.
///
/// @param handle Transport handle.
/// @param out_position Output position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyTransport_GetPosition(BarelyTransportHandle handle, double* out_position);

/// Gets playback tempo.
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

/// Gets whether playback is active or not.
///
/// @param handle Transport handle.
/// @param out_is_playing Output true if active, false otherwise.
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

/// Sets playback position.
///
/// @param handle Transport handle.
/// @param position Position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyTransport_SetPosition(BarelyTransportHandle handle, double position);

/// Sets playback tempo.
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

/// Updates internal state at timestamp.
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

#endif  // BARELYMUSICIAN_API_TRANSPORT_H_
