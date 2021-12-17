#ifndef PLATFORMS_CAPI_BARELYMUSICIAN_H_
#define PLATFORMS_CAPI_BARELYMUSICIAN_H_

#include <stdint.h>

#include "platforms/capi/visibility.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Handle type.
typedef struct BarelyMusician* BarelyHandle;

/// Optional double value type.
typedef struct BarelyOptionalDouble {
  /// Denotes whether the value should be used or not.
  bool has_value;

  /// Optional value.
  double value;
} BarelyOptionalDouble;

/// Optional float value type.
typedef struct BarelyOptionalFloat {
  /// Denotes whether the value should be used or not.
  bool has_value;

  /// Optional value.
  float value;
} BarelyOptionalFloat;

/// Status type.
typedef int32_t BarelyStatus;
enum BarelyStatusValues {
  /// Success.
  kBarelyStatus_Ok = 0,
  /// Invalid argument error.
  kBarelyStatus_InvalidArgument = 1,
  /// Not found error.
  kBarelyStatus_NotFound = 2,
  /// Already exists error.
  kBarelyStatus_AlreadyExists = 3,
  /// Failed precondition error.
  kBarelyStatus_FailedPrecondition = 4,
  /// Unimplemented error.
  kBarelyStatus_Unimplemented = 5,
  /// Internal error.
  kBarelyStatus_Internal = 6,
  /// Unknown error.
  kBarelyStatus_Unknown = 7,
};

// TODO(#85): Remove this in favor of context specific id types.
/// Id type.
typedef int64_t BarelyId;

/// Parameter id type.
typedef int32_t BarelyParamId;

// TODO(#85): Should these structs be further refactored?
/// Parameter definition.
typedef struct BarelyParamDefinition {
  /// Default value.
  float default_value;

  /// Optional maximum value.
  BarelyOptionalFloat max_value;

  /// Optional minimum value.
  BarelyOptionalFloat min_value;
} BarelyParamDefinition;

typedef struct BarelyParamIdDefinitionPair {
  /// Id.
  BarelyId id;

  /// Definition.
  BarelyParamDefinition definition;
} BarelyParamIdDefinitionPair;

typedef struct BarelyParamDefinitionMap {
  /// Definition pairs.
  BarelyParamIdDefinitionPair* definitions;

  /// Number of definition pairs.
  int32_t num_definitions;
} BarelyParamDefinitionMap;

// TODO(#85): Add |BarelyConductorDefinition|.

/// Instrument id type.
// TODO(#85): Use this instead of |BarelyId|.
// typedef int64_t BarelyInstrumentId;

/// Instrument state type.
typedef void* BarelyInstrumentState;

/// Instrument create function signature.
///
/// @param state Pointer to instrument state.
/// @param sample_rate Sampling rate in Hz.
typedef void (*BarelyInstrumentCreateFn)(BarelyInstrumentState* state,
                                         int32_t sample_rate);

/// Instrument destroy function signature.
///
/// @param state Pointer to instrument state.
typedef void (*BarelyInstrumentDestroyFn)(BarelyInstrumentState* state);

/// Instrument process function signature.
///
/// @param state Pointer to instrument state.
/// @param output Pointer to output buffer.
/// @param num_channels Number of channels.
/// @param num_frames Number of frames.
typedef void (*BarelyInstrumentProcessFn)(BarelyInstrumentState* state,
                                          float* output, int32_t num_channels,
                                          int32_t num_frames);

/// Instrument set custom data function signature.
///
/// @param state Pointer to instrument state.
/// @param custom_data Custom data.
typedef void (*BarelyInstrumentSetCustomDataFn)(BarelyInstrumentState* state,
                                                void* custom_data);

/// Instrument set note off function signature.
///
/// @param state Pointer to instrument state.
/// @param pitch Note pitch.
typedef void (*BarelyInstrumentSetNoteOffFn)(BarelyInstrumentState* state,
                                             float pitch);

/// Instrument set note on function signature.
///
/// @param state Pointer to instrument state.
/// @param pitch Note pitch.
/// @param intensity Note intensity.
typedef void (*BarelyInstrumentSetNoteOnFn)(BarelyInstrumentState* state,
                                            float pitch, float intensity);

/// Instrument set parameter function signature.
///
/// @param state Pointer to instrument state.
/// @param id Parameter id.
/// @param value Parameter value.
typedef void (*BarelyInstrumentSetParamFn)(BarelyInstrumentState* state,
                                           BarelyParamId id, float value);

/// Instrument definition.
typedef struct BarelyInstrumentDefinition {
  /// Create function.
  BarelyInstrumentCreateFn create_fn;

  /// Destroy function.
  BarelyInstrumentDestroyFn destroy_fn;

  /// Process function.
  BarelyInstrumentProcessFn process_fn;

  /// Set custom_data function.
  BarelyInstrumentSetCustomDataFn set_custom_data_fn;

  /// Set note off function.
  BarelyInstrumentSetNoteOffFn set_note_off_fn;

  /// Set note on function.
  BarelyInstrumentSetNoteOnFn set_note_on_fn;

  /// Set parameter function.
  BarelyInstrumentSetParamFn set_param_fn;
} BarelyInstrumentDefinition;

/// Instrument note off callback signature.
///
/// @param instrument_id Instrument id.
/// @param timestamp Timestamp in seconds.
/// @param note_pitch Note pitch.
typedef void (*BarelyInstrumentNoteOffCallback)(BarelyId instrument_id,
                                                double timestamp,
                                                float note_pitch);

/// Instrument note on callback signature.
///
/// @param instrument_id Instrument id.
/// @param timestamp Timestamp in seconds.
/// @param note_pitch Note pitch.
/// @param note_intensity Note intensity.
typedef void (*BarelyInstrumentNoteOnCallback)(BarelyId instrument_id,
                                               double timestamp,
                                               float note_pitch,
                                               float note_intensity);

// TODO(#85): Should this be more generic (beyond the beat granularity)?
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
/// @param definition Instrument definition.
/// @param param_definitions Instrument parameter definitions.
/// @param instrument_id_ptr Pointer to instrument id.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyAddInstrument(
    BarelyHandle handle, BarelyInstrumentDefinition definition,
    BarelyParamDefinitionMap param_definitions, BarelyId* instrument_id_ptr);

// TODO(#85): Temporary shortcut to test instruments.
BARELY_EXPORT BarelyStatus
BarelyAddSynthInstrument(BarelyHandle handle, BarelyId* instrument_id_ptr);

/// Adds new performer.
///
/// @param handle BarelyMusician handle.
/// @param performer_id_ptr Pointer to performer id.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyAddPerformer(BarelyHandle handle,
                                              BarelyId* performer_id_ptr);

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
/// @param note_id_ptr Pointer to note id.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyAddPerformerNote(
    BarelyHandle handle, BarelyId performer_id, double note_position,
    double note_duration, float note_pitch, float note_intensity,
    BarelyId* note_id_ptr);

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

/// Gets performer begin offset.
///
/// @param handle BarelyMusician handle.
/// @param performer_id Performer id.
/// @param begin_offset_ptr Pointer to begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyGetPerformerBeginOffset(
    BarelyHandle handle, BarelyId performer_id, double* begin_offset_ptr);

/// Gets performer begin position.
///
/// @param handle BarelyMusician handle.
/// @param performer_id Performer id.
/// @param begin_position_ptr Pointer to begin position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyGetPerformerBeginPosition(
    BarelyHandle handle, BarelyId performer_id, double* begin_position_ptr);

/// Gets performer end position.
///
/// @param handle BarelyMusician handle.
/// @param performer_id Performer id.
/// @param end_position_ptr Pointer to end position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyGetPerformerEndPosition(
    BarelyHandle handle, BarelyId performer_id, double* end_position_ptr);

/// Gets performer loop begin offset.
///
/// @param handle BarelyMusician handle.
/// @param performer_id Performer id.
/// @param loop_begin_offset_ptr Pointer to loop begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyGetPerformerLoopBeginOffset(
    BarelyHandle handle, BarelyId performer_id, double* begin_offset_ptr);

/// Gets performer loop length.
///
/// @param handle BarelyMusician handle.
/// @param performer_id Performer id.
/// @param loop_length_ptr Pointer to loop length in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyGetPerformerLoopLength(
    BarelyHandle handle, BarelyId performer_id, double* loop_length_ptr);

/// Gets the playback position.
///
/// @param handle BarelyMusician handle.
/// @param position_ptr Pointer to position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyGetPlaybackPosition(BarelyHandle handle,
                                                     double* position_ptr);

/// Gets the playback tempo.
///
/// @param handle BarelyMusician handle.
/// @param tempo_ptr Pointer to tempo in BPM.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyGetPlaybackTempo(BarelyHandle handle,
                                                  double* tempo_ptr);

/// Gets whether the performer is empty or not.
///
/// @param handle BarelyMusician handle.
/// @param performer_id Performer id.
/// @param is_empty_ptr Pointer to true if empty, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyIsPerformerEmpty(BarelyHandle handle,
                                                  BarelyId performer_id,
                                                  bool* is_empty_ptr);

/// Gets whether the performer is looping or not.
///
/// @param handle BarelyMusician handle.
/// @param performer_id Performer id.
/// @param is_looping_ptr Ponter to true if looping, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyIsPerformerLooping(BarelyHandle handle,
                                                    BarelyId performer_id,
                                                    bool* is_looping_ptr);

/// Gets whether the playback is currently active or not.
///
/// @param handle BarelyMusician handle.
/// @param is_playing_ptr Pointer to true if playing, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyIsPlaying(BarelyHandle handle,
                                           bool* is_playing_ptr);

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
                                                    BarelyParamId param_id,
                                                    float param_value);

/// Sets instrument parameter to default.
///
/// @param handle BarelyMusician handle.
/// @param instrument_id Instrument id.
/// @param param_id Parameter id.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetInstrumentParamToDefault(
    BarelyHandle handle, BarelyId instrument_id, BarelyParamId param_id);

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
