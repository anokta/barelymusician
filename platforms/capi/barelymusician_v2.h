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

/// Parameter identifier type.
typedef int32_t BarelyParamId;

/// Parameter.
typedef struct BarelyParam {
  /// Identifier.
  BarelyParamId id;

  /// Value.
  float value;
} BarelyParam;

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
/// @param timestamp Timestamp in seconds.
typedef void (*BarelyPlaybackBeatCallback)(double position, double timestamp);

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
/// @param id Parameter identifier.
/// @param value Parameter value.
typedef void (*BarelyConductorSetParamFn)(BarelyConductorState* state,
                                          BarelyParamId id, float value);

// TODO(#85): Add BarelyTransformNoteDurationFn.
// TODO(#85): Add BarelyTransformNoteIntensityFn.
// TODO(#85): Add BarelyTransformNotePitchFn.

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

  // TODO(#85): Add BarelyTransformNoteDurationFn.
  // TODO(#85): Add BarelyTransformNoteIntensityFn.
  // TODO(#85): Add BarelyTransformNotePitchFn.
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
/// @param id Parameter identifier.
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

  /// Set custom data function.
  BarelyInstrumentSetCustomDataFn set_custom_data_fn;

  /// Set note off function.
  BarelyInstrumentSetNoteOffFn set_note_off_fn;

  /// Set note on function.
  BarelyInstrumentSetNoteOnFn set_note_on_fn;

  /// Set parameter function.
  BarelyInstrumentSetParamFn set_param_fn;
} BarelyInstrumentDefinition;

/// Creates new BarelyMusician API.
///
/// @param api Output BarelyMusician API.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApiCreate(BarelyApi* api);

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

/// Stops all active notes of all instruments.
///
/// @param api BarelyMusician API.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyApiSetAllInstrumentNotesOff(BarelyApi api);

/// Resets all parameters of all instruments to default value.
///
/// @param api BarelyMusician API.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyApiSetAllInstrumentParamsToDefault(BarelyApi api);

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

/// Gets all conductor parameters.
///
/// @param api BarelyMusician API.
/// @param out_params Output list of parameters.
/// @param out_num_params Output number of parameters.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductorGetAllParams(BarelyApi api,
                                                       BarelyParam** out_params,
                                                       int32_t* out_num_params);

/// Gets conductor parameter value.
///
/// @param api BarelyMusician API.
/// @param param_id Parameter identifier.
/// @param out_param_value Output parameter value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductorGetParam(BarelyApi api,
                                                   BarelyParamId param_id,
                                                   float* out_param_value);

/// Gets conductor root note.
///
/// @param api BarelyMusician API.
/// @param out_root_note_pitch Output root note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyConductorGetRootNote(BarelyApi api, float* out_root_note_pitch);

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
/// @param param_definitions List of conductor parameter definitions.
/// @param num_param_definitions Number of conductor parameter definitions.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductorSetDefinition(
    BarelyApi api, BarelyConductorDefinition definition,
    BarelyParamDefinition* param_definitions, int32_t num_param_definitions);

/// Sets conductor parameter value.
///
/// @param api BarelyMusician API.
/// @param param_id Parameter identifier.
/// @param param_value Parameter value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductorSetParam(BarelyApi api,
                                                   BarelyParamId param_id,
                                                   float param_value);

/// Resets conductor parameter to default value.
///
/// @param api BarelyMusician API.
/// @param param_id Parameter identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyConductorSetParamToDefault(BarelyApi api, BarelyParamId param_id);

/// Sets conductor root note.
///
/// @param api BarelyMusician API.
/// @param root_note_pitch Root note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductorSetRootNote(BarelyApi api,
                                                      float root_note_pitch);

/// Creates new instrument.
///
/// @param api BarelyMusician API.
/// @param definition Instrument definition.
/// @param param_definitions List of instrument parameter definitions.
/// @param num_param_definitions Number of instrument parameter definitions.
/// @param out_instrument_id Output instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrumentCreate(
    BarelyApi api, BarelyInstrumentDefinition definition,
    BarelyParamDefinition* param_definitions, int32_t num_param_definitions,
    BarelyId* out_instrument_id);

/// Destroys instrument.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrumentDestroy(BarelyApi api,
                                                   BarelyId instrument_id);

/// Gets all active instrument notes.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param out_note_pitches Output list of note pitches.
/// @param out_num_note_pitches Output number of note pitches.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrumentGetAllNotes(
    BarelyApi api, BarelyId instrument_id, float** out_note_pitches,
    int32_t* out_num_note_pitches);

/// Gets all instrument parameters.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param out_params Output list of parameters.
/// @param out_num_params Output number of parameters.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrumentGetAllParams(BarelyApi api, BarelyId instrument_id,
                             BarelyParam** out_params, int32_t* out_num_params);

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
/// @param param_id Parameter identifier.
/// @param out_param_value Output parameter value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrumentGetParam(BarelyApi api,
                                                    BarelyId instrument_id,
                                                    BarelyParamId param_id,
                                                    float* out_param_value);

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
/// @param param_id Parameter identifier.
/// @param param_value Parameter value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrumentSetParam(BarelyApi api,
                                                    BarelyId instrument_id,
                                                    BarelyParamId param_id,
                                                    float param_value);

/// Resets instrument parameter to default value.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param param_id Parameter identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrumentSetParamToDefault(
    BarelyApi api, BarelyId instrument_id, BarelyParamId param_id);

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

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // PLATFORMS_CAPI_BARELYMUSICIAN_H_
