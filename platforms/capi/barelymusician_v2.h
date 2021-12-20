#ifndef PLATFORMS_CAPI_BARELYMUSICIAN_H_
#define PLATFORMS_CAPI_BARELYMUSICIAN_H_

#include <stdint.h>

#include "platforms/capi/visibility.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// BarelyMusician API type.
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

/// Identifier type.
typedef int64_t BarelyId;

/// Parameter identifier type.
typedef int32_t BarelyParamId;

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

/// Creates new BarelyMusician API.
///
/// @param sample_rate Sampling rate in Hz.
/// @param api Output BarelyMusician API.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyCreateApi(int32_t sample_rate, BarelyApi* api);

/// Creates new instrument.
///
/// @param api BarelyMusician API.
/// @param definition Instrument definition.
/// @param param_definitions List of instrument parameter definitions.
/// @param num_param_definitions Number of instrument parameter definitions.
/// @param instrument_id_ptr Output instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyCreateInstrument(
    BarelyApi api, BarelyInstrumentDefinition definition,
    BarelyParamDefinition* param_definitions, int32_t num_param_definitions,
    BarelyId* instrument_id_ptr);

/// Destroys BarelyMusician API.
///
/// @param api BarelyMusician API.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyDestroyApi(BarelyApi api);

/// Destroys instrument.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyDestroyInstrument(BarelyApi api,
                                                   BarelyId instrument_id);

/// Gets instrument parameter value.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param param_id Parameter identifier.
/// @param param_value_ptr Output parameter value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyGetInstrumentParam(BarelyApi api,
                                                    BarelyId instrument_id,
                                                    BarelyParamId param_id,
                                                    float* param_value_ptr);

/// Gets the playback position.
///
/// @param api BarelyMusician API.
/// @param position_ptr Output playback position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyGetPlaybackPosition(BarelyApi api,
                                                     double* position_ptr);

/// Gets the playback tempo.
///
/// @param api BarelyMusician API.
/// @param tempo_ptr Output playback tempo in BPM.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyGetPlaybackTempo(BarelyApi api,
                                                  double* tempo_ptr);

/// Gets the sampling rate.
///
/// @param api BarelyMusician API.
/// @param sample_rate_ptr Output sampling rate in Hz.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyGetSampleRate(BarelyApi api,
                                               int32_t* sample_rate_ptr);

/// Gets whether instrument note is on or not.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param note_pitch Note pitch.
/// @param is_note_on_ptr Output true if on, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyIsInstrumentNoteOn(BarelyApi api,
                                                    BarelyId instrument_id,
                                                    float note_pitch,
                                                    bool* is_note_on_ptr);

/// Gets whether the playback is active or not.
///
/// @param api BarelyMusician API.
/// @param is_playing_ptr Output true if active, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyIsPlaying(BarelyApi api, bool* is_playing_ptr);

/// Sets instrument custom data.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param custom_data Custom data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetInstrumentCustomData(BarelyApi api,
                                                         BarelyId instrument_id,
                                                         void* custom_data);

/// Sets instrument note off.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param note_pitch Note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetInstrumentNoteOff(BarelyApi api,
                                                      BarelyId instrument_id,
                                                      float note_pitch);

/// Sets instrument note on.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param note_pitch Note pitch.
/// @param note_intensity Note intensity.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetInstrumentNoteOn(BarelyApi api,
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
BARELY_EXPORT BarelyStatus BarelySetInstrumentParam(BarelyApi api,
                                                    BarelyId instrument_id,
                                                    BarelyParamId param_id,
                                                    float param_value);

/// Sets instrument parameter value to default.
///
/// @param api BarelyMusician API.
/// @param instrument_id Instrument identifier.
/// @param param_id Parameter identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySetInstrumentParamToDefault(
    BarelyApi api, BarelyId instrument_id, BarelyParamId param_id);

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

/// Sets the sampling rate.
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
