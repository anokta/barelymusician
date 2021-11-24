#ifndef PLATFORMS_CAPI_BARELYMUSICIAN_H_
#define PLATFORMS_CAPI_BARELYMUSICIAN_H_

#include <stdint.h>

#include "platforms/common/visibility.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Handle type.
typedef struct BarelyMusician* BarelyHandle;

/// Id type.
typedef uint64_t BarelyId;
enum BarelyIdConstants {
  /// Invalid id.
  kBarelyInvalidId = -1,
};

/// Status type.
typedef uint32_t BarelyStatus;
enum BarelyStatusConstants {
  /// Success.
  kBarelyOk = 0,
  /// Invalid argument error.
  kBarelyInvalidArgument = 1,
  /// Not found error.
  kBarelyNotFound = 2,
  /// Already exists error.
  kBarelyAlreadyExists = 3,
  /// Failed precondition error.
  kBarelyFailedPrecondition = 4,
  /// Unimplemented error.
  kBarelyUnimplemented = 5,
  /// Internal error.
  kBarelyInternal = 6,
  /// Unknown error.
  kBarelyUnknown = 7,
};

/// Playback beat callback signature.
///
/// @param position Beat position in beats.
typedef void (*BarelyPlaybackBeatCallback)(double position);

/// Playback update callback signature.
///
/// @param begin_position Begin position in beats.
/// @param end_position End position in beats.
typedef void (*BarelyPlaybackUpdateCallback)(double begin_position,
                                             double end_position);

/// Instrument note off callback signature.
///
/// @param instrument_id Instrument id.
/// @param note_pitch Note pitch.
typedef void (*BarelyInstrumentNoteOffCallback)(BarelyId instrument_id,
                                                float note_pitch);

/// Instrument note on callback signature.
///
/// @param instrument_id Instrument id.
/// @param note_pitch Note pitch.
/// @param note_intensity Note intensity.
typedef void (*BarelyInstrumentNoteOnCallback)(BarelyId instrument_id,
                                               float note_pitch,
                                               float note_intensity);

/// Creates new BarelyMusician.
///
/// @param sample_rate Sampling rate in Hz.
/// @return BarelyMusician handle.
BARELY_EXPORT BarelyHandle BarelyCreate(int32_t sample_rate);

/// Destroys BarelyMusician.
///
/// @param handle BarelyMusician handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyDestroy(BarelyHandle handle);

/// Sets the instrument note off callback.
///
/// @param handle BarelyMusician handle.
/// @param instrument_note_off_callback Instrument note off callback.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetInstrumentNoteOffCallback(
    BarelyHandle handle,
    BarelyInstrumentNoteOffCallback instrument_note_off_callback);

/// Sets the instrument note on callback.
///
/// @param handle BarelyMusician handle.
/// @param instrument_note_off_callback Instrument note on callback.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetInstrumentNoteOnCallback(
    BarelyHandle handle,
    BarelyInstrumentNoteOnCallback instrument_note_on_callback);

/// Sets the playback beat callback.
///
/// @param handle BarelyMusician handle.
/// @param playback_beat_callback Playback beat callback.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetPlaybackBeatCallback(
    BarelyHandle handle, BarelyPlaybackBeatCallback playback_beat_callback);

/// Sets the playback position.
///
/// @param handle BarelyMusician handle.
/// @param tempo Playback position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetPlaybackPosition(BarelyHandle handle,
                                                     double position);

/// Sets the playback tempo.
///
/// @param handle BarelyMusician handle.
/// @param tempo Playback tempo in BPM.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetPlaybackTempo(BarelyHandle handle,
                                                  double tempo);

/// Sets the playback update callback.
///
/// @param handle BarelyMusician handle.
/// @param playback_update_callback Playback update callback.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetPlaybackUpdateCallback(
    BarelyHandle handle, BarelyPlaybackUpdateCallback playback_update_callback);

/// Starts the playback.
///
/// @param handle BarelyMusician handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyStartPlayback(BarelyHandle handle);

/// Stops the playback.
///
/// @param handle BarelyMusician handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyStopPlayback(BarelyHandle handle);

/// Updates the internal state at timestamp.
///
/// @param handle BarelyMusician handle.
/// @param timestamp Timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyUpdate(BarelyHandle handle, double timestamp);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // PLATFORMS_CAPI_BARELYMUSICIAN_H_
