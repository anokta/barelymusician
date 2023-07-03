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

/// ====================
/// barelymusician C API
/// ====================
///
/// barelymusician is a real-time music engine for interactive systems. It is
/// used to generate and perform musical sounds from scratch in a sample
/// accurate way.
///
/// @note To use barelymusician in a C++ project, see the C++ API below.
///
/// -------------
/// Example usage
/// -------------
///
/// - Musician:
///
///   @code{.cpp}
///   #include "barelymusician/barelymusician.h"
///
///   // Create.
///   BarelyMusicianHandle handle;
///   BarelyMusician_Create(&handle);
///
///   // Set the tempo.
///   BarelyMusician_SetTempo(handle, /*tempo=*/124.0);
///
///   // Update the timestamp.
///   //
///   // Timestamp updates must happen prior to processing of instruments for
///   // the respective timestamps. Otherwise, those process calls will be
///   // *late* to receive any relevant state changes. Therefore, this should
///   // typically be called from a main thread update callback with an
///   // additional "lookahead" in order to compensate for the potential thread
///   // synchronization issues in real-time audio applications.
///   double timestamp = 1.0;
///   BarelyMusician_Update(handle, timestamp);
///
///   // Destroy.
///   BarelyMusician_Destroy(handle);
///   @endcode
///
/// - Instrument:
///
///   @code{.cpp}
///   #include "barelymusician/effects/low_pass_effect.h"
///   #include "barelymusician/instruments/synth_instrument.h"
///
///   // Create.
///   BarelyId instrument_id = BarelyId_kInvalid;
///   BarelyInstrument_Create(handle, BarelySynthInstrument_GetDefinition(),
///                           /*frame_rate=*/48000, &instrument_id);
///
///   // Set a note on.
///   //
///   // Pitch values are normalized, where each `1.0` shifts one octave, and
///   // `0.0` represents the middle A (A4) for a typical instrument definition.
///   // However, this is not a strict rule, since both `pitch` and `intensity`
///   // values can be interpreted in any desired way by a custom instrument.
///   BarelyInstrument_SetNoteOn(handle, instrument_id, /*pitch=*/-1.0,
///                              /*intensity=*/0.25);
///
///   // Check if the note is on.
///   bool is_note_on = false;
///   BarelyInstrument_IsNoteOn(handle, instrument_id, /*pitch=*/-1.0,
///                             &is_note_on);
///
///   // Set a control value.
///   BarelyInstrument_SetControl(handle, instrument_id,
///                               BarelySynthControl_kGain,
///                               /*value=*/0.5, /*slope_per_beat=*/0.0);
///
///   // Create a low-pass effect.
///   BarelyId effect_id = BarelyId_kInvalid;
///   BarelyEffect_Create(handle, instrument_id,
///                       BarelyLowPassEffect_GetDefinition(),
///                       /*process_order=*/0, &effect_id);
///
///   // Set the low-pass cutoff frequency to increase by 100 hertz per beat.
///   BarelyEffect_SetControl(handle, instrument_id, effect_id,
///                           BarelyLowPassControl_kCutoffFrequency,
///                           /*value=*/0.0, /*slope_per_beat=*/100.0);
///
///   // Process.
///   //
///   // Instruments expect raw PCM audio buffer to be processed with a
///   // synchronous call. Therefore, this should typically be called from an
///   // audio thread process callback in real-time audio applications.
///   double output_samples[2 * 1024];
///   int output_channel_count = 2;
///   int output_frame_count = 1024;
///   double timestamp = 0.0;
///   BarelyInstrument_Process(handle, instrument_id, output_samples,
///                            output_channel_count, output_frame_count,
///                            timestamp);
///
///   // Destroy.
///   BarelyInstrument_Destroy(handle, instrument_id);
///   @endcode
///
/// - Performer:
///
///   @code{.cpp}
///   // Create.
///   BarelyId performer_id = BarelyId_kInvalid;
///   BarelyPerformer_Create(handle, &performer_id);
///
///   // Create a task.
///   BarelyTaskDefinition definition;  // populate this.
///   BarelyId task_id = BarelyId_kInvalid;
///   BarelyTask_Create(handle, performer_id, definition, /*is_one_off=*/false,
///                     /*position=*/0.0, /*process_order=*/0,
///                     /*user_data=*/nullptr, &task_id);
///
///   // Set looping on.
///   BarelyPerformer_SetLooping(handle, performer_id, /*is_looping=*/true);
///
///   // Start.
///   BarelyPerformer_Start(handle, performer_id);
///
///   // Check if started playing.
///   bool is_playing = false;
///   BarelyPerformer_IsPlaying(handle, performer_id, &is_playing);
///
///   // Destroy the task.
///   BarelyTask_Destroy(handle, performer_id, task_id);
///
///   // Destroy.
///   BarelyPerformer_Destroy(handle, performer_id);
///   @endcode

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Control event callback signature.
///
/// @param index Control index.
/// @param value Control value.
/// @param user_data Pointer to user data.
typedef void (*BarelyControlEventCallback)(int32_t index, double value,
                                           void* user_data);

/// Note control event callback signature.
///
/// @param pitch Note pitch.
/// @param index Note control index.
/// @param value Note control value.
/// @param user_data Pointer to user data.
typedef void (*BarelyNoteControlEventCallback)(double pitch, int32_t index,
                                               double value, void* user_data);

/// Note off event callback signature.
///
/// @param pitch Note pitch.
/// @param user_data Pointer to user data.
typedef void (*BarelyNoteOffEventCallback)(double pitch, void* user_data);

/// Note on event callback signature.
///
/// @param pitch Note pitch.
/// @param intensity Note intensity.
/// @param user_data Pointer to user data.
typedef void (*BarelyNoteOnEventCallback)(double pitch, double intensity,
                                          void* user_data);

/// Effect definition create callback signature.
///
/// @param state Pointer to effect state.
/// @param frame_rate Frame rate in hertz.
typedef void (*BarelyEffectDefinition_CreateCallback)(void** state,
                                                      int32_t frame_rate);

/// Effect definition destroy callback signature.
///
/// @param state Pointer to effect state.
typedef void (*BarelyEffectDefinition_DestroyCallback)(void** state);

/// Effect definition process callback signature.
///
/// @param state Pointer to effect state.
/// @param output_samples Array of output samples.
/// @param output_channel_count Number of output channels.
/// @param output_frame_count Number of output frames.
typedef void (*BarelyEffectDefinition_ProcessCallback)(
    void** state, double* output_samples, int32_t output_channel_count,
    int32_t output_frame_count);

/// Effect definition set control callback signature.
///
/// @param state Pointer to effect state.
/// @param index Control index.
/// @param value Control value.
/// @param slope_per_frame Control slope in value change per frame.
typedef void (*BarelyEffectDefinition_SetControlCallback)(
    void** state, int32_t index, double value, double slope_per_frame);

/// Effect definition set data callback signature.
///
/// @param state Pointer to effect state.
/// @param data Pointer to immutable data.
/// @param size Data size in bytes.
typedef void (*BarelyEffectDefinition_SetDataCallback)(void** state,
                                                       const void* data,
                                                       int32_t size);

/// Instrument definition create callback signature.
///
/// @param state Pointer to instrument state.
/// @param frame_rate Frame rate in hertz.
typedef void (*BarelyInstrumentDefinition_CreateCallback)(void** state,
                                                          int32_t frame_rate);

/// Instrument definition destroy callback signature.
///
/// @param state Pointer to instrument state.
typedef void (*BarelyInstrumentDefinition_DestroyCallback)(void** state);

/// Instrument definition process callback signature.
///
/// @param state Pointer to instrument state.
/// @param output_samples Array of output samples.
/// @param output_channel_count Number of output channels.
/// @param output_frame_count Number of output frames.
typedef void (*BarelyInstrumentDefinition_ProcessCallback)(
    void** state, double* output_samples, int32_t output_channel_count,
    int32_t output_frame_count);

/// Instrument definition set control callback signature.
///
/// @param state Pointer to instrument state.
/// @param index Control index.
/// @param value Control value.
/// @param slope_per_frame Control slope in value change per frame.
typedef void (*BarelyInstrumentDefinition_SetControlCallback)(
    void** state, int32_t index, double value, double slope_per_frame);

/// Instrument definition set data callback signature.
///
/// @param state Pointer to instrument state.
/// @param data Pointer to immutable data.
/// @param size Data size in bytes.
typedef void (*BarelyInstrumentDefinition_SetDataCallback)(void** state,
                                                           const void* data,
                                                           int32_t size);

/// Instrument definition set note control callback signature.
///
/// @param state Pointer to instrument state.
/// @param pitch Note pitch.
/// @param index Note control index.
/// @param value Note control value.
/// @param slope_per_frame Note control slope in value change per frame.
typedef void (*BarelyInstrumentDefinition_SetNoteControlCallback)(
    void** state, double pitch, int32_t index, double value,
    double slope_per_frame);

/// Instrument definition set note off callback signature.
///
/// @param state Pointer to instrument state.
/// @param pitch Note pitch.
typedef void (*BarelyInstrumentDefinition_SetNoteOffCallback)(void** state,
                                                              double pitch);

/// Instrument definition set note on callback signature.
///
/// @param state Pointer to instrument state.
/// @param pitch Note pitch.
/// @param intensity Note intensity.
typedef void (*BarelyInstrumentDefinition_SetNoteOnCallback)(void** state,
                                                             double pitch,
                                                             double intensity);

/// Task definition create callback signature.
///
/// @param state Pointer to task state.
/// @param user_data Pointer to user data.
typedef void (*BarelyTaskDefinition_CreateCallback)(void** state,
                                                    void* user_data);

/// Task definition destroy callback signature.
///
/// @param state Pointer to task state.
typedef void (*BarelyTaskDefinition_DestroyCallback)(void** state);

/// Task definition process callback signature.
///
/// @param state Pointer to task state.
typedef void (*BarelyTaskDefinition_ProcessCallback)(void** state);

/// Control definition.
typedef struct BarelyControlDefinition {
  /// Default value.
  double default_value;

  /// Minimum value.
  double min_value;

  /// Maximum value.
  double max_value;
} BarelyControlDefinition;

/// Effect definition.
typedef struct BarelyEffectDefinition {
  /// Create callback.
  BarelyEffectDefinition_CreateCallback create_callback;

  /// Destroy callback.
  BarelyEffectDefinition_DestroyCallback destroy_callback;

  /// Process callback.
  BarelyEffectDefinition_ProcessCallback process_callback;

  /// Set control callback.
  BarelyEffectDefinition_SetControlCallback set_control_callback;

  /// Set data callback.
  BarelyEffectDefinition_SetDataCallback set_data_callback;

  /// Array of control definitions.
  const BarelyControlDefinition* control_definitions;

  /// Number of control definitions.
  int32_t control_definition_count;
} BarelyEffectDefinition;

/// Instrument definition.
typedef struct BarelyInstrumentDefinition {
  /// Create callback.
  BarelyInstrumentDefinition_CreateCallback create_callback;

  /// Destroy callback.
  BarelyInstrumentDefinition_DestroyCallback destroy_callback;

  /// Process callback.
  BarelyInstrumentDefinition_ProcessCallback process_callback;

  /// Set control callback.
  BarelyInstrumentDefinition_SetControlCallback set_control_callback;

  /// Set data callback.
  BarelyInstrumentDefinition_SetDataCallback set_data_callback;

  /// Set note control callback.
  BarelyInstrumentDefinition_SetNoteControlCallback set_note_control_callback;

  /// Set note off callback.
  BarelyInstrumentDefinition_SetNoteOffCallback set_note_off_callback;

  /// Set note on callback.
  BarelyInstrumentDefinition_SetNoteOnCallback set_note_on_callback;

  /// Array of control definitions.
  const BarelyControlDefinition* control_definitions;

  /// Number of control definitions.
  int32_t control_definition_count;

  /// Array of note control definitions.
  const BarelyControlDefinition* note_control_definitions;

  /// Number of note control definitions.
  int32_t note_control_definition_count;
} BarelyInstrumentDefinition;

/// Task definition.
typedef struct BarelyTaskDefinition {
  /// Create callback.
  BarelyTaskDefinition_CreateCallback create_callback;

  /// Destroy callback.
  BarelyTaskDefinition_DestroyCallback destroy_callback;

  /// Process callback.
  BarelyTaskDefinition_ProcessCallback process_callback;
} BarelyTaskDefinition;

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
  /// Unimplemented error.
  BarelyStatus_kUnimplemented = 3,
  /// Internal error.
  BarelyStatus_kInternal = 4,
};

/// Creates a new effect.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param definition Effect definition.
/// @param process_order Effect process order.
/// @param out_effect_id Output effect identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyEffect_Create(BarelyMusicianHandle handle, BarelyId instrument_id,
                    BarelyEffectDefinition definition, int32_t process_order,
                    BarelyId* out_effect_id);

/// Destroys an effect.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param effect_id Effect identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyEffect_Destroy(BarelyMusicianHandle handle,
                                                BarelyId instrument_id,
                                                BarelyId effect_id);

/// Gets an effect control value.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param effect_id Effect identifier.
/// @param index Control index.
/// @param out_value Output control value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyEffect_GetControl(BarelyMusicianHandle handle,
                                                   BarelyId instrument_id,
                                                   BarelyId effect_id,
                                                   int32_t index,
                                                   double* out_value);

/// Gets the process order of an effect.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param effect_id Effect identifier.
/// @param out_process_order Output process order.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyEffect_GetProcessOrder(
    BarelyMusicianHandle handle, BarelyId instrument_id, BarelyId effect_id,
    int32_t* out_process_order);

/// Resets all effect control values.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param effect_id Effect identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyEffect_ResetAllControls(
    BarelyMusicianHandle handle, BarelyId instrument_id, BarelyId effect_id);

/// Resets an effect control value.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param effect_id Effect identifier.
/// @param index Control index.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyEffect_ResetControl(BarelyMusicianHandle handle, BarelyId instrument_id,
                          BarelyId effect_id, int32_t index);

/// Sets an effect control value.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param effect_id Effect identifier.
/// @param index Control index.
/// @param value Control value.
/// @param slope_per_beat Control slope in value change per beat.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyEffect_SetControl(BarelyMusicianHandle handle,
                                                   BarelyId instrument_id,
                                                   BarelyId effect_id,
                                                   int32_t index, double value,
                                                   double slope_per_beat);

/// Sets the control event callback of an effect.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param effect_id Effect identifier.
/// @param callback Control event callback.
/// @param user_data Pointer to user data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyEffect_SetControlEventCallback(
    BarelyMusicianHandle handle, BarelyId instrument_id, BarelyId effect_id,
    BarelyControlEventCallback callback, void* user_data);

/// Sets effect data.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param effect_id Effect identifier.
/// @param data Pointer to immutable data.
/// @param size Data size in bytes.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyEffect_SetData(BarelyMusicianHandle handle,
                                                BarelyId instrument_id,
                                                BarelyId effect_id,
                                                const void* data, int32_t size);

/// Sets the process order of an effect.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param effect_id Effect identifier.
/// @param process_order Process order.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyEffect_SetProcessOrder(
    BarelyMusicianHandle handle, BarelyId instrument_id, BarelyId effect_id,
    int32_t process_order);

/// Creates a new instrument.
///
/// @param handle Musician handle.
/// @param definition Instrument definition.
/// @param frame_rate Frame rate in hertz.
/// @param out_instrument_id Output instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_Create(
    BarelyMusicianHandle handle, BarelyInstrumentDefinition definition,
    int32_t frame_rate, BarelyId* out_instrument_id);

/// Destroys an instrument.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_Destroy(BarelyMusicianHandle handle,
                                                    BarelyId instrument_id);

/// Gets an instrument control value.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param index Control index.
/// @param out_value Output control value.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_GetControl(BarelyMusicianHandle handle, BarelyId instrument_id,
                            int32_t index, double* out_value);

/// Gets an instrument note control value.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @param index Note control index.
/// @param out_value Output note control value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_GetNoteControl(
    BarelyMusicianHandle handle, BarelyId instrument_id, double pitch,
    int32_t index, double* out_value);

/// Gets whether an instrument note is on or not.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @param out_is_note_on Output true if on, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_IsNoteOn(BarelyMusicianHandle handle, BarelyId instrument_id,
                          double pitch, bool* out_is_note_on);

/// Processes instrument output samples at timestamp.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param output_samples Array of interleaved output samples.
/// @param output_channel_count Number of output channels.
/// @param output_frame_count Number of output frames.
/// @param timestamp Timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_Process(
    BarelyMusicianHandle handle, BarelyId instrument_id, double* output_samples,
    int32_t output_channel_count, int32_t output_frame_count, double timestamp);

/// Resets all instrument control values.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_ResetAllControls(
    BarelyMusicianHandle handle, BarelyId instrument_id);

/// Resets all instrument note control values.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_ResetAllNoteControls(
    BarelyMusicianHandle handle, BarelyId instrument_id, double pitch);

/// Resets an instrument control value.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param index Control index.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_ResetControl(
    BarelyMusicianHandle handle, BarelyId instrument_id, int32_t index);

/// Resets an instrument note control value.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @param index Note control index.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_ResetNoteControl(
    BarelyMusicianHandle handle, BarelyId instrument_id, double pitch,
    int32_t index);

/// Sets all instrument notes off.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetAllNotesOff(
    BarelyMusicianHandle handle, BarelyId instrument_id);

/// Sets an instrument control value.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param index Control index.
/// @param value Control value.
/// @param slope_per_beat Control slope in value change per beat.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_SetControl(BarelyMusicianHandle handle, BarelyId instrument_id,
                            int32_t index, double value, double slope_per_beat);

/// Sets the control event callback of an instrument.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param callback Control event callback.
/// @param user_data Pointer to user data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetControlEventCallback(
    BarelyMusicianHandle handle, BarelyId instrument_id,
    BarelyControlEventCallback callback, void* user_data);

/// Sets instrument data.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param data Pointer to immutable data.
/// @param size Data size in bytes.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetData(BarelyMusicianHandle handle,
                                                    BarelyId instrument_id,
                                                    const void* data,
                                                    int32_t size);

/// Sets an instrument note control value.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @param index Note control index.
/// @param value Note control value.
/// @param slope_per_beat Note control slope in value change per beat.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetNoteControl(
    BarelyMusicianHandle handle, BarelyId instrument_id, double pitch,
    int32_t index, double value, double slope_per_beat);

/// Sets the note control event callback of an instrument.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param callback Note control event callback.
/// @param user_data Pointer to user data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetNoteControlEventCallback(
    BarelyMusicianHandle handle, BarelyId instrument_id,
    BarelyNoteControlEventCallback callback, void* user_data);

/// Sets an instrument note off.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetNoteOff(
    BarelyMusicianHandle handle, BarelyId instrument_id, double pitch);

/// Sets the note off event callback of an instrument.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param callback Note off event callback.
/// @param user_data Pointer to user data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetNoteOffEventCallback(
    BarelyMusicianHandle handle, BarelyId instrument_id,
    BarelyNoteOffEventCallback callback, void* user_data);

/// Sets an instrument note on.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @param intensity Note intensity.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_SetNoteOn(BarelyMusicianHandle handle, BarelyId instrument_id,
                           double pitch, double intensity);

/// Sets the note on event callback of an instrument.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param callback Note on event callback.
/// @param user_data Pointer to user data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetNoteOnEventCallback(
    BarelyMusicianHandle handle, BarelyId instrument_id,
    BarelyNoteOnEventCallback callback, void* user_data);

/// Creates a new musician.
///
/// @param out_handle Output musician handle.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyMusician_Create(BarelyMusicianHandle* out_handle);

/// Destroys a musician.
///
/// @param handle Musician handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyMusician_Destroy(BarelyMusicianHandle handle);

/// Gets the tempo of a musician.
///
/// @param handle Musician handle.
/// @param out_tempo Output tempo in beats per minute.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyMusician_GetTempo(BarelyMusicianHandle handle,
                                                   double* out_tempo);

/// Gets the timestamp of a musician.
///
/// @param handle Musician handle.
/// @param out_timestamp Output timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyMusician_GetTimestamp(BarelyMusicianHandle handle, double* out_timestamp);

/// Sets the tempo of a musician.
///
/// @param handle Musician handle.
/// @param tempo Tempo in beats per minute.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyMusician_SetTempo(BarelyMusicianHandle handle,
                                                   double tempo);

/// Updates a musician at timestamp.
///
/// @param handle Musician handle.
/// @param timestamp Timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyMusician_Update(BarelyMusicianHandle handle,
                                                 double timestamp);

/// Creates a new performer.
///
/// @param handle Musician handle.
/// @param out_performer_id Output performer identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_Create(BarelyMusicianHandle handle,
                                                  BarelyId* out_performer_id);

/// Destroys a performer.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_Destroy(BarelyMusicianHandle handle,
                                                   BarelyId performer_id);

/// Gets the loop begin position of a performer.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @param out_loop_begin_position Output loop begin position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_GetLoopBeginPosition(
    BarelyMusicianHandle handle, BarelyId performer_id,
    double* out_loop_begin_position);

/// Gets the loop length of a performer.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @param out_loop_length Output loop length.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyPerformer_GetLoopLength(BarelyMusicianHandle handle,
                              BarelyId performer_id, double* out_loop_length);

/// Gets the position of a performer.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @param out_position Output position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_GetPosition(
    BarelyMusicianHandle handle, BarelyId performer_id, double* out_position);

/// Gets whether a performer is looping or not.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @param out_is_looping Output true if looping, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_IsLooping(
    BarelyMusicianHandle handle, BarelyId performer_id, bool* out_is_looping);

/// Gets whether a performer is playing or not.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @param out_is_playing Output true if playing, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_IsPlaying(
    BarelyMusicianHandle handle, BarelyId performer_id, bool* out_is_playing);

/// Sets the loop begin position of a performer.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @param loop_begin_position Loop begin position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_SetLoopBeginPosition(
    BarelyMusicianHandle handle, BarelyId performer_id,
    double loop_begin_position);

/// Sets the loop length of a performer.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @param loop_length Loop length in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_SetLoopLength(
    BarelyMusicianHandle handle, BarelyId performer_id, double loop_length);

/// Sets whether a performer is looping or not.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @param is_looping True if looping, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_SetLooping(
    BarelyMusicianHandle handle, BarelyId performer_id, bool is_looping);

/// Sets the position of a performer.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @param position Position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_SetPosition(
    BarelyMusicianHandle handle, BarelyId performer_id, double position);

/// Starts a performer.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_Start(BarelyMusicianHandle handle,
                                                 BarelyId performer_id);

/// Stops a performer.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_Stop(BarelyMusicianHandle handle,
                                                BarelyId performer_id);

/// Creates a new task.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @param definition Task definition.
/// @param is_one_off True if one-off task, false otherwise.
/// @param position Task position in beats.
/// @param process_order Task process order.
/// @param user_data Pointer to user data.
/// @param out_task_id Output task identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyTask_Create(
    BarelyMusicianHandle handle, BarelyId performer_id,
    BarelyTaskDefinition definition, bool is_one_off, double position,
    int32_t process_order, void* user_data, BarelyId* out_task_id);

/// Destroys a task.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @param task_id Task identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyTask_Destroy(BarelyMusicianHandle handle,
                                              BarelyId performer_id,
                                              BarelyId task_id);

/// Gets the position of a task.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @param task_id Task identifier.
/// @param out_position Output position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyTask_GetPosition(BarelyMusicianHandle handle,
                                                  BarelyId performer_id,
                                                  BarelyId task_id,
                                                  double* out_position);

/// Gets the process order of a task.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @param task_id Task identifier.
/// @param out_process_order Output process order.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyTask_GetProcessOrder(BarelyMusicianHandle handle, BarelyId performer_id,
                           BarelyId task_id, int32_t* out_process_order);

/// Sets the position of a task.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @param task_id Task identifier.
/// @param position Position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyTask_SetPosition(BarelyMusicianHandle handle,
                                                  BarelyId performer_id,
                                                  BarelyId task_id,
                                                  double position);

/// Sets the process order of a task.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @param task_id Task identifier.
/// @param process_order Process order.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyTask_SetProcessOrder(BarelyMusicianHandle handle, BarelyId performer_id,
                           BarelyId task_id, int32_t process_order);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
// NOLINTEND

#ifdef __cplusplus
#include <cassert>
#include <compare>
#include <functional>
#include <limits>
#include <memory>
#include <span>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>

/// ======================
/// barelymusician C++ API
/// ======================
///
/// barelymusician is a real-time music engine for interactive systems. It is
/// used to generate and perform musical sounds from scratch in a sample
/// accurate way.
///
/// -------------
/// Example usage
/// -------------
///
/// - Musician:
///
///   @code{.cpp}
///   #include "barelymusician/barelymusician.h"
///
///   // Create.
///   barely::Musician musician;
///
///   // Set the tempo.
///   musician.SetTempo(/*tempo=*/124.0);
///
///   // Update the timestamp.
///   //
///   // Timestamp updates must happen prior to processing of instruments for
///   // the respective timestamps. Otherwise, those process calls will be
///   // *late* to receive any relevant state changes. Therefore, this should
///   // typically be called from a main thread update callback with an
///   // additional "lookahead" in order to compensate for the potential thread
///   // synchronization issues in real-time audio applications.
///   double timestamp = 1.0;
///   musician.Update(timestamp);
///   @endcode
///
/// - Instrument:
///
///   @code{.cpp}
///   #include "barelymusician/instruments/low_pass_effect.h"
///   #include "barelymusician/instruments/synth_instrument.h"
///
///   // Create.
///   auto instrument =
///       musician.CreateInstrument(barely::SynthInstrument::GetDefinition(),
///                                 /*frame_rate=*/48000);
///
///   // Set a note on.
///   //
///   // Pitch values are normalized, where each `1.0` shifts one octave, and
///   // `0.0` represents the middle A (A4) for a typical instrument definition.
///   // However, this is not a strict rule, since both `pitch` and `intensity`
///   // values can be interpreted in any desired way by a custom instrument.
///   instrument.SetNoteOn(/*pitch=*/-1.0, /*intensity=*/0.25);
///
///   // Check if the note is on.
///   const bool is_note_on = *instrument.IsNoteOn(/*pitch=*/-1.0);
///
///   // Set a control value.
///   instrument.SetControl(barely::SynthControl::kGain, /*value=*/0.5,
///                         /*slope_per_beat=*/0.0);
///
///   // Create a low-pass effect.
///   auto effect =
///       instrument.CreateEffect(barely::LowPassEffect::GetDefinition());
///
///   // Set the low-pass cutoff frequency to increase by 100 hertz per beat.
///   effect->SetControl(barely::LowPassControl::kCutoffFrequency,
///                      /*value=*/0.0, /*slope_per_beat=*/100.0);
///
///   // Process.
///   //
///   // Instruments expect raw PCM audio buffer to be processed with a
///   // synchronous call. Therefore, this should typically be called from an
///   // audio thread process callback in real-time audio applications.
///   const int output_channel_count = 2;
///   const int output_frame_count = 1024;
///   std::vector<double> output_samples(
///       output_channel_count * output_frame_count, 0.0);
///   double timestamp = 0.0;
///   instrument.Process(output_samples.data(), output_channel_count,
///                      output_frame_count, timestamp);
///
///   // Destroy.
///   musician.DestroyInstrument(std::move(instrument));
///   @endcode
///
/// - Performer:
///
///   @code{.cpp}
///   // Create.
///   auto performer = musician.CreatePerformer();
///
///   // Create a task.
///   auto task = performer.CreateTask([]() {},  // populate this.
///                                    /*is_one_off=*/false, /*position=*/0.0,
///                                    /*process_order=*/0);
///
///   // Set looping on.
///   performer.SetLooping(/*is_looping=*/true);
///
///   // Start.
///   performer.Start();
///
///   // Check if started playing.
///   const bool is_playing = *performer.IsPlaying();
///
///   // Destroy the task.
///   performer.DestroyTask(std::move(task));
///
///   // Destroy.
///   musician.DestroyPerformer(std::move(performer));
///   @endcode

namespace barely {

/// Control event callback signature.
///
/// @param index Control index.
/// @param value Control value.
using ControlEventCallback = std::function<void(int index, double value)>;

/// Note control event callback signature.
///
/// @param pitch Note pitch.
/// @param index Note control index.
/// @param value Note control value.
using NoteControlEventCallback =
    std::function<void(double pitch, int index, double value)>;

/// Note off event callback signature.
///
/// @param pitch Note pitch.
using NoteOffEventCallback = std::function<void(double pitch)>;

/// Note on event callback signature.
///
/// @param pitch Note pitch.
/// @param intensity Note intensity.
using NoteOnEventCallback = std::function<void(double pitch, double intensity)>;

/// Task callback.
using TaskCallback = std::function<void()>;

/// Status.
class Status {
 public:
  /// Enum values.
  enum Enum : BarelyStatus {
    /// Success.
    kOk = BarelyStatus_kOk,
    /// Invalid argument error.
    kInvalidArgument = BarelyStatus_kInvalidArgument,
    /// Not found error.
    kNotFound = BarelyStatus_kNotFound,
    /// Unimplemented error.
    kUnimplemented = BarelyStatus_kUnimplemented,
    /// Internal error.
    kInternal = BarelyStatus_kInternal,
  };

  /// Returns a new `Status` with `Status::kOk`.
  ///
  /// @return Status.
  static Status Ok() noexcept { return Status::kOk; }

  /// Returns a new `Status` with `Status::kInvalidArgument`.
  ///
  /// @return Status.
  static Status InvalidArgument() noexcept { return Status::kInvalidArgument; }

  /// Returns a new `Status` with `Status::kNotFound`.
  ///
  /// @return Status.
  static Status NotFound() noexcept { return Status::kNotFound; }

  /// Returns a new `Status` with `Status::kUnimplemented`.
  ///
  /// @return Status.
  static Status Unimplemented() noexcept { return Status::kUnimplemented; }

  /// Returns a new `Status` with `Status::kInternal`.
  ///
  /// @return Status.
  static Status Internal() noexcept { return Status::kInternal; }

  /// Constructs a new `Status`.
  ///
  /// @param status Status enum.
  // NOLINTNEXTLINE(google-explicit-constructor)
  Status(Enum status) noexcept : status_(status) {}

  /// Constructs a new `Status` from a raw type.
  ///
  /// @param status Raw status enum.
  // NOLINTNEXTLINE(google-explicit-constructor)
  Status(BarelyStatus status) noexcept : status_(static_cast<Enum>(status)) {}

  /// Returns the enum value.
  ///
  /// @return Enum value.
  // NOLINTNEXTLINE(google-explicit-constructor)
  operator Enum() const noexcept { return status_; }

  /// Enum comparators.
  auto operator<=>(Enum status) const noexcept { return status_ <=> status; }

  /// Returns whether the status is okay or not.
  ///
  /// @return True if okay, false otherwise.
  [[nodiscard]] bool IsOk() const noexcept { return status_ == kOk; }

  /// Returns the status string.
  ///
  /// @return Status string.
  [[nodiscard]] std::string ToString() const noexcept {
    switch (status_) {
      case kOk:
        return "Ok";
      case kInvalidArgument:
        return "Invalid argument error";
      case kNotFound:
        return "Not found error";
      case kUnimplemented:
        return "Unimplemented error";
      case kInternal:
        return "Internal error";
      default:
        return "Unknown error";
    }
  }

 private:
  // Enum value.
  Enum status_;
};

/// Value, or an error status.
template <typename ValueType>
class StatusOr {
 public:
  /// Constructs a new `StatusOr` with an error status.
  ///
  /// @param error_status Error status.
  // NOLINTNEXTLINE(google-explicit-constructor)
  StatusOr(Status error_status) noexcept : value_or_(error_status) {
    assert(!error_status.IsOk());
  }

  /// Constructs a new `StatusOr` with a value.
  ///
  /// @param value Value.
  // NOLINTNEXTLINE(google-explicit-constructor)
  StatusOr(ValueType value) noexcept : value_or_(std::move(value)) {}

  /// Member access operators.
  const ValueType& operator*() const noexcept { return GetValue(); }
  const ValueType* operator->() const noexcept { return &GetValue(); }
  ValueType& operator*() noexcept { return GetValue(); }
  ValueType* operator->() noexcept { return &GetValue(); }

  /// Returns the contained error status.
  ///
  /// @return Error status.
  [[nodiscard]] Status GetErrorStatus() const noexcept {
    assert(std::holds_alternative<Status>(value_or_));
    return std::get<Status>(value_or_);
  }

  /// Returns the contained value.
  ///
  /// @return Value.
  [[nodiscard]] const ValueType& GetValue() const noexcept {
    assert(std::holds_alternative<ValueType>(value_or_));
    return std::get<ValueType>(value_or_);
  }

  /// Returns the contained value.
  ///
  /// @return Mutable value.
  [[nodiscard]] ValueType& GetValue() noexcept {
    assert(std::holds_alternative<ValueType>(value_or_));
    return std::get<ValueType>(value_or_);
  }

  /// Returns whether a value is contained or not.
  ///
  /// @return True if contained, false otherwise.
  [[nodiscard]] bool IsOk() const noexcept {
    return std::holds_alternative<ValueType>(value_or_);
  }

 private:
  // Value or an error status.
  std::variant<Status, ValueType> value_or_;
};

/// Control definition.
struct ControlDefinition : public BarelyControlDefinition {
  /// Constructs a new `ControlDefinition`.
  ///
  /// @param default_value Default value.
  /// @param min_value Minimum value.
  /// @param max_value Maximum value.
  explicit ControlDefinition(
      double default_value,
      double min_value = std::numeric_limits<double>::lowest(),
      double max_value = std::numeric_limits<double>::max()) noexcept
      : ControlDefinition(BarelyControlDefinition{
            default_value,
            min_value,
            max_value,
        }) {}

  /// Constructs a new `ControlDefinition` with a boolean type.
  ///
  /// @param default_value Default boolean value.
  explicit ControlDefinition(bool default_value) noexcept
      : ControlDefinition(static_cast<double>(default_value)) {}

  /// Constructs a new `ControlDefinition` with an integer type.
  ///
  /// @param default_value Default integer value.
  /// @param min_value Minimum integer value.
  /// @param max_value Maximum integer value.
  explicit ControlDefinition(
      int default_value, int min_value = std::numeric_limits<int>::lowest(),
      int max_value = std::numeric_limits<int>::max()) noexcept
      : ControlDefinition(static_cast<double>(default_value),
                          static_cast<double>(min_value),
                          static_cast<double>(max_value)) {}

  /// Constructs a new `ControlDefinition` from a raw type.
  ///
  /// @param definition Raw control definition.
  // NOLINTNEXTLINE(google-explicit-constructor)
  ControlDefinition(BarelyControlDefinition definition) noexcept
      : BarelyControlDefinition{definition} {
    assert(default_value >= min_value && default_value <= max_value);
  }
};

/// Effect definition.
struct EffectDefinition : public BarelyEffectDefinition {
  /// Create callback signature.
  using CreateCallback = BarelyEffectDefinition_CreateCallback;

  /// Destroy callback signature.
  using DestroyCallback = BarelyEffectDefinition_DestroyCallback;

  /// Process callback signature.
  using ProcessCallback = BarelyEffectDefinition_ProcessCallback;

  /// Set control callback signature.
  using SetControlCallback = BarelyEffectDefinition_SetControlCallback;

  /// Set data callback signature.
  using SetDataCallback = BarelyEffectDefinition_SetDataCallback;

  /// Constructs a new `EffectDefinition`.
  ///
  /// @param create_callback Create callback.
  /// @param destroy_callback Destroy callback.
  /// @param process_callback Process callback.
  /// @param set_control_callback Set control callback.
  /// @param set_data_callback Set data callback.
  /// @param control_definitions Span of control definitions.
  explicit EffectDefinition(
      CreateCallback create_callback, DestroyCallback destroy_callback,
      ProcessCallback process_callback, SetControlCallback set_control_callback,
      SetDataCallback set_data_callback,
      std::span<const ControlDefinition> control_definitions) noexcept
      : EffectDefinition({
            create_callback,
            destroy_callback,
            process_callback,
            set_control_callback,
            set_data_callback,
            control_definitions.data(),
            static_cast<int>(control_definitions.size()),
        }) {}

  /// Constructs a new `EffectDefinition` from a raw type.
  ///
  /// @param definition Raw effect definition.
  // NOLINTNEXTLINE(google-explicit-constructor)
  EffectDefinition(BarelyEffectDefinition definition) noexcept
      : BarelyEffectDefinition{definition} {
    assert(control_definitions || control_definition_count == 0);
    assert(control_definition_count >= 0);
  }
};

/// Instrument definition.
struct InstrumentDefinition : public BarelyInstrumentDefinition {
  /// Create callback signature.
  using CreateCallback = BarelyInstrumentDefinition_CreateCallback;

  /// Destroy callback signature.
  using DestroyCallback = BarelyInstrumentDefinition_DestroyCallback;

  /// Process callback signature.
  using ProcessCallback = BarelyInstrumentDefinition_ProcessCallback;

  /// Set control callback signature.
  using SetControlCallback = BarelyInstrumentDefinition_SetControlCallback;

  /// Set data callback signature.
  using SetDataCallback = BarelyInstrumentDefinition_SetDataCallback;

  /// Set note control callback signature
  using SetNoteControlCallback =
      BarelyInstrumentDefinition_SetNoteControlCallback;

  /// Set note off callback signature
  using SetNoteOffCallback = BarelyInstrumentDefinition_SetNoteOffCallback;

  /// Set note on callback signature.
  using SetNoteOnCallback = BarelyInstrumentDefinition_SetNoteOnCallback;

  /// Constructs a new `InstrumentDefinition`.
  ///
  /// @param create_callback Create callback.
  /// @param destroy_callback Destroy callback.
  /// @param process_callback Process callback.
  /// @param set_control_callback Set control callback.
  /// @param set_data_callback Set data callback.
  /// @param set_note_off_callback Set note off callback.
  /// @param set_note_on_callback Set note on callback.
  /// @param control_definitions Span of control definitions.
  /// @param note_control_definitions Span of note control definitions.
  explicit InstrumentDefinition(
      CreateCallback create_callback, DestroyCallback destroy_callback,
      ProcessCallback process_callback, SetControlCallback set_control_callback,
      SetDataCallback set_data_callback,
      SetNoteControlCallback set_note_control_callback,
      SetNoteOffCallback set_note_off_callback,
      SetNoteOnCallback set_note_on_callback,
      std::span<const ControlDefinition> control_definitions,
      std::span<const ControlDefinition> note_control_definitions) noexcept
      : InstrumentDefinition({
            create_callback,
            destroy_callback,
            process_callback,
            set_control_callback,
            set_data_callback,
            set_note_control_callback,
            set_note_off_callback,
            set_note_on_callback,
            control_definitions.data(),
            static_cast<int>(control_definitions.size()),
            note_control_definitions.data(),
            static_cast<int>(note_control_definitions.size()),
        }) {}

  /// Constructs a new `InstrumentDefinition` from a raw type.
  ///
  /// @param definition Raw instrument definition.
  // NOLINTNEXTLINE(google-explicit-constructor)
  InstrumentDefinition(BarelyInstrumentDefinition definition) noexcept
      : BarelyInstrumentDefinition{definition} {
    assert(control_definitions || control_definition_count == 0);
    assert(control_definition_count >= 0);
    assert(note_control_definitions || note_control_definition_count == 0);
    assert(note_control_definition_count >= 0);
  }
};

/// Task definition.
struct TaskDefinition : public BarelyTaskDefinition {
  /// Create callback signature.
  using CreateCallback = BarelyTaskDefinition_CreateCallback;

  /// Destroy callback signature.
  using DestroyCallback = BarelyTaskDefinition_DestroyCallback;

  /// Process callback signature.
  using ProcessCallback = BarelyTaskDefinition_ProcessCallback;

  /// Constructs a new `TaskDefinition`.
  ///
  /// @param create_callback Create callback.
  /// @param destroy_callback Destroy callback.
  /// @param process_callback Process callback.
  explicit TaskDefinition(CreateCallback create_callback,
                          DestroyCallback destroy_callback,
                          ProcessCallback process_callback) noexcept
      : TaskDefinition(BarelyTaskDefinition{
            create_callback,
            destroy_callback,
            process_callback,
        }) {}

  /// Constructs a new `TaskDefinition` from a raw type.
  ///
  /// @param definition Raw task definition.
  // NOLINTNEXTLINE(google-explicit-constructor)
  TaskDefinition(BarelyTaskDefinition definition) noexcept
      : BarelyTaskDefinition{definition} {}
};

/// Effect reference.
class EffectRef {
 public:
  /// Returns a control value.
  ///
  /// @param index Control index.
  /// @return Control value, or an error status.
  template <typename IndexType, typename ValueType>
  [[nodiscard]] StatusOr<ValueType> GetControl(IndexType index) const noexcept {
    static_assert(
        std::is_integral<IndexType>::value || std::is_enum<IndexType>::value,
        "IndexType is not supported");
    double value = 0.0;
    if (const Status status = BarelyEffect_GetControl(
            handle_, instrument_id_, id_, static_cast<int>(index), &value);
        !status.IsOk()) {
      return status;
    }
    return static_cast<ValueType>(value);
  }

  /// Returns the process order.
  ///
  /// @return Process order, or an error status.
  [[nodiscard]] StatusOr<int> GetProcessOrder() const noexcept {
    int process_order = 0;
    if (const Status status = BarelyEffect_GetProcessOrder(
            handle_, instrument_id_, id_, &process_order);
        !status.IsOk()) {
      return status;
    }
    return process_order;
  }

  /// Resets all control values.
  ///
  /// @return Status.
  Status ResetAllControls() noexcept {
    return BarelyEffect_ResetAllControls(handle_, instrument_id_, id_);
  }

  /// Resets a control value.
  ///
  /// @param index Control index.
  /// @return Status.
  template <typename IndexType>
  Status ResetControl(IndexType index) noexcept {
    static_assert(
        std::is_integral<IndexType>::value || std::is_enum<IndexType>::value,
        "IndexType is not supported");
    return BarelyEffect_ResetControl(handle_, instrument_id_, id_,
                                     static_cast<int>(index));
  }

  /// Sets a control value.
  ///
  /// @param index Control index.
  /// @param value Control value.
  /// @param slope_per_beat Control slope in value change per beat.
  /// @return Status.
  template <typename IndexType, typename ValueType>
  Status SetControl(IndexType index, ValueType value,
                    double slope_per_beat = 0.0) noexcept {
    static_assert(
        std::is_integral<IndexType>::value || std::is_enum<IndexType>::value,
        "IndexType is not supported");
    static_assert(
        std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
        "ValueType is not supported");
    return BarelyEffect_SetControl(handle_, instrument_id_, id_,
                                   static_cast<int>(index),
                                   static_cast<double>(value), slope_per_beat);
  }

  /// Sets the control event callback.
  ///
  /// @param callback Control event callback.
  /// @return Status.
  Status SetControlEventCallback(ControlEventCallback callback) noexcept {
    *control_event_callback_ = std::move(callback);
    if (*control_event_callback_) {
      return BarelyEffect_SetControlEventCallback(
          handle_, instrument_id_, id_,
          [](int32_t index, double value, void* user_data) noexcept {
            (*static_cast<ControlEventCallback*>(user_data))(index, value);
          },
          static_cast<void*>(control_event_callback_.get()));
    }
    return BarelyEffect_SetControlEventCallback(handle_, instrument_id_, id_,
                                                /*callback=*/nullptr,
                                                /*user_data=*/nullptr);
  }

  /// Sets data.
  ///
  /// @param data Immutable data.
  /// @return Status.
  template <typename DataType>
  Status SetData(const DataType& data) noexcept {
    static_assert(std::is_trivially_copyable<DataType>::value,
                  "DataType is not trivially copyable");
    return SetData(static_cast<const void*>(&data), sizeof(decltype(data)));
  }

  /// Sets data.
  ///
  /// @param data Pointer to immutable data.
  /// @param size Data size in bytes.
  /// @return Status.
  Status SetData(const void* data, int size) noexcept {
    return BarelyEffect_SetData(handle_, instrument_id_, id_, data, size);
  }

  /// Sets the process order.
  ///
  /// @param process_order Process order.
  /// @return Status.
  Status SetProcessOrder(int process_order) noexcept {
    return BarelyEffect_SetProcessOrder(handle_, instrument_id_, id_,
                                        process_order);
  }

 private:
  // Ensures that `EffectRef` can only be constructed by `InstrumentRef`.
  friend class InstrumentRef;

  // Constructs a new `EffectRef`.
  explicit EffectRef(BarelyMusicianHandle handle, BarelyId instrument_id,
                     BarelyId id) noexcept
      : handle_(handle),
        instrument_id_(instrument_id),
        id_(id),
        control_event_callback_(std::make_shared<ControlEventCallback>()) {}

  // Raw musician handle.
  BarelyMusicianHandle handle_ = nullptr;

  // Instrument identifier.
  BarelyId instrument_id_ = BarelyId_kInvalid;

  // Identifier.
  BarelyId id_ = BarelyId_kInvalid;

  // Control event callback.
  std::shared_ptr<ControlEventCallback> control_event_callback_ = nullptr;
};

/// Instrument reference.
class InstrumentRef {
 public:
  /// Creates a new effect.
  ///
  /// @param definition Effect definition.
  /// @param process_order Task process order.
  /// @return Effect reference, or an error status.
  StatusOr<EffectRef> CreateEffect(EffectDefinition definition,
                                   int process_order = 0) noexcept {
    BarelyId effect_id = BarelyId_kInvalid;
    if (const Status status = BarelyEffect_Create(handle_, id_, definition,
                                                  process_order, &effect_id);
        !status.IsOk()) {
      return status;
    }
    const auto [it, success] =
        effect_refs_.emplace(effect_id, EffectRef(handle_, id_, effect_id));
    assert(success);
    return it->second;
  }

  /// Destroys an effect.
  ///
  /// @param effect_ref Effect reference.
  /// @return Status.
  Status DestroyEffect(EffectRef effect_ref) noexcept {
    const auto status = BarelyEffect_Destroy(handle_, id_, effect_ref.id_);
    effect_refs_.erase(effect_ref.id_);
    return status;
  }

  /// Returns a control value.
  ///
  /// @param index Control index.
  /// @return Control value, or an error status.
  template <typename IndexType, typename ValueType>
  [[nodiscard]] StatusOr<ValueType> GetControl(IndexType index) const noexcept {
    static_assert(
        std::is_integral<IndexType>::value || std::is_enum<IndexType>::value,
        "IndexType is not supported");
    double value = 0.0;
    if (const Status status = BarelyInstrument_GetControl(
            handle_, id_, static_cast<int>(index), &value);
        !status.IsOk()) {
      return status;
    }
    return static_cast<ValueType>(value);
  }

  /// Returns a note control value.
  ///
  /// @param index Note control index.
  /// @return Note control value, or an error status.
  template <typename IndexType, typename ValueType>
  [[nodiscard]] StatusOr<ValueType> GetNoteControl(
      double pitch, IndexType index) const noexcept {
    static_assert(
        std::is_integral<IndexType>::value || std::is_enum<IndexType>::value,
        "IndexType is not supported");
    double value = 0.0;
    if (const Status status = BarelyInstrument_GetNoteControl(
            handle_, id_, pitch, static_cast<int>(index), &value);
        !status.IsOk()) {
      return status;
    }
    return static_cast<ValueType>(value);
  }

  /// Returns whether a note is on or not.
  ///
  /// @param pitch Note pitch.
  /// @return True if active, false otherwise, or an error status.
  [[nodiscard]] StatusOr<bool> IsNoteOn(double pitch) const noexcept {
    bool is_note_on = false;
    if (const Status status =
            BarelyInstrument_IsNoteOn(handle_, id_, pitch, &is_note_on);
        !status.IsOk()) {
      return status;
    }
    return is_note_on;
  }

  /// Processes output samples at timestamp.
  ///
  /// @param output_samples Interleaved array of output samples.
  /// @param output_channel_count Number of output channels.
  /// @param output_frame_count Number of output frames.
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status Process(double* output_samples, int output_channel_count,
                 int output_frame_count, double timestamp) noexcept {
    return BarelyInstrument_Process(handle_, id_, output_samples,
                                    output_channel_count, output_frame_count,
                                    timestamp);
  }

  /// Resets all control values.
  ///
  /// @return Status.
  Status ResetAllControls() noexcept {
    return BarelyInstrument_ResetAllControls(handle_, id_);
  }

  /// Resets all note control values.
  ///
  /// @param pitch Note pitch
  /// @return Status.
  Status ResetAllNoteControls(double pitch) noexcept {
    return BarelyInstrument_ResetAllNoteControls(handle_, id_, pitch);
  }

  /// Resets a control value.
  ///
  /// @param index Control index.
  /// @return Status.
  template <typename IndexType>
  Status ResetControl(IndexType index) noexcept {
    static_assert(
        std::is_integral<IndexType>::value || std::is_enum<IndexType>::value,
        "IndexType is not supported");
    return BarelyInstrument_ResetControl(handle_, id_, static_cast<int>(index));
  }

  /// Resets a note control value.
  ///
  /// @param pitch Note pitch.
  /// @param index Note control index.
  /// @return Status.
  template <typename IndexType>
  Status ResetNoteControl(double pitch, IndexType index) noexcept {
    static_assert(
        std::is_integral<IndexType>::value || std::is_enum<IndexType>::value,
        "IndexType is not supported");
    return BarelyInstrument_ResetNoteControl(handle_, id_, pitch,
                                             static_cast<int>(index));
  }

  /// Sets all notes off.
  ///
  /// @return Status.
  Status SetAllNotesOff() noexcept {
    return BarelyInstrument_SetAllNotesOff(handle_, id_);
  }

  /// Sets a control value.
  ///
  /// @param index Control index.
  /// @param value Control value.
  /// @param slope_per_beat Control slope in value change per beat.
  /// @return Status.
  template <typename IndexType, typename ValueType>
  Status SetControl(IndexType index, ValueType value,
                    double slope_per_beat = 0.0) noexcept {
    static_assert(
        std::is_integral<IndexType>::value || std::is_enum<IndexType>::value,
        "IndexType is not supported");
    static_assert(
        std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
        "ValueType is not supported");
    return BarelyInstrument_SetControl(handle_, id_, static_cast<int>(index),
                                       static_cast<double>(value),
                                       slope_per_beat);
  }

  /// Sets the control event callback.
  ///
  /// @param callback Control event callback.
  /// @return Status.
  Status SetControlEventCallback(ControlEventCallback callback) noexcept {
    *control_event_callback_ = std::move(callback);
    if (*control_event_callback_) {
      return BarelyInstrument_SetControlEventCallback(
          handle_, id_,
          [](int32_t index, double value, void* user_data) noexcept {
            (*static_cast<ControlEventCallback*>(user_data))(index, value);
          },
          static_cast<void*>(control_event_callback_.get()));
    }
    return BarelyInstrument_SetControlEventCallback(
        handle_, id_, /*callback=*/nullptr, /*user_data=*/nullptr);
  }

  /// Sets data.
  ///
  /// @param data Immutable data.
  /// @return Status.
  template <typename DataType>
  Status SetData(const DataType& data) noexcept {
    static_assert(std::is_trivially_copyable<DataType>::value,
                  "DataType is not trivially copyable");
    return SetData(static_cast<const void*>(&data), sizeof(decltype(data)));
  }

  /// Sets data.
  ///
  /// @param data Pointer to immutable data.
  /// @param size Data size in bytes.
  /// @return Status.
  Status SetData(const void* data, int size) noexcept {
    return BarelyInstrument_SetData(handle_, id_, data, size);
  }

  /// Sets a note control value.
  ///
  /// @param pitch Note pitch.
  /// @param index Note control index.
  /// @param value Note control value.
  /// @param slope_per_beat Note control slope in value change per beat.
  /// @return Status.
  template <typename IndexType, typename ValueType>
  Status SetNoteControl(double pitch, IndexType index, ValueType value,
                        double slope_per_beat = 0.0) noexcept {
    static_assert(
        std::is_integral<IndexType>::value || std::is_enum<IndexType>::value,
        "IndexType is not supported");
    static_assert(
        std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
        "ValueType is not supported");
    return BarelyInstrument_SetNoteControl(
        handle_, id_, pitch, static_cast<int>(index),
        static_cast<double>(value), slope_per_beat);
  }

  /// Sets the note control event callback.
  ///
  /// @param callback Note control event callback.
  /// @return Status.
  Status SetNoteControlEventCallback(
      NoteControlEventCallback callback) noexcept {
    *note_control_event_callback_ = std::move(callback);
    if (*note_control_event_callback_) {
      return BarelyInstrument_SetNoteControlEventCallback(
          handle_, id_,
          [](double pitch, int32_t index, double value,
             void* user_data) noexcept {
            (*static_cast<NoteControlEventCallback*>(user_data))(pitch, index,
                                                                 value);
          },
          static_cast<void*>(note_control_event_callback_.get()));
    }
    return BarelyInstrument_SetNoteControlEventCallback(
        handle_, id_, /*callback=*/nullptr, /*user_data=*/nullptr);
  }

  /// Sets a note off.
  ///
  /// @param pitch Note pitch.
  /// @return Status.
  Status SetNoteOff(double pitch) noexcept {
    return BarelyInstrument_SetNoteOff(handle_, id_, pitch);
  }

  /// Sets the note off event callback.
  ///
  /// @param callback Note off event callback.
  /// @return Status.
  Status SetNoteOffEventCallback(NoteOffEventCallback callback) noexcept {
    *note_off_event_callback_ = std::move(callback);
    if (*note_off_event_callback_) {
      return BarelyInstrument_SetNoteOffEventCallback(
          handle_, id_,
          [](double pitch, void* user_data) noexcept {
            (*static_cast<NoteOffEventCallback*>(user_data))(pitch);
          },
          static_cast<void*>(note_off_event_callback_.get()));
    }
    return BarelyInstrument_SetNoteOffEventCallback(
        handle_, id_, /*callback=*/nullptr, /*user_data=*/nullptr);
  }

  /// Sets a note on.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  /// @return Status.
  Status SetNoteOn(double pitch, double intensity = 1.0) noexcept {
    return BarelyInstrument_SetNoteOn(handle_, id_, pitch, intensity);
  }

  /// Sets the note on event callback.
  ///
  /// @param callback Note on event callback.
  /// @return Status.
  Status SetNoteOnEventCallback(NoteOnEventCallback callback) noexcept {
    *note_on_event_callback_ = std::move(callback);
    if (*note_on_event_callback_) {
      return BarelyInstrument_SetNoteOnEventCallback(
          handle_, id_,
          [](double pitch, double intensity, void* user_data) noexcept {
            (*static_cast<NoteOnEventCallback*>(user_data))(pitch, intensity);
          },
          static_cast<void*>(note_on_event_callback_.get()));
    }
    return BarelyInstrument_SetNoteOnEventCallback(
        handle_, id_, /*callback=*/nullptr, /*user_data=*/nullptr);
  }

 private:
  // Ensures that `InstrumentRef` can only be constructed by `Musician`.
  friend class Musician;

  // Constructs a new `InstrumentRef`.
  explicit InstrumentRef(BarelyMusicianHandle handle, BarelyId id) noexcept
      : handle_(handle),
        id_(id),
        control_event_callback_(std::make_shared<ControlEventCallback>()),
        note_control_event_callback_(
            std::make_shared<NoteControlEventCallback>()),
        note_off_event_callback_(std::make_shared<NoteOffEventCallback>()),
        note_on_event_callback_(std::make_shared<NoteOnEventCallback>()) {}

  // Raw musician handle.
  BarelyMusicianHandle handle_ = nullptr;

  // Identifier.
  BarelyId id_ = BarelyId_kInvalid;

  // Control event callback.
  std::shared_ptr<ControlEventCallback> control_event_callback_ = nullptr;

  // Note control event callback.
  std::shared_ptr<NoteControlEventCallback> note_control_event_callback_ =
      nullptr;

  // Note off event callback.
  std::shared_ptr<NoteOffEventCallback> note_off_event_callback_ = nullptr;

  // Note on event callback.
  std::shared_ptr<NoteOnEventCallback> note_on_event_callback_ = nullptr;

  // Map of effect references by their identifiers.
  std::unordered_map<BarelyId, EffectRef> effect_refs_;
};

/// Task reference.
class TaskRef {
 public:
  /// Returns the position.
  ///
  /// @return Position in beats, or an error status.
  [[nodiscard]] StatusOr<double> GetPosition() const noexcept {
    double position = 0.0;
    if (const Status status =
            BarelyTask_GetPosition(handle_, performer_id_, id_, &position);
        !status.IsOk()) {
      return status;
    }
    return position;
  }

  /// Returns the process order.
  ///
  /// @return Process order, or an error status.
  [[nodiscard]] StatusOr<int> GetProcessOrder() const noexcept {
    int process_order = 0;
    if (const Status status = BarelyTask_GetProcessOrder(handle_, performer_id_,
                                                         id_, &process_order);
        !status.IsOk()) {
      return status;
    }
    return process_order;
  }

  /// Sets the position.
  ///
  /// @param position Position in beats.
  /// @return Status.
  Status SetPosition(double position) noexcept {
    return BarelyTask_SetPosition(handle_, performer_id_, id_, position);
  }

  /// Sets the process order.
  ///
  /// @param process_order Process order.
  /// @return Status.
  Status SetProcessOrder(int process_order) noexcept {
    return BarelyTask_SetProcessOrder(handle_, performer_id_, id_,
                                      process_order);
  }

 private:
  // Ensures that `TaskRef` can only be constructed by `PerformerRef`.
  friend class PerformerRef;

  // Constructs a new `TaskRef`.
  explicit TaskRef(BarelyMusicianHandle handle, BarelyId performer_id,
                   BarelyId id) noexcept
      : handle_(handle), performer_id_(performer_id), id_(id) {}

  // Raw musician handle.
  BarelyMusicianHandle handle_ = nullptr;

  // Performer identifier.
  BarelyId performer_id_ = BarelyId_kInvalid;

  // Identifier.
  BarelyId id_ = BarelyId_kInvalid;
};

/// Performer reference.
class PerformerRef {
 public:
  /// Creates a new task.
  ///
  /// @param definition Task definition.
  /// @param is_one_off True if one-off task, false otherwise.
  /// @param position Task position in beats.
  /// @param process_order Task process order.
  /// @param user_data Pointer to user data.
  /// @return Task reference, or an error status.
  StatusOr<TaskRef> CreateTask(TaskDefinition definition, bool is_one_off,
                               double position, int process_order = 0,
                               void* user_data = nullptr) noexcept {
    BarelyId task_id = BarelyId_kInvalid;
    if (const Status status =
            BarelyTask_Create(handle_, id_, definition, is_one_off, position,
                              process_order, user_data, &task_id);
        !status.IsOk()) {
      return status;
    }
    return TaskRef(handle_, id_, task_id);
  }

  /// Creates a new task with a callback.
  ///
  /// @param callback Task callback.
  /// @param is_one_off True if one-off task, false otherwise.
  /// @param position Task position in beats.
  /// @param process_order Task process order.
  /// @return Task reference, or an error status.
  StatusOr<TaskRef> CreateTask(TaskCallback callback, bool is_one_off,
                               double position,
                               int process_order = 0) noexcept {
    return CreateTask(
        TaskDefinition(
            [](void** state, void* user_data) noexcept {
              *state = new TaskCallback(
                  std::move(*static_cast<TaskCallback*>(user_data)));
            },
            [](void** state) noexcept {
              delete static_cast<TaskCallback*>(*state);
            },
            [](void** state) noexcept {
              if (const auto* callback_ptr = static_cast<TaskCallback*>(*state);
                  *callback_ptr) {
                (*callback_ptr)();
              }
            }),
        is_one_off, position, process_order, static_cast<void*>(&callback));
  }

  /// Destroys a task.
  ///
  /// @param task_ref Task reference.
  /// @return Status.
  Status DestroyTask(TaskRef task_ref) noexcept {
    return BarelyTask_Destroy(handle_, id_, task_ref.id_);
  }

  /// Returns the loop begin position.
  ///
  /// @return Loop begin position in beats, or an error status.
  [[nodiscard]] StatusOr<double> GetLoopBeginPosition() const noexcept {
    double loop_begin_position = 0.0;
    if (const Status status = BarelyPerformer_GetLoopBeginPosition(
            handle_, id_, &loop_begin_position);
        !status.IsOk()) {
      return status;
    }
    return loop_begin_position;
  }

  /// Returns the loop length.
  ///
  /// @return Loop length in beats, or an error status.
  [[nodiscard]] StatusOr<double> GetLoopLength() const noexcept {
    double loop_length = 0.0;
    if (const Status status =
            BarelyPerformer_GetLoopLength(handle_, id_, &loop_length);
        !status.IsOk()) {
      return status;
    }
    return loop_length;
  }

  /// Returns the position.
  ///
  /// @return Position in beats, or an error status.
  [[nodiscard]] StatusOr<double> GetPosition() const noexcept {
    double position = 0.0;
    if (const Status status =
            BarelyPerformer_GetPosition(handle_, id_, &position);
        !status.IsOk()) {
      return status;
    }
    return position;
  }

  /// Returns whether the performer is looping or not.
  ///
  /// @return True if looping, false otherwise, or an error status.
  [[nodiscard]] StatusOr<bool> IsLooping() const noexcept {
    bool is_looping = false;
    if (const Status status =
            BarelyPerformer_IsLooping(handle_, id_, &is_looping);
        !status.IsOk()) {
      return status;
    }
    return is_looping;
  }

  /// Returns whether the performer is playing or not.
  ///
  /// @return True if playing, false otherwise, or an error status.
  [[nodiscard]] StatusOr<bool> IsPlaying() const noexcept {
    bool is_playing = false;
    if (const Status status =
            BarelyPerformer_IsPlaying(handle_, id_, &is_playing);
        !status.IsOk()) {
      return status;
    }
    return is_playing;
  }

  /// Sets the loop begin position.
  ///
  /// @param loop_begin_position Loop begin position in beats.
  /// @return Status.
  Status SetLoopBeginPosition(double loop_begin_position) noexcept {
    return BarelyPerformer_SetLoopBeginPosition(handle_, id_,
                                                loop_begin_position);
  }

  /// Sets the loop length.
  ///
  /// @param loop_length Loop length in beats.
  /// @return Status.
  Status SetLoopLength(double loop_length) noexcept {
    return BarelyPerformer_SetLoopLength(handle_, id_, loop_length);
  }

  /// Sets whether the performer is looping or not.
  ///
  /// @param is_looping True if looping, false otherwise.
  /// @return Status.
  Status SetLooping(bool is_looping) noexcept {
    return BarelyPerformer_SetLooping(handle_, id_, is_looping);
  }

  /// Sets the position.
  ///
  /// @param position Position in beats.
  /// @return Status.
  Status SetPosition(double position) noexcept {
    return BarelyPerformer_SetPosition(handle_, id_, position);
  }

  /// Starts the performer.
  ///
  /// @return Status.
  Status Start() noexcept { return BarelyPerformer_Start(handle_, id_); }

  /// Stops the performer.
  ///
  /// @return Status.
  Status Stop() noexcept { return BarelyPerformer_Stop(handle_, id_); }

 private:
  // Ensures that `PerformerRef` can only be constructed by `Musician`.
  friend class Musician;

  // Constructs a new `PerformerRef`.
  explicit PerformerRef(BarelyMusicianHandle handle, BarelyId id) noexcept
      : handle_(handle), id_(id) {}

  // Raw musician handle.
  BarelyMusicianHandle handle_ = nullptr;

  // Identifier.
  BarelyId id_ = BarelyId_kInvalid;
};

/// Musician.
class Musician {
 public:
  /// Constructs a new `Musician`.
  Musician() noexcept {
    [[maybe_unused]] const Status status = BarelyMusician_Create(&handle_);
    assert(status.IsOk());
  }

  /// Destroys `Musician`.
  ~Musician() noexcept {
    if (handle_) {
      [[maybe_unused]] const Status status =
          BarelyMusician_Destroy(std::exchange(handle_, nullptr));
      assert(status.IsOk());
    }
  }

  /// Non-copyable.
  Musician(const Musician& other) noexcept = delete;
  Musician& operator=(const Musician& other) noexcept = delete;

  /// Constructs new `Musician` via move.
  ///
  /// @param other Other musician.
  Musician(Musician&& other) noexcept
      : handle_(std::exchange(other.handle_, nullptr)) {}

  /// Assigns `Musician` via move.
  ///
  /// @param other Other musician.
  Musician& operator=(Musician&& other) noexcept {
    if (this != &other) {
      if (handle_) {
        [[maybe_unused]] const Status status = BarelyMusician_Destroy(handle_);
        assert(status.IsOk());
      }
      handle_ = std::exchange(other.handle_, nullptr);
    }
    return *this;
  }

  /// Creates a new instrument.
  ///
  /// @param definition Instrument definition.
  /// @param frame_rate Frame rate in hertz.
  /// @return Instrument reference.
  [[nodiscard]] InstrumentRef CreateInstrument(InstrumentDefinition definition,
                                               int frame_rate) noexcept {
    BarelyId instrument_id = BarelyId_kInvalid;
    [[maybe_unused]] const Status status = BarelyInstrument_Create(
        handle_, definition, frame_rate, &instrument_id);
    assert(status.IsOk());
    const auto [it, success] = instrument_refs_.emplace(
        instrument_id, InstrumentRef(handle_, instrument_id));
    assert(success);
    return it->second;
  }

  /// Creates a new performer.
  ///
  /// @return Performer reference.
  [[nodiscard]] PerformerRef CreatePerformer() noexcept {
    BarelyId performer_id = BarelyId_kInvalid;
    [[maybe_unused]] const Status status =
        BarelyPerformer_Create(handle_, &performer_id);
    assert(status.IsOk());
    return PerformerRef(handle_, performer_id);
  }

  /// Destroys an instrument.
  ///
  /// @param instrument_ref Instrument reference.
  /// @return Status.
  Status DestroyInstrument(InstrumentRef instrument_ref) noexcept {
    const auto status = BarelyInstrument_Destroy(handle_, instrument_ref.id_);
    instrument_refs_.erase(instrument_ref.id_);
    return status;
  }

  /// Destroys a performer.
  ///
  /// @param performer_ref Performer reference.
  /// @return Status.
  Status DestroyPerformer(PerformerRef performer_ref) noexcept {
    return BarelyPerformer_Destroy(handle_, performer_ref.id_);
  }

  /// Returns the tempo.
  ///
  /// @return Tempo in beats per minute.
  [[nodiscard]] double GetTempo() const noexcept {
    double tempo = 0.0;
    [[maybe_unused]] const Status status =
        BarelyMusician_GetTempo(handle_, &tempo);
    assert(status.IsOk());
    return tempo;
  }

  /// Returns the timestamp.
  ///
  /// @return Timestamp in seconds.
  [[nodiscard]] double GetTimestamp() const noexcept {
    double timestamp = 0.0;
    [[maybe_unused]] const Status status =
        BarelyMusician_GetTimestamp(handle_, &timestamp);
    assert(status.IsOk());
    return timestamp;
  }

  /// Sets the tempo.
  ///
  /// @param tempo Tempo in beats per minute.
  /// @return Status.
  Status SetTempo(double tempo) noexcept {
    return BarelyMusician_SetTempo(handle_, tempo);
  }

  /// Updates the musician at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status Update(double timestamp) noexcept {
    return BarelyMusician_Update(handle_, timestamp);
  }

 private:
  // Raw handle.
  BarelyMusicianHandle handle_ = nullptr;

  // Map of instrument references by their identifiers.
  std::unordered_map<BarelyId, InstrumentRef> instrument_refs_;
};

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_BARELYMUSICIAN_H_
