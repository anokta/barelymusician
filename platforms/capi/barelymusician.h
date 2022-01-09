#ifndef PLATFORMS_CAPI_BARELYMUSICIAN_H_
#define PLATFORMS_CAPI_BARELYMUSICIAN_H_

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

/// Identifier alias.
typedef int64_t BarelyId;

/// Identifier values.
enum BarelyId_Values {
  /// Invalid identifier.
  BarelyId_kInvalid = -1,
};

/// Note pitch type enum alias.
typedef int32_t BarelyNotePitchType;

/// Note pitch type enum values.
enum BarelyNotePitchType_Values {
  /// Absolute pitch.
  BarelyNotePitchType_kAbsolutePitch = 0,
  /// Relative pitch with respect to conductor root note.
  BarelyNotePitchType_kRelativePitch = 1,
  /// Scale index with respect to conductor root note and scale.
  BarelyNotePitchType_kScaleIndex = 2,
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
  /// Failed precondition error.
  BarelyStatus_kFailedPrecondition = 4,
  /// Unimplemented error.
  BarelyStatus_kUnimplemented = 5,
  /// Internal error.
  BarelyStatus_kInternal = 6,
  /// Unknown error.
  BarelyStatus_kUnknown = 7,
};

/// Conductor adjust note duration function signature.
///
/// @param state Pointer to conductor state.
/// @param duration Pointer to note duration.
typedef void (*BarelyConductorDefinition_AdjustNoteDurationFn)(
    void** state, double* duration);

/// Conductor adjust note intensity function signature.
///
/// @param state Pointer to conductor state.
/// @param intensity Pointer to note intensity.
typedef void (*BarelyConductorDefinition_AdjustNoteIntensityFn)(
    void** state, float* intensity);

/// Conductor adjust note pitch function signature.
///
/// @param state Pointer to conductor state.
/// @param pitch_type Pointer to note pitch type.
/// @param pitch Pointer to note pitch.
typedef void (*BarelyConductorDefinition_AdjustNotePitchFn)(
    void** state, BarelyNotePitchType* pitch_type, float* pitch);

/// Conductor adjust tempo function signature.
///
/// @param state Pointer to conductor state.
/// @param tempo Pointer to tempo.
typedef void (*BarelyConductorDefinition_AdjustTempoFn)(void** state,
                                                        double* tempo);

/// Conductor create function signature.
///
/// @param state Pointer to conductor state.
typedef void (*BarelyConductorDefinition_CreateFn)(void** state);

/// Conductor destroy function signature.
///
/// @param state Pointer to conductor state.
typedef void (*BarelyConductorDefinition_DestroyFn)(void** state);

/// Conductor set data function signature.
///
/// @param state Pointer to conductor state.
/// @param data Data.
typedef void (*BarelyConductorDefinition_SetDataFn)(void** state, void* data);

/// Conductor set energy function signature.
///
/// @param state Pointer to conductor state.
/// @param energy Energy.
typedef void (*BarelyConductorDefinition_SetEnergyFn)(void** state,
                                                      float energy);

/// Conductor set parameter function signature.
///
/// @param state Pointer to conductor state.
/// @param index Parameter index.
/// @param value Parameter value.
typedef void (*BarelyConductorDefinition_SetParamFn)(void** state,
                                                     int32_t index,
                                                     float value);

/// Conductor set stress function signature.
///
/// @param state Pointer to conductor state.
/// @param stress Stress.
typedef void (*BarelyConductorDefinition_SetStressFn)(void** state,
                                                      float stress);

/// Instrument create function signature.
///
/// @param state Pointer to instrument state.
/// @param sample_rate Sampling rate in hz.
typedef void (*BarelyInstrumentDefinition_CreateFn)(void** state,
                                                    int32_t sample_rate);

/// Instrument destroy function signature.
///
/// @param state Pointer to instrument state.
typedef void (*BarelyInstrumentDefinition_DestroyFn)(void** state);

/// Instrument process function signature.
///
/// @param state Pointer to instrument state.
/// @param output Output buffer.
/// @param num_output_channels Number of channels.
/// @param num_output_frames Number of frames.
typedef void (*BarelyInstrumentDefinition_ProcessFn)(
    void** state, float* output, int32_t num_output_channels,
    int32_t num_output_frames);

/// Instrument set data function signature.
///
/// @param state Pointer to instrument state.
/// @param data Data.
typedef void (*BarelyInstrumentDefinition_SetDataFn)(void** state, void* data);

/// Instrument set note off function signature.
///
/// @param state Pointer to instrument state.
/// @param pitch Note pitch.
typedef void (*BarelyInstrumentDefinition_SetNoteOffFn)(void** state,
                                                        float pitch);

/// Instrument set note on function signature.
///
/// @param state Pointer to instrument state.
/// @param pitch Note pitch.
/// @param intensity Note intensity.
typedef void (*BarelyInstrumentDefinition_SetNoteOnFn)(void** state,
                                                       float pitch,
                                                       float intensity);

/// Instrument set parameter function signature.
///
/// @param state Pointer to instrument state.
/// @param index Parameter index.
/// @param value Parameter value.
typedef void (*BarelyInstrumentDefinition_SetParamFn)(void** state,
                                                      int32_t index,
                                                      float value);

/// BarelyMusician api.
typedef struct BarelyMusician* BarelyApi;

/// Note definition.
typedef struct BarelyNoteDefinition {
  /// Duration.
  double duration;

  /// Pitch type.
  BarelyNotePitchType pitch_type;

  /// Pitch value.
  float pitch;

  /// Intensity.
  float intensity;

  /// Denotes whether conductor adjustment should be bypassed or not.
  bool bypass_adjustment;
} BarelyNoteDefinition;

/// Parameter definition.
typedef struct BarelyParamDefinition {
  /// Default value.
  float default_value;

  /// Minimum value.
  float min_value;

  /// Maximum value.
  float max_value;
} BarelyParamDefinition;

/// Conductor definition.
typedef struct BarelyConductorDefinition {
  /// Adjust note duration function.
  BarelyConductorDefinition_AdjustNoteDurationFn adjust_note_duration_fn;

  /// Adjust note intensity function.
  BarelyConductorDefinition_AdjustNoteIntensityFn adjust_note_intensity_fn;

  /// Adjust note pitch function.
  BarelyConductorDefinition_AdjustNotePitchFn adjust_note_pitch_fn;

  /// Adjust tempo function.
  BarelyConductorDefinition_AdjustTempoFn adjust_tempo_fn;

  /// Create function.
  BarelyConductorDefinition_CreateFn create_fn;

  /// Destroy function.
  BarelyConductorDefinition_DestroyFn destroy_fn;

  /// Set data function.
  BarelyConductorDefinition_SetDataFn set_data_fn;

  /// Set energy function.
  BarelyConductorDefinition_SetEnergyFn set_energy_fn;

  /// Set parameter function.
  BarelyConductorDefinition_SetParamFn set_param_fn;

  /// Set stress function.
  BarelyConductorDefinition_SetStressFn set_stress_fn;

  /// List of parameter definitions.
  BarelyParamDefinition* param_definitions;

  /// Number of parameter definitions.
  int32_t num_param_definitions;
} BarelyConductorDefinition;

/// Instrument definition.
typedef struct BarelyInstrumentDefinition {
  /// Create function.
  BarelyInstrumentDefinition_CreateFn create_fn;

  /// Destroy function.
  BarelyInstrumentDefinition_DestroyFn destroy_fn;

  /// Process function.
  BarelyInstrumentDefinition_ProcessFn process_fn;

  /// Set data function.
  BarelyInstrumentDefinition_SetDataFn set_data_fn;

  /// Set note off function.
  BarelyInstrumentDefinition_SetNoteOffFn set_note_off_fn;

  /// Set note on function.
  BarelyInstrumentDefinition_SetNoteOnFn set_note_on_fn;

  /// Set parameter function.
  BarelyInstrumentDefinition_SetParamFn set_param_fn;

  /// List of parameter definitions.
  BarelyParamDefinition* param_definitions;

  /// Number of parameter definitions.
  int32_t num_param_definitions;
} BarelyInstrumentDefinition;

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
/// @param api BarelyMusician api.
/// @param out_performer_id Output performer id.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyAddPerformer(BarelyApi api,
                                              BarelyId* out_performer_id);

/// Adds performer note.
///
/// @param api BarelyMusician api.
/// @param performer_id Performer id.
/// @param note_position Note position.
/// @param note_duration Note duration.
/// @param note_pitch Note pitch.
/// @param note_intensity Note intensity.
/// @param out_note_id Output note id.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyAddPerformerNote(
    BarelyApi api, BarelyId performer_id, double note_position,
    double note_duration, float note_pitch, float note_intensity,
    BarelyId* out_note_id);

/// Creates new BarelyMusician api.
///
/// @param sample_rate Sampling rate in hz.
/// @return BarelyMusician api.
BARELY_EXPORT BarelyApi BarelyCreateApi(int32_t sample_rate);

/// Create new instrument.
///
/// @param api BarelyMusician api.
/// @param definition Instrument definition.
/// @param out_instrument_id Output instrument id.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyCreateInstrument(BarelyApi api, BarelyInstrumentDefinition definition,
                       BarelyId* out_instrument_id);

// TODO(#85): Temporary shortcut to test instruments.
BARELY_EXPORT BarelyStatus
BarelyCreateSynthInstrument(BarelyApi api, BarelyId* out_instrument_id);

/// Destroys BarelyMusician api.
///
/// @param api BarelyMusician api.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyDestroyApi(BarelyApi api);

/// Destroys instrument.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument id.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyDestroyInstrument(BarelyApi api,
                                                   BarelyId instrument_id);

/// Gets performer begin offset.
///
/// @param api BarelyMusician api.
/// @param performer_id Performer id.
/// @param out_begin_offset Output begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyGetPerformerBeginOffset(
    BarelyApi api, BarelyId performer_id, double* out_begin_offset);

/// Gets performer begin position.
///
/// @param api BarelyMusician api.
/// @param performer_id Performer id.
/// @param out_begin_position Output optional begin position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyGetPerformerBeginPosition(
    BarelyApi api, BarelyId performer_id, double* out_begin_position);

/// Gets performer end position.
///
/// @param api BarelyMusician api.
/// @param performer_id Performer id.
/// @param out_end_position Output optional end position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyGetPerformerEndPosition(
    BarelyApi api, BarelyId performer_id, double* out_end_position);

/// Gets performer loop begin offset.
///
/// @param api BarelyMusician api.
/// @param performer_id Performer id.
/// @param out_loop_begin_offset Output loop begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyGetPerformerLoopBeginOffset(
    BarelyApi api, BarelyId performer_id, double* out_begin_offset);

/// Gets performer loop length.
///
/// @param api BarelyMusician api.
/// @param performer_id Performer id.
/// @param out_loop_length Output loop length in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyGetPerformerLoopLength(
    BarelyApi api, BarelyId performer_id, double* out_loop_length);

/// Gets the playback position.
///
/// @param api BarelyMusician api.
/// @param out_position Output position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyGetPlaybackPosition(BarelyApi api,
                                                     double* out_position);

/// Gets the playback tempo.
///
/// @param api BarelyMusician api.
/// @param out_tempo Output tempo in bpm.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyGetPlaybackTempo(BarelyApi api,
                                                  double* out_tempo);

/// Gets whether the performer is empty or not.
///
/// @param api BarelyMusician api.
/// @param performer_id Performer id.
/// @param out_is_empty Output true if empty, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyIsPerformerEmpty(BarelyApi api,
                                                  BarelyId performer_id,
                                                  bool* out_is_empty);

/// Gets whether the performer is looping or not.
///
/// @param api BarelyMusician api.
/// @param performer_id Performer id.
/// @param out_is_looping Ponter to true if looping, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyIsPerformerLooping(BarelyApi api,
                                                    BarelyId performer_id,
                                                    bool* out_is_looping);

/// Gets whether the playback is currently active or not.
///
/// @param api BarelyMusician api.
/// @param out_is_playing Output true if playing, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyIsPlaying(BarelyApi api, bool* out_is_playing);

/// Processes the next instrument output buffer at timestamp.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument id.
/// @param timestamp Timestamp in seconds.
/// @param output Output buffer.
/// @param num_channels Number of output channels.
/// @param num_frames Number of output frames.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyProcessInstrument(
    BarelyApi api, BarelyId instrument_id, double timestamp, float* output,
    int32_t num_channels, int32_t num_frames);

/// Removes all performer notes.
///
/// @param api BarelyMusician api.
/// @param performer_id Performer id.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyRemoveAllPerformerNotes(BarelyApi api,
                                                         BarelyId performer_id);

/// Removes all performer notes at given range.
///
/// @param api BarelyMusician api.
/// @param performer_id Performer id.
/// @param begin_position Begin position in beats.
/// @param end_position End position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyRemoveAllPerformerNotesAt(BarelyApi api, BarelyId performer_id,
                                double begin_position, double end_position);

/// Removes performer.
///
/// @param api BarelyMusician api.
/// @param performer_id Performer id.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyRemovePerformer(BarelyApi api,
                                                 BarelyId performer_id);

/// Removes performer note.
///
/// @param api BarelyMusician api.
/// @param performer_id Performer id.
/// @param note_id Note id.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyRemovePerformerNote(BarelyApi api,
                                                     BarelyId performer_id,
                                                     BarelyId note_id);

// TODO(#85): Add |BarelySetAllInstrumentNotesOff| for all.

/// Sets all instrument notes off.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument id.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelySetAllInstrumentNotesOff(BarelyApi api, BarelyId instrument_id);

// TODO(#85): Add |BarelySetAllInstrumentParamsToDefault| for all.

/// Sets all instrument parameters to default.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument id.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelySetAllInstrumentParamsToDefault(BarelyApi api, BarelyId instrument_id);

// TODO(#85): Add |SetConductor|.

// TODO(#85): Add |SetCustomInstrumentData|.

/// Sets instrument note off.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument id.
/// @param note_pitch Note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetInstrumentNoteOff(BarelyApi api,
                                                      BarelyId instrument_id,
                                                      float note_pitch);

/// Sets the instrument note off callback.
///
/// @param api BarelyMusician api.
/// @param instrument_note_off_callback Instrument note off callback.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetInstrumentNoteOffCallback(
    BarelyApi api,
    BarelyInstrumentNoteOffCallback instrument_note_off_callback);

/// Sets instrument note on.
///
/// @param api BarelyMusician api.
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
/// @param api BarelyMusician api.
/// @param instrument_note_on_callback Instrument note on callback.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetInstrumentNoteOnCallback(
    BarelyApi api, BarelyInstrumentNoteOnCallback instrument_note_on_callback);

/// Sets instrument parameter.
///
/// @param api BarelyMusician api.
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
/// @param api BarelyMusician api.
/// @param instrument_id Instrument id.
/// @param param_index Parameter index.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetInstrumentParamToDefault(
    BarelyApi api, BarelyId instrument_id, int32_t param_index);

/// Sets performer begin offset.
///
/// @param api BarelyMusician api.
/// @param performer_id Performer id.
/// @param begin_offset Begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetPerformerBeginOffset(BarelyApi api,
                                                         BarelyId performer_id,
                                                         double begin_offset);

/// Sets performer begin position.
///
/// @param api BarelyMusician api.
/// @param performer_id Performer id.
/// @param begin_position Optional begin position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetPerformerBeginPosition(
    BarelyApi api, BarelyId performer_id, double* begin_position);

/// Sets performer end position.
///
/// @param api BarelyMusician api.
/// @param performer_id Performer id.
/// @param end_position Optional end position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetPerformerEndPosition(BarelyApi api,
                                                         BarelyId performer_id,
                                                         double* end_position);

/// Sets performer instrument.
///
/// @param api BarelyMusician api.
/// @param performer_id Performer id.
/// @param instrument_id Instrument id.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetPerformerInstrument(BarelyApi api,
                                                        BarelyId performer_id,
                                                        BarelyId instrument_id);

/// Sets whether performer should be looping or not.
///
/// @param api BarelyMusician api.
/// @param performer_id Performer id.
/// @param loop True if looping.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetPerformerLoop(BarelyApi api,
                                                  BarelyId performer_id,
                                                  bool loop);

/// Sets performer loop begin offset.
///
/// @param api BarelyMusician api.
/// @param performer_id Performer id.
/// @param loop_begin_offset Loop begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetPerformerLoopBeginOffset(
    BarelyApi api, BarelyId performer_id, double loop_begin_offset);

/// Sets performer loop length.
///
/// @param api BarelyMusician api.
/// @param performer_id Performer id.
/// @param loop_length Loop length in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetPerformerLoopLength(BarelyApi api,
                                                        BarelyId performer_id,
                                                        double loop_length);

/// Sets the playback beat callback.
///
/// @param api BarelyMusician api.
/// @param playback_beat_callback Playback beat callback.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetPlaybackBeatCallback(
    BarelyApi api, BarelyPlaybackBeatCallback playback_beat_callback);

/// Sets the playback position.
///
/// @param api BarelyMusician api.
/// @param position Playback position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetPlaybackPosition(BarelyApi api,
                                                     double position);

/// Sets the playback tempo.
///
/// @param api BarelyMusician api.
/// @param tempo Playback tempo in bpm.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetPlaybackTempo(BarelyApi api, double tempo);

/// Sets the playback update callback.
///
/// @param api BarelyMusician api.
/// @param playback_update_callback Playback update callback.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetPlaybackUpdateCallback(
    BarelyApi api, BarelyPlaybackUpdateCallback playback_update_callback);

/// Sets the sample rate.
///
/// @param api BarelyMusician api.
/// @param sample_rate Sampling rate in hz.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetSampleRate(BarelyApi api,
                                               int32_t sample_rate);

/// Starts the playback.
///
/// @param api BarelyMusician api.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyStartPlayback(BarelyApi api);

/// Stops the playback.
///
/// @param api BarelyMusician api.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyStopPlayback(BarelyApi api);

/// Updates the internal state at timestamp.
///
/// @param api BarelyMusician api.
/// @param timestamp Timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyUpdate(BarelyApi api, double timestamp);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // PLATFORMS_CAPI_BARELYMUSICIAN_H_
