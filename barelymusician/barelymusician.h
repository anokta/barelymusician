#ifndef BARELYMUSICIAN_BARELYMUSICIAN_H_
#define BARELYMUSICIAN_BARELYMUSICIAN_H_

// NOLINTBEGIN
#include <stdbool.h>
#include <stdint.h>

#if defined(_WIN32) || defined(__CYGWIN__)
#ifdef BARELYMUSICIAN_EXPORTS
#ifdef __GNUC__
#define BARELY_EXPORT __attribute__((dllexport))
#else  // __GNUC__
#define BARELY_EXPORT __declspec(dllexport)
#endif  // __GNUC__
#else   // BARELYMUSICIAN_EXPORTS
#ifdef __GNUC__
#define BARELY_EXPORT __attribute__((dllimport))
#else  // __GNUC__
#define BARELY_EXPORT __declspec(dllimport)
#endif  // __GNUC__
#endif  // BARELYMUSICIAN_EXPORTS
#else   // defined(_WIN32) || defined(__CYGWIN__)
#if __GNUC__ >= 4
#define BARELY_EXPORT __attribute__((visibility("default")))
#else  // __GNUC__ >= 4
#define BARELY_EXPORT
#endif  // __GNUC__ >= 4
#endif  // defined(_WIN32) || defined(__CYGWIN__)

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Musician handle.
typedef struct BarelyMusician* BarelyMusicianHandle;

/// Identifier alias.
typedef int64_t BarelyId;

/// Identifier values.
enum BarelyId_Values {
  /// Invalid identifier.
  BarelyId_kInvalid = 0,
};

/// Status enum alias.
typedef int32_t BarelyStatus;

/// Status enum values.
enum BarelyStatus_Values {
  /// Success.
  BarelyStatus_kOk = 0,
  /// Invalid argument error.
  BarelyStatus_kInvalidArgument = 1,
  /// Not found error.
  BarelyStatus_kNotFound = 2,
  /// Already exists error.
  BarelyStatus_kAlreadyExists = 3,
  /// Unimplemented error.
  BarelyStatus_kUnimplemented = 4,
  /// Internal error.
  BarelyStatus_kInternal = 5,
  /// Unknown error.
  BarelyStatus_kUnknown = 6,
};

/// Data definition move callback signature.
///
/// @param other_data Other data to move.
/// @param out_data Output data.
typedef void (*BarelyDataDefinition_MoveCallback)(void* other_data,
                                                  void** out_data);

/// Data definition destroy callback signature.
///
/// @param data Data to destroy.
typedef void (*BarelyDataDefinition_DestroyCallback)(void* data);

/// Data definition.
typedef struct BarelyDataDefinition {
  /// Move callback.
  BarelyDataDefinition_MoveCallback move_callback;

  /// Destroy callback.
  BarelyDataDefinition_DestroyCallback destroy_callback;

  /// Data.
  void* data;
} BarelyDataDefinition;

/// Note pitch type enum alias.
typedef int32_t BarelyNotePitchType;

/// Note pitch type enum values.
enum BarelyNotePitchType_Values {
  /// Absolute pitch.
  BarelyNotePitchType_kAbsolutePitch = 0,
  /// Relative pitch with respect to root note.
  BarelyNotePitchType_kRelativePitch = 1,
  /// Scale index with respect to root note and scale.
  BarelyNotePitchType_kScaleIndex = 2,
};

/// Note definition pitch.
typedef struct BarelyNoteDefinition_Pitch {
  /// Type.
  BarelyNotePitchType type;

  /// Value.
  union {
    /// Absolute pitch.
    double absolute_pitch;
    /// Relative pitch.
    double relative_pitch;
    /// Scale index.
    int32_t scale_index;
  };
} BarelyNoteDefinition_Pitch;

/// Note definition.
typedef struct BarelyNoteDefinition {
  /// Duration.
  double duration;

  /// Intensity.
  double intensity;

  /// Pitch.
  BarelyNoteDefinition_Pitch pitch;
} BarelyNoteDefinition;

/// Parameter automation definition.
typedef struct BarelyParameterAutomationDefinition {
  /// Index.
  int32_t index;

  /// Value.
  double value;
} BarelyParameterAutomationDefinition;

/// Parameter definition.
typedef struct BarelyParameterDefinition {
  /// Default value.
  double default_value;

  /// Minimum value.
  double min_value;

  /// Maximum value.
  double max_value;
} BarelyParameterDefinition;

/// Instrument create callback signature.
///
/// @param state Pointer to instrument state.
/// @param frame_rate Frame rate in hz.
typedef void (*BarelyInstrumentDefinition_CreateCallback)(void** state,
                                                          int32_t frame_rate);

/// Instrument destroy callback signature.
///
/// @param state Pointer to instrument state.
typedef void (*BarelyInstrumentDefinition_DestroyCallback)(void** state);

/// Instrument process callback signature.
///
/// @param state Pointer to instrument state.
/// @param output Output buffer.
/// @param num_output_channels Number of channels.
/// @param num_output_frames Number of frames.
typedef void (*BarelyInstrumentDefinition_ProcessCallback)(
    void** state, double* output, int32_t num_output_channels,
    int32_t num_output_frames);

/// Instrument set data callback signature.
///
/// @param state Pointer to instrument state.
/// @param data Data.
typedef void (*BarelyInstrumentDefinition_SetDataCallback)(void** state,
                                                           void* data);

/// Instrument set note off callback signature.
///
/// @param state Pointer to instrument state.
/// @param pitch Note pitch.
typedef void (*BarelyInstrumentDefinition_SetNoteOffCallback)(void** state,
                                                              double pitch);

/// Instrument set note on callback signature.
///
/// @param state Pointer to instrument state.
/// @param pitch Note pitch.
/// @param intensity Note intensity.
typedef void (*BarelyInstrumentDefinition_SetNoteOnCallback)(void** state,
                                                             double pitch,
                                                             double intensity);

/// Instrument set parameter callback signature.
///
/// @param state Pointer to instrument state.
/// @param index Parameter index.
/// @param value Parameter value.
/// @param slope Parameter slope in value change per frame.
typedef void (*BarelyInstrumentDefinition_SetParameterCallback)(void** state,
                                                                int32_t index,
                                                                double value,
                                                                double slope);

/// Instrument definition.
typedef struct BarelyInstrumentDefinition {
  /// Create callback.
  BarelyInstrumentDefinition_CreateCallback create_callback;

  /// Destroy callback.
  BarelyInstrumentDefinition_DestroyCallback destroy_callback;

  /// Process callback.
  BarelyInstrumentDefinition_ProcessCallback process_callback;

  /// Set data callback.
  BarelyInstrumentDefinition_SetDataCallback set_data_callback;

  /// Set note off callback.
  BarelyInstrumentDefinition_SetNoteOffCallback set_note_off_callback;

  /// Set note on callback.
  BarelyInstrumentDefinition_SetNoteOnCallback set_note_on_callback;

  /// Set parameter callback.
  BarelyInstrumentDefinition_SetParameterCallback set_parameter_callback;

  /// List of parameter definitions.
  const BarelyParameterDefinition* parameter_definitions;

  /// Number of parameter definitions.
  int32_t num_parameter_definitions;
} BarelyInstrumentDefinition;

/// Instrument note off callback signature.
///
/// @param pitch Note pitch.
/// @param timestamp Note timestamp in seconds.
/// @param user_data User data.
typedef void (*BarelyInstrument_NoteOffCallback)(double pitch, double timestamp,
                                                 void* user_data);

/// Instrument note on callback signature.
///
/// @param pitch Note pitch.
/// @param intensity Note intensity.
/// @param timestamp Note timestamp in seconds.
/// @param user_data User data.
typedef void (*BarelyInstrument_NoteOnCallback)(double pitch, double intensity,
                                                double timestamp,
                                                void* user_data);

/// Musician adjust note callback signature.
///
/// @param definition Mutable note definition.
/// @param user_data User data.
typedef void (*BarelyMusician_AdjustNoteCallback)(
    BarelyNoteDefinition* definition, void* user_data);

/// Musician adjust parameter automation callback signature.
///
/// @param definition Mutable parameter automation definition.
/// @param user_data User data.
typedef void (*BarelyMusician_AdjustParameterAutomationCallback)(
    BarelyParameterAutomationDefinition* definition, void* user_data);

/// Musician adjust tempo callback signature.
///
/// @param tempo Mutable tempo in bpm.
/// @param user_data User data.
typedef void (*BarelyMusician_AdjustTempoCallback)(double* tempo,
                                                   void* user_data);

/// Musician beat callback signature.
///
/// @param position Beat position in beats.
/// @param timestamp Beat timestamp in seconds.
/// @param user_data User data.
typedef void (*BarelyMusician_BeatCallback)(double position, double timestamp,
                                            void* user_data);

/// Creates new instrument.
///
/// @param handle Musician handle.
/// @param definition Instrument definition.
/// @param frame_rate Frame rate in hz.
/// @param out_instrument_id Output instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_Create(
    BarelyMusicianHandle handle, BarelyInstrumentDefinition definition,
    int32_t frame_rate, BarelyId* out_instrument_id);

/// Destroys instrument.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_Destroy(BarelyMusicianHandle handle,
                                                    BarelyId instrument_id);

/// Gets instrument parameter value.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param index Parameter index.
/// @param out_value Output parameter value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_GetParameter(
    BarelyMusicianHandle handle, BarelyId instrument_id, int32_t index,
    double* out_value);

/// Gets instrument parameter definition.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param index Parameter index.
/// @param out_definition Output parameter definition.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_GetParameterDefinition(
    BarelyMusicianHandle handle, BarelyId instrument_id, int32_t index,
    BarelyParameterDefinition* out_definition);

/// Gets whether instrument note is active or not.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @param out_is_note_on Output true if active, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_IsNoteOn(BarelyMusicianHandle handle, BarelyId instrument_id,
                          double pitch, bool* out_is_note_on);

/// Processes instrument output buffer at timestamp.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param output Output buffer.
/// @param num_output_channels Number of output channels.
/// @param num_output_frames Number of output frames.
/// @param timestamp Timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_Process(
    BarelyMusicianHandle handle, BarelyId instrument_id, double* output,
    int32_t num_output_channels, int32_t num_output_frames, double timestamp);

/// Resets all instrument parameters to default value.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_ResetAllParameters(
    BarelyMusicianHandle handle, BarelyId instrument_id);

/// Resets instrument parameter to default value.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param index Parameter index.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_ResetParameter(
    BarelyMusicianHandle handle, BarelyId instrument_id, int32_t index);

/// Sets instrument data.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param definition Data definition.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_SetData(BarelyMusicianHandle handle, BarelyId instrument_id,
                         BarelyDataDefinition definition);

/// Sets instrument note off callback.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param callback Note off callback.
/// @param user_data User data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetNoteOffCallback(
    BarelyMusicianHandle handle, BarelyId instrument_id,
    BarelyInstrument_NoteOffCallback callback, void* user_data);

/// Sets instrument note on callback.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param callback Note on callback.
/// @param user_data User data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetNoteOnCallback(
    BarelyMusicianHandle handle, BarelyId instrument_id,
    BarelyInstrument_NoteOnCallback callback, void* user_data);

/// Sets instrument parameter value.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param index Parameter index.
/// @param value Parameter value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetParameter(
    BarelyMusicianHandle handle, BarelyId instrument_id, int32_t index,
    double value);

/// Starts instrument note.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @param intensity Note intensity.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_StartNote(BarelyMusicianHandle handle, BarelyId instrument_id,
                           double pitch, double intensity);

/// Stops all instrument notes.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_StopAllNotes(
    BarelyMusicianHandle handle, BarelyId instrument_id);

/// Stops instrument note.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_StopNote(
    BarelyMusicianHandle handle, BarelyId instrument_id, double pitch);

/// Creates new musician.
///
/// @param out_handle Output musician handle.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyMusician_Create(BarelyMusicianHandle* out_handle);

/// Destroys musician.
///
/// @param handle Musician handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyMusician_Destroy(BarelyMusicianHandle handle);

/// Gets musician note.
///
/// @param handle Musician handle.
/// @param pitch Note pitch.
/// @param out_pitch Output note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyMusician_GetNote(BarelyMusicianHandle handle,
                       BarelyNoteDefinition_Pitch pitch, double* out_pitch);

/// Gets musician position.
///
/// @param handle Musician handle.
/// @param out_position Output position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyMusician_GetPosition(BarelyMusicianHandle handle, double* out_position);

/// Gets musician root note.
///
/// @param handle Musician handle.
/// @param out_root_pitch Output root note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyMusician_GetRootNote(BarelyMusicianHandle handle, double* out_root_pitch);

/// Gets musician scale.
///
/// @param handle Musician handle.
/// @param out_scale_pitches Output list of scale note pitches.
/// @param out_num_scale_pitches Output number of scale note pitches.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyMusician_GetScale(BarelyMusicianHandle handle, double** out_scale_pitches,
                        int32_t* out_num_scale_pitches);

/// Gets musician tempo.
///
/// @param handle Musician handle.
/// @param out_tempo Output tempo in bpm.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyMusician_GetTempo(BarelyMusicianHandle handle,
                                                   double* out_tempo);

/// Gets musician timestamp.
///
/// @param handle Musician handle.
/// @param out_timestamp Output timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyMusician_GetTimestamp(BarelyMusicianHandle handle, double* out_timestamp);

/// Gets musician timestamp at position.
///
/// @param handle Musician handle.
/// @param position Position in beats.
/// @param out_timestamp Output timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyMusician_GetTimestampAtPosition(
    BarelyMusicianHandle handle, double position, double* out_timestamp);

/// Gets whether musician is playing or not.
///
/// @param handle Musician handle.
/// @param out_is_playing Output true if playing, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyMusician_IsPlaying(BarelyMusicianHandle handle,
                                                    bool* out_is_playing);

/// Sets musician adjust note callback.
///
/// @param handle Musician handle.
/// @param callback Adjust note callback.
/// @param user_data User data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyMusician_SetAdjustNoteCallback(
    BarelyMusicianHandle handle, BarelyMusician_AdjustNoteCallback callback,
    void* user_data);

/// Sets musician adjust parameter automation callback.
///
/// @param handle Musician handle.
/// @param callback Adjust parameter automation callback.
/// @param user_data User data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyMusician_SetAdjustParameterAutomationCallback(
    BarelyMusicianHandle handle,
    BarelyMusician_AdjustParameterAutomationCallback callback, void* user_data);

/// Sets musician adjust tempo callback.
///
/// @param handle Musician handle.
/// @param callback Adjust tempo callback.
/// @param user_data User data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyMusician_SetAdjustTempoCallback(
    BarelyMusicianHandle handle, BarelyMusician_AdjustTempoCallback callback,
    void* user_data);

/// Sets musician beat callback.
///
/// @param handle Musician handle.
/// @param callback Beat callback.
/// @param user_data User data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyMusician_SetBeatCallback(
    BarelyMusicianHandle handle, BarelyMusician_BeatCallback callback,
    void* user_data);

/// Sets musician position.
///
/// @param handle Musician handle.
/// @param position Position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyMusician_SetPosition(BarelyMusicianHandle handle, double position);

/// Sets musician root note.
///
/// @param handle Musician handle.
/// @param root_pitch Root note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyMusician_SetRootNote(BarelyMusicianHandle handle, double root_pitch);

/// Sets musician scale.
///
/// @param handle Musician handle.
/// @param scale_pitches List of scale note pitches.
/// @param num_scale_pitches Number of scale note pitches.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyMusician_SetScale(BarelyMusicianHandle handle,
                                                   double* scale_pitches,
                                                   int32_t num_scale_pitches);

/// Sets musician tempo.
///
/// @param handle Musician handle.
/// @param tempo Tempo in bpm.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyMusician_SetTempo(BarelyMusicianHandle handle,
                                                   double tempo);

/// Sets musician timestamp.
///
/// @param handle Musician handle.
/// @param timestamp Timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyMusician_SetTimestamp(BarelyMusicianHandle handle, double timestamp);

/// Starts musician playback.
///
/// @param handle Musician handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyMusician_Start(BarelyMusicianHandle handle);

/// Stops musician playback.
///
/// @param handle Musician handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyMusician_Stop(BarelyMusicianHandle handle);

/// Updates musician at timestamp.
///
/// @param handle Musician handle.
/// @param timestamp Timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyMusician_Update(BarelyMusicianHandle handle,
                                                 double timestamp);

/// Adds sequence note at position.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param definition Note definition.
/// @param position Note position in beats.
/// @param out_note_id Output note handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_AddNote(
    BarelyMusicianHandle handle, BarelyId sequence_id,
    BarelyNoteDefinition definition, double position, BarelyId* out_note_id);

/// Adds sequence parameter automation at position.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param definition Parameter automation definition.
/// @param position Parameter automation position in beats.
/// @param out_parameter_automation_id Output parameter automation handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_AddParameterAutomation(
    BarelyMusicianHandle handle, BarelyId sequence_id,
    BarelyParameterAutomationDefinition definition, double position,
    BarelyId* out_parameter_automation_id);

/// Creates new sequence.
///
/// @param out_sequence_id Output sequence identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_Create(BarelyMusicianHandle handle,
                                                 BarelyId* out_sequence_id);

/// Destroys sequence.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_Destroy(BarelyMusicianHandle handle,
                                                  BarelyId sequence_id);

/// Gets sequence begin offset in beats.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param out_begin_offset Output begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelySequence_GetBeginOffset(BarelyMusicianHandle handle, BarelyId sequence_id,
                              double* out_begin_offset);

/// Gets sequence begin position in beats.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param out_begin_position Output begin position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetBeginPosition(
    BarelyMusicianHandle handle, BarelyId sequence_id,
    double* out_begin_position);

/// Gets sequence end position in beats.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param out_end_position Output end position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelySequence_GetEndPosition(BarelyMusicianHandle handle, BarelyId sequence_id,
                              double* out_end_position);

/// Gets sequence instrument.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param out_instrument_id Output instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelySequence_GetInstrument(BarelyMusicianHandle handle, BarelyId sequence_id,
                             BarelyId* out_instrument_id);

/// Gets sequence loop begin offset.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param out_loop_begin_offset Output loop begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetLoopBeginOffset(
    BarelyMusicianHandle handle, BarelyId sequence_id,
    double* out_loop_begin_offset);

/// Gets sequence loop length.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param out_loop_length Output loop length.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetLoopLength(
    BarelyMusicianHandle handle, BarelyId sequence_id, double* out_loop_length);

/// Gets sequence note definition.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param note_id Note identifier.
/// @param out_definition Output note definition.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetNoteDefinition(
    BarelyMusicianHandle handle, BarelyId sequence_id, BarelyId note_id,
    BarelyNoteDefinition* out_definition);

/// Gets sequence note position.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param note_id Note identifier.
/// @param out_position Output note position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetNotePosition(
    BarelyMusicianHandle handle, BarelyId sequence_id, BarelyId note_id,
    double* out_position);

/// Gets sequence parameter automation definition.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param parameter_automation_id Parameter automation identifier.
/// @param out_definition Output parameter automation definition.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetParameterAutomationDefinition(
    BarelyMusicianHandle handle, BarelyId sequence_id,
    BarelyId parameter_automation_id,
    BarelyParameterAutomationDefinition* out_definition);

/// Gets sequence parameter automation position.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param parameter_automation_id Parameter automation identifier.
/// @param out_position Output parameter automation position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetParameterAutomationPosition(
    BarelyMusicianHandle handle, BarelyId sequence_id,
    BarelyId parameter_automation_id, double* out_position);

/// Gets whether sequence is empty or not.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param out_is_empty Output true if empty, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_IsEmpty(BarelyMusicianHandle handle,
                                                  BarelyId sequence_id,
                                                  bool* out_is_empty);

/// Gets whether sequence is looping or not.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param out_is_looping Output true if looping, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_IsLooping(BarelyMusicianHandle handle,
                                                    BarelyId sequence_id,
                                                    bool* out_is_looping);

/// Performs sequence at range.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param musician_handle Musician handle.
/// @param begin_position Begin position in beats.
/// @param end_position End position in beats.
BARELY_EXPORT BarelyStatus
BarelySequence_Perform(BarelyMusicianHandle handle, BarelyId sequence_id,
                       BarelyMusicianHandle musician_handle,
                       double begin_position, double end_position);

/// Removes all sequence notes.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_RemoveAllNotes(
    BarelyMusicianHandle handle, BarelyId sequence_id);

/// Removes sequence note at position.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param position Position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_RemoveAllNotesAtPosition(
    BarelyMusicianHandle handle, BarelyId sequence_id, double position);

/// Removes all sequence notes at range.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param begin_position Begin position in beats.
/// @param end_position End position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_RemoveAllNotesAtRange(
    BarelyMusicianHandle handle, BarelyId sequence_id, double begin_position,
    double end_position);

/// Removes all sequence parameter automations.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_RemoveAllParameterAutomations(
    BarelyMusicianHandle handle, BarelyId sequence_id);

/// Removes all sequence parameter automations at position.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param position Position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelySequence_RemoveAllParameterAutomationsAtPosition(
    BarelyMusicianHandle handle, BarelyId sequence_id, double position);

/// Removes all sequence parameter automations at range.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param begin_position Begin position in beats.
/// @param end_position End position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_RemoveAllParameterAutomationsAtRange(
    BarelyMusicianHandle handle, BarelyId sequence_id, double begin_position,
    double end_position);

/// Removes sequence note.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param note_id Note identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_RemoveNote(
    BarelyMusicianHandle handle, BarelyId sequence_id, BarelyId note_id);

/// Removes sequence parameter automation.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param parameter_automation_id Parameter automation identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_RemoveParameterAutomation(
    BarelyMusicianHandle handle, BarelyId sequence_id,
    BarelyId parameter_automation_id);

/// Sets sequence begin offset.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param begin_offset Begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetBeginOffset(
    BarelyMusicianHandle handle, BarelyId sequence_id, double begin_offset);

/// Sets sequence begin position.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param begin_position Begin position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetBeginPosition(
    BarelyMusicianHandle handle, BarelyId sequence_id, double begin_position);

/// Sets sequence end position.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param end_position End position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetEndPosition(
    BarelyMusicianHandle handle, BarelyId sequence_id, double end_position);

/// Sets sequence instrument.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param instrument_id Instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetInstrument(
    BarelyMusicianHandle handle, BarelyId sequence_id, BarelyId instrument_id);

/// Sets sequence loop begin offset.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param loop_begin_offset Loop begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetLoopBeginOffset(
    BarelyMusicianHandle handle, BarelyId sequence_id,
    double loop_begin_offset);

/// Sets sequence loop length.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param loop_length Loop length in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetLoopLength(
    BarelyMusicianHandle handle, BarelyId sequence_id, double loop_length);

/// Sets whether sequence should be looping or not.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param is_looping True if looping, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetLooping(
    BarelyMusicianHandle handle, BarelyId sequence_id, bool is_looping);

/// Sets sequence note definition.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param note_id Note identifier.
/// @param definition Note definition.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetNoteDefinition(
    BarelyMusicianHandle handle, BarelyId sequence_id, BarelyId note_id,
    BarelyNoteDefinition definition);

/// Sets sequence note position.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param note_id Note identifier.
/// @param position Note position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetNotePosition(
    BarelyMusicianHandle handle, BarelyId sequence_id, BarelyId note_id,
    double position);

/// Sets sequence parameter automation definition.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param parameter_automation_id Parameter automation identifier.
/// @param definition Parameter automation definition.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetParameterAutomationDefinition(
    BarelyMusicianHandle handle, BarelyId sequence_id,
    BarelyId parameter_automation_id,
    BarelyParameterAutomationDefinition definition);

/// Sets sequence parameter automation position.
///
/// @param handle Musician handle.
/// @param sequence_id Sequence identifier.
/// @param parameter_automation_id Parameter automation identifier.
/// @param position Parameter automation position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetParameterAutomationPosition(
    BarelyMusicianHandle handle, BarelyId sequence_id,
    BarelyId parameter_automation_id, double position);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
// NOLINTEND

#ifdef __cplusplus
#include <cassert>
#include <functional>
#include <limits>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace barely {

/// Status.
enum class Status : BarelyStatus {
  /// Success.
  kOk = BarelyStatus_kOk,
  /// Invalid argument error.
  kInvalidArgument = BarelyStatus_kInvalidArgument,
  /// Not found error.
  kNotFound = BarelyStatus_kNotFound,
  /// Already exists error.
  kAlreadyExists = BarelyStatus_kAlreadyExists,
  /// Unimplemented error.
  kUnimplemented = BarelyStatus_kUnimplemented,
  /// Internal error.
  kInternal = BarelyStatus_kInternal,
  /// Unknown error.
  kUnknown = BarelyStatus_kUnknown,
};

/// Returns whether status is okay or not.
///
/// @param status Status.
/// @return True if okay, false otherwise.
inline bool IsOk(Status status) { return status == Status::kOk; }

/// Returns status string.
///
/// @param status Status.
/// @return Status string.
inline std::string ToString(Status status) {
  switch (status) {
    case Status::kOk:
      return "Ok";
    case Status::kInvalidArgument:
      return "Invalid argument error";
    case Status::kNotFound:
      return "Not found error";
    case Status::kAlreadyExists:
      return "Already exists error";
    case Status::kUnimplemented:
      return "Unimplemented error";
    case Status::kInternal:
      return "Internal error";
    case Status::kUnknown:
    default:
      return "Unknown error";
  }
}

/// Value or error status.
template <typename ValueType>
class StatusOr {
 public:
  /// Constructs new `StatusOr` with an error status.
  ///
  /// @param error_status Error status.
  // NOLINTNEXTLINE(google-explicit-constructor)
  StatusOr(Status error_status) : value_or_(error_status) {
    assert(error_status != Status::kOk);
  }

  /// Constructs new `StatusOr` with a value.
  ///
  /// @param value Value.
  // NOLINTNEXTLINE(google-explicit-constructor)
  StatusOr(ValueType value) : value_or_(std::move(value)) {}

  /// Returns contained error status.
  ///
  /// @return Error status.
  [[nodiscard]] Status GetErrorStatus() const {
    assert(std::holds_alternative<Status>(value_or_));
    return std::get<Status>(value_or_);
  }

  /// Returns contained value.
  ///
  /// @return Value.
  [[nodiscard]] const ValueType& GetValue() const {
    assert(std::holds_alternative<ValueType>(value_or_));
    return std::get<ValueType>(value_or_);
  }

  /// Returns contained value.
  ///
  /// @return Mutable value.
  [[nodiscard]] ValueType& GetValue() {
    assert(std::holds_alternative<ValueType>(value_or_));
    return std::get<ValueType>(value_or_);
  }

  /// Returns whether value is contained or not.
  ///
  /// @return True if contained, false otherwise.
  [[nodiscard]] bool IsOk() const {
    return std::holds_alternative<ValueType>(value_or_);
  }

 private:
  // Value or error status.
  std::variant<Status, ValueType> value_or_;
};

/// Data definition.
struct DataDefinition : public BarelyDataDefinition {
  /// Move callback signature.
  using MoveCallback = BarelyDataDefinition_MoveCallback;

  /// Destroy callback signature.
  using DestroyCallback = BarelyDataDefinition_DestroyCallback;

  /// Constructs new `DataDefinition` of type.
  ///
  /// @param typed_data Typed data.
  template <typename DataType>
  explicit DataDefinition(DataType typed_data)
      : BarelyDataDefinition{
            [](void* other_data, void** out_data) {
              *out_data = static_cast<void*>(
                  new DataType(std::move(*static_cast<DataType*>(other_data))));
            },
            [](void* data) { delete static_cast<DataType*>(data); },
            static_cast<void*>(&typed_data)} {}

  /// Constructs new `DataDefinition` from internal type.
  ///
  /// @param definition Internal data definition.
  // NOLINTNEXTLINE(google-explicit-constructor)
  DataDefinition(BarelyDataDefinition definition)
      : BarelyDataDefinition{definition} {}
};

/// Note definition.
// TODO: Refactor this similar to other definitions.
using NoteDefinition = BarelyNoteDefinition;

/// Parameter definition.
struct ParameterDefinition : public BarelyParameterDefinition {
  /// Constructs new `ParameterDefinition`.
  ///
  /// @param default_value Default value.
  /// @param min_value Minimum value.
  /// @param max_value Maximum value.
  explicit ParameterDefinition(
      double default_value,
      double min_value = std::numeric_limits<double>::lowest(),
      double max_value = std::numeric_limits<double>::max())
      : BarelyParameterDefinition{default_value, min_value, max_value} {}

  /// Constructs new `ParameterDefinition` for a boolean value.
  ///
  /// @param default_value Default boolean value.
  explicit ParameterDefinition(bool default_value)
      : ParameterDefinition(static_cast<double>(default_value)) {}

  /// Constructs new `ParameterDefinition` for an integer value.
  ///
  /// @param default_value Default integer value.
  /// @param min_value Minimum integer value.
  /// @param max_value Maximum integer value.
  explicit ParameterDefinition(
      int default_value, int min_value = std::numeric_limits<int>::lowest(),
      int max_value = std::numeric_limits<int>::max())
      : ParameterDefinition(static_cast<double>(default_value),
                            static_cast<double>(min_value),
                            static_cast<double>(max_value)) {}

  /// Constructs new `ParameterDefinition` from internal type.
  ///
  /// @param definition Internal parameter definition.
  // NOLINTNEXTLINE(google-explicit-constructor)
  ParameterDefinition(BarelyParameterDefinition definition)
      : BarelyParameterDefinition{definition} {}
};

/// Instrument definition.
struct InstrumentDefinition : public BarelyInstrumentDefinition {
  /// Create callback signature.
  using CreateCallback = BarelyInstrumentDefinition_CreateCallback;

  /// Destroy callback signature.
  using DestroyCallback = BarelyInstrumentDefinition_DestroyCallback;

  /// Process callback signature.
  using ProcessCallback = BarelyInstrumentDefinition_ProcessCallback;

  /// Set data callback signature.
  using SetDataCallback = BarelyInstrumentDefinition_SetDataCallback;

  /// Set note off callback signature
  using SetNoteOffCallback = BarelyInstrumentDefinition_SetNoteOffCallback;

  /// Set note on callback signature.
  using SetNoteOnCallback = BarelyInstrumentDefinition_SetNoteOnCallback;

  /// Set parameter callback signature.
  using SetParameterCallback = BarelyInstrumentDefinition_SetParameterCallback;

  /// Constructs new `InstrumentDefinition`.
  ///
  /// @param create_callback Create callback.
  /// @param destroy_callback Destroy callback.
  /// @param process_callback Process callback.
  /// @param set_data_callback Set data callback.
  /// @param set_note_off_callback Set note off callback.
  /// @param set_note_on_callback Set note on callback.
  /// @param set_parameter_callback Set parameter callback.
  /// @param parameter_definitions List of parameter definitions.
  InstrumentDefinition(
      CreateCallback create_callback, DestroyCallback destroy_callback,
      ProcessCallback process_callback, SetDataCallback set_data_callback,
      SetNoteOffCallback set_note_off_callback,
      SetNoteOnCallback set_note_on_callback,
      SetParameterCallback set_parameter_callback = nullptr,
      const std::vector<ParameterDefinition>& parameter_definitions = {})
      : BarelyInstrumentDefinition{
            create_callback,
            destroy_callback,
            process_callback,
            set_data_callback,
            set_note_off_callback,
            set_note_on_callback,
            set_parameter_callback,
            parameter_definitions.data(),
            static_cast<int>(parameter_definitions.size())} {}

  /// Constructs new `InstrumentDefinition` from internal type.
  ///
  /// @param definition Internal instrument definition.
  // NOLINTNEXTLINE(google-explicit-constructor)
  InstrumentDefinition(BarelyInstrumentDefinition definition)
      : BarelyInstrumentDefinition{definition} {}
};

/// Instrument.
class Instrument {
 public:
  /// Note off callback signature.
  ///
  /// @param pitch Note pitch.
  /// @param timestamp Note timestamp in seconds.
  using NoteOffCallback = std::function<void(double pitch, double timestamp)>;

  /// Note on callback signature.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  /// @param timestamp Note timestamp in seconds.
  using NoteOnCallback =
      std::function<void(double pitch, double intensity, double timestamp)>;

  /// Destroys `Instrument`.
  ~Instrument() {
    if (id_ != BarelyId_kInvalid) {
      const auto status =
          BarelyInstrument_Destroy(std::exchange(handle_, nullptr),
                                   std::exchange(id_, BarelyId_kInvalid));
      assert(IsOk(static_cast<Status>(status)));
    }
  }

  /// Non-copyable.
  Instrument(const Instrument& other) = delete;
  Instrument& operator=(const Instrument& other) = delete;

  /// Constructs new `Instrument` via move.
  ///
  /// @param other Other instrument.
  Instrument(Instrument&& other) noexcept
      : handle_(std::exchange(other.handle_, nullptr)),
        id_(std::exchange(other.id_, BarelyId_kInvalid)) {
    SetNoteOffCallback(std::exchange(other.note_off_callback_, nullptr));
    SetNoteOnCallback(std::exchange(other.note_on_callback_, nullptr));
  }

  /// Assigns `Instrument` via move.
  ///
  /// @param other Other instrument.
  Instrument& operator=(Instrument&& other) noexcept {
    if (this != &other) {
      if (id_ != BarelyId_kInvalid) {
        const auto status = BarelyInstrument_Destroy(handle_, id_);
        assert(IsOk(static_cast<Status>(status)));
      }
      handle_ = std::exchange(other.handle_, nullptr);
      id_ = std::exchange(other.id_, BarelyId_kInvalid);
      SetNoteOffCallback(std::exchange(other.note_off_callback_, nullptr));
      SetNoteOnCallback(std::exchange(other.note_on_callback_, nullptr));
    }
    return *this;
  }

  /// Returns parameter value.
  ///
  /// @param index Parameter index.
  /// @return Parameter value, or error status.
  [[nodiscard]] StatusOr<double> GetParameter(int index) const {
    double value = 0.0;
    if (const auto status = static_cast<Status>(
            BarelyInstrument_GetParameter(handle_, id_, index, &value));
        !IsOk(status)) {
      return status;
    }
    return value;
  }

  /// Returns parameter definition.
  ///
  /// @param index Parameter index.
  /// @return Parameter definition, or error status.
  [[nodiscard]] StatusOr<ParameterDefinition> GetParameterDefinition(
      int index) const {
    BarelyParameterDefinition definition;
    if (const auto status =
            static_cast<Status>(BarelyInstrument_GetParameterDefinition(
                handle_, id_, index, &definition));
        !IsOk(status)) {
      return status;
    }
    return ParameterDefinition(definition);
  }

  /// Returns whether note is active or not.
  ///
  /// @param pitch Note pitch.
  /// @return True if active, false otherwise.
  [[nodiscard]] bool IsNoteOn(double pitch) const {
    bool is_note_on = false;
    const auto status =
        BarelyInstrument_IsNoteOn(handle_, id_, pitch, &is_note_on);
    assert(IsOk(static_cast<Status>(status)));
    return is_note_on;
  }

  /// Processes output buffer at timestamp.
  ///
  /// @param output Output buffer.
  /// @param num_output_channels Number of output channels.
  /// @param num_output_frames Number of output frames.
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status Process(double* output, int num_output_channels, int num_output_frames,
                 double timestamp) {
    return static_cast<Status>(
        BarelyInstrument_Process(handle_, id_, output, num_output_channels,
                                 num_output_frames, timestamp));
  }

  /// Resets all parameters.
  ///
  /// @return Status.
  Status ResetAllParameters() {
    return static_cast<Status>(
        BarelyInstrument_ResetAllParameters(handle_, id_));
  }

  /// Resets parameter value.
  ///
  /// @param index Parameter index.
  /// @return Status.
  Status ResetParameter(int index) {
    return static_cast<Status>(
        BarelyInstrument_ResetParameter(handle_, id_, index));
  }

  /// Sets data.
  ///
  /// @param data Data.
  /// @return Status.
  template <typename DataType>
  Status SetData(DataType data) {
    return static_cast<Status>(BarelyInstrument_SetData(
        handle_, id_, DataDefinition(std::move(data))));
  }

  /// Sets note off callback.
  ///
  /// @param callback Note off callback.
  /// @return Status.
  Status SetNoteOffCallback(NoteOffCallback callback) {
    if (callback) {
      note_off_callback_ = std::move(callback);
      return static_cast<Status>(BarelyInstrument_SetNoteOffCallback(
          handle_, id_,
          [](double pitch, double timestamp, void* user_data) {
            (*static_cast<NoteOffCallback*>(user_data))(pitch, timestamp);
          },
          static_cast<void*>(&note_off_callback_)));
    }
    return static_cast<Status>(
        BarelyInstrument_SetNoteOffCallback(handle_, id_, nullptr, nullptr));
  }

  /// Sets note on callback.
  ///
  /// @param callback Note on callback.
  /// @return Status.
  Status SetNoteOnCallback(NoteOnCallback callback) {
    if (callback) {
      note_on_callback_ = std::move(callback);
      return static_cast<Status>(BarelyInstrument_SetNoteOnCallback(
          handle_, id_,
          [](double pitch, double intensity, double timestamp,
             void* user_data) {
            (*static_cast<NoteOnCallback*>(user_data))(pitch, intensity,
                                                       timestamp);
          },
          static_cast<void*>(&note_on_callback_)));
    }
    return static_cast<Status>(
        BarelyInstrument_SetNoteOnCallback(handle_, id_, nullptr, nullptr));
  }

  /// Sets parameter value.
  ///
  /// @param index Parameter index.
  /// @param value Parameter value.
  /// @return Status.
  Status SetParameter(int index, double value) {
    return static_cast<Status>(
        BarelyInstrument_SetParameter(handle_, id_, index, value));
  }

  /// Starts note.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  /// @return Status.
  Status StartNote(double pitch, double intensity = 1.0) {
    return static_cast<Status>(
        BarelyInstrument_StartNote(handle_, id_, pitch, intensity));
  }

  /// Stops all notes.
  ///
  /// @return Status.
  Status StopAllNotes() {
    return static_cast<Status>(BarelyInstrument_StopAllNotes(handle_, id_));
  }

  /// Stops note.
  ///
  /// @param pitch Note pitch.
  /// @return Status.
  Status StopNote(double pitch) {
    return static_cast<Status>(BarelyInstrument_StopNote(handle_, id_, pitch));
  }

 private:
  friend class Musician;
  friend class Sequence;

  // Constructs new `Instrument`.
  explicit Instrument(BarelyMusicianHandle handle,
                      InstrumentDefinition definition, int frame_rate)
      : handle_(handle) {
    const auto status =
        BarelyInstrument_Create(handle_, definition, frame_rate, &id_);
    assert(IsOk(static_cast<Status>(status)));
  }

  // Internal musician handle.
  BarelyMusicianHandle handle_ = nullptr;

  // Identifier.
  BarelyId id_ = BarelyId_kInvalid;

  // Note off callback.
  NoteOffCallback note_off_callback_;

  // Note on callback.
  NoteOnCallback note_on_callback_;
};

/// Sequence.
class Sequence {
 public:
  /// Destroys `Sequence`.
  ~Sequence() {
    if (id_ != BarelyId_kInvalid) {
      const auto status =
          BarelySequence_Destroy(std::exchange(handle_, nullptr),
                                 std::exchange(id_, BarelyId_kInvalid));
      assert(IsOk(static_cast<Status>(status)));
    }
  }

  /// Non-copyable.
  Sequence(const Sequence& other) = delete;
  Sequence& operator=(const Sequence& other) = delete;

  /// Constructs new `Sequence` via move.
  ///
  /// @param other Other sequence.
  Sequence(Sequence&& other) noexcept
      : handle_(std::exchange(other.handle_, nullptr)) {}

  /// Assigns `Sequence` via move.
  ///
  /// @param other Other sequence.
  Sequence& operator=(Sequence&& other) noexcept {
    if (this != &other) {
      if (handle_) {
        BarelySequence_Destroy(handle_, id_);
      }
      handle_ = std::exchange(other.handle_, nullptr);
    }
    return *this;
  }

  /// Adds note at position.
  ///
  /// @param position Note position.
  /// @param definition Note definition.
  /// @return Note reference.
  // TODO: refactor note api type.
  BarelyId AddNote(double position, NoteDefinition definition) {
    BarelyId note_id = BarelyId_kInvalid;
    const auto status =
        BarelySequence_AddNote(handle_, id_, definition, position, &note_id);
    assert(IsOk(static_cast<Status>(status)));
    return note_id;
  }

  // TODO: add parameter automation functions.

  /// Returns begin offset.
  ///
  /// @return Begin offset in beats.
  [[nodiscard]] double GetBeginOffset() const {
    double begin_offset = 0.0;
    const auto status =
        BarelySequence_GetBeginOffset(handle_, id_, &begin_offset);
    assert(IsOk(static_cast<Status>(status)));
    return begin_offset;
  }

  /// Returns begin position.
  ///
  /// @return Begin position in beats.
  [[nodiscard]] double GetBeginPosition() const {
    double begin_position = 0.0;
    const auto status =
        BarelySequence_GetBeginPosition(handle_, id_, &begin_position);
    assert(IsOk(static_cast<Status>(status)));
    return begin_position;
  }

  /// Returns end position.
  ///
  /// @return End position in beats.
  [[nodiscard]] double GetEndPosition() const {
    double end_position = 0.0;
    const auto status =
        BarelySequence_GetEndPosition(handle_, id_, &end_position);
    assert(IsOk(static_cast<Status>(status)));
    return end_position;
  }

  /// Returns instrument.
  ///
  /// @return Pointer to instrument, or nullptr.
  [[nodiscard]] const Instrument* GetInstrument() const { return instrument_; }

  /// Returns loop begin offset.
  ///
  /// @return Loop begin offset in beats.
  [[nodiscard]] double GetLoopBeginOffset() const {
    double loop_begin_offset = 0.0;
    const auto status =
        BarelySequence_GetLoopBeginOffset(handle_, id_, &loop_begin_offset);
    assert(IsOk(static_cast<Status>(status)));
    return loop_begin_offset;
  }

  /// Returns loop length.
  ///
  /// @return Loop length in beats.
  [[nodiscard]] double GetLoopLength() const {
    double loop_length = 0.0;
    const auto status =
        BarelySequence_GetLoopLength(handle_, id_, &loop_length);
    assert(IsOk(static_cast<Status>(status)));
    return loop_length;
  }

  /// Returns whether sequence is empty or not.
  ///
  /// @return True if empty, false otherwise.
  [[nodiscard]] bool IsEmpty() const {
    bool is_empty = false;
    const auto status = BarelySequence_IsEmpty(handle_, id_, &is_empty);
    assert(IsOk(static_cast<Status>(status)));
    return is_empty;
  }

  /// Returns whether sequence should be looping or not.
  ///
  /// @return True if looping, false otherwise.
  [[nodiscard]] bool IsLooping() const {
    bool is_looping = false;
    const auto status = BarelySequence_IsLooping(handle_, id_, &is_looping);
    assert(IsOk(static_cast<Status>(status)));
    return is_looping;
  }

  /// Removes all notes.
  ///
  /// @return Status.
  Status RemoveAllNotes() {
    return static_cast<Status>(BarelySequence_RemoveAllNotes(handle_, id_));
  }

  /// Removes all notes at position.
  ///
  /// @param position Position in beats.
  /// @return Status.
  Status RemoveAllNotes(double position) {
    return static_cast<Status>(
        BarelySequence_RemoveAllNotesAtPosition(handle_, id_, position));
  }

  /// Removes all notes at range.
  ///
  /// @param begin_position Begin position in beats.
  /// @param end_position End position in beats.
  /// @return Status.
  Status RemoveAllNotes(double begin_position, double end_position) {
    return static_cast<Status>(BarelySequence_RemoveAllNotesAtRange(
        handle_, id_, begin_position, end_position));
  }

  /// Removes note.
  ///
  /// @param note Note reference.
  /// @return Status.
  // TODO: refactor note api type.
  Status RemoveNote(BarelyId note) {
    return static_cast<Status>(BarelySequence_RemoveNote(handle_, id_, note));
  }

  /// Sets begin offset.
  ///
  /// @param begin_offset Begin offset in beats.
  /// @return Status.
  Status SetBeginOffset(double begin_offset) {
    return static_cast<Status>(
        BarelySequence_SetBeginOffset(handle_, id_, begin_offset));
  }

  /// Sets begin position.
  ///
  /// @param begin_position Begin position in beats.
  /// @return Status.
  Status SetBeginPosition(double begin_position) {
    return static_cast<Status>(
        BarelySequence_SetBeginPosition(handle_, id_, begin_position));
  }

  /// Sets end position.
  ///
  /// @param end_position End position in beats.
  /// @return Status.
  Status SetEndPosition(double end_position) {
    return static_cast<Status>(
        BarelySequence_SetEndPosition(handle_, id_, end_position));
  }

  /// Sets instrument.
  ///
  /// @param instrument Pointer to instrument, or nullptr.
  /// @return Status.
  Status SetInstrument(const Instrument* instrument) {
    instrument_ = instrument;
    return static_cast<Status>(BarelySequence_SetInstrument(
        handle_, id_, instrument_ ? instrument_->id_ : BarelyId_kInvalid));
  }

  /// Sets loop begin offset.
  ///
  /// @param loop_begin_offset Loop begin offset in beats.
  /// @return Status.
  Status SetLoopBeginOffset(double loop_begin_offset) {
    return static_cast<Status>(
        BarelySequence_SetLoopBeginOffset(handle_, id_, loop_begin_offset));
  }

  /// Sets loop length.
  ///
  /// @param loop_length Loop length in beats.
  /// @return Status.
  Status SetLoopLength(double loop_length) {
    return static_cast<Status>(
        BarelySequence_SetLoopLength(handle_, id_, loop_length));
  }

  /// Sets whether sequence should be looping or not.
  ///
  /// @param is_looping True if looping, false otherwise.
  /// @return Status.
  Status SetLooping(bool is_looping) {
    return static_cast<Status>(
        BarelySequence_SetLooping(handle_, id_, is_looping));
  }

 private:
  friend class Musician;

  // Constructs new `Sequence`.
  explicit Sequence(BarelyMusicianHandle handle) : handle_(handle) {
    const auto status = BarelySequence_Create(handle_, &id_);
    assert(IsOk(static_cast<Status>(status)));
  }

  // Internal musician handle.
  BarelyMusicianHandle handle_ = nullptr;

  // Identifier.
  BarelyId id_ = BarelyId_kInvalid;

  // Instrument.
  const Instrument* instrument_ = nullptr;
};

/// Musician.
class Musician {
 public:
  /// Beat callback signature.
  ///
  /// @param position Beat position in beats.
  /// @param timestamp Beat timestamp in seconds.
  using BeatCallback = std::function<void(double position, double timestamp)>;

  /// Constructs new `Musician`.
  Musician() {
    const auto status = BarelyMusician_Create(&handle_);
    assert(IsOk(static_cast<Status>(status)));
  }

  /// Destroys `Musician`.
  ~Musician() {
    if (handle_) {
      const auto status =
          BarelyMusician_Destroy(std::exchange(handle_, nullptr));
      assert(IsOk(static_cast<Status>(status)));
    }
  }

  /// Non-copyable.
  Musician(const Musician& other) = delete;
  Musician& operator=(const Musician& other) = delete;

  /// Constructs new `Musician` via move.
  ///
  /// @param other Other musician.
  Musician(Musician&& other) noexcept
      : handle_(std::exchange(other.handle_, nullptr)) {
    SetBeatCallback(std::exchange(other.beat_callback_, nullptr));
  }

  /// Assigns `Musician` via move.
  ///
  /// @param other Other musician.
  Musician& operator=(Musician&& other) noexcept {
    if (this != &other) {
      if (handle_) {
        const auto status = BarelyMusician_Destroy(handle_);
        assert(IsOk(static_cast<Status>(status)));
      }
      handle_ = std::exchange(other.handle_, nullptr);
      SetBeatCallback(std::exchange(other.beat_callback_, nullptr));
    }
    return *this;
  }

  /// Creates new instrument.
  ///
  /// @param definition Instrument definition.
  /// @param frame_rate Frame rate in hz.
  /// @return Instrument.
  Instrument CreateInstrument(InstrumentDefinition definition, int frame_rate) {
    return Instrument(handle_, definition, frame_rate);
  }

  /// Creates new sequence.
  ///
  /// @return Sequence.
  Sequence CreateSequence() { return Sequence(handle_); }

  /// Returns position.
  ///
  /// @return Position in beats.
  [[nodiscard]] double GetPosition() const {
    double position = 0.0;
    if (handle_) {
      const auto status = BarelyMusician_GetPosition(handle_, &position);
      assert(IsOk(static_cast<Status>(status)));
    }
    return position;
  }

  /// Returns tempo.
  ///
  /// @return Tempo in bpm.
  [[nodiscard]] double GetTempo() const {
    double tempo = 0.0;
    const auto status = BarelyMusician_GetTempo(handle_, &tempo);
    assert(IsOk(static_cast<Status>(status)));
    return tempo;
  }

  /// Returns timestamp.
  ///
  /// @return Timestamp in seconds.
  [[nodiscard]] double GetTimestamp() const {
    double timestamp = 0.0;
    const auto status = BarelyMusician_GetTimestamp(handle_, &timestamp);
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
        BarelyMusician_GetTimestampAtPosition(handle_, position, &timestamp);
    assert(IsOk(static_cast<Status>(status)));
    return timestamp;
  }

  /// Returns whether musician is playing or not.
  ///
  /// @return True if playing, false otherwise.
  [[nodiscard]] bool IsPlaying() const {
    bool is_playing = false;
    const auto status = BarelyMusician_IsPlaying(handle_, &is_playing);
    assert(status == BarelyStatus_kOk);
    return is_playing;
  }

  /// Sets beat callback.
  ///
  /// @param beat_callback Beat callback.
  /// @return Status.
  Status SetBeatCallback(BeatCallback callback) {
    if (callback) {
      beat_callback_ = std::move(callback);
      return static_cast<Status>(BarelyMusician_SetBeatCallback(
          handle_,
          [](double beat, double timestamp, void* user_data) {
            (*static_cast<BeatCallback*>(user_data))(beat, timestamp);
          },
          static_cast<void*>(&beat_callback_)));
    }
    return static_cast<Status>(
        BarelyMusician_SetBeatCallback(handle_, nullptr, nullptr));
  }

  /// Sets position.
  ///
  /// @param position Position in beats.
  /// @return Status.
  Status SetPosition(double position) {
    return static_cast<Status>(BarelyMusician_SetPosition(handle_, position));
  }

  /// Sets tempo.
  ///
  /// @param tempo Tempo in bpm.
  /// @return Status.
  Status SetTempo(double tempo) {
    return static_cast<Status>(BarelyMusician_SetTempo(handle_, tempo));
  }

  /// Sets timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status SetTimestamp(double timestamp) {
    return static_cast<Status>(BarelyMusician_SetTimestamp(handle_, timestamp));
  }

  /// Starts playback.
  ///
  /// @return Status.
  Status Start() { return static_cast<Status>(BarelyMusician_Start(handle_)); }

  /// Stops playback.
  ///
  /// @return Status.
  Status Stop() { return static_cast<Status>(BarelyMusician_Stop(handle_)); }

  /// Updates internal state at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status Update(double timestamp) {
    return static_cast<Status>(BarelyMusician_Update(handle_, timestamp));
  }

 private:
  // Internal handle.
  BarelyMusicianHandle handle_ = nullptr;

  // Beat callback.
  BeatCallback beat_callback_;
};

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_BARELYMUSICIAN_H_
