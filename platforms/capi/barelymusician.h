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
typedef int64_t BarelyId;
enum BarelyIdConstants {
  /// Invalid id.
  kBarelyInvalidId = -1,
};

/// Status type.
typedef int32_t BarelyStatus;
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

// TODO: Temporary shortcut until |InstrumentDefinition| is ported.
enum BarelyInstrumentTypes {
  /// Synth instrument.
  kBarelySynthInstrument = 1,
};

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

/// Adds new instrument.
///
/// @param handle BarelyMusician handle.
/// @return Instrument id.
BARELY_EXPORT BarelyId BarelyAddInstrument(BarelyHandle handle,
                                           int32_t instrument_type);

/// Adds new performer.
///
/// @param handle BarelyMusician handle.
/// @return Performer id.
BARELY_EXPORT BarelyId BarelyAddPerformer(BarelyHandle handle);

/// Adds performer instrument.
///
/// @param handle BarelyMusician handle.
/// @param performer_id Performer id.
/// @param instrument_id Instrument id.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyAddPerformerInstrument(BarelyHandle handle,
                                                        BarelyId performer_id,
                                                        BarelyId instrument_id);

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

/// Returns whether the playback is currently active or not.
///
/// @param handle BarelyMusician handle.
/// @return True if playing, false otherwise.
BARELY_EXPORT bool BarelyIsPlaying(BarelyHandle handle);

/// Processes the next instrument output buffer at timestamp.
///
/// @param handle BarelyMusician handle.
/// @param instrument_id Instrument id.
/// @param timestamp Timestamp in seconds.
/// @param output Pointer to the output buffer.
/// @param num_channels Number of output channels.
/// @param num_frames Number of output frames.
BARELY_EXPORT BarelyStatus BarelyProcessInstrument(
    BarelyHandle handle, BarelyId instrument_id, double timestamp,
    float* output, int32_t num_channels, int32_t num_frames);

/// Removes all performerinstruments.
///
/// @param handle BarelyMusician handle.
/// @param performer_id Performer id.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyRemoveAllPerformerInstruments(BarelyHandle handle, BarelyId performer_id);

/// Removes instrument.
///
/// @param handle BarelyMusician handle.
/// @param instrument_id Instrument id.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyRemoveInstrument(BarelyHandle handle,
                                                  BarelyId instrument_id);

/// Removes performer.
///
/// @param handle BarelyMusician handle.
/// @param performer_id Performer id.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyRemovePerformer(BarelyHandle handle,
                                                 BarelyId performer_id);

/// Removes performer instrument.
///
/// @param handle BarelyMusician handle.
/// @param performer_id Performer id.
/// @param instrument_id Instrument id.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyRemovePerformerInstrument(
    BarelyHandle handle, BarelyId performer_id, BarelyId instrument_id);

/// Sets all instrument notes off.
///
/// @param handle BarelyMusician handle.
/// @param instrument_id Instrument id.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelySetAllInstrumentNotesOff(BarelyHandle handle, BarelyId instrument_id);

/// Sets all instrument parameters to default.
///
/// @param handle BarelyMusician handle.
/// @param instrument_id Instrument id.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetAllInstrumentParamsToDefault(
    BarelyHandle handle, BarelyId instrument_id);

/// Sets instrument note off.
///
/// @param handle BarelyMusician handle.
/// @param instrument_id Instrument id.
/// @param note_pitch Note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetInstrumentNoteOff(BarelyHandle handle,
                                                      BarelyId instrument_id,
                                                      float note_pitch);

/// Sets the instrument note off callback.
///
/// @param handle BarelyMusician handle.
/// @param instrument_note_off_callback Instrument note off callback.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetInstrumentNoteOffCallback(
    BarelyHandle handle,
    BarelyInstrumentNoteOffCallback instrument_note_off_callback);

/// Sets instrument note on.
///
/// @param handle BarelyMusician handle.
/// @param instrument_id Instrument id.
/// @param note_pitch Note pitch.
/// @param note_intensity Note intensity.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetInstrumentNoteOn(BarelyHandle handle,
                                                     BarelyId instrument_id,
                                                     float note_pitch,
                                                     float note_intensity);

/// Sets the instrument note on callback.
///
/// @param handle BarelyMusician handle.
/// @param instrument_note_off_callback Instrument note on callback.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetInstrumentNoteOnCallback(
    BarelyHandle handle,
    BarelyInstrumentNoteOnCallback instrument_note_on_callback);

/// Sets instrument parameter.
///
/// @param handle BarelyMusician handle.
/// @param instrument_id Instrument id.
/// @param param_id Parameter id.
/// @param param_value Parameter value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetInstrumentParam(BarelyHandle handle,
                                                    BarelyId instrument_id,
                                                    int32_t param_id,
                                                    float param_value);

/// Sets instrument parameter to default.
///
/// @param handle BarelyMusician handle.
/// @param instrument_id Instrument id.
/// @param param_id Parameter id.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetInstrumentParamToDefault(
    BarelyHandle handle, BarelyId instrument_id, int32_t param_id);

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

/// Sets the sample rate.
///
/// @param handle BarelyMusician handle.
/// @param sample_rate Sampling rate in Hz.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetSampleRate(BarelyHandle handle,
                                               int32_t sample_rate);

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
