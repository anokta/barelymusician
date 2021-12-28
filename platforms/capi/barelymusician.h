#ifndef PLATFORMS_CAPI_BARELYMUSICIAN_H_
#define PLATFORMS_CAPI_BARELYMUSICIAN_H_

#include <stdint.h>

#include "platforms/capi/visibility.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// BarelyMusician C API type.
typedef struct BarelyMusician* BarelyApi;

/// Status type.
typedef int32_t BarelyStatus;

/// Status values.
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

/// Id type.
typedef int64_t BarelyId;

/// Parameter definition.
typedef struct BarelyParamDefinition {
  /// Default value.
  float default_value;

  /// Minimum value.
  float min_value;

  /// Maximum value.
  float max_value;
} BarelyParamDefinition;

// TODO(#85): Add |BarelyConductorDefinition|.

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
/// @param output Output buffer.
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
/// @param index Parameter index.
/// @param value Parameter value.
typedef void (*BarelyInstrumentSetParamFn)(BarelyInstrumentState* state,
                                           int32_t index, float value);

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

  /// Number of parameter definitions.
  int32_t num_param_definitions;

  /// List of parameter definitions.
  BarelyParamDefinition* param_definitions;
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

/// Adds new performer.
///
/// @param api BarelyMusician API.
/// @param performer_id_ptr Pointer to performer id.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyAddPerformer(BarelyApi api,
                                              BarelyId* performer_id_ptr);

/// Adds performer instrument.
///
/// @param api BarelyMusician API.
/// @param performer_id Performer id.
/// @param instrument_id Instrument id.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyAddPerformerInstrument(BarelyApi api,
                                                        BarelyId performer_id,
                                                        BarelyId instrument_id);

/// Adds performer note.
///
/// @param api BarelyMusician API.
/// @param performer_id Performer id.
/// @param note_position Note position.
/// @param note_duration Note duration.
/// @param note_pitch Note pitch.
/// @param note_intensity Note intensity.
/// @param note_id_ptr Pointer to note id.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyAddPerformerNote(
    BarelyApi api, BarelyId performer_id, double note_position,
    double note_duration, float note_pitch, float note_intensity,
    BarelyId* note_id_ptr);

/// Creates new BarelyMusician API.
///
/// @param sample_rate Sampling rate in Hz.
/// @return BarelyMusician API.
BARELY_EXPORT BarelyApi BarelyCreateApi(int32_t sample_rate);

/// Create new instrument.
///
/// @param api BarelyMusician API.
/// @param definition Instrument definition.
/// @param instrument_id_ptr Pointer to instrument id.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyCreateInstrument(BarelyApi api, BarelyInstrumentDefinition definition,
                       BarelyId* instrument_id_ptr);

// TODO(#85): Temporary shortcut to test instruments.
BARELY_EXPORT BarelyStatus
BarelyCreateSynthInstrument(BarelyApi api, BarelyId* instrument_id_ptr);

/// Destroys BarelyMusician API.
///
/// @param api BarelyMusician API.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyDestroyApi(BarelyApi api);

/// Destroys instrument.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument id.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyDestroyInstrument(BarelyApi api,
                                                   BarelyId instrument_id);

/// Gets performer begin offset.
///
/// @param api BarelyMusician API.
/// @param performer_id Performer id.
/// @param begin_offset_ptr Pointer to begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyGetPerformerBeginOffset(
    BarelyApi api, BarelyId performer_id, double* begin_offset_ptr);

/// Gets performer begin position.
///
/// @param api BarelyMusician API.
/// @param performer_id Performer id.
/// @param begin_position_ptr Pointer to optional begin position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyGetPerformerBeginPosition(
    BarelyApi api, BarelyId performer_id, double* begin_position_ptr);

/// Gets performer end position.
///
/// @param api BarelyMusician API.
/// @param performer_id Performer id.
/// @param end_position_ptr Pointer to optional end position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyGetPerformerEndPosition(
    BarelyApi api, BarelyId performer_id, double* end_position_ptr);

/// Gets performer loop begin offset.
///
/// @param api BarelyMusician API.
/// @param performer_id Performer id.
/// @param loop_begin_offset_ptr Pointer to loop begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyGetPerformerLoopBeginOffset(
    BarelyApi api, BarelyId performer_id, double* begin_offset_ptr);

/// Gets performer loop length.
///
/// @param api BarelyMusician API.
/// @param performer_id Performer id.
/// @param loop_length_ptr Pointer to loop length in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyGetPerformerLoopLength(
    BarelyApi api, BarelyId performer_id, double* loop_length_ptr);

/// Gets the playback position.
///
/// @param api BarelyMusician API.
/// @param position_ptr Pointer to position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyGetPlaybackPosition(BarelyApi api,
                                                     double* position_ptr);

/// Gets the playback tempo.
///
/// @param api BarelyMusician API.
/// @param tempo_ptr Pointer to tempo in BPM.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyGetPlaybackTempo(BarelyApi api,
                                                  double* tempo_ptr);

/// Gets whether the performer is empty or not.
///
/// @param api BarelyMusician API.
/// @param performer_id Performer id.
/// @param is_empty_ptr Pointer to true if empty, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyIsPerformerEmpty(BarelyApi api,
                                                  BarelyId performer_id,
                                                  bool* is_empty_ptr);

/// Gets whether the performer is looping or not.
///
/// @param api BarelyMusician API.
/// @param performer_id Performer id.
/// @param is_looping_ptr Ponter to true if looping, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyIsPerformerLooping(BarelyApi api,
                                                    BarelyId performer_id,
                                                    bool* is_looping_ptr);

/// Gets whether the playback is currently active or not.
///
/// @param api BarelyMusician API.
/// @param is_playing_ptr Pointer to true if playing, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyIsPlaying(BarelyApi api, bool* is_playing_ptr);

/// Processes the next instrument output buffer at timestamp.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument id.
/// @param timestamp Timestamp in seconds.
/// @param output Output buffer.
/// @param num_channels Number of output channels.
/// @param num_frames Number of output frames.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyProcessInstrument(
    BarelyApi api, BarelyId instrument_id, double timestamp, float* output,
    int32_t num_channels, int32_t num_frames);

/// Removes all performerinstruments.
///
/// @param api BarelyMusician API.
/// @param performer_id Performer id.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyRemoveAllPerformerInstruments(BarelyApi api, BarelyId performer_id);

/// Removes all performer notes.
///
/// @param api BarelyMusician API.
/// @param performer_id Performer id.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyRemoveAllPerformerNotes(BarelyApi api,
                                                         BarelyId performer_id);

/// Removes all performer notes at given range.
///
/// @param api BarelyMusician API.
/// @param performer_id Performer id.
/// @param begin_position Begin position in beats.
/// @param end_position End position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyRemoveAllPerformerNotesAt(BarelyApi api, BarelyId performer_id,
                                double begin_position, double end_position);

/// Removes performer.
///
/// @param api BarelyMusician API.
/// @param performer_id Performer id.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyRemovePerformer(BarelyApi api,
                                                 BarelyId performer_id);

/// Removes performer instrument.
///
/// @param api BarelyMusician API.
/// @param performer_id Performer id.
/// @param instrument_id Instrument id.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyRemovePerformerInstrument(
    BarelyApi api, BarelyId performer_id, BarelyId instrument_id);

/// Removes performer note.
///
/// @param api BarelyMusician API.
/// @param performer_id Performer id.
/// @param note_id Note id.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyRemovePerformerNote(BarelyApi api,
                                                     BarelyId performer_id,
                                                     BarelyId note_id);

// TODO(#85): Add |BarelySetAllInstrumentNotesOff| for all.

/// Sets all instrument notes off.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument id.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelySetAllInstrumentNotesOff(BarelyApi api, BarelyId instrument_id);

// TODO(#85): Add |BarelySetAllInstrumentParamsToDefault| for all.

/// Sets all instrument parameters to default.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument id.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelySetAllInstrumentParamsToDefault(BarelyApi api, BarelyId instrument_id);

// TODO(#85): Add |SetConductor|.

// TODO(#85): Add |SetCustomInstrumentData|.

/// Sets instrument note off.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument id.
/// @param note_pitch Note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetInstrumentNoteOff(BarelyApi api,
                                                      BarelyId instrument_id,
                                                      float note_pitch);

/// Sets the instrument note off callback.
///
/// @param api BarelyMusician API.
/// @param instrument_note_off_callback Instrument note off callback.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetInstrumentNoteOffCallback(
    BarelyApi api,
    BarelyInstrumentNoteOffCallback instrument_note_off_callback);

/// Sets instrument note on.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument id.
/// @param note_pitch Note pitch.
/// @param note_intensity Note intensity.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetInstrumentNoteOn(BarelyApi api,
                                                     BarelyId instrument_id,
                                                     float note_pitch,
                                                     float note_intensity);

/// Sets the instrument note on callback.
///
/// @param api BarelyMusician API.
/// @param instrument_note_on_callback Instrument note on callback.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetInstrumentNoteOnCallback(
    BarelyApi api, BarelyInstrumentNoteOnCallback instrument_note_on_callback);

/// Sets instrument parameter.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument id.
/// @param param_index Parameter index.
/// @param param_value Parameter value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetInstrumentParam(BarelyApi api,
                                                    BarelyId instrument_id,
                                                    int32_t param_index,
                                                    float param_value);

/// Sets instrument parameter to default.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument id.
/// @param param_index Parameter index.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetInstrumentParamToDefault(
    BarelyApi api, BarelyId instrument_id, int32_t param_index);

/// Sets performer begin offset.
///
/// @param api BarelyMusician API.
/// @param performer_id Performer id.
/// @param begin_offset Begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetPerformerBeginOffset(BarelyApi api,
                                                         BarelyId performer_id,
                                                         double begin_offset);

/// Sets performer begin position.
///
/// @param api BarelyMusician API.
/// @param performer_id Performer id.
/// @param begin_position Optional begin position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetPerformerBeginPosition(
    BarelyApi api, BarelyId performer_id, double* begin_position);

/// Sets performer end position.
///
/// @param api BarelyMusician API.
/// @param performer_id Performer id.
/// @param end_position Optional end position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetPerformerEndPosition(BarelyApi api,
                                                         BarelyId performer_id,
                                                         double* end_position);

/// Sets whether performer should be looping or not.
///
/// @param api BarelyMusician API.
/// @param performer_id Performer id.
/// @param loop True if looping.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetPerformerLoop(BarelyApi api,
                                                  BarelyId performer_id,
                                                  bool loop);

/// Sets performer loop begin offset.
///
/// @param api BarelyMusician API.
/// @param performer_id Performer id.
/// @param loop_begin_offset Loop begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetPerformerLoopBeginOffset(
    BarelyApi api, BarelyId performer_id, double loop_begin_offset);

/// Sets performer loop length.
///
/// @param api BarelyMusician API.
/// @param performer_id Performer id.
/// @param loop_length Loop length in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetPerformerLoopLength(BarelyApi api,
                                                        BarelyId performer_id,
                                                        double loop_length);

/// Sets the playback beat callback.
///
/// @param api BarelyMusician API.
/// @param playback_beat_callback Playback beat callback.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetPlaybackBeatCallback(
    BarelyApi api, BarelyPlaybackBeatCallback playback_beat_callback);

/// Sets the playback position.
///
/// @param api BarelyMusician API.
/// @param position Playback position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetPlaybackPosition(BarelyApi api,
                                                     double position);

/// Sets the playback tempo.
///
/// @param api BarelyMusician API.
/// @param tempo Playback tempo in BPM.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetPlaybackTempo(BarelyApi api, double tempo);

/// Sets the playback update callback.
///
/// @param api BarelyMusician API.
/// @param playback_update_callback Playback update callback.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetPlaybackUpdateCallback(
    BarelyApi api, BarelyPlaybackUpdateCallback playback_update_callback);

/// Sets the sample rate.
///
/// @param api BarelyMusician API.
/// @param sample_rate Sampling rate in Hz.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetSampleRate(BarelyApi api,
                                               int32_t sample_rate);

/// Starts the playback.
///
/// @param api BarelyMusician API.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyStartPlayback(BarelyApi api);

/// Stops the playback.
///
/// @param api BarelyMusician API.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyStopPlayback(BarelyApi api);

/// Updates the internal state at timestamp.
///
/// @param api BarelyMusician API.
/// @param timestamp Timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyUpdate(BarelyApi api, double timestamp);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // PLATFORMS_CAPI_BARELYMUSICIAN_H_
