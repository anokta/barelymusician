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

/// BarelyMusician API type.
typedef struct BarelyMusician* BarelyApi;

/// Identifier type.
typedef int64_t BarelyId;

// TODO(#85): Make this a variant type.
/// Note duration type.
typedef double BarelyNoteDuration;

// TODO(#85): Make this a variant type.
/// Note intensity type.
typedef float BarelyNoteIntensity;

// TODO(#85): Make this a variant type.
/// Note pitch type.
typedef float BarelyNotePitch;

/// Parameter definition.
typedef struct BarelyParamDefinition {
  /// Default value.
  float default_value;

  /// Minimum value.
  float min_value;

  /// Maximum value.
  float max_value;
} BarelyParamDefinition;

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

/// Playback beat callback signature.
///
/// @param position Beat position in beats.
/// @param timestamp Beat timestamp in seconds.
typedef void (*BarelyPlaybackBeatCallback)(double position, double timestamp);

/// Playback update callback signature.
///
/// @param begin_position Begin position in beats.
/// @param end_position End position in beats.
/// @param begin_timestamp Begin timestamp in seconds.
/// @param end_timestamp End timestamp in seconds.
typedef void (*BarelyPlaybackUpdateCallback)(double begin_position,
                                             double end_position,
                                             double begin_timestamp,
                                             double end_timestamp);

/// Conductor state type.
typedef void* BarelyConductorState;

/// Conductor create function signature.
///
/// @param state Pointer to conductor state.
typedef void (*BarelyConductorCreateFn)(BarelyConductorState* state);

/// Conductor destroy function signature.
///
/// @param state Pointer to conductor state.
typedef void (*BarelyConductorDestroyFn)(BarelyConductorState* state);

/// Conductor set custom data function signature.
///
/// @param state Pointer to conductor state.
/// @param custom_data Custom data.
typedef void (*BarelyConductorSetCustomDataFn)(BarelyConductorState* state,
                                               void* custom_data);

/// Conductor set parameter function signature.
///
/// @param state Pointer to conductor state.
/// @param param_index Parameter index.
/// @param param_value Parameter value.
typedef void (*BarelyConductorSetParamFn)(BarelyConductorState* state,
                                          int32_t param_index,
                                          float param_value);

/// Conductor transform note duration function signature.
///
/// @param state Pointer to conductor state.
/// @param note_duration Note duration.
/// @param out_raw_note_duration Output raw note duration.
/// @return Status.
typedef BarelyStatus (*BarelyConductorTransformNoteDurationFn)(
    BarelyConductorState* state, BarelyNoteDuration note_duration,
    double* out_raw_note_duration);

/// Conductor transform note intensity function signature.
///
/// @param state Pointer to conductor state.
/// @param note_intensity Note intensity.
/// @param out_raw_note_intensity Output raw note intensity.
/// @return Status.
typedef BarelyStatus (*BarelyConductorTransformNoteIntensityFn)(
    BarelyConductorState* state, BarelyNoteIntensity note_intensity,
    float* out_raw_note_intensity);

/// Conductor transform note pitch function signature.
///
/// @param state Pointer to conductor state.
/// @param note_pitch Note pitch.
/// @param out_raw_note_pitch Output raw note pitch.
/// @return Status.
typedef BarelyStatus (*BarelyConductorTransformNotePitchFn)(
    BarelyConductorState* state, BarelyNotePitch note_pitch,
    float* out_raw_note_pitch);

/// Conductor definition.
typedef struct BarelyConductorDefinition {
  /// Create function.
  BarelyConductorCreateFn create_fn;

  /// Destroy function.
  BarelyConductorDestroyFn destroy_fn;

  /// Set custom data function.
  BarelyConductorSetCustomDataFn set_custom_data_fn;

  /// Set parameter function.
  BarelyConductorSetParamFn set_param_fn;

  /// Transform note duration function.
  BarelyConductorTransformNoteDurationFn transform_note_duration_fn;

  /// Transform note intensity function.
  BarelyConductorTransformNoteIntensityFn transform_note_intensity_fn;

  /// Transform note pitch function.
  BarelyConductorTransformNotePitchFn transform_note_pitch_fn;

  /// Number of parameter definitions.
  int32_t num_param_definitions;

  /// List of parameter definitions.
  BarelyParamDefinition* param_definitions;
} BarelyConductorDefinition;

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
/// @param num_output_channels Number of channels.
/// @param num_output_frames Number of frames.
typedef void (*BarelyInstrumentProcessFn)(BarelyInstrumentState* state,
                                          float* output,
                                          int32_t num_output_channels,
                                          int32_t num_output_frames);

/// Instrument set custom data function signature.
///
/// @param state Pointer to instrument state.
/// @param custom_data Custom data.
typedef void (*BarelyInstrumentSetCustomDataFn)(BarelyInstrumentState* state,
                                                void* custom_data);

/// Instrument set note off function signature.
///
/// @param state Pointer to instrument state.
/// @param note_pitch Note pitch.
typedef void (*BarelyInstrumentSetNoteOffFn)(BarelyInstrumentState* state,
                                             float note_pitch);

/// Instrument set note on function signature.
///
/// @param state Pointer to instrument state.
/// @param note_pitch Note pitch.
/// @param note_intensity Note intensity.
typedef void (*BarelyInstrumentSetNoteOnFn)(BarelyInstrumentState* state,
                                            float note_pitch,
                                            float note_intensity);

/// Instrument set parameter function signature.
///
/// @param state Pointer to instrument state.
/// @param param_index Parameter index.
/// @param param_value Parameter value.
typedef void (*BarelyInstrumentSetParamFn)(BarelyInstrumentState* state,
                                           int32_t param_index,
                                           float param_value);

/// Instrument definition.
typedef struct BarelyInstrumentDefinition {
  /// Create function.
  BarelyInstrumentCreateFn create_fn;

  /// Destroy function.
  BarelyInstrumentDestroyFn destroy_fn;

  /// Process function.
  BarelyInstrumentProcessFn process_fn;

  /// Set custom data function.
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

/// Creates new BarelyMusician API.
///
/// @param out_api Output BarelyMusician API.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApiCreate(BarelyApi* out_api);

/// Destroys BarelyMusician API.
///
/// @param api BarelyMusician API.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApiDestroy(BarelyApi api);

/// Gets the playback position.
///
/// @param api BarelyMusician API.
/// @param out_position Output playback position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApiGetPlaybackPosition(BarelyApi api,
                                                        double* out_position);

/// Gets the playback tempo.
///
/// @param api BarelyMusician API.
/// @param out_tempo Output playback tempo in BPM.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApiGetPlaybackTempo(BarelyApi api,
                                                     double* out_tempo);

/// Gets the sampling rate.
///
/// @param api BarelyMusician API.
/// @param out_sample_rate Output sampling rate in Hz.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApiGetSampleRate(BarelyApi api,
                                                  int32_t* out_sample_rate);

/// Gets whether the playback is active or not.
///
/// @param api BarelyMusician API.
/// @param out_is_playing Output true if active, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApiIsPlaying(BarelyApi api,
                                              bool* out_is_playing);

/// Sets the instrument note off callback.
///
/// @param api BarelyMusician API.
/// @param instrument_note_off_callback Instrument note off callback.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApiSetInstrumentNoteOffCallback(
    BarelyApi api,
    BarelyInstrumentNoteOffCallback instrument_note_off_callback);

/// Sets the instrument note on callback.
///
/// @param api BarelyMusician API.
/// @param instrument_note_on_callback Instrument note on callback.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApiSetInstrumentNoteOnCallback(
    BarelyApi api, BarelyInstrumentNoteOnCallback instrument_note_on_callback);

/// Sets the playback beat callback.
///
/// @param api BarelyMusician API.
/// @param playback_beat_callback Playback beat callback.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApiSetPlaybackBeatCallback(
    BarelyApi api, BarelyPlaybackBeatCallback playback_beat_callback);

/// Sets the playback position.
///
/// @param api BarelyMusician API.
/// @param position Playback position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApiSetPlaybackPosition(BarelyApi api,
                                                        double position);

/// Sets the playback tempo.
///
/// @param api BarelyMusician API.
/// @param tempo Playback tempo in BPM.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApiSetPlaybackTempo(BarelyApi api,
                                                     double tempo);

/// Sets the playback update callback.
///
/// @param api BarelyMusician API.
/// @param playback_update_callback Playback update callback.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApiSetPlaybackUpdateCallback(
    BarelyApi api, BarelyPlaybackUpdateCallback playback_update_callback);

/// Sets the sampling rate.
///
/// @param api BarelyMusician API.
/// @param sample_rate Sampling rate in Hz.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApiSetSampleRate(BarelyApi api,
                                                  int32_t sample_rate);

/// Starts the playback.
///
/// @param api BarelyMusician API.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApiStartPlayback(BarelyApi api);

/// Stops the playback.
///
/// @param api BarelyMusician API.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApiStopPlayback(BarelyApi api);

/// Updates the internal state at timestamp.
///
/// @param api BarelyMusician API.
/// @param timestamp Timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApiUpdate(BarelyApi api, double timestamp);

/// Gets conductor energy (i.e., arousal).
///
/// @param api BarelyMusician API.
/// @param out_energy Output energy in range [0, 1].
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductorGetEnergy(BarelyApi api,
                                                    float* out_energy);

/// Gets conductor parameter value.
///
/// @param api BarelyMusician API.
/// @param param_index Parameter index.
/// @param out_param_value Output parameter value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductorGetParam(BarelyApi api,
                                                   int32_t param_index,
                                                   float* out_param_value);

/// Gets conductor root note.
///
/// @param api BarelyMusician API.
/// @param out_root_note_pitch Output root note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyConductorGetRootNote(BarelyApi api, float* out_root_note_pitch);

/// Gets conductor stress (i.e., valence).
///
/// @param api BarelyMusician API.
/// @param out_stress Stress in range [0, 1].
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductorGetStress(BarelyApi api,
                                                    float* out_stress);

/// Resets all conductor parameters to default value.
///
/// @param api BarelyMusician API.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductorSetAllParamsToDefault(BarelyApi api);

/// Sets custom conductor data.
///
/// @param api BarelyMusician API.
/// @param custom_data Custom data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductorSetCustomData(BarelyApi api,
                                                        void* custom_data);

/// Sets conductor definition.
///
/// @param api BarelyMusician API.
/// @param definition Conductor definition.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductorSetDefinition(
    BarelyApi api, BarelyConductorDefinition definition);

/// Sets conductor energy (i.e., arousal).
///
/// @param api BarelyMusician API.
/// @param energy Energy in range [0, 1].
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductorSetEnergy(BarelyApi api,
                                                    float energy);

/// Sets conductor parameter value.
///
/// @param api BarelyMusician API.
/// @param param_index Parameter index.
/// @param param_value Parameter value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductorSetParam(BarelyApi api,
                                                   int32_t param_index,
                                                   float param_value);

/// Resets conductor parameter to default value.
///
/// @param api BarelyMusician API.
/// @param param_index Parameter index.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyConductorSetParamToDefault(BarelyApi api, int32_t param_index);

/// Sets conductor root note.
///
/// @param api BarelyMusician API.
/// @param root_note_pitch Root note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductorSetRootNote(BarelyApi api,
                                                      float root_note_pitch);

/// Sets conductor stress (i.e., valence).
///
/// @param api BarelyMusician API.
/// @param stress Stress in range [0, 1].
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductorSetStress(BarelyApi api,
                                                    float stress);

/// Cancels all scheduled instrument note events.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrumentCancelAllScheduledNoteEvents(
    BarelyApi api, BarelyId instrument_id);

/// Cancels scheduled instrument note event.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param note_event_id Note event identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrumentCancelScheduledNoteEvent(
    BarelyApi api, BarelyId instrument_id, BarelyId note_event_id);

/// Creates new instrument.
///
/// @param api BarelyMusician API.
/// @param definition Instrument definition.
/// @param out_instrument_id Output instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrumentCreate(BarelyApi api, BarelyInstrumentDefinition definition,
                       BarelyId* out_instrument_id);

/// Destroys instrument.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrumentDestroy(BarelyApi api,
                                                   BarelyId instrument_id);
/// Gets instrument gain.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param out_gain Output gain in amplitude.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrumentGetGain(BarelyApi api,
                                                   BarelyId instrument_id,
                                                   float* out_gain);

/// Gets instrument parameter value.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param param_index Parameter index.
/// @param out_param_value Output parameter value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrumentGetParam(BarelyApi api,
                                                    BarelyId instrument_id,
                                                    int32_t param_index,
                                                    float* out_param_value);

/// Gets whether instrument is muted or not.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param out_is_muted Output true if muted, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrumentIsMuted(BarelyApi api,
                                                   BarelyId instrument_id,
                                                   float* out_is_muted);

/// Gets whether instrument note is active or not.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param note_pitch Note pitch.
/// @param out_is_note_on Output true if active, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrumentIsNoteOn(BarelyApi api,
                                                    BarelyId instrument_id,
                                                    float note_pitch,
                                                    bool* out_is_note_on);

/// Processes instrument at timestamp.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param timestamp Timestamp in seconds.
/// @param output Output buffer.
/// @param num_output_channels Number of output channels.
/// @param num_output_frames Number of output frames.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrumentProcess(
    BarelyApi api, BarelyId instrument_id, double timestamp, float* output,
    int32_t num_output_channels, int32_t num_output_frames);

/// Schedules instrument note event at position.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param note_position Note position in beats.
/// @param note_duration Note duration.
/// @param note_pitch Note pitch.
/// @param note_intensity Note intensity.
/// @param out_note_event_id Output note event identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrumentScheduleNoteEvent(
    BarelyApi api, BarelyId instrument_id, double note_position,
    BarelyNoteDuration note_duration, BarelyNotePitch note_pitch,
    BarelyNoteIntensity note_intensity, BarelyId* out_note_event_id);

/// Stops all active instrument notes.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrumentSetAllNotesOff(BarelyApi api, BarelyId instrument_id);

/// Resets all instrument parameters to default value.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrumentSetAllParamsToDefault(BarelyApi api, BarelyId instrument_id);

/// Sets custom instrument data.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param custom_data Custom data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrumentSetCustomData(BarelyApi api,
                                                         BarelyId instrument_id,
                                                         void* custom_data);

/// Sets instrument gain.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param gain Gain in amplitude.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrumentSetGain(BarelyApi api,
                                                   BarelyId instrument_id,
                                                   float gain);

/// Sets whether instrument should be muted or not.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param is_muted True if muted, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrumentSetMuted(BarelyApi api,
                                                    BarelyId instrument_id,
                                                    bool is_muted);

/// Stops instrument note.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param note_pitch Note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrumentSetNoteOff(BarelyApi api,
                                                      BarelyId instrument_id,
                                                      float note_pitch);

/// Starts instrument note.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param note_pitch Note pitch.
/// @param note_intensity Note intensity.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrumentSetNoteOn(BarelyApi api,
                                                     BarelyId instrument_id,
                                                     float note_pitch,
                                                     float note_intensity);

/// Sets instrument parameter value.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param param_index Parameter index.
/// @param param_value Parameter value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrumentSetParam(BarelyApi api,
                                                    BarelyId instrument_id,
                                                    int32_t param_index,
                                                    float param_value);

/// Resets instrument parameter to default value.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param param_index Parameter index.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrumentSetParamToDefault(
    BarelyApi api, BarelyId instrument_id, int32_t param_index);

/// Adds instrument to sequence.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param instrument_id Instrument identifier.
BARELY_EXPORT BarelyStatus BarelySequenceAddInstrument(BarelyApi api,
                                                       BarelyId sequence_id,
                                                       BarelyId instrument_id);

/// Adds sequence note event at position.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param note_position Note position in beats.
/// @param note_duration Note duration.
/// @param note_pitch Note pitch.
/// @param note_intensity Note intensity.
/// @param out_note_event_id Output note event identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequenceAddNoteEvent(
    BarelyApi api, BarelyId sequence_id, double note_position,
    BarelyNoteDuration note_duration, BarelyNotePitch note_pitch,
    BarelyNoteIntensity note_intensity, BarelyId* out_note_event_id);

/// Creates new sequence.
///
/// @param api BarelyMusician API.
/// @param out_sequence_id Output sequence identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequenceCreate(BarelyApi api,
                                                BarelyId* out_sequence_id);

/// Destroys sequence.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequenceDestroy(BarelyApi api,
                                                 BarelyId sequence_id);

/// Gets sequence begin offset.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param out_begin_offset Output begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequenceGetBeginOffset(
    BarelyApi api, BarelyId sequence_id, double* out_begin_offset);

/// Gets sequence begin position.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param out_begin_position Output begin position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequenceGetBeginPosition(
    BarelyApi api, BarelyId sequence_id, double* out_begin_position);

/// Gets sequence end position.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param out_end_position Output end position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequenceGetEndPosition(
    BarelyApi api, BarelyId sequence_id, double* out_end_position);

/// Gets sequence loop begin offset.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param out_loop_begin_offset Output loop begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequenceGetLoopBeginOffset(
    BarelyApi api, BarelyId sequence_id, double* out_loop_begin_offset);

/// Gets sequence loop length.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param out_loop_length Output loop length in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequenceGetLoopLength(BarelyApi api,
                                                       BarelyId sequence_id,
                                                       double* out_loop_length);

/// Gets sequence note event duration.
///
/// @param api BarelyMusician API.
/// @param instrument_id Sequence identifier.
/// @param note_event_id Note event identifier.
/// @param out_note_duration Output note duration.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequenceGetNoteEventDuration(
    BarelyApi api, BarelyId sequence_id, BarelyId note_event_id,
    BarelyNoteDuration* out_note_duration);

/// Gets sequence note event intensity.
///
/// @param api BarelyMusician API.
/// @param instrument_id Sequence identifier.
/// @param note_event_id Note event identifier.
/// @param out_note_intensity Output note intensity.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequenceGetNoteEventIntensity(
    BarelyApi api, BarelyId sequence_id, BarelyId note_event_id,
    BarelyNoteIntensity* out_note_intensity);

/// Gets sequence note event pitch.
///
/// @param api BarelyMusician API.
/// @param instrument_id Sequence identifier.
/// @param note_event_id Note event identifier.
/// @param out_note_pitch Output note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequenceGetNoteEventPitch(
    BarelyApi api, BarelyId sequence_id, BarelyId note_event_id,
    BarelyNotePitch* out_note_pitch);

/// Gets whether sequence has instrument or not.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param instrument_id Instrument identifier.
/// @param out_has_instrument Output true if instrument is had, false otherwise.
BARELY_EXPORT BarelyStatus
BarelySequenceHasInstrument(BarelyApi api, BarelyId sequence_id,
                            BarelyId instrument_id, bool* out_has_instrument);

/// Gets whether sequence is empty or not.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param out_is_empty Output true if empty, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequenceIsEmpty(BarelyApi api,
                                                 BarelyId sequence_id,
                                                 bool* out_is_empty);

/// Gets whether sequence is looping or not.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param out_is_looping Output true if looping, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequenceIsLooping(BarelyApi api,
                                                   BarelyId sequence_id,
                                                   bool* out_is_looping);

/// Removes all instruments from sequence.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
BARELY_EXPORT BarelyStatus
BarelySequenceRemoveAllInstruments(BarelyApi api, BarelyId sequence_id);

/// Removes all sequence note events.
///
/// @param api BarelyMusician API.
/// @param instrument_id Sequence identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelySequenceRemoveAllNoteEvents(BarelyApi api, BarelyId instrument_id);

/// Removes instrument from sequence.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param instrument_id Instrument identifier.
BARELY_EXPORT BarelyStatus BarelySequenceRemoveInstrument(
    BarelyApi api, BarelyId sequence_id, BarelyId instrument_id);

/// Removes sequence note event.
///
/// @param api BarelyMusician API.
/// @param instrument_id Sequence identifier.
/// @param note_event_id Note event identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequenceRemoveNoteEvent(
    BarelyApi api, BarelyId sequence_id, BarelyId note_event_id);

/// Sets sequence begin offset.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param begin_offset Begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequenceSetBeginOffset(BarelyApi api,
                                                        BarelyId sequence_id,
                                                        double begin_offset);

/// Sets sequence begin position.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param begin_position Begin position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequenceSetBeginPosition(
    BarelyApi api, BarelyId sequence_id, double begin_position);

/// Sets sequence end position.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param end_position End position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequenceSetEndPosition(BarelyApi api,
                                                        BarelyId sequence_id,
                                                        double end_position);

/// Sets sequence loop begin offset.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param loop_begin_offset Loop begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequenceSetLoopBeginOffset(
    BarelyApi api, BarelyId sequence_id, double loop_begin_offset);

/// Sets sequence loop length.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param loop_length Loop length in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequenceSetLoopLength(BarelyApi api,
                                                       BarelyId sequence_id,
                                                       double loop_length);

/// Sets whether sequence should be looping or not.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param is_looping True if looping.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequenceSetLooping(BarelyApi api,
                                                    BarelyId sequence_id,
                                                    bool is_looping);

/// Sets sequence note event duration.
///
/// @param api BarelyMusician API.
/// @param instrument_id Sequence identifier.
/// @param note_event_id Note event identifier.
/// @param note_duration Note duration.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequenceSetNoteEventDuration(
    BarelyApi api, BarelyId sequence_id, BarelyId note_event_id,
    BarelyNoteDuration note_duration);

/// Sets sequence note event intensity.
///
/// @param api BarelyMusician API.
/// @param instrument_id Sequence identifier.
/// @param note_event_id Note event identifier.
/// @param note_intensity Note intensity.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequenceSetNoteEventIntensity(
    BarelyApi api, BarelyId sequence_id, BarelyId note_event_id,
    BarelyNoteIntensity note_intensity);

/// Sets sequence note event pitch.
///
/// @param api BarelyMusician API.
/// @param instrument_id Sequence identifier.
/// @param note_event_id Note event identifier.
/// @param note_pitch Note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequenceSetNoteEventPitch(
    BarelyApi api, BarelyId sequence_id, BarelyId note_event_id,
    BarelyNotePitch note_pitch);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // PLATFORMS_CAPI_BARELYMUSICIAN_H_
