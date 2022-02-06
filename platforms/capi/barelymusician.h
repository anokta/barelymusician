#ifndef PLATFORMS_CAPI_BARELYMUSICIAN_H_
#define PLATFORMS_CAPI_BARELYMUSICIAN_H_

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

/// Beat callback signature.
///
/// @param position Beat position in beats.
/// @param timestamp Beat timestamp in seconds.
/// @param user_data User data.
typedef void (*BarelyApi_BeatCallback)(double position, double timestamp,
                                       void* user_data);

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
typedef void (*BarelyConductorDefinition_SetParameterFn)(void** state,
                                                         int32_t index,
                                                         float value);

/// Conductor set stress function signature.
///
/// @param state Pointer to conductor state.
/// @param stress Stress.
typedef void (*BarelyConductorDefinition_SetStressFn)(void** state,
                                                      float stress);

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
/// @param index Parameter index.
/// @param value Parameter value.
typedef void (*BarelyInstrumentDefinition_SetParameterFn)(void** state,
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
typedef struct BarelyParameterDefinition {
  /// Default value.
  float default_value;

  /// Minimum value.
  float min_value;

  /// Maximum value.
  float max_value;
} BarelyParameterDefinition;

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
  BarelyConductorDefinition_SetParameterFn set_parameter_fn;

  /// Set stress function.
  BarelyConductorDefinition_SetStressFn set_stress_fn;

  /// List of parameter definitions.
  BarelyParameterDefinition* parameter_definitions;

  /// Number of parameter definitions.
  int32_t num_parameter_definitions;
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
  BarelyInstrumentDefinition_SetParameterFn set_parameter_fn;

  /// List of parameter definitions.
  BarelyParameterDefinition* parameter_definitions;

  /// Number of parameter definitions.
  int32_t num_parameter_definitions;
} BarelyInstrumentDefinition;

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

/// Gets playback position.
///
/// @param api BarelyMusician api.
/// @param out_position Output position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_GetPosition(BarelyApi api,
                                                 double* out_position);

/// Gets playback tempo.
///
/// @param api BarelyMusician api.
/// @param out_tempo Output tempo in bpm.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_GetTempo(BarelyApi api, double* out_tempo);

/// Gets whether playback is active or not.
///
/// @param api BarelyMusician api.
/// @param out_is_playing Output true if active, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_IsPlaying(BarelyApi api,
                                               bool* out_is_playing);

/// Sets beat callback.
///
/// @param api BarelyMusician api.
/// @param beat_callback Beat callback.
/// @param user_data User data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_SetBeatCallback(
    BarelyApi api, BarelyApi_BeatCallback beat_callback, void* user_data);

/// Sets playback position.
///
/// @param api BarelyMusician api.
/// @param position Position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_SetPosition(BarelyApi api,
                                                 double position);

/// Sets playback tempo.
///
/// @param api BarelyMusician api.
/// @param tempo Tempo in bpm.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_SetTempo(BarelyApi api, double tempo);

/// Starts playback.
///
/// @param api BarelyMusician api.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_Start(BarelyApi api);

/// Stops playback.
///
/// @param api BarelyMusician api.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_Stop(BarelyApi api);

/// Updates internal state at timestamp.
///
/// @param api BarelyMusician api.
/// @param timestamp Timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApi_Update(BarelyApi api, double timestamp);

/// Conducts note.
///
/// @param api BarelyMusician api.
/// @param pitch_type Note pitch type.
/// @param pitch Note pitch.
/// @param bypass_adjustment True to bypass conductor adjustment.
/// @param out_pitch Output note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_ConductNote(
    BarelyApi api, BarelyNotePitchType pitch_type, float pitch,
    bool bypass_adjustment, float* out_pitch);

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
/// @param index Parameter index.
/// @param out_value Output parameter value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_GetParameter(BarelyApi api,
                                                        int32_t index,
                                                        float* out_value);

/// Gets conductor parameter definition.
///
/// @param api BarelyMusician api.
/// @param index Parameter index.
/// @param out_parameter_definition Output parameter definition.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_GetParameterDefinition(
    BarelyApi api, int32_t index,
    BarelyParameterDefinition* out_parameter_definition);

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
BARELY_EXPORT BarelyStatus BarelyConductor_ResetAllParameters(BarelyApi api);

/// Resets conductor parameter to default value.
///
/// @param api BarelyMusician api.
/// @param index Parameter index.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_ResetParameter(BarelyApi api,
                                                          int32_t index);

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
/// @param index Parameter index.
/// @param value Parameter value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_SetParameter(BarelyApi api,
                                                        int32_t index,
                                                        float value);

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

/// Creates new instrument.
///
/// @param api BarelyMusician api.
/// @param definition Instrument definition.
/// @param sample_rate Sampling rate in hz.
/// @param out_instrument_id Output instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_Create(BarelyApi api, BarelyInstrumentDefinition definition,
                        int32_t sample_rate, BarelyId* out_instrument_id);

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
/// @param index Parameter index.
/// @param out_value Output parameter value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_GetParameter(BarelyApi api,
                                                         BarelyId instrument_id,
                                                         int32_t index,
                                                         float* out_value);

/// Gets instrument parameter definition.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument identifier.
/// @param index Parameter index.
/// @param out_parameter_definition Output parameter definition.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_GetParameterDefinition(
    BarelyApi api, BarelyId instrument_id, int32_t index,
    BarelyParameterDefinition* out_parameter_definition);

/// Gets whether instrument is muted or not.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument identifier.
/// @param out_is_muted Output true if muted, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_IsMuted(BarelyApi api,
                                                    BarelyId instrument_id,
                                                    bool* out_is_muted);

/// Gets whether instrument note is playing or not.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @param out_is_note_on Output true if playing, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_IsNoteOn(BarelyApi api,
                                                     BarelyId instrument_id,
                                                     float pitch,
                                                     bool* out_is_note_on);

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

/// Resets all instrument parameters to default value.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_ResetAllParameters(BarelyApi api, BarelyId instrument_id);

/// Resets instrument parameter to default value.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument identifier.
/// @param index Parameter index.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_ResetParameter(
    BarelyApi api, BarelyId instrument_id, int32_t index);

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
/// @param index Parameter index.
/// @param value Parameter value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetParameter(BarelyApi api,
                                                         BarelyId instrument_id,
                                                         int32_t index,
                                                         float value);

/// Starts instrument note.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_StartNote(BarelyApi api,
                                                      BarelyId instrument_id,
                                                      float pitch,
                                                      float intensity);

/// Stops all instrument notes.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_StopAllNotes(BarelyApi api, BarelyId instrument_id);

/// Stops instrument note.
///
/// @param api BarelyMusician api.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_StopNote(BarelyApi api,
                                                     BarelyId instrument_id,
                                                     float pitch);

/// Adds sequence note at position.
///
/// @param api BarelyMusician api.
/// @param sequence_id Sequence identifier.
/// @param position Note position in beats.
/// @param definition Note definition.
/// @param out_note_id Output note identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelySequence_AddNote(BarelyApi api, BarelyId sequence_id, double position,
                       BarelyNoteDefinition definition, BarelyId* out_note_id);

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

/// Gets sequence note pitch.
///
/// @param api BarelyMusician api.
/// @param sequence_id Sequence identifier.
/// @param note_id Note identifier.
/// @param out_definition Note definition.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetNoteDefinition(
    BarelyApi api, BarelyId sequence_id, BarelyId note_id,
    BarelyNoteDefinition* out_definition);

/// Gets sequence note position.
///
/// @param api BarelyMusician api.
/// @param sequence_id Sequence identifier.
/// @param note_id Note identifier.
/// @param out_position Output note position.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetNotePosition(BarelyApi api,
                                                          BarelyId sequence_id,
                                                          BarelyId note_id,
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

/// Removes all sequence notes.
///
/// @param api BarelyMusician api.
/// @param sequence_id Sequence identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_RemoveAllNotes(BarelyApi api,
                                                         BarelyId sequence_id);

/// Removes sequence note at position.
///
/// @param api BarelyMusician api.
/// @param sequence_id Sequence identifier.
/// @param position Position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_RemoveAllNotesAtPosition(
    BarelyApi api, BarelyId sequence_id, double position);

/// Removes all sequence notes at range.
///
/// @param api BarelyMusician api.
/// @param sequence_id Sequence identifier.
/// @param begin_position Begin position in beats.
/// @param end_position End position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_RemoveAllNotesAtRange(
    BarelyApi api, BarelyId sequence_id, double begin_position,
    double end_position);

/// Removes sequence note.
///
/// @param api BarelyMusician api.
/// @param sequence_id Sequence identifier.
/// @param note_id Note identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_RemoveNote(BarelyApi api,
                                                     BarelyId sequence_id,
                                                     BarelyId note_id);

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

/// Sets sequence note pitch.
///
/// @param api BarelyMusician api.
/// @param sequence_id Sequence identifier.
/// @param note_id Note identifier.
/// @param definition Note definition.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetNoteDefinition(
    BarelyApi api, BarelyId sequence_id, BarelyId note_id,
    BarelyNoteDefinition definition);

/// Sets sequence note position.
///
/// @param api BarelyMusician api.
/// @param sequence_id Sequence identifier.
/// @param note_id Note identifier.
/// @param position Note position.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetNotePosition(BarelyApi api,
                                                          BarelyId sequence_id,
                                                          BarelyId note_id,
                                                          double position);

// TODO(#85): Temporary shortcut to test instruments, move to `//examples/capi`.
BARELY_EXPORT BarelyStatus BarelyExamples_CreateSynthInstrument(
    BarelyApi api, int32_t sample_rate, BarelyId* out_instrument_id);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
// NOLINTEND

#endif  // PLATFORMS_CAPI_BARELYMUSICIAN_H_
