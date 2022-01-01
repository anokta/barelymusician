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
/// Note pitch type.
typedef float BarelyNotePitch;

/// Parameter identifier type.
typedef int32_t BarelyParamId;

/// Parameter definition.
typedef struct BarelyParamDefinition {
  /// Identifier.
  BarelyParamId id;

  /// Default value.
  float default_value;

  /// Minimum value.
  float min_value;

  /// Maximum value.
  float max_value;
} BarelyParamDefinition;

/// Status enum type.
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

/// BarelyMusician API beat callback signature.
///
/// @param position Beat position in beats.
typedef void (*BarelyApi_BeatCallback)(double position);

/// BarelyMusician API note off callback signature.
///
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
typedef void (*BarelyApi_NoteOffCallback)(BarelyId instrument_id, float pitch);

/// BarelyMusician API note on callback signature.
///
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @param intensity Note intensity.
typedef void (*BarelyApi_NoteOnCallback)(BarelyId instrument_id, float pitch,
                                         float intensity);

/// BarelyMusician API position callback signature.
///
/// @param begin_position Begin position in beats.
/// @param end_position End position in beats.
typedef void (*BarelyApi_PositionCallback)(double begin_position,
                                           double end_position);

/// Conductor create function signature.
///
/// @param state Pointer to conductor state.
typedef void (*BarelyConductorCreateFn)(void** state);

/// Conductor destroy function signature.
///
/// @param state Pointer to conductor state.
typedef void (*BarelyConductorDestroyFn)(void** state);

/// Conductor set data function signature.
///
/// @param state Pointer to conductor state.
/// @param data Data.
typedef void (*BarelyConductorSetDataFn)(void** state, void* data);

/// Conductor set energy function signature.
///
/// @param state Pointer to conductor state.
/// @param energy Energy.
typedef void (*BarelyConductorSetEnergyFn)(void** state, float energy);

/// Conductor set parameter function signature.
///
/// @param state Pointer to conductor state.
/// @param param_id Parameter identifier.
/// @param param_value Parameter value.
typedef void (*BarelyConductorSetParamFn)(void** state, int32_t param_id,
                                          float param_value);

/// Conductor set stress function signature.
///
/// @param state Pointer to conductor state.
/// @param stress Stress.
typedef void (*BarelyConductorSetStressFn)(void** state, float stress);

/// Conductor transform note duration function signature.
///
/// @param state Pointer to conductor state.
/// @param note_duration Note duration.
/// @param out_note_duration Output note duration.
/// @return Status.
typedef BarelyStatus (*BarelyConductorTransformNoteDurationFn)(
    void** state, double note_duration, double* out_note_duration);

/// Conductor transform note intensity function signature.
///
/// @param state Pointer to conductor state.
/// @param note_intensity Note intensity.
/// @param out_note_intensity Output note intensity.
/// @return Status.
typedef BarelyStatus (*BarelyConductorTransformNoteIntensityFn)(
    void** state, float note_intensity, float* out_note_intensity);

/// Conductor transform note pitch function signature.
///
/// @param state Pointer to conductor state.
/// @param note_pitch Note pitch.
/// @param out_note_pitch Output note pitch.
/// @return Status.
typedef BarelyStatus (*BarelyConductorTransformNotePitchFn)(
    void** state, BarelyNotePitch note_pitch, float* out_note_pitch);

/// Conductor transform playback tempo function signature.
///
/// @param state Pointer to conductor state.
/// @param playback_tempo Playback tempo in BPM.
/// @param out_playback_tempo Output playback tempo in BPM.
/// @return Status.
typedef BarelyStatus (*BarelyConductorTransformPlaybackTempoFn)(
    void** state, double playback_tempo, double* out_playback_tempo);

/// Conductor definition.
typedef struct BarelyConductorDefinition {
  /// Create function.
  BarelyConductorCreateFn create_fn;

  /// Destroy function.
  BarelyConductorDestroyFn destroy_fn;

  /// Set data function.
  BarelyConductorSetDataFn set_data_fn;

  /// Set energy function.
  BarelyConductorSetEnergyFn set_energy_fn;

  /// Set parameter function.
  BarelyConductorSetParamFn set_param_fn;

  /// Set stress function.
  BarelyConductorSetStressFn set_stress_fn;

  /// Transform note duration function.
  BarelyConductorTransformNoteDurationFn transform_note_duration_fn;

  /// Transform note intensity function.
  BarelyConductorTransformNoteIntensityFn transform_note_intensity_fn;

  /// Transform note pitch function.
  BarelyConductorTransformNotePitchFn transform_note_pitch_fn;

  /// Transform playback tempo function.
  BarelyConductorTransformPlaybackTempoFn transform_playback_tempo_fn;

  /// Number of parameter definitions.
  int32_t num_param_definitions;

  /// List of parameter definitions.
  BarelyParamDefinition* param_definitions;
} BarelyConductorDefinition;

/// Instrument create function signature.
///
/// @param state Pointer to instrument state.
/// @param sample_rate Sampling rate in Hz.
typedef void (*BarelyInstrumentCreateFn)(void** state, int32_t sample_rate);

/// Instrument destroy function signature.
///
/// @param state Pointer to instrument state.
typedef void (*BarelyInstrumentDestroyFn)(void** state);

/// Instrument process function signature.
///
/// @param state Pointer to instrument state.
/// @param output Output buffer.
/// @param num_output_channels Number of channels.
/// @param num_output_frames Number of frames.
typedef void (*BarelyInstrumentProcessFn)(void** state, float* output,
                                          int32_t num_output_channels,
                                          int32_t num_output_frames);

/// Instrument set data function signature.
///
/// @param state Pointer to instrument state.
/// @param data Data.
typedef void (*BarelyInstrumentSetDataFn)(void** state, void* data);

/// Instrument set note off function signature.
///
/// @param state Pointer to instrument state.
/// @param note_pitch Note pitch.
typedef void (*BarelyInstrumentSetNoteOffFn)(void** state, float note_pitch);

/// Instrument set note on function signature.
///
/// @param state Pointer to instrument state.
/// @param note_pitch Note pitch.
/// @param note_intensity Note intensity.
typedef void (*BarelyInstrumentSetNoteOnFn)(void** state, float note_pitch,
                                            float note_intensity);

/// Instrument set parameter function signature.
///
/// @param state Pointer to instrument state.
/// @param param_id Parameter identifier.
/// @param param_value Parameter value.
typedef void (*BarelyInstrumentSetParamFn)(void** state, int32_t param_id,
                                           float param_value);

/// Instrument definition.
typedef struct BarelyInstrumentDefinition {
  /// Create function.
  BarelyInstrumentCreateFn create_fn;

  /// Destroy function.
  BarelyInstrumentDestroyFn destroy_fn;

  /// Process function.
  BarelyInstrumentProcessFn process_fn;

  /// Set data function.
  BarelyInstrumentSetDataFn set_data_fn;

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
BARELY_EXPORT BarelyStatus BarelyApi_Create(BarelyApi* out_api);

/// Destroys BarelyMusician API.
///
/// @param api BarelyMusician API.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_Destroy(BarelyApi api);

/// Gets the playback position.
///
/// @param api BarelyMusician API.
/// @param out_position Output playback position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_GetPlaybackPosition(BarelyApi api,
                                                         double* out_position);

/// Gets the playback tempo.
///
/// @param api BarelyMusician API.
/// @param out_tempo Output playback tempo in BPM.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_GetPlaybackTempo(BarelyApi api,
                                                      double* out_tempo);

/// Gets root note.
///
/// @param api BarelyMusician API.
/// @param out_root_note_pitch Output root note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_GetRootNote(BarelyApi api,
                                                 float* out_root_note_pitch);

/// Gets the sampling rate.
///
/// @param api BarelyMusician API.
/// @param out_sample_rate Output sampling rate in Hz.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_GetSampleRate(BarelyApi api,
                                                   int32_t* out_sample_rate);

/// Gets scale.
///
/// @param api BarelyMusician API.
/// @param out_scale Output scale.
/// @param out_scale_length Output scale length.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_GetScale(BarelyApi api, float** out_scale,
                                              int32_t* out_scale_length);

/// Gets whether the playback is active or not.
///
/// @param api BarelyMusician API.
/// @param out_is_playing Output true if active, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_IsPlaying(BarelyApi api,
                                               bool* out_is_playing);

/// Sets the beat callback.
///
/// @param api BarelyMusician API.
/// @param beat_callback Beat callback.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyApi_SetBeatCallback(BarelyApi api, BarelyApi_BeatCallback beat_callback);

/// Sets the note off callback.
///
/// @param api BarelyMusician API.
/// @param note_off_callback Note off callback.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_SetNoteOffCallback(
    BarelyApi api, BarelyApi_NoteOffCallback note_off_callback);

/// Sets the note on callback.
///
/// @param api BarelyMusician API.
/// @param note_on_callback Note on callback.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_SetNoteOnCallback(
    BarelyApi api, BarelyApi_NoteOnCallback note_on_callback);

/// Sets the playback position.
///
/// @param api BarelyMusician API.
/// @param position Playback position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_SetPlaybackPosition(BarelyApi api,
                                                         double position);

/// Sets the playback tempo.
///
/// @param api BarelyMusician API.
/// @param tempo Playback tempo in BPM.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_SetPlaybackTempo(BarelyApi api,
                                                      double tempo);

/// Sets the position callback.
///
/// @param api BarelyMusician API.
/// @param position_callback Position callback.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_SetPositionCallback(
    BarelyApi api, BarelyApi_PositionCallback position_callback);

/// Sets root note.
///
/// @param api BarelyMusician API.
/// @param root_note_pitch Root note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_SetRootNote(BarelyApi api,
                                                 float root_note_pitch);

/// Sets the sampling rate.
///
/// @param api BarelyMusician API.
/// @param sample_rate Sampling rate in Hz.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_SetSampleRate(BarelyApi api,
                                                   int32_t sample_rate);

/// Sets scale.
///
/// @param api BarelyMusician API.
/// @param scale Scale.
/// @param scale_length Scale length.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_SetScale(BarelyApi api, float* scale,
                                              int32_t scale_length);

/// Starts the playback.
///
/// @param api BarelyMusician API.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_StartPlayback(BarelyApi api);

/// Stops the playback.
///
/// @param api BarelyMusician API.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_StopPlayback(BarelyApi api);

/// Updates the internal state at timestamp.
///
/// @param api BarelyMusician API.
/// @param timestamp Timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_Update(BarelyApi api, double timestamp);

/// Gets conductor energy (i.e., arousal).
///
/// @param api BarelyMusician API.
/// @param out_energy Output energy in range [0, 1].
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_GetEnergy(BarelyApi api,
                                                     float* out_energy);

/// Gets conductor parameter value.
///
/// @param api BarelyMusician API.
/// @param param_id Parameter identifier.
/// @param out_param_value Output parameter value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_GetParam(BarelyApi api,
                                                    int32_t param_id,
                                                    float* out_param_value);

/// Gets conductor parameter definition.
///
/// @param api BarelyMusician API.
/// @param param_id Parameter identifier.
/// @param out_param_definition Output parameter definition.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyConductor_GetParamDefinition(BarelyApi api, int32_t param_id,
                                   BarelyParamDefinition* out_param_definition);

/// Gets conductor stress (i.e., valence).
///
/// @param api BarelyMusician API.
/// @param out_stress Stress in range [0, 1].
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_GetStress(BarelyApi api,
                                                     float* out_stress);

/// Resets all conductor parameters to default value.
///
/// @param api BarelyMusician API.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_SetAllParamsToDefault(BarelyApi api);

/// Sets conductor data.
///
/// @param api BarelyMusician API.
/// @param data Data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_SetData(BarelyApi api, void* data);

/// Sets conductor definition.
///
/// @param api BarelyMusician API.
/// @param definition Conductor definition.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_SetDefinition(
    BarelyApi api, BarelyConductorDefinition definition);

/// Sets conductor energy (i.e., arousal).
///
/// @param api BarelyMusician API.
/// @param energy Energy in range [0, 1].
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_SetEnergy(BarelyApi api,
                                                     float energy);

/// Sets conductor parameter value.
///
/// @param api BarelyMusician API.
/// @param param_id Parameter identifier.
/// @param param_value Parameter value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_SetParam(BarelyApi api,
                                                    int32_t param_id,
                                                    float param_value);

/// Resets conductor parameter to default value.
///
/// @param api BarelyMusician API.
/// @param param_id Parameter identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_SetParamToDefault(BarelyApi api,
                                                             int32_t param_id);

/// Sets conductor stress (i.e., valence).
///
/// @param api BarelyMusician API.
/// @param stress Stress in range [0, 1].
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_SetStress(BarelyApi api,
                                                     float stress);

/// Cancels all scheduled instrument note events.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_CancelAllScheduledNoteEvents(
    BarelyApi api, BarelyId instrument_id);

/// Cancels scheduled instrument note event.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param note_event_id Note event identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_CancelScheduledNoteEvent(
    BarelyApi api, BarelyId instrument_id, BarelyId note_event_id);

/// Clones instrument.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param out_instrument_id Output instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_Clone(BarelyApi api,
                                                  BarelyId instrument_id,
                                                  BarelyId* out_instrument_id);

/// Creates new instrument.
///
/// @param api BarelyMusician API.
/// @param definition Instrument definition.
/// @param out_instrument_id Output instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_Create(BarelyApi api, BarelyInstrumentDefinition definition,
                        BarelyId* out_instrument_id);

/// Destroys instrument.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_Destroy(BarelyApi api,
                                                    BarelyId instrument_id);
/// Gets instrument gain.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param out_gain Output gain in amplitude.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_GetGain(BarelyApi api,
                                                    BarelyId instrument_id,
                                                    float* out_gain);

/// Gets instrument parameter value.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param param_id Parameter identifier.
/// @param out_param_value Output parameter value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_GetParam(BarelyApi api,
                                                     BarelyId instrument_id,
                                                     int32_t param_id,
                                                     float* out_param_value);

/// Gets instrument parameter definition.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param param_id Parameter identifier.
/// @param out_param_definition Output parameter definition.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_GetParamDefinition(
    BarelyApi api, BarelyId instrument_id, int32_t param_id,
    BarelyParamDefinition* out_param_definition);

/// Gets whether instrument is muted or not.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param out_is_muted Output true if muted, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_IsMuted(BarelyApi api,
                                                    BarelyId instrument_id,
                                                    float* out_is_muted);

/// Gets whether instrument note is active or not.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param note_pitch Note pitch.
/// @param out_is_note_on Output true if active, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_IsNoteOn(BarelyApi api,
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
BARELY_EXPORT BarelyStatus BarelyInstrument_Process(
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
BARELY_EXPORT BarelyStatus BarelyInstrument_ScheduleNoteEvent(
    BarelyApi api, BarelyId instrument_id, double note_position,
    double note_duration, BarelyNotePitch note_pitch, float note_intensity,
    BarelyId* out_note_event_id);

/// Stops all active instrument notes.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_SetAllNotesOff(BarelyApi api, BarelyId instrument_id);

/// Resets all instrument parameters to default value.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_SetAllParamsToDefault(BarelyApi api, BarelyId instrument_id);

/// Sets instrument data.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param data Data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetData(BarelyApi api,
                                                    BarelyId instrument_id,
                                                    void* data);

/// Sets instrument gain.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param gain Gain in amplitude.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetGain(BarelyApi api,
                                                    BarelyId instrument_id,
                                                    float gain);

/// Sets whether instrument should be muted or not.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param is_muted True if muted, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetMuted(BarelyApi api,
                                                     BarelyId instrument_id,
                                                     bool is_muted);

/// Stops instrument note.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param note_pitch Note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetNoteOff(BarelyApi api,
                                                       BarelyId instrument_id,
                                                       float note_pitch);

/// Starts instrument note.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param note_pitch Note pitch.
/// @param note_intensity Note intensity.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetNoteOn(BarelyApi api,
                                                      BarelyId instrument_id,
                                                      float note_pitch,
                                                      float note_intensity);

/// Sets instrument parameter value.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param param_id Parameter identifier.
/// @param param_value Parameter value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetParam(BarelyApi api,
                                                     BarelyId instrument_id,
                                                     int32_t param_id,
                                                     float param_value);

/// Resets instrument parameter to default value.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param param_id Parameter identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetParamToDefault(
    BarelyApi api, BarelyId instrument_id, int32_t param_id);

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
BARELY_EXPORT BarelyStatus BarelySequence_AddNoteEvent(
    BarelyApi api, BarelyId sequence_id, double note_position,
    double note_duration, BarelyNotePitch note_pitch, float note_intensity,
    BarelyId* out_note_event_id);

/// Clones sequence.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param out_sequence_id Output sequence identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_Clone(BarelyApi api,
                                                BarelyId sequence_id,
                                                BarelyId* out_sequence_id);

/// Creates new sequence.
///
/// @param api BarelyMusician API.
/// @param out_sequence_id Output sequence identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_Create(BarelyApi api,
                                                 BarelyId* out_sequence_id);

/// Destroys sequence.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_Destroy(BarelyApi api,
                                                  BarelyId sequence_id);

/// Gets sequence begin offset.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param out_begin_offset Output begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetBeginOffset(
    BarelyApi api, BarelyId sequence_id, double* out_begin_offset);

/// Gets sequence begin position.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param out_begin_position Output begin position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetBeginPosition(
    BarelyApi api, BarelyId sequence_id, double* out_begin_position);

/// Gets sequence end position.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param out_end_position Output end position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetEndPosition(
    BarelyApi api, BarelyId sequence_id, double* out_end_position);

/// Gets sequence instrument.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param out_instrument_id Instrument identifier.
BARELY_EXPORT BarelyStatus BarelySequence_GetInstrument(
    BarelyApi api, BarelyId sequence_id, BarelyId* out_instrument_id);

/// Gets sequence loop begin offset.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param out_loop_begin_offset Output loop begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetLoopBeginOffset(
    BarelyApi api, BarelyId sequence_id, double* out_loop_begin_offset);

/// Gets sequence loop length.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param out_loop_length Output loop length in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetLoopLength(
    BarelyApi api, BarelyId sequence_id, double* out_loop_length);

/// Gets sequence note event duration.
///
/// @param api BarelyMusician API.
/// @param instrument_id Sequence identifier.
/// @param note_event_id Note event identifier.
/// @param out_note_duration Output note duration.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetNoteEventDuration(
    BarelyApi api, BarelyId sequence_id, BarelyId note_event_id,
    double* out_note_duration);

/// Gets sequence note event intensity.
///
/// @param api BarelyMusician API.
/// @param instrument_id Sequence identifier.
/// @param note_event_id Note event identifier.
/// @param out_note_intensity Output note intensity.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetNoteEventIntensity(
    BarelyApi api, BarelyId sequence_id, BarelyId note_event_id,
    float* out_note_intensity);

/// Gets sequence note event pitch.
///
/// @param api BarelyMusician API.
/// @param instrument_id Sequence identifier.
/// @param note_event_id Note event identifier.
/// @param out_note_pitch Output note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetNoteEventPitch(
    BarelyApi api, BarelyId sequence_id, BarelyId note_event_id,
    BarelyNotePitch* out_note_pitch);

/// Gets whether sequence is empty or not.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param out_is_empty Output true if empty, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_IsEmpty(BarelyApi api,
                                                  BarelyId sequence_id,
                                                  bool* out_is_empty);

/// Gets whether sequence is looping or not.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param out_is_looping Output true if looping, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_IsLooping(BarelyApi api,
                                                    BarelyId sequence_id,
                                                    bool* out_is_looping);

/// Removes all sequence note events.
///
/// @param api BarelyMusician API.
/// @param instrument_id Sequence identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelySequence_RemoveAllNoteEvents(BarelyApi api, BarelyId instrument_id);

/// Removes sequence note event.
///
/// @param api BarelyMusician API.
/// @param instrument_id Sequence identifier.
/// @param note_event_id Note event identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_RemoveNoteEvent(
    BarelyApi api, BarelyId sequence_id, BarelyId note_event_id);

/// Sets sequence begin offset.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param begin_offset Begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetBeginOffset(BarelyApi api,
                                                         BarelyId sequence_id,
                                                         double begin_offset);

/// Sets sequence begin position.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param begin_position Begin position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetBeginPosition(
    BarelyApi api, BarelyId sequence_id, double begin_position);

/// Sets sequence end position.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param end_position End position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetEndPosition(BarelyApi api,
                                                         BarelyId sequence_id,
                                                         double end_position);

/// Sets sequence instrument.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param instrument_id Instrument identifier.
BARELY_EXPORT BarelyStatus BarelySequence_SetInstrument(BarelyApi api,
                                                        BarelyId sequence_id,
                                                        BarelyId instrument_id);

/// Sets sequence loop begin offset.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param loop_begin_offset Loop begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetLoopBeginOffset(
    BarelyApi api, BarelyId sequence_id, double loop_begin_offset);

/// Sets sequence loop length.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param loop_length Loop length in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetLoopLength(BarelyApi api,
                                                        BarelyId sequence_id,
                                                        double loop_length);

/// Sets whether sequence should be looping or not.
///
/// @param api BarelyMusician API.
/// @param sequence_id Sequence identifier.
/// @param is_looping True if looping.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetLooping(BarelyApi api,
                                                     BarelyId sequence_id,
                                                     bool is_looping);

/// Sets sequence note event duration.
///
/// @param api BarelyMusician API.
/// @param instrument_id Sequence identifier.
/// @param note_event_id Note event identifier.
/// @param note_duration Note duration.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetNoteEventDuration(
    BarelyApi api, BarelyId sequence_id, BarelyId note_event_id,
    double note_duration);

/// Sets sequence note event intensity.
///
/// @param api BarelyMusician API.
/// @param instrument_id Sequence identifier.
/// @param note_event_id Note event identifier.
/// @param note_intensity Note intensity.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetNoteEventIntensity(
    BarelyApi api, BarelyId sequence_id, BarelyId note_event_id,
    float note_intensity);

/// Sets sequence note event pitch.
///
/// @param api BarelyMusician API.
/// @param instrument_id Sequence identifier.
/// @param note_event_id Note event identifier.
/// @param note_pitch Note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetNoteEventPitch(
    BarelyApi api, BarelyId sequence_id, BarelyId note_event_id,
    BarelyNotePitch note_pitch);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // PLATFORMS_CAPI_BARELYMUSICIAN_H_
