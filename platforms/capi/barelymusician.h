#ifndef PLATFORMS_CAPI_BARELYMUSICIAN_H_
#define PLATFORMS_CAPI_BARELYMUSICIAN_H_

#include <stdint.h>

#include "platforms/capi/visibility.h"

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

// TODO(#85): Add |BarelyConductorDefinition|.

// TODO(#85): Add |BarelyInstrumentDefinition|.
// TODO(#85): Add |BarelyParamDefinitions|.
// TODO(#85): Temporary shortcut until |InstrumentDefinition| is ported.
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

// TODO(#85): Add |BarelyAddInstrument| with |BarelyInstrumentDefinition|.

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

/// Adds performer note.
///
/// @param handle BarelyMusician handle.
/// @param performer_id Performer id.
/// @param note_position Note position.
/// @param note_duration Note duration.
/// @param note_pitch Note pitch.
/// @param note_intensity Note intensity.
/// @return Note id.
BARELY_EXPORT BarelyId BarelyAddPerformerNote(
    BarelyHandle handle, BarelyId performer_id, double note_position,
    double note_duration, float note_pitch, float note_intensity);

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

/// Returns performer begin offset.
///
/// @param handle BarelyMusician handle.
/// @param performer_id Performer id.
/// @return Begin offset in beats.
BARELY_EXPORT double BarelyGetPerformerBeginOffset(BarelyHandle handle,
                                                   BarelyId performer_id);

// TODO(#85): Add |BarelyGetPerformerBeginPosition|.
// TODO(#85): Add |BarelyGetPerformerEndPosition|.

/// Returns performer loop begin offset.
///
/// @param handle BarelyMusician handle.
/// @param performer_id Performer id.
/// @return Loop begin offset in beats.
BARELY_EXPORT double BarelyGetPerformerLoopBeginOffset(BarelyHandle handle,
                                                       BarelyId performer_id);

/// Returns performer loop length.
///
/// @param handle BarelyMusician handle.
/// @param performer_id Performer id.
/// @return Loop length in beats.
BARELY_EXPORT double BarelyGetPerformerLoopLength(BarelyHandle handle,
                                                  BarelyId performer_id);

/// Returns the playback position.
///
/// @param handle BarelyMusician handle.
/// @return Position in beats.
BARELY_EXPORT double BarelyGetPlaybackPosition(BarelyHandle handle);

/// Returns the playback tempo.
///
/// @param handle BarelyMusician handle.
/// @return Tempo in BPM.
BARELY_EXPORT double BarelyGetPlaybackTempo(BarelyHandle handle);

/// Returns whether the performer is empty or not.
///
/// @param handle BarelyMusician handle.
/// @param performer_id Performer id.
/// @return True if empty (i.e., has no notes), false otherwise.
BARELY_EXPORT bool BarelyIsPerformerEmpty(BarelyHandle handle,
                                          BarelyId performer_id);

/// Returns whether the performer is looping or not.
///
/// @param handle BarelyMusician handle.
/// @param performer_id Performer id.
/// @return True if looping, false otherwise.
BARELY_EXPORT bool BarelyIsPerformerLooping(BarelyHandle handle,
                                            BarelyId performer_id);

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
/// @return Status.
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

/// Removes all performer notes.
///
/// @param handle BarelyMusician handle.
/// @param performer_id Performer id.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyRemoveAllPerformerNotes(BarelyHandle handle,
                                                         BarelyId performer_id);

/// Removes all performer notes at given range.
///
/// @param handle BarelyMusician handle.
/// @param performer_id Performer id.
/// @param begin_position Begin position in beats.
/// @param end_position End position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyRemoveAllPerformerNotesAt(BarelyHandle handle, BarelyId performer_id,
                                double begin_position, double end_position);

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

/// Removes performer note.
///
/// @param handle BarelyMusician handle.
/// @param performer_id Performer id.
/// @param note_id Note id.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyRemovePerformerNote(BarelyHandle handle,
                                                     BarelyId performer_id,
                                                     BarelyId note_id);

// TODO(#85): Add |BarelySetAllInstrumentNotesOff| for all.

/// Sets all instrument notes off.
///
/// @param handle BarelyMusician handle.
/// @param instrument_id Instrument id.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelySetAllInstrumentNotesOff(BarelyHandle handle, BarelyId instrument_id);

// TODO(#85): Add |BarelySetAllInstrumentParamsToDefault| for all.

/// Sets all instrument parameters to default.
///
/// @param handle BarelyMusician handle.
/// @param instrument_id Instrument id.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetAllInstrumentParamsToDefault(
    BarelyHandle handle, BarelyId instrument_id);

// TODO(#85): Add |SetConductor|.

// TODO(#85): Add |SetCustomInstrumentData|.

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

/// Sets performer begin offset.
///
/// @param handle BarelyMusician handle.
/// @param performer_id Performer id.
/// @param begin_offset Begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetPerformerBeginOffset(BarelyHandle handle,
                                                         BarelyId performer_id,
                                                         double begin_offset);

/// Sets performer begin position.
///
/// @param handle BarelyMusician handle.
/// @param performer_id Performer id.
/// @param begin_position Optional begin position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetPerformerBeginPosition(
    BarelyHandle handle, BarelyId performer_id, double* begin_position);

/// Sets performer end position.
///
/// @param handle BarelyMusician handle.
/// @param performer_id Performer id.
/// @param end_position Optional end position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetPerformerEndPosition(BarelyHandle handle,
                                                         BarelyId performer_id,
                                                         double* end_position);

/// Sets whether performer should be looping or not.
///
/// @param handle BarelyMusician handle.
/// @param performer_id Performer id.
/// @param loop True if looping.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetPerformerLoop(BarelyHandle handle,
                                                  BarelyId performer_id,
                                                  bool loop);

/// Sets performer loop begin offset.
///
/// @param handle BarelyMusician handle.
/// @param performer_id Performer id.
/// @param loop_begin_offset Loop begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetPerformerLoopBeginOffset(
    BarelyHandle handle, BarelyId performer_id, double loop_begin_offset);

/// Sets performer loop length.
///
/// @param handle BarelyMusician handle.
/// @param performer_id Performer id.
/// @param loop_length Loop length in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetPerformerLoopLength(BarelyHandle handle,
                                                        BarelyId performer_id,
                                                        double loop_length);

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
