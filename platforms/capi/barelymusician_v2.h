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

/// BarelyMusician api type.
typedef struct BarelyMusician* BarelyApi;

/// Identifier type.
typedef int64_t BarelyId;

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
/// @param id Parameter identifier.
/// @param value Parameter value.
typedef void (*BarelyConductorDefinition_SetParamFn)(void** state,
                                                     BarelyParamId id,
                                                     float value);

/// Conductor set stress function signature.
///
/// @param state Pointer to conductor state.
/// @param stress Stress.
typedef void (*BarelyConductorDefinition_SetStressFn)(void** state,
                                                      float stress);

/// Conductor transform note duration function signature.
///
/// @param state Pointer to conductor state.
/// @param duration Note duration.
/// @param out_duration Output note duration.
/// @return Status.
typedef BarelyStatus (*BarelyConductorDefinition_TransformNoteDurationFn)(
    void** state, double duration, double* out_duration);

/// Conductor transform note intensity function signature.
///
/// @param state Pointer to conductor state.
/// @param intensity Note intensity.
/// @param out_intensity Output note intensity.
/// @return Status.
typedef BarelyStatus (*BarelyConductorDefinition_TransformNoteIntensityFn)(
    void** state, float intensity, float* out_intensity);

/// Conductor transform note pitch function signature.
///
/// @param state Pointer to conductor state.
/// @param pitch Note pitch.
/// @param out_pitch Output note pitch.
/// @return Status.
typedef BarelyStatus (*BarelyConductorDefinition_TransformNotePitchFn)(
    void** state, float pitch, float* out_pitch);

/// Conductor transform tempo function signature.
///
/// @param state Pointer to conductor state.
/// @param tempo Tempo in BPM.
/// @param out_tempo Output tempo in BPM.
/// @return Status.
typedef BarelyStatus (*BarelyConductorDefinition_TransformTempoFn)(
    void** state, double tempo, double* out_tempo);

/// Conductor definition.
typedef struct BarelyConductorDefinition {
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

  /// Transform note duration function.
  BarelyConductorDefinition_TransformNoteDurationFn transform_duration_fn;

  /// Transform note intensity function.
  BarelyConductorDefinition_TransformNoteIntensityFn transform_intensity_fn;

  /// Transform note pitch function.
  BarelyConductorDefinition_TransformNotePitchFn transform_pitch_fn;

  /// Transform tempo function.
  BarelyConductorDefinition_TransformTempoFn transform_tempo_fn;

  /// List of parameter definitions.
  BarelyParamDefinition* param_definitions;

  /// Number of parameter definitions.
  int32_t num_param_definitions;
} BarelyConductorDefinition;

/// Instrument note off callback signature.
///
/// @param pitch Note pitch.
/// @param timestamp Note timestamp in seconds.
/// @param user_data User data.
typedef void (*BarelyInstrument_NoteOffCallback)(float pitch, double timestamp,
                                                 void* user_data);

/// Instrument note on callback signature.
///
/// @param pitch Note pitch.
/// @param intensity Note intensity.
/// @param timestamp Note timestamp in seconds.
/// @param user_data User data.
typedef void (*BarelyInstrument_NoteOnCallback)(float pitch, float intensity,
                                                double timestamp,
                                                void* user_data);

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
/// @param id Parameter identifier.
/// @param value Parameter value.
typedef void (*BarelyInstrumentDefinition_SetParamFn)(void** state,
                                                      BarelyParamId id,
                                                      float value);

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

/// Transport beat callback signature.
///
/// @param position Beat position in beats.
/// @param timestamp Beat timestamp in seconds.
/// @param user_data User data.
typedef void (*BarelyTransport_BeatCallback)(double position, double timestamp,
                                             void* user_data);

/// Transport update callback signature.
///
/// @param begin_position Begin position in beats.
/// @param end_position End position in beats.
/// @param begin_timestamp Begin timestamp in seconds.
/// @param end_timestamp End timestamp in seconds.
/// @param user_data User data.
typedef void (*BarelyTransport_UpdateCallback)(double begin_position,
                                               double end_position,
                                               double begin_timestamp,
                                               double end_timestamp,
                                               void* user_data);

/// Creates new BarelyMusician api.
///
/// @param out_api Output BarelyMusician api.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_Create(BarelyApi* out_api);

/// Destroys BarelyMusician api.
///
/// @param api BarelyMusician api.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_Destroy(BarelyApi api);

/// Gets sampling rate.
///
/// @param api BarelyMusician api.
/// @param out_sample_rate Output sampling rate in hz.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_GetSampleRate(BarelyApi api,
                                                   int32_t* out_sample_rate);

/// Sets sampling rate.
///
/// @param api BarelyMusician api.
/// @param sample_rate Sampling rate in hz.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_SetSampleRate(BarelyApi api,
                                                   int32_t sample_rate);

/// Updates internal state at timestamp.
///
/// @param api BarelyMusician api.
/// @param timestamp Timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_Update(BarelyApi api, double timestamp);

/// Gets conductor energy.
///
/// @param api BarelyMusician api.
/// @param out_energy Output energy in range [0, 1].
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_GetEnergy(BarelyApi api,
                                                     float* out_energy);

/// Gets conductor parameter value.
///
/// @param api BarelyMusician api.
/// @param id Parameter identifier.
/// @param out_value Output parameter value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_GetParam(BarelyApi api,
                                                    BarelyParamId id,
                                                    float* out_value);

/// Gets conductor parameter definition.
///
/// @param api BarelyMusician api.
/// @param id Parameter identifier.
/// @param out_param_definition Output parameter definition.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyConductor_GetParamDefinition(BarelyApi api, BarelyParamId id,
                                   BarelyParamDefinition* out_param_definition);

/// Gets conductor root note.
///
/// @param api BarelyMusician api.
/// @param out_root_pitch Output root note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_GetRootNote(BarelyApi api,
                                                       float* out_root_pitch);

/// Gets conductor scale.
///
/// @param api BarelyMusician api.
/// @param out_scale_pitches Output list of scale note pitches.
/// @param out_num_scale_pitches Output number of scale note pitches.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_GetScale(
    BarelyApi api, float** out_scale_pitches, int32_t* out_num_scale_pitches);

/// Gets conductor stress.
///
/// @param api BarelyMusician api.
/// @param out_stress Stress in range [0, 1].
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_GetStress(BarelyApi api,
                                                     float* out_stress);

/// Resets all conductor parameters to default value.
///
/// @param api BarelyMusician api.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_SetAllParamsToDefault(BarelyApi api);

/// Sets conductor data.
///
/// @param api BarelyMusician api.
/// @param data Data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_SetData(BarelyApi api, void* data);

/// Sets conductor definition.
///
/// @param api BarelyMusician api.
/// @param definition Conductor definition.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_SetDefinition(
    BarelyApi api, BarelyConductorDefinition definition);

/// Sets conductor energy.
///
/// @param api BarelyMusician api.
/// @param energy Energy in range [0, 1].
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_SetEnergy(BarelyApi api,
                                                     float energy);

/// Sets conductor parameter value.
///
/// @param api BarelyMusician api.
/// @param id Parameter identifier.
/// @param value Parameter value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_SetParam(BarelyApi api,
                                                    BarelyParamId id,
                                                    float value);

/// Resets conductor parameter to default value.
///
/// @param api BarelyMusician api.
/// @param id Parameter identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_SetParamToDefault(BarelyApi api,
                                                             BarelyParamId id);

/// Sets conductor root note.
///
/// @param api BarelyMusician api.
/// @param root_pitch Root note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_SetRootNote(BarelyApi api,
                                                       float root_pitch);

/// Sets conductor scale.
///
/// @param api BarelyMusician api.
/// @param scale_pitches List of scale note pitches.
/// @param num_scale_pitches Number of scale note pitches.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_SetScale(BarelyApi api,
                                                    float* scale_pitches,
                                                    int32_t num_scale_pitches);

/// Sets conductor stress.
///
/// @param api BarelyMusician api.
/// @param stress Stress in range [0, 1].
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_SetStress(BarelyApi api,
                                                     float stress);

/// Cancels all scheduled instrument note events.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_CancelAllScheduledNoteEvents(
    BarelyApi api, BarelyId instrument_id);

/// Cancels scheduled instrument note event.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument identifier.
/// @param note_event_id Note event identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_CancelScheduledNoteEvent(
    BarelyApi api, BarelyId instrument_id, BarelyId note_event_id);

/// Clones instrument.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument identifier.
/// @param out_instrument_id Output instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_Clone(BarelyApi api,
                                                  BarelyId instrument_id,
                                                  BarelyId* out_instrument_id);

/// Creates new instrument.
///
/// @param api BarelyMusician api.
/// @param definition Instrument definition.
/// @param out_instrument_id Output instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_Create(BarelyApi api, BarelyInstrumentDefinition definition,
                        BarelyId* out_instrument_id);

/// Destroys instrument.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_Destroy(BarelyApi api,
                                                    BarelyId instrument_id);
/// Gets instrument gain.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument identifier.
/// @param out_gain Output gain in amplitude.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_GetGain(BarelyApi api,
                                                    BarelyId instrument_id,
                                                    float* out_gain);

/// Gets instrument parameter value.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument identifier.
/// @param id Parameter identifier.
/// @param out_value Output parameter value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_GetParam(BarelyApi api,
                                                     BarelyId instrument_id,
                                                     BarelyParamId id,
                                                     float* out_value);

/// Gets instrument parameter definition.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument identifier.
/// @param id Parameter identifier.
/// @param out_param_definition Output parameter definition.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_GetParamDefinition(
    BarelyApi api, BarelyId instrument_id, BarelyParamId id,
    BarelyParamDefinition* out_param_definition);

/// Gets whether instrument is muted or not.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument identifier.
/// @param out_is_muted Output true if muted, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_IsMuted(BarelyApi api,
                                                    BarelyId instrument_id,
                                                    bool* out_is_muted);

/// Gets whether instrument note is active or not.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @param out_is_note_on Output true if active, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_IsNoteOn(BarelyApi api,
                                                     BarelyId instrument_id,
                                                     float pitch,
                                                     bool* out_is_note_on);

/// Gets whether relative instrument note is active or not.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument identifier.
/// @param relative_pitch Note pitch relative to conductor root note pitch.
/// @param out_is_relative_note_on Output true if active, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_IsRelativeNoteOn(
    BarelyApi api, BarelyId instrument_id, float relative_pitch,
    bool* out_is_relative_note_on);

/// Gets whether scaled instrument note is active or not.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument identifier.
/// @param scale_index Index relative to conductor root note pitch and scale.
/// @param out_is_scaled_note_on Output true if active, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_IsScaledNoteOn(BarelyApi api, BarelyId instrument_id,
                                int scale_index, bool* out_is_scaled_note_on);

/// Processes instrument output buffer at timestamp.
///
/// @param api BarelyMusician api.
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
/// @param api BarelyMusician api.
/// @param instrument_id Instrument identifier.
/// @param position Note position in beats.
/// @param duration Note duration.
/// @param pitch Note pitch.
/// @param intensity Note intensity.
/// @param out_note_event_id Output note event identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_ScheduleNoteEvent(
    BarelyApi api, BarelyId instrument_id, double position, double duration,
    float pitch, float intensity, BarelyId* out_note_event_id);

/// Sets all instrument notes off.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_SetAllNotesOff(BarelyApi api, BarelyId instrument_id);

/// Sets all instrument parameters to default value.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_SetAllParamsToDefault(BarelyApi api, BarelyId instrument_id);

/// Sets instrument data.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument identifier.
/// @param data Data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetData(BarelyApi api,
                                                    BarelyId instrument_id,
                                                    void* data);

/// Sets instrument gain.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument identifier.
/// @param gain Gain in amplitude.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetGain(BarelyApi api,
                                                    BarelyId instrument_id,
                                                    float gain);

/// Sets whether volume should be muted or not.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument identifier.
/// @param is_muted True if muted, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetMuted(BarelyApi api,
                                                     BarelyId instrument_id,
                                                     bool is_muted);

/// Sets instrument note off.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetNoteOff(BarelyApi api,
                                                       BarelyId instrument_id,
                                                       float pitch);

/// Sets instrument note off callback.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument identifier.
/// @param note_off_callback Note off callback.
/// @param user_data User data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetNoteOffCallback(
    BarelyApi api, BarelyId instrument_id,
    BarelyInstrument_NoteOffCallback note_off_callback, void* user_data);

/// Sets instrument note on.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @param intensity Note intensity.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetNoteOn(BarelyApi api,
                                                      BarelyId instrument_id,
                                                      float pitch,
                                                      float intensity);

/// Sets instrument note on callback.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument identifier.
/// @param note_on_callback Note on callback.
/// @param user_data User data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetNoteOnCallback(
    BarelyApi api, BarelyId instrument_id,
    BarelyInstrument_NoteOnCallback note_on_callback, void* user_data);

/// Sets instrument parameter value.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument identifier.
/// @param id Parameter identifier.
/// @param value Parameter value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetParam(BarelyApi api,
                                                     BarelyId instrument_id,
                                                     BarelyParamId id,
                                                     float value);

/// Sets instrument parameter to default value.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument identifier.
/// @param id Parameter identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetParamToDefault(
    BarelyApi api, BarelyId instrument_id, BarelyParamId id);

/// Adds sequence note event at position.
///
/// @param api BarelyMusician api.
/// @param sequence_id Sequence identifier.
/// @param position Note position in beats.
/// @param duration Note duration.
/// @param pitch Note pitch.
/// @param intensity Note intensity.
/// @param out_note_event_id Output note event identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_AddNoteEvent(
    BarelyApi api, BarelyId sequence_id, double position, double duration,
    float pitch, float intensity, BarelyId* out_note_event_id);

/// Clones sequence.
///
/// @param api BarelyMusician api.
/// @param sequence_id Sequence identifier.
/// @param out_sequence_id Output sequence identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_Clone(BarelyApi api,
                                                BarelyId sequence_id,
                                                BarelyId* out_sequence_id);

/// Creates new sequence.
///
/// @param api BarelyMusician api.
/// @param out_sequence_id Output sequence identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_Create(BarelyApi api,
                                                 BarelyId* out_sequence_id);

/// Destroys sequence.
///
/// @param api BarelyMusician api.
/// @param sequence_id Sequence identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_Destroy(BarelyApi api,
                                                  BarelyId sequence_id);

/// Gets sequence begin offset.
///
/// @param api BarelyMusician api.
/// @param sequence_id Sequence identifier.
/// @param out_begin_offset Output begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetBeginOffset(
    BarelyApi api, BarelyId sequence_id, double* out_begin_offset);

/// Gets sequence begin position.
///
/// @param api BarelyMusician api.
/// @param sequence_id Sequence identifier.
/// @param out_begin_position Output begin position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetBeginPosition(
    BarelyApi api, BarelyId sequence_id, double* out_begin_position);

/// Gets sequence end position.
///
/// @param api BarelyMusician api.
/// @param sequence_id Sequence identifier.
/// @param out_end_position Output end position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetEndPosition(
    BarelyApi api, BarelyId sequence_id, double* out_end_position);

/// Gets sequence instrument.
///
/// @param api BarelyMusician api.
/// @param sequence_id Sequence identifier.
/// @param out_instrument_id Instrument identifier.
BARELY_EXPORT BarelyStatus BarelySequence_GetInstrument(
    BarelyApi api, BarelyId sequence_id, BarelyId* out_instrument_id);

/// Gets sequence loop begin offset.
///
/// @param api BarelyMusician api.
/// @param sequence_id Sequence identifier.
/// @param out_loop_begin_offset Output loop begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetLoopBeginOffset(
    BarelyApi api, BarelyId sequence_id, double* out_loop_begin_offset);

/// Gets sequence loop length.
///
/// @param api BarelyMusician api.
/// @param sequence_id Sequence identifier.
/// @param out_loop_length Output loop length in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetLoopLength(
    BarelyApi api, BarelyId sequence_id, double* out_loop_length);

/// Gets sequence note event duration.
///
/// @param api BarelyMusician api.
/// @param instrument_id Sequence identifier.
/// @param note_event_id Note event identifier.
/// @param out_duration Output note event duration.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetNoteEventDuration(
    BarelyApi api, BarelyId sequence_id, BarelyId note_event_id,
    double* out_duration);

/// Gets sequence note event intensity.
///
/// @param api BarelyMusician api.
/// @param instrument_id Sequence identifier.
/// @param note_event_id Note event identifier.
/// @param out_intensity Output note event intensity.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetNoteEventIntensity(
    BarelyApi api, BarelyId sequence_id, BarelyId note_event_id,
    float* out_intensity);

/// Gets sequence note event pitch.
///
/// @param api BarelyMusician api.
/// @param instrument_id Sequence identifier.
/// @param note_event_id Note event identifier.
/// @param out_pitch Output note event pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelySequence_GetNoteEventPitch(BarelyApi api, BarelyId sequence_id,
                                 BarelyId note_event_id, float* out_pitch);

/// Gets sequence note event position.
///
/// @param api BarelyMusician api.
/// @param instrument_id Sequence identifier.
/// @param note_event_id Note event identifier.
/// @param out_position Output note event position.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetNoteEventPosition(
    BarelyApi api, BarelyId sequence_id, BarelyId note_event_id,
    double* out_position);

/// Gets whether sequence is empty or not.
///
/// @param api BarelyMusician api.
/// @param sequence_id Sequence identifier.
/// @param out_is_empty Output true if empty, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_IsEmpty(BarelyApi api,
                                                  BarelyId sequence_id,
                                                  bool* out_is_empty);

/// Gets whether sequence is looping or not.
///
/// @param api BarelyMusician api.
/// @param sequence_id Sequence identifier.
/// @param out_is_looping Output true if looping, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_IsLooping(BarelyApi api,
                                                    BarelyId sequence_id,
                                                    bool* out_is_looping);

/// Removes all sequence note events.
///
/// @param api BarelyMusician api.
/// @param instrument_id Sequence identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelySequence_RemoveAllNoteEvents(BarelyApi api, BarelyId instrument_id);

/// Removes sequence note event.
///
/// @param api BarelyMusician api.
/// @param instrument_id Sequence identifier.
/// @param note_event_id Note event identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_RemoveNoteEvent(
    BarelyApi api, BarelyId sequence_id, BarelyId note_event_id);

/// Sets sequence begin offset.
///
/// @param api BarelyMusician api.
/// @param sequence_id Sequence identifier.
/// @param begin_offset Begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetBeginOffset(BarelyApi api,
                                                         BarelyId sequence_id,
                                                         double begin_offset);

/// Sets sequence begin position.
///
/// @param api BarelyMusician api.
/// @param sequence_id Sequence identifier.
/// @param begin_position Begin position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetBeginPosition(
    BarelyApi api, BarelyId sequence_id, double begin_position);

/// Sets sequence end position.
///
/// @param api BarelyMusician api.
/// @param sequence_id Sequence identifier.
/// @param end_position End position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetEndPosition(BarelyApi api,
                                                         BarelyId sequence_id,
                                                         double end_position);

/// Sets sequence instrument.
///
/// @param api BarelyMusician api.
/// @param sequence_id Sequence identifier.
/// @param instrument_id Instrument identifier.
BARELY_EXPORT BarelyStatus BarelySequence_SetInstrument(BarelyApi api,
                                                        BarelyId sequence_id,
                                                        BarelyId instrument_id);

/// Sets sequence loop begin offset.
///
/// @param api BarelyMusician api.
/// @param sequence_id Sequence identifier.
/// @param loop_begin_offset Loop begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetLoopBeginOffset(
    BarelyApi api, BarelyId sequence_id, double loop_begin_offset);

/// Sets sequence loop length.
///
/// @param api BarelyMusician api.
/// @param sequence_id Sequence identifier.
/// @param loop_length Loop length in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetLoopLength(BarelyApi api,
                                                        BarelyId sequence_id,
                                                        double loop_length);

/// Sets whether sequence should be looping or not.
///
/// @param api BarelyMusician api.
/// @param sequence_id Sequence identifier.
/// @param is_looping True if looping, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetLooping(BarelyApi api,
                                                     BarelyId sequence_id,
                                                     bool is_looping);

/// Sets sequence note event duration.
///
/// @param api BarelyMusician api.
/// @param instrument_id Sequence identifier.
/// @param note_event_id Note event identifier.
/// @param duration Note duration.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelySequence_SetNoteEventDuration(BarelyApi api, BarelyId sequence_id,
                                    BarelyId note_event_id, double duration);

/// Sets sequence note event intensity.
///
/// @param api BarelyMusician api.
/// @param instrument_id Sequence identifier.
/// @param note_event_id Note event identifier.
/// @param intensity Note event intensity.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelySequence_SetNoteEventIntensity(BarelyApi api, BarelyId sequence_id,
                                     BarelyId note_event_id, float intensity);

/// Sets sequence note event pitch.
///
/// @param api BarelyMusician api.
/// @param instrument_id Sequence identifier.
/// @param note_event_id Note event identifier.
/// @param pitch Note event pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelySequence_SetNoteEventPitch(BarelyApi api, BarelyId sequence_id,
                                 BarelyId note_event_id, BarelyNotePitch pitch);

/// Sets sequence note event position.
///
/// @param api BarelyMusician api.
/// @param instrument_id Sequence identifier.
/// @param note_event_id Note event identifier.
/// @param pitch Note event position.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelySequence_SetNoteEventPosition(BarelyApi api, BarelyId sequence_id,
                                    BarelyId note_event_id, double position);

/// Gets transport position.
///
/// @param api BarelyMusician api.
/// @param out_position Output position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyTransport_GetPosition(BarelyApi api,
                                                       double* out_position);

/// Gets transport tempo.
///
/// @param api BarelyMusician api.
/// @param out_tempo Output tempo in BPM.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyTransport_GetTempo(BarelyApi api,
                                                    double* out_tempo);

/// Gets whether transport is playing or not.
///
/// @param api BarelyMusician api.
/// @param out_is_playing Output true if playing, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyTransport_IsPlaying(BarelyApi api,
                                                     bool* out_is_playing);

/// Sets transport beat callback.
///
/// @param api BarelyMusician api.
/// @param beat_callback Beat callback.
/// @param user_data User data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyTransport_SetBeatCallback(
    BarelyApi api, BarelyTransport_BeatCallback beat_callback, void* user_data);

/// Sets transport position.
///
/// @param api BarelyMusician api.
/// @param position Transport position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyTransport_SetPosition(BarelyApi api,
                                                       double position);

/// Sets transport tempo.
///
/// @param api BarelyMusician api.
/// @param tempo Tempo in BPM.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyTransport_SetTempo(BarelyApi api,
                                                    double tempo);

/// Sets transport update callback.
///
/// @param api BarelyMusician api.
/// @param update_callback Update callback.
/// @param user_data User data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyTransport_SetUpdateCallback(
    BarelyApi api, BarelyTransport_UpdateCallback update_callback,
    void* user_data);

/// Starts transport playback.
///
/// @param api BarelyMusician api.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyTransport_Start(BarelyApi api);

/// Stops transport playback.
///
/// @param api BarelyMusician api.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyTransport_Stop(BarelyApi api);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // PLATFORMS_CAPI_BARELYMUSICIAN_H_
