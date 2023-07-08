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
///   BarelyMusicianHandle musician;
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
///   BarelyInstrumentHandle instrument;
///   BarelyInstrument_Create(handle, BarelySynthInstrument_GetDefinition(),
///                           /*frame_rate=*/48000, &instrument);
///
///   // Set a note on.
///   //
///   // Pitch values are normalized, where each `1.0` shifts one octave, and
///   // `0.0` represents the middle A (A4) for a typical instrument definition.
///   // However, this is not a strict rule, since both `pitch` and `intensity`
///   // values can be interpreted in any desired way by a custom instrument.
///   BarelyInstrument_SetNoteOn(instrument, /*pitch=*/-1.0,
///                              /*intensity=*/0.25);
///
///   // Check if the note is on.
///   bool is_note_on = false;
///   BarelyInstrument_IsNoteOn(instrument, /*pitch=*/-1.0, &is_note_on);
///
///   // Set a control value.
///   BarelyInstrument_SetControl(instrument, BarelySynthControl_kGain,
///                               /*value=*/0.5, /*slope_per_beat=*/0.0);
///
///   // Create a low-pass effect.
///   BarelyEffectHandle effect;
///   BarelyEffect_Create(instrument, BarelyLowPassEffect_GetDefinition(),
///                       /*process_order=*/0, &effect);
///
///   // Set the low-pass cutoff frequency to increase by 100 hertz per beat.
///   BarelyEffect_SetControl(effect, BarelyLowPassControl_kCutoffFrequency,
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
///   BarelyInstrument_Process(instrument, output_samples, output_channel_count,
///                            output_frame_count, timestamp);
///
///   // Destroy the effect.
///   BarelyEffect_Destroy(effect);
///
///   // Destroy.
///   BarelyInstrument_Destroy(instrument);
///   @endcode
///
/// - Performer:
///
///   @code{.cpp}
///   // Create.
///   BarelyPerformerHandle performer;
///   BarelyPerformer_Create(handle, &performer);
///
///   // Create a task.
///   BarelyTaskDefinition definition;  // populate this.
///   BarelyTaskHandle task;
///   BarelyTask_Create(performer, definition, /*is_one_off=*/false,
///                     /*position=*/0.0, /*process_order=*/0,
///                     /*user_data=*/nullptr, &task);
///
///   // Set looping on.
///   BarelyPerformer_SetLooping(performer, /*is_looping=*/true);
///
///   // Start.
///   BarelyPerformer_Start(performer);
///
///   // Check if started playing.
///   bool is_playing = false;
///   BarelyPerformer_IsPlaying(performer, &is_playing);
///
///   // Destroy the task.
///   BarelyTask_Destroy(task);
///
///   // Destroy.
///   BarelyPerformer_Destroy(performer);
///   @endcode

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

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

/// Control definition.
typedef struct BarelyControlDefinition {
  /// Default value.
  double default_value;

  /// Minimum value.
  double min_value;

  /// Maximum value.
  double max_value;
} BarelyControlDefinition;

/// Control event definition create callback signature.
///
/// @param state Pointer to control event state.
/// @param user_data Pointer to user data.
typedef void (*BarelyControlEventDefinition_CreateCallback)(void** state,
                                                            void* user_data);

/// Control event definition destroy callback signature.
///
/// @param state Pointer to control event state.
typedef void (*BarelyControlEventDefinition_DestroyCallback)(void** state);

/// Control event definition process callback signature.
///
/// @param state Pointer to control event state.
/// @param index Control index.
/// @param value Control value.
typedef void (*BarelyControlEventDefinition_ProcessCallback)(void** state,
                                                             int32_t index,
                                                             double value);

/// Control event definition.
typedef struct BarelyControlEventDefinition {
  /// Create callback.
  BarelyControlEventDefinition_CreateCallback create_callback;

  /// Destroy callback.
  BarelyControlEventDefinition_DestroyCallback destroy_callback;

  /// Process callback.
  BarelyControlEventDefinition_ProcessCallback process_callback;
} BarelyControlEventDefinition;

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

/// Note control event definition create callback signature.
///
/// @param state Pointer to note control state.
/// @param user_data Pointer to user data.
typedef void (*BarelyNoteControlEventDefinition_CreateCallback)(
    void** state, void* user_data);

/// Note control event definition destroy callback signature.
///
/// @param state Pointer to note control state.
typedef void (*BarelyNoteControlEventDefinition_DestroyCallback)(void** state);

/// Note control event definition process callback signature.
///
/// @param state Pointer to note control event state.
/// @param pitch Note pitch.
/// @param index Note control index.
/// @param value Note control value.
typedef void (*BarelyNoteControlEventDefinition_ProcessCallback)(void** state,
                                                                 double pitch,
                                                                 int32_t index,
                                                                 double value);

/// Note control event definition.
typedef struct BarelyNoteControlEventDefinition {
  /// Create callback.
  BarelyNoteControlEventDefinition_CreateCallback create_callback;

  /// Destroy callback.
  BarelyNoteControlEventDefinition_DestroyCallback destroy_callback;

  /// Process callback.
  BarelyNoteControlEventDefinition_ProcessCallback process_callback;
} BarelyNoteControlEventDefinition;

/// Note off event definition create callback signature.
///
/// @param state Pointer to note off state.
/// @param user_data Pointer to user data.
typedef void (*BarelyNoteOffEventDefinition_CreateCallback)(void** state,
                                                            void* user_data);

/// Note off event definition destroy callback signature.
///
/// @param state Pointer to note off state.
typedef void (*BarelyNoteOffEventDefinition_DestroyCallback)(void** state);

/// Note off event definition process callback signature.
///
/// @param state Pointer to note off event state.
/// @param pitch Note pitch.
typedef void (*BarelyNoteOffEventDefinition_ProcessCallback)(void** state,
                                                             double pitch);

/// Note off event definition.
typedef struct BarelyNoteOffEventDefinition {
  /// Create callback.
  BarelyNoteOffEventDefinition_CreateCallback create_callback;

  /// Destroy callback.
  BarelyNoteOffEventDefinition_DestroyCallback destroy_callback;

  /// Process callback.
  BarelyNoteOffEventDefinition_ProcessCallback process_callback;
} BarelyNoteOffEventDefinition;

/// Note on event definition create callback signature.
///
/// @param state Pointer to note on state.
/// @param user_data Pointer to user data.
typedef void (*BarelyNoteOnEventDefinition_CreateCallback)(void** state,
                                                           void* user_data);

/// Note on event definition destroy callback signature.
///
/// @param state Pointer to note on state.
typedef void (*BarelyNoteOnEventDefinition_DestroyCallback)(void** state);

/// Note on event definition process callback signature.
///
/// @param state Pointer to note on event state.
/// @param pitch Note pitch.
/// @param intensity Note intensity.
typedef void (*BarelyNoteOnEventDefinition_ProcessCallback)(void** state,
                                                            double pitch,
                                                            double intensity);

/// Note on event definition.
typedef struct BarelyNoteOnEventDefinition {
  /// Create callback.
  BarelyNoteOnEventDefinition_CreateCallback create_callback;

  /// Destroy callback.
  BarelyNoteOnEventDefinition_DestroyCallback destroy_callback;

  /// Process callback.
  BarelyNoteOnEventDefinition_ProcessCallback process_callback;
} BarelyNoteOnEventDefinition;

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

/// Task definition.
typedef struct BarelyTaskDefinition {
  /// Create callback.
  BarelyTaskDefinition_CreateCallback create_callback;

  /// Destroy callback.
  BarelyTaskDefinition_DestroyCallback destroy_callback;

  /// Process callback.
  BarelyTaskDefinition_ProcessCallback process_callback;
} BarelyTaskDefinition;

/// Effect handle.
typedef struct BarelyEffect* BarelyEffectHandle;

/// Instrument handle.
typedef struct BarelyInstrument* BarelyInstrumentHandle;

/// Musician handle.
typedef struct BarelyMusician* BarelyMusicianHandle;

/// Performer handle.
typedef struct BarelyPerformer* BarelyPerformerHandle;

/// Task handle.
typedef struct BarelyTask* BarelyTaskHandle;

/// Creates a new effect.
///
/// @param instrument Instrument handle.
/// @param definition Effect definition.
/// @param process_order Effect process order.
/// @param out_effect Output effect handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyEffect_Create(
    BarelyInstrumentHandle instrument, BarelyEffectDefinition definition,
    int32_t process_order, BarelyEffectHandle* out_effect);

/// Destroys an effect.
///
/// @param effect Effect handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyEffect_Destroy(BarelyEffectHandle effect);

/// Gets an effect control value.
///
/// @param effect Effect handle.
/// @param index Control index.
/// @param out_value Output control value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyEffect_GetControl(BarelyEffectHandle effect,
                                                   int32_t index,
                                                   double* out_value);

/// Gets the process order of an effect.
///
/// @param effect Effect handle.
/// @param out_process_order Output process order.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyEffect_GetProcessOrder(
    BarelyEffectHandle effect, int32_t* out_process_order);

/// Resets all effect control values.
///
/// @param effect Effect handle.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyEffect_ResetAllControls(BarelyEffectHandle effect);

/// Resets an effect control value.
///
/// @param effect Effect handle.
/// @param index Control index.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyEffect_ResetControl(BarelyEffectHandle effect,
                                                     int32_t index);

/// Sets an effect control value.
///
/// @param effect Effect handle.
/// @param index Control index.
/// @param value Control value.
/// @param slope_per_beat Control slope in value change per beat.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyEffect_SetControl(BarelyEffectHandle effect,
                                                   int32_t index, double value,
                                                   double slope_per_beat);

/// Sets the control event of an effect.
///
/// @param effect Effect handle.
/// @param definition Control event definition.
/// @param user_data Pointer to user data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyEffect_SetControlEvent(
    BarelyEffectHandle effect, BarelyControlEventDefinition definition,
    void* user_data);

/// Sets effect data.
///
/// @param effect Effect handle.
/// @param data Pointer to immutable data.
/// @param size Data size in bytes.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyEffect_SetData(BarelyEffectHandle effect,
                                                const void* data, int32_t size);

/// Sets the process order of an effect.
///
/// @param effect Effect handle.
/// @param process_order Process order.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyEffect_SetProcessOrder(BarelyEffectHandle effect, int32_t process_order);

/// Creates a new instrument.
///
/// @param musician Musician handle.
/// @param definition Instrument definition.
/// @param frame_rate Frame rate in hertz.
/// @param out_instrument Output instrument handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_Create(
    BarelyMusicianHandle musician, BarelyInstrumentDefinition definition,
    int32_t frame_rate, BarelyInstrumentHandle* out_instrument);

/// Destroys an instrument.
///
/// @param instrument Instrument handle.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_Destroy(BarelyInstrumentHandle instrument);

/// Gets an instrument control value.
///
/// @param instrument Instrument handle.
/// @param index Control index.
/// @param out_value Output control value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_GetControl(
    BarelyInstrumentHandle instrument, int32_t index, double* out_value);

/// Gets an instrument note control value.
///
/// @param instrument Instrument handle.
/// @param pitch Note pitch.
/// @param index Note control index.
/// @param out_value Output note control value.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_GetNoteControl(BarelyInstrumentHandle instrument, double pitch,
                                int32_t index, double* out_value);

/// Gets whether an instrument note is on or not.
///
/// @param instrument Instrument handle.
/// @param pitch Note pitch.
/// @param out_is_note_on Output true if on, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_IsNoteOn(
    BarelyInstrumentHandle instrument, double pitch, bool* out_is_note_on);

/// Processes instrument output samples at timestamp.
///
/// @param instrument Instrument handle.
/// @param output_samples Array of interleaved output samples.
/// @param output_channel_count Number of output channels.
/// @param output_frame_count Number of output frames.
/// @param timestamp Timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_Process(
    BarelyInstrumentHandle instrument, double* output_samples,
    int32_t output_channel_count, int32_t output_frame_count, double timestamp);

/// Resets all instrument control values.
///
/// @param instrument Instrument handle.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_ResetAllControls(BarelyInstrumentHandle instrument);

/// Resets all instrument note control values.
///
/// @param instrument Instrument handle.
/// @param pitch Note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_ResetAllNoteControls(
    BarelyInstrumentHandle instrument, double pitch);

/// Resets an instrument control value.
///
/// @param instrument Instrument handle.
/// @param index Control index.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_ResetControl(BarelyInstrumentHandle instrument, int32_t index);

/// Resets an instrument note control value.
///
/// @param instrument Instrument handle.
/// @param pitch Note pitch.
/// @param index Note control index.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_ResetNoteControl(
    BarelyInstrumentHandle instrument, double pitch, int32_t index);

/// Sets all instrument notes off.
///
/// @param instrument Instrument handle.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_SetAllNotesOff(BarelyInstrumentHandle instrument);

/// Sets an instrument control value.
///
/// @param instrument Instrument handle.
/// @param index Control index.
/// @param value Control value.
/// @param slope_per_beat Control slope in value change per beat.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_SetControl(BarelyInstrumentHandle instrument, int32_t index,
                            double value, double slope_per_beat);

/// Sets the control event of an instrument.
///
/// @param instrument Instrument handle.
/// @param definition Control event definition.
/// @param user_data Pointer to user data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetControlEvent(
    BarelyInstrumentHandle instrument, BarelyControlEventDefinition definition,
    void* user_data);

/// Sets instrument data.
///
/// @param instrument Instrument handle.
/// @param data Pointer to immutable data.
/// @param size Data size in bytes.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetData(
    BarelyInstrumentHandle instrument, const void* data, int32_t size);

/// Sets an instrument note control value.
///
/// @param instrument Instrument handle.
/// @param pitch Note pitch.
/// @param index Note control index.
/// @param value Note control value.
/// @param slope_per_beat Note control slope in value change per beat.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetNoteControl(
    BarelyInstrumentHandle instrument, double pitch, int32_t index,
    double value, double slope_per_beat);

/// Sets the note control event of an instrument.
///
/// @param instrument Instrument handle.
/// @param definition Note control event definition.
/// @param user_data Pointer to user data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetNoteControlEvent(
    BarelyInstrumentHandle instrument,
    BarelyNoteControlEventDefinition definition, void* user_data);

/// Sets an instrument note off.
///
/// @param instrument Instrument handle.
/// @param pitch Note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_SetNoteOff(BarelyInstrumentHandle instrument, double pitch);

/// Sets the note off event of an instrument.
///
/// @param instrument Instrument handle.
/// @param definition Note off event definition.
/// @param user_data Pointer to user data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetNoteOffEvent(
    BarelyInstrumentHandle instrument, BarelyNoteOffEventDefinition definition,
    void* user_data);

/// Sets an instrument note on.
///
/// @param instrument Instrument handle.
/// @param pitch Note pitch.
/// @param intensity Note intensity.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetNoteOn(
    BarelyInstrumentHandle instrument, double pitch, double intensity);

/// Sets the note on event of an instrument.
///
/// @param instrument Instrument handle.
/// @param definition Note on event definition.
/// @param user_data Pointer to user data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetNoteOnEvent(
    BarelyInstrumentHandle instrument, BarelyNoteOnEventDefinition definition,
    void* user_data);

/// Creates a new musician.
///
/// @param out_musician Output musician handle.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyMusician_Create(BarelyMusicianHandle* out_musician);

/// Destroys a musician.
///
/// @param musician Musician handle.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyMusician_Destroy(BarelyMusicianHandle musician);

/// Gets the tempo of a musician.
///
/// @param musician Musician handle.
/// @param out_tempo Output tempo in beats per minute.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyMusician_GetTempo(BarelyMusicianHandle musician, double* out_tempo);

/// Gets the timestamp of a musician.
///
/// @param musician Musician handle.
/// @param out_timestamp Output timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyMusician_GetTimestamp(
    BarelyMusicianHandle musician, double* out_timestamp);

/// Sets the tempo of a musician.
///
/// @param musician Musician handle.
/// @param tempo Tempo in beats per minute.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyMusician_SetTempo(BarelyMusicianHandle musician, double tempo);

/// Updates a musician at timestamp.
///
/// @param musician Musician handle.
/// @param timestamp Timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyMusician_Update(BarelyMusicianHandle musician,
                                                 double timestamp);

/// Creates a new performer.
///
/// @param musician Musician handle.
/// @param out_performer Output performer handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_Create(
    BarelyMusicianHandle musician, BarelyPerformerHandle* out_performer);

/// Destroys a performer.
///
/// @param performer Performer handle.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyPerformer_Destroy(BarelyPerformerHandle performer);

/// Gets the loop begin position of a performer.
///
/// @param performer Performer handle.
/// @param out_loop_begin_position Output loop begin position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_GetLoopBeginPosition(
    BarelyPerformerHandle performer, double* out_loop_begin_position);

/// Gets the loop length of a performer.
///
/// @param performer Performer handle.
/// @param out_loop_length Output loop length.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_GetLoopLength(
    BarelyPerformerHandle performer, double* out_loop_length);

/// Gets the position of a performer.
///
/// @param performer Performer handle.
/// @param out_position Output position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_GetPosition(
    BarelyPerformerHandle performer, double* out_position);

/// Gets whether a performer is looping or not.
///
/// @param performer Performer handle.
/// @param out_is_looping Output true if looping, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_IsLooping(
    BarelyPerformerHandle performer, bool* out_is_looping);

/// Gets whether a performer is playing or not.
///
/// @param performer Performer handle.
/// @param out_is_playing Output true if playing, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_IsPlaying(
    BarelyPerformerHandle performer, bool* out_is_playing);

/// Sets the loop begin position of a performer.
///
/// @param performer Performer handle.
/// @param loop_begin_position Loop begin position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_SetLoopBeginPosition(
    BarelyPerformerHandle performer, double loop_begin_position);

/// Sets the loop length of a performer.
///
/// @param performer Performer handle.
/// @param loop_length Loop length in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_SetLoopLength(
    BarelyPerformerHandle performer, double loop_length);

/// Sets whether a performer is looping or not.
///
/// @param performer Performer handle.
/// @param is_looping True if looping, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyPerformer_SetLooping(BarelyPerformerHandle performer, bool is_looping);

/// Sets the position of a performer.
///
/// @param performer Performer handle.
/// @param position Position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyPerformer_SetPosition(BarelyPerformerHandle performer, double position);

/// Starts a performer.
///
/// @param performer Performer handle.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyPerformer_Start(BarelyPerformerHandle performer);

/// Stops a performer.
///
/// @param performer Performer handle.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyPerformer_Stop(BarelyPerformerHandle performer);

/// Creates a new task.
///
/// @param performer Performer handle.
/// @param definition Task definition.
/// @param is_one_off True if one-off task, false otherwise.
/// @param position Task position in beats.
/// @param process_order Task process order.
/// @param user_data Pointer to user data.
/// @param out_task Output task handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyTask_Create(BarelyPerformerHandle performer,
                                             BarelyTaskDefinition definition,
                                             bool is_one_off, double position,
                                             int32_t process_order,
                                             void* user_data,
                                             BarelyTaskHandle* out_task);

/// Destroys a task.
///
/// @param task Task handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyTask_Destroy(BarelyTaskHandle task);

/// Gets the position of a task.
///
/// @param task Task handle.
/// @param out_position Output position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyTask_GetPosition(BarelyTaskHandle task,
                                                  double* out_position);

/// Gets the process order of a task.
///
/// @param task Task handle.
/// @param out_process_order Output process order.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyTask_GetProcessOrder(BarelyTaskHandle task, int32_t* out_process_order);

/// Sets the position of a task.
///
/// @param task Task handle.
/// @param position Position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyTask_SetPosition(BarelyTaskHandle task,
                                                  double position);

/// Sets the process order of a task.
///
/// @param task Task handle.
/// @param process_order Process order.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyTask_SetProcessOrder(BarelyTaskHandle task,
                                                      int32_t process_order);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
// NOLINTEND

#ifdef __cplusplus
#include <cassert>
#include <compare>
#include <functional>
#include <limits>
#include <span>
#include <string>
#include <type_traits>
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

/// Control event definition.
struct ControlEventDefinition : public BarelyControlEventDefinition {
  /// Callback signature.
  ///
  /// @param index Control index.
  /// @param value Control value.
  using Callback = std::function<void(int index, double value)>;

  /// Create callback signature.
  using CreateCallback = BarelyControlEventDefinition_CreateCallback;

  /// Destroy callback signature.
  using DestroyCallback = BarelyControlEventDefinition_DestroyCallback;

  /// Process callback signature.
  using ProcessCallback = BarelyControlEventDefinition_ProcessCallback;

  /// Constructs a new `ControlEventDefinition`.
  ///
  /// @param create_callback Create callback.
  /// @param destroy_callback Destroy callback.
  /// @param process_callback Process callback.
  explicit ControlEventDefinition(CreateCallback create_callback,
                                  DestroyCallback destroy_callback,
                                  ProcessCallback process_callback) noexcept
      : ControlEventDefinition(BarelyControlEventDefinition{
            create_callback,
            destroy_callback,
            process_callback,
        }) {}

  /// Constructs a new `ControlEventDefinition` from a raw type.
  ///
  /// @param definition Raw control event definition.
  // NOLINTNEXTLINE(google-explicit-constructor)
  ControlEventDefinition(BarelyControlEventDefinition definition) noexcept
      : BarelyControlEventDefinition{definition} {}

  /// Returns a new `ControlEventDefinition` with `Callback`.
  ///
  /// @return Control event definition.
  static ControlEventDefinition WithCallback() {
    return ControlEventDefinition(
        [](void** state, void* user_data) noexcept {
          *state = new Callback(std::move(*static_cast<Callback*>(user_data)));
        },
        [](void** state) noexcept { delete static_cast<Callback*>(*state); },
        [](void** state, int32_t index, double value) noexcept {
          if (const auto* callback = static_cast<Callback*>(*state);
              *callback) {
            (*callback)(index, value);
          }
        });
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

/// Note control event definition.
struct NoteControlEventDefinition : public BarelyNoteControlEventDefinition {
  /// Callback signature.
  ///
  /// @param pitch Note pitch.
  /// @param index Note control index.
  /// @param value Note control value.
  using Callback = std::function<void(double pitch, int index, double value)>;

  /// Create callback signature.
  using CreateCallback = BarelyNoteControlEventDefinition_CreateCallback;

  /// Destroy callback signature.
  using DestroyCallback = BarelyNoteControlEventDefinition_DestroyCallback;

  /// Process callback signature.
  using ProcessCallback = BarelyNoteControlEventDefinition_ProcessCallback;

  /// Constructs a new `NoteControlEventDefinition`.
  ///
  /// @param create_callback Create callback.
  /// @param destroy_callback Destroy callback.
  /// @param process_callback Process callback.
  explicit NoteControlEventDefinition(CreateCallback create_callback,
                                      DestroyCallback destroy_callback,
                                      ProcessCallback process_callback) noexcept
      : NoteControlEventDefinition(BarelyNoteControlEventDefinition{
            create_callback,
            destroy_callback,
            process_callback,
        }) {}

  /// Constructs a new `NoteControlEventDefinition` from a raw type.
  ///
  /// @param definition Raw note control event definition.
  // NOLINTNEXTLINE(google-explicit-constructor)
  NoteControlEventDefinition(
      BarelyNoteControlEventDefinition definition) noexcept
      : BarelyNoteControlEventDefinition{definition} {}

  /// Returns a new `NoteControlEventDefinition` with `Callback`.
  ///
  /// @return Note control event definition.
  static NoteControlEventDefinition WithCallback() {
    return NoteControlEventDefinition(
        [](void** state, void* user_data) noexcept {
          *state = new Callback(std::move(*static_cast<Callback*>(user_data)));
        },
        [](void** state) noexcept { delete static_cast<Callback*>(*state); },
        [](void** state, double pitch, int32_t index, double value) noexcept {
          if (const auto* callback = static_cast<Callback*>(*state);
              *callback) {
            (*callback)(pitch, index, value);
          }
        });
  }
};

/// Note off event definition.
struct NoteOffEventDefinition : public BarelyNoteOffEventDefinition {
  /// Callback signature.
  ///
  /// @param pitch Note pitch.
  using Callback = std::function<void(double pitch)>;

  /// Create callback signature.
  using CreateCallback = BarelyNoteOffEventDefinition_CreateCallback;

  /// Destroy callback signature.
  using DestroyCallback = BarelyNoteOffEventDefinition_DestroyCallback;

  /// Process callback signature.
  using ProcessCallback = BarelyNoteOffEventDefinition_ProcessCallback;

  /// Constructs a new `NoteOffEventDefinition`.
  ///
  /// @param create_callback Create callback.
  /// @param destroy_callback Destroy callback.
  /// @param process_callback Process callback.
  explicit NoteOffEventDefinition(CreateCallback create_callback,
                                  DestroyCallback destroy_callback,
                                  ProcessCallback process_callback) noexcept
      : NoteOffEventDefinition(BarelyNoteOffEventDefinition{
            create_callback,
            destroy_callback,
            process_callback,
        }) {}

  /// Constructs a new `NoteOffEventDefinition` from a raw type.
  ///
  /// @param definition Raw note off event definition.
  // NOLINTNEXTLINE(google-explicit-constructor)
  NoteOffEventDefinition(BarelyNoteOffEventDefinition definition) noexcept
      : BarelyNoteOffEventDefinition{definition} {}

  /// Returns a new `NoteOffEventDefinition` with `Callback`.
  ///
  /// @return Note off event definition.
  static NoteOffEventDefinition WithCallback() {
    return NoteOffEventDefinition(
        [](void** state, void* user_data) noexcept {
          *state = new Callback(std::move(*static_cast<Callback*>(user_data)));
        },
        [](void** state) noexcept { delete static_cast<Callback*>(*state); },
        [](void** state, double pitch) noexcept {
          if (const auto* callback = static_cast<Callback*>(*state);
              *callback) {
            (*callback)(pitch);
          }
        });
  }
};

/// Note on event definition.
struct NoteOnEventDefinition : public BarelyNoteOnEventDefinition {
  /// Callback signature.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  using Callback = std::function<void(double pitch, double intensity)>;

  /// Create callback signature.
  using CreateCallback = BarelyNoteOnEventDefinition_CreateCallback;

  /// Destroy callback signature.
  using DestroyCallback = BarelyNoteOnEventDefinition_DestroyCallback;

  /// Process callback signature.
  using ProcessCallback = BarelyNoteOnEventDefinition_ProcessCallback;

  /// Constructs a new `NoteOnEventDefinition`.
  ///
  /// @param create_callback Create callback.
  /// @param destroy_callback Destroy callback.
  /// @param process_callback Process callback.
  explicit NoteOnEventDefinition(CreateCallback create_callback,
                                 DestroyCallback destroy_callback,
                                 ProcessCallback process_callback) noexcept
      : NoteOnEventDefinition(BarelyNoteOnEventDefinition{
            create_callback,
            destroy_callback,
            process_callback,
        }) {}

  /// Constructs a new `NoteOnEventDefinition` from a raw type.
  ///
  /// @param definition Raw note on event definition.
  // NOLINTNEXTLINE(google-explicit-constructor)
  NoteOnEventDefinition(BarelyNoteOnEventDefinition definition) noexcept
      : BarelyNoteOnEventDefinition{definition} {}

  /// Returns a new `NoteOnEventDefinition` with `Callback`.
  ///
  /// @return Note on event definition.
  static NoteOnEventDefinition WithCallback() {
    return NoteOnEventDefinition(
        [](void** state, void* user_data) noexcept {
          *state = new Callback(std::move(*static_cast<Callback*>(user_data)));
        },
        [](void** state) noexcept { delete static_cast<Callback*>(*state); },
        [](void** state, double pitch, double intensity) noexcept {
          if (const auto* callback = static_cast<Callback*>(*state);
              *callback) {
            (*callback)(pitch, intensity);
          }
        });
  }
};

/// Task definition.
struct TaskDefinition : public BarelyTaskDefinition {
  /// Callback signature.
  using Callback = std::function<void()>;

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

  /// Returns a new `TaskDefinition` with `Callback`.
  ///
  /// @return Task definition.
  static TaskDefinition WithCallback() {
    return TaskDefinition(
        [](void** state, void* user_data) noexcept {
          *state = new Callback(std::move(*static_cast<Callback*>(user_data)));
        },
        [](void** state) noexcept { delete static_cast<Callback*>(*state); },
        [](void** state) noexcept {
          if (const auto* callback = static_cast<Callback*>(*state);
              *callback) {
            (*callback)();
          }
        });
  }
};

/// Effect handle.
class EffectHandle {
 public:
  /// Default constructor.
  EffectHandle() = default;

  /// Equality comparator.
  bool operator==(const BarelyEffectHandle& handle) const {
    return handle_ == handle;
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
    if (const Status status =
            BarelyEffect_GetControl(handle_, static_cast<int>(index), &value);
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
    if (const Status status =
            BarelyEffect_GetProcessOrder(handle_, &process_order);
        !status.IsOk()) {
      return status;
    }
    return process_order;
  }

  /// Resets all control values.
  ///
  /// @return Status.
  Status ResetAllControls() noexcept {
    return BarelyEffect_ResetAllControls(handle_);
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
    return BarelyEffect_ResetControl(handle_, static_cast<int>(index));
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
    return BarelyEffect_SetControl(handle_, static_cast<int>(index),
                                   static_cast<double>(value), slope_per_beat);
  }

  /// Sets the control event.
  ///
  /// @param definition Control event definition.
  /// @return Status.
  Status SetControlEvent(ControlEventDefinition definition,
                         void* user_data = nullptr) noexcept {
    return BarelyEffect_SetControlEvent(handle_, definition, user_data);
  }

  /// Sets the control event with a callback.
  ///
  /// @param callback Control event callback.
  /// @return Status.
  Status SetControlEvent(ControlEventDefinition::Callback callback) noexcept {
    return SetControlEvent(ControlEventDefinition::WithCallback(),
                           static_cast<void*>(&callback));
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
    return BarelyEffect_SetData(handle_, data, size);
  }

  /// Sets the process order.
  ///
  /// @param process_order Process order.
  /// @return Status.
  Status SetProcessOrder(int process_order) noexcept {
    return BarelyEffect_SetProcessOrder(handle_, process_order);
  }

 private:
  // Ensures that `EffectHandle` can only be constructed by `InstrumentHandle`.
  friend class InstrumentHandle;

  // Constructs a new `EffectHandle`.
  explicit EffectHandle(BarelyEffectHandle handle) noexcept : handle_(handle) {}

  // Raw handle.
  BarelyEffectHandle handle_ = nullptr;
};

/// Instrument handle.
class InstrumentHandle {
 public:
  /// Default constructor.
  InstrumentHandle() = default;

  /// Creates a new effect.
  ///
  /// @param definition Effect definition.
  /// @param process_order Task process order.
  /// @return Effect handle, or an error status.
  StatusOr<EffectHandle> CreateEffect(EffectDefinition definition,
                                      int process_order = 0) noexcept {
    BarelyEffectHandle effect = nullptr;
    if (const Status status =
            BarelyEffect_Create(handle_, definition, process_order, &effect);
        !status.IsOk()) {
      return status;
    }
    return EffectHandle(effect);
  }

  /// Destroys an effect.
  ///
  /// @param effect Effect handle.
  /// @return Status.
  Status DestroyEffect(EffectHandle effect) noexcept {
    return BarelyEffect_Destroy(effect.handle_);
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
            handle_, static_cast<int>(index), &value);
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
            handle_, pitch, static_cast<int>(index), &value);
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
            BarelyInstrument_IsNoteOn(handle_, pitch, &is_note_on);
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
    return BarelyInstrument_Process(handle_, output_samples,
                                    output_channel_count, output_frame_count,
                                    timestamp);
  }

  /// Resets all control values.
  ///
  /// @return Status.
  Status ResetAllControls() noexcept {
    return BarelyInstrument_ResetAllControls(handle_);
  }

  /// Resets all note control values.
  ///
  /// @param pitch Note pitch
  /// @return Status.
  Status ResetAllNoteControls(double pitch) noexcept {
    return BarelyInstrument_ResetAllNoteControls(handle_, pitch);
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
    return BarelyInstrument_ResetControl(handle_, static_cast<int>(index));
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
    return BarelyInstrument_ResetNoteControl(handle_, pitch,
                                             static_cast<int>(index));
  }

  /// Sets all notes off.
  ///
  /// @return Status.
  Status SetAllNotesOff() noexcept {
    return BarelyInstrument_SetAllNotesOff(handle_);
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
    return BarelyInstrument_SetControl(handle_, static_cast<int>(index),
                                       static_cast<double>(value),
                                       slope_per_beat);
  }

  /// Sets the control event.
  ///
  /// @param definition Control event definition.
  /// @return Status.
  Status SetControlEvent(ControlEventDefinition definition,
                         void* user_data = nullptr) noexcept {
    return BarelyInstrument_SetControlEvent(handle_, definition, user_data);
  }

  /// Sets the control event with a callback.
  ///
  /// @param callback Control event callback.
  /// @return Status.
  Status SetControlEvent(ControlEventDefinition::Callback callback) noexcept {
    return SetControlEvent(ControlEventDefinition::WithCallback(),
                           static_cast<void*>(&callback));
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
    return BarelyInstrument_SetData(handle_, data, size);
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
        handle_, pitch, static_cast<int>(index), static_cast<double>(value),
        slope_per_beat);
  }

  /// Sets the note control event.
  ///
  /// @param definition Note control event definition.
  /// @return Status.
  Status SetNoteControlEvent(NoteControlEventDefinition definition,
                             void* user_data = nullptr) noexcept {
    return BarelyInstrument_SetNoteControlEvent(handle_, definition, user_data);
  }

  /// Sets the note control event with a callback.
  ///
  /// @param callback Note control event callback.
  /// @return Status.
  Status SetNoteControlEvent(
      NoteControlEventDefinition::Callback callback) noexcept {
    return SetNoteControlEvent(NoteControlEventDefinition::WithCallback(),
                               static_cast<void*>(&callback));
  }

  /// Sets a note off.
  ///
  /// @param pitch Note pitch.
  /// @return Status.
  Status SetNoteOff(double pitch) noexcept {
    return BarelyInstrument_SetNoteOff(handle_, pitch);
  }

  /// Sets the note off event.
  ///
  /// @param definition Note off event definition.
  /// @return Status.
  Status SetNoteOffEvent(NoteOffEventDefinition definition,
                         void* user_data = nullptr) noexcept {
    return BarelyInstrument_SetNoteOffEvent(handle_, definition, user_data);
  }

  /// Sets the note off event with a callback.
  ///
  /// @param callback Note off event callback.
  /// @return Status.
  Status SetNoteOffEvent(NoteOffEventDefinition::Callback callback) noexcept {
    return SetNoteOffEvent(NoteOffEventDefinition::WithCallback(),
                           static_cast<void*>(&callback));
  }

  /// Sets a note on.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  /// @return Status.
  Status SetNoteOn(double pitch, double intensity = 1.0) noexcept {
    return BarelyInstrument_SetNoteOn(handle_, pitch, intensity);
  }

  /// Sets the note on event.
  ///
  /// @param definition Note on event definition.
  /// @return Status.
  Status SetNoteOnEvent(NoteOnEventDefinition definition,
                        void* user_data = nullptr) noexcept {
    return BarelyInstrument_SetNoteOnEvent(handle_, definition, user_data);
  }

  /// Sets the note off event with a callback.
  ///
  /// @param callback Note off event callback.
  /// @return Status.
  Status SetNoteOnEvent(NoteOnEventDefinition::Callback callback) noexcept {
    return SetNoteOnEvent(NoteOnEventDefinition::WithCallback(),
                          static_cast<void*>(&callback));
  }

 private:
  // Ensures that `InstrumentHandle` can only be constructed by `Musician`.
  friend class Musician;

  // Constructs a new `InstrumentHandle`.
  explicit InstrumentHandle(BarelyInstrumentHandle handle) noexcept
      : handle_(handle) {}

  // Raw handle.
  BarelyInstrumentHandle handle_ = nullptr;
};

/// Task handle.
class TaskHandle {
 public:
  /// Default constructor.
  TaskHandle() = default;

  /// Returns the position.
  ///
  /// @return Position in beats, or an error status.
  [[nodiscard]] StatusOr<double> GetPosition() const noexcept {
    double position = 0.0;
    if (const Status status = BarelyTask_GetPosition(handle_, &position);
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
    if (const Status status =
            BarelyTask_GetProcessOrder(handle_, &process_order);
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
    return BarelyTask_SetPosition(handle_, position);
  }

  /// Sets the process order.
  ///
  /// @param process_order Process order.
  /// @return Status.
  Status SetProcessOrder(int process_order) noexcept {
    return BarelyTask_SetProcessOrder(handle_, process_order);
  }

 private:
  // Ensures that `TaskHandle` can only be constructed by `PerformerHandle`.
  friend class PerformerHandle;

  // Constructs a new `TaskHandle`.
  explicit TaskHandle(BarelyTaskHandle handle) noexcept : handle_(handle) {}

  // Raw handle.
  BarelyTaskHandle handle_ = nullptr;
};

/// Performer handle.
class PerformerHandle {
 public:
  /// Default constructor.
  PerformerHandle() = default;

  /// Creates a new task.
  ///
  /// @param definition Task definition.
  /// @param is_one_off True if one-off task, false otherwise.
  /// @param position Task position in beats.
  /// @param process_order Task process order.
  /// @param user_data Pointer to user data.
  /// @return Task handle, or an error status.
  StatusOr<TaskHandle> CreateTask(TaskDefinition definition, bool is_one_off,
                                  double position, int process_order = 0,
                                  void* user_data = nullptr) noexcept {
    BarelyTaskHandle task = nullptr;
    if (const Status status =
            BarelyTask_Create(handle_, definition, is_one_off, position,
                              process_order, user_data, &task);
        !status.IsOk()) {
      return status;
    }
    return TaskHandle(task);
  }

  /// Creates a new task with a callback.
  ///
  /// @param callback Task callback.
  /// @param is_one_off True if one-off task, false otherwise.
  /// @param position Task position in beats.
  /// @param process_order Task process order.
  /// @return Task handle, or an error status.
  StatusOr<TaskHandle> CreateTask(TaskDefinition::Callback callback,
                                  bool is_one_off, double position,
                                  int process_order = 0) noexcept {
    return CreateTask(TaskDefinition::WithCallback(), is_one_off, position,
                      process_order, static_cast<void*>(&callback));
  }

  /// Destroys a task.
  ///
  /// @param task Task handle.
  /// @return Status.
  Status DestroyTask(TaskHandle task) noexcept {
    return BarelyTask_Destroy(task.handle_);
  }

  /// Returns the loop begin position.
  ///
  /// @return Loop begin position in beats, or an error status.
  [[nodiscard]] StatusOr<double> GetLoopBeginPosition() const noexcept {
    double loop_begin_position = 0.0;
    if (const Status status =
            BarelyPerformer_GetLoopBeginPosition(handle_, &loop_begin_position);
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
            BarelyPerformer_GetLoopLength(handle_, &loop_length);
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
    if (const Status status = BarelyPerformer_GetPosition(handle_, &position);
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
    if (const Status status = BarelyPerformer_IsLooping(handle_, &is_looping);
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
    if (const Status status = BarelyPerformer_IsPlaying(handle_, &is_playing);
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
    return BarelyPerformer_SetLoopBeginPosition(handle_, loop_begin_position);
  }

  /// Sets the loop length.
  ///
  /// @param loop_length Loop length in beats.
  /// @return Status.
  Status SetLoopLength(double loop_length) noexcept {
    return BarelyPerformer_SetLoopLength(handle_, loop_length);
  }

  /// Sets whether the performer is looping or not.
  ///
  /// @param is_looping True if looping, false otherwise.
  /// @return Status.
  Status SetLooping(bool is_looping) noexcept {
    return BarelyPerformer_SetLooping(handle_, is_looping);
  }

  /// Sets the position.
  ///
  /// @param position Position in beats.
  /// @return Status.
  Status SetPosition(double position) noexcept {
    return BarelyPerformer_SetPosition(handle_, position);
  }

  /// Starts the performer.
  ///
  /// @return Status.
  Status Start() noexcept { return BarelyPerformer_Start(handle_); }

  /// Stops the performer.
  ///
  /// @return Status.
  Status Stop() noexcept { return BarelyPerformer_Stop(handle_); }

 private:
  // Ensures that `PerformerHandle` can only be constructed by `Musician`.
  friend class Musician;

  // Constructs a new `PerformerHandle`.
  explicit PerformerHandle(BarelyPerformerHandle handle) noexcept
      : handle_(handle) {}

  // Raw handle.
  BarelyPerformerHandle handle_ = nullptr;
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
  /// @return Musician.
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
  /// @return Instrument handle.
  [[nodiscard]] InstrumentHandle CreateInstrument(
      InstrumentDefinition definition, int frame_rate) noexcept {
    BarelyInstrumentHandle instrument = nullptr;
    [[maybe_unused]] const Status status =
        BarelyInstrument_Create(handle_, definition, frame_rate, &instrument);
    assert(status.IsOk());
    return InstrumentHandle(instrument);
  }

  /// Creates a new performer.
  ///
  /// @return Performer musician.
  [[nodiscard]] PerformerHandle CreatePerformer() noexcept {
    BarelyPerformerHandle performer = nullptr;
    [[maybe_unused]] const Status status =
        BarelyPerformer_Create(handle_, &performer);
    assert(status.IsOk());
    return PerformerHandle(performer);
  }

  /// Destroys an instrument.
  ///
  /// @param instrument Instrument handle.
  /// @return Status.
  Status DestroyInstrument(InstrumentHandle instrument) noexcept {
    return BarelyInstrument_Destroy(instrument.handle_);
  }

  /// Destroys a performer.
  ///
  /// @param performer Performer handle.
  /// @return Status.
  Status DestroyPerformer(PerformerHandle performer) noexcept {
    return BarelyPerformer_Destroy(performer.handle_);
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
};

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_BARELYMUSICIAN_H_
