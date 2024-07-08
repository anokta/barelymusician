/// ==================
/// barelymusician API
/// ==================
///
/// barelymusician is a real-time music engine for interactive systems.
/// It is used to generate and perform musical sounds from scratch in a sample accurate way.
///
/// -----------------
/// Example C++ Usage
/// -----------------
///
/// - Musician:
///
///   @code{.cpp}
///   #include "barelymusician/barelymusician.h"
///
///   // Create.
///   barely::ScopedMusician musician;
///
///   // Set the tempo.
///   musician.SetTempo(/*tempo=*/124.0);
///
///   // Update the timestamp.
///   //
///   // Timestamp updates must happen prior to processing of instruments with respective
///   // timestamps. Otherwise, such `Process` calls will be *late* to receive any relevant state
///   // changes. Therefore, this should typically be called from a main thread update callback,
///   // with an additional "lookahead", in order to avoid any potential thread synchronization
///   // issues that could occur in real-time audio applications.
///   double timestamp = 1.0;
///   musician.Update(timestamp);
///   @endcode
///
/// - Instrument:
///
///   @code{.cpp}
///   #include "barelymusician/instruments/synth_instrument.h"
///
///   // Set a note on.
///   //
///   // Pitch values are normalized, where each `1.0` shifts one octave, and `0.0` represents the
///   // middle A (A4) for a typical instrument definition. However, this is not a strict rule,
///   // since `pitch` and `intensity` can be interpreted in any desired way by a custom instrument.
///   instrument.SetNoteOn(/*pitch=*/-1.0, /*intensity=*/0.25);
///
///   // Check if the note is on.
///   const bool is_note_on = instrument.IsNoteOn(/*pitch=*/-1.0);
///
///   // Set a control value.
///   instrument.GetControl(barely::SynthInstrument::Control::kGain).SetValue(/*value=*/0.5);
///
///   // Process.
///   //
///   // Instruments expect raw PCM audio samples to be processed with a synchronous call.
///   // Therefore, this should typically be called from an audio thread process callback in
///   // real-time audio applications.
///   const int output_channel_count = 2;
///   const int output_frame_count = 1024;
///   std::vector<double> output_samples(output_channel_count * output_frame_count, 0.0);
///   double timestamp = 0.0;
///   instrument.Process(output_samples.data(), output_channel_count, output_frame_count,
///                      timestamp);
///   @endcode
///
/// - Effect:
///
///   @code{.cpp}
///   #include "barelymusician/instruments/low_pass_effect.h"
///
///   // Create.
///   auto effect = musician.CreateEffect<barely::LowPassEffect>(/*frame_rate=*/48000);
///
///   // Set a control value.
///   effect.GetControl(barely::LowPassEffect::Control::kCutoffFrequency).SetValue(/*value=*/800.0);
///
///   // Process.
///   effect.Process(output_samples.data(), output_channel_count, output_frame_count, timestamp);
///   @endcode
///
/// - Performer:
///
///   @code{.cpp}
///   // Create.
///   auto performer = musician.CreatePerformer(/*process_order=*/0);
///
///   // Create a task.
///   auto task = performer.CreateTask([]() { /*populate this*/ }, /*position=*/0.0);
///
///   // Set looping on.
///   performer.SetLooping(/*is_looping=*/true);
///
///   // Start.
///   performer.Start();
///
///   // Check if started playing.
///   const bool is_playing = performer.IsPlaying();
///   @endcode
///
/// ---------------
/// Example C Usage
/// ---------------
///
/// - Musician:
///
///   @code{.cpp}
///   #include "barelymusician/barelymusician.h"
///
///   // Create.
///   BarelyMusicianHandle musician;
///   BarelyMusician_Create(&musician);
///
///   // Set the tempo.
///   BarelyMusician_SetTempo(musician, /*tempo=*/124.0);
///
///   // Update the timestamp.
///   //
///   // Timestamp updates must happen prior to processing of instruments with respective
///   // timestamps. Otherwise, such `Process` calls will be *late* to receive any relevant state
///   // changes. Therefore, this should typically be called from a main thread update callback,
///   // with an additional "lookahead", in order to avoid any potential thread synchronization
///   // issues that could occur in real-time audio applications.
///   double timestamp = 1.0;
///   BarelyMusician_Update(musician, timestamp);
///
///   // Destroy.
///   BarelyMusician_Destroy(musician);
///   @endcode
///
/// - Instrument:
///
///   @code{.cpp}
///   #include "barelymusician/instruments/synth_instrument.h"
///
///   // Create.
///   BarelyInstrumentHandle instrument;
///   BarelyMusician_CreateInstrument(musician, BarelySynthInstrument_GetDefinition(),
///                                   /*frame_rate=*/48000, &instrument);
///
///   // Set a note on.
///   //
///   // Pitch values are normalized, where each `1.0` shifts one octave, and `0.0` represents the
///   // middle A (A4) for a typical instrument definition. However, this is not a strict rule,
///   // since `pitch` and `intensity` can be interpreted in any desired way by a custom instrument.
///   BarelyInstrument_SetNoteOn(instrument, /*pitch=*/-1.0, /*intensity=*/0.25);
///
///   // Check if the note is on.
///   bool is_note_on = false;
///   BarelyInstrument_IsNoteOn(instrument, /*pitch=*/-1.0, &is_note_on);
///
///   // Set a control value.
///   BarelyControlHandle control;
///   BarelyInstrument_GetControl(instrument, /*id=*/0, &control);
///   BarelyControl_SetValue(control, /*value=*/0.5);
///
///   // Process.
///   //
///   // Instruments expect raw PCM audio buffers to be processed with a synchronous call.
///   // Therefore, this should typically be called from an audio thread process callback in
///   // real-time audio applications.
///   double output_samples[2 * 1024];
///   int output_channel_count = 2;
///   int output_frame_count = 1024;
///   double timestamp = 0.0;
///   BarelyInstrument_Process(instrument, output_samples, output_channel_count, output_frame_count,
///                            timestamp);
///
///   // Destroy.
///   BarelyMusician_DestroyInstrument(musician, instrument);
///   @endcode
///
/// - Effect:
///
///   @code{.cpp}
///   #include "barelymusician/effects/low_pass_effect.h"
///
///   // Create.
///   BarelyEffectHandle effect;
///   BarelyMusician_CreateEffect(musician, BarelyLowPassEffect_GetDefinition(),
///                               /*frame_rate=*/48000, &effect);
///
///   // Set a control value.
///   BarelyControlHandle control;
///   BarelyEffect_GetControl(effect, /*id=*/0, &control);
///   BarelyControl_SetValue(control, /*value=*/800.0);
///
///   // Process.
///   BarelyEffect_Process(effect, output_samples, output_channel_count, output_frame_count,
///                        timestamp);
///
///   // Destroy the effect.
///   BarelyMusician_DestroyEffect(musician, effect);
///   @endcode
///
/// - Performer:
///
///   @code{.cpp}
///   // Create.
///   BarelyPerformerHandle performer;
///   BarelyMusician_CreatePerformer(musician, /*process_order=*/0, &performer);
///
///   // Create a task.
///   BarelyTaskHandle task;
///   BarelyPerformer_CreateTask(performer, BarelyTaskDefinition{/*populate this*/},
///                              /*position=*/0.0, /*user_data=*/nullptr, &task);
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
///   BarelyPerformer_DestroyTask(performer, task);
///
///   // Destroy.
///   BarelyMusician_DestroyPerformer(musician, performer);
///   @endcode

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

/// Control definition.
typedef struct BarelyControlDefinition {
  /// Identifier.
  int32_t id;

  /// Default value.
  double default_value;

  /// Minimum value.
  double min_value;

  /// Maximum value.
  double max_value;
} BarelyControlDefinition;

/// Effect definition create callback signature.
///
/// @param state Pointer to effect state.
/// @param frame_rate Frame rate in hertz.
typedef void (*BarelyEffectDefinition_CreateCallback)(void** state, int32_t frame_rate);

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
typedef void (*BarelyEffectDefinition_ProcessCallback)(void** state, double* output_samples,
                                                       int32_t output_channel_count,
                                                       int32_t output_frame_count);

/// Effect definition set control callback signature.
///
/// @param state Pointer to effect state.
/// @param id Control identifier.
/// @param value Control value.
typedef void (*BarelyEffectDefinition_SetControlCallback)(void** state, int32_t id, double value);

/// Effect definition set data callback signature.
///
/// @param state Pointer to effect state.
/// @param data Pointer to immutable data.
/// @param size Data size in bytes.
typedef void (*BarelyEffectDefinition_SetDataCallback)(void** state, const void* data,
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
typedef void (*BarelyInstrumentDefinition_CreateCallback)(void** state, int32_t frame_rate);

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
typedef void (*BarelyInstrumentDefinition_ProcessCallback)(void** state, double* output_samples,
                                                           int32_t output_channel_count,
                                                           int32_t output_frame_count);

/// Instrument definition set control callback signature.
///
/// @param state Pointer to instrument state.
/// @param id Control identifier.
/// @param value Control value.
typedef void (*BarelyInstrumentDefinition_SetControlCallback)(void** state, int32_t id,
                                                              double value);

/// Instrument definition set data callback signature.
///
/// @param state Pointer to instrument state.
/// @param data Pointer to immutable data.
/// @param size Data size in bytes.
typedef void (*BarelyInstrumentDefinition_SetDataCallback)(void** state, const void* data,
                                                           int32_t size);

/// Instrument definition set note control callback signature.
///
/// @param state Pointer to instrument state.
/// @param pitch Note pitch.
/// @param id Note control identifier.
/// @param value Note control value.
typedef void (*BarelyInstrumentDefinition_SetNoteControlCallback)(void** state, double pitch,
                                                                  int32_t id, double value);

/// Instrument definition set note off callback signature.
///
/// @param state Pointer to instrument state.
/// @param pitch Note pitch.
typedef void (*BarelyInstrumentDefinition_SetNoteOffCallback)(void** state, double pitch);

/// Instrument definition set note on callback signature.
///
/// @param state Pointer to instrument state.
/// @param pitch Note pitch.
/// @param intensity Note intensity.
typedef void (*BarelyInstrumentDefinition_SetNoteOnCallback)(void** state, double pitch,
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

/// Note off event definition create callback signature.
///
/// @param state Pointer to note off state.
/// @param user_data Pointer to user data.
typedef void (*BarelyNoteOffEventDefinition_CreateCallback)(void** state, void* user_data);

/// Note off event definition destroy callback signature.
///
/// @param state Pointer to note off state.
typedef void (*BarelyNoteOffEventDefinition_DestroyCallback)(void** state);

/// Note off event definition process callback signature.
///
/// @param state Pointer to note off event state.
/// @param pitch Note pitch.
typedef void (*BarelyNoteOffEventDefinition_ProcessCallback)(void** state, double pitch);

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
typedef void (*BarelyNoteOnEventDefinition_CreateCallback)(void** state, void* user_data);

/// Note on event definition destroy callback signature.
///
/// @param state Pointer to note on state.
typedef void (*BarelyNoteOnEventDefinition_DestroyCallback)(void** state);

/// Note on event definition process callback signature.
///
/// @param state Pointer to note on event state.
/// @param pitch Note pitch.
/// @param intensity Note intensity.
typedef void (*BarelyNoteOnEventDefinition_ProcessCallback)(void** state, double pitch,
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
typedef void (*BarelyTaskDefinition_CreateCallback)(void** state, void* user_data);

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

/// Control handle.
typedef struct BarelyControl* BarelyControlHandle;

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

/// Gets a control value.
///
/// @param control Control handle.
/// @param out_value Output value.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyControl_GetValue(BarelyControlHandle control, double* out_value);

/// Resets a control value.
///
/// @param control Control handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyControl_ResetValue(BarelyControlHandle control);

/// Sets a control value.
///
/// @param control Control handle.
/// @param value Value.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyControl_SetValue(BarelyControlHandle control, double value);

/// Gets an effect control.
///
/// @param effect Effect handle.
/// @param id Control identifier.
/// @param out_control Output control handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyEffect_GetControl(BarelyEffectHandle effect, int32_t id,
                                           BarelyControlHandle* out_control);

/// Processes effect output samples at timestamp.
/// @note This is *not* thread-safe during a corresponding `BarelyMusician_DestroyEffect` call.
///
/// @param effect Effect handle.
/// @param output_samples Array of interleaved output samples.
/// @param output_channel_count Number of output channels.
/// @param output_frame_count Number of output frames.
/// @param timestamp Timestamp in seconds.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyEffect_Process(BarelyEffectHandle instrument, double* output_samples,
                                        int32_t output_channel_count, int32_t output_frame_count,
                                        double timestamp);

/// Sets effect data.
///
/// @param effect Effect handle.
/// @param data Pointer to immutable data.
/// @param size Data size in bytes.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyEffect_SetData(BarelyEffectHandle effect, const void* data, int32_t size);

/// Gets an instrument control.
///
/// @param instrument Instrument handle.
/// @param id Control identifier.
/// @param out_control Output control handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_GetControl(BarelyInstrumentHandle instrument, int32_t id,
                                               BarelyControlHandle* out_control);

/// Gets an instrument note control.
///
/// @param instrument Instrument handle.
/// @param pitch Note pitch.
/// @param id Note control identifier.
/// @param out_control Output control handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_GetNoteControl(BarelyInstrumentHandle instrument, double pitch,
                                                   int32_t id, BarelyControlHandle* out_control);

/// Gets whether an instrument note is on or not.
///
/// @param instrument Instrument handle.
/// @param pitch Note pitch.
/// @param out_is_note_on Output true if on, false otherwise.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_IsNoteOn(BarelyInstrumentHandle instrument, double pitch,
                                             bool* out_is_note_on);

/// Processes instrument output samples at timestamp.
/// @note This is *not* thread-safe during a corresponding `BarelyMusician_DestroyInstrument` call.
///
/// @param instrument Instrument handle.
/// @param output_samples Array of interleaved output samples.
/// @param output_channel_count Number of output channels.
/// @param output_frame_count Number of output frames.
/// @param timestamp Timestamp in seconds.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_Process(BarelyInstrumentHandle instrument,
                                            double* output_samples, int32_t output_channel_count,
                                            int32_t output_frame_count, double timestamp);

/// Sets all instrument notes off.
///
/// @param instrument Instrument handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_SetAllNotesOff(BarelyInstrumentHandle instrument);

/// Sets instrument data.
///
/// @param instrument Instrument handle.
/// @param data Pointer to immutable data.
/// @param size Data size in bytes.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_SetData(BarelyInstrumentHandle instrument, const void* data,
                                            int32_t size);

/// Sets an instrument note off.
///
/// @param instrument Instrument handle.
/// @param pitch Note pitch.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_SetNoteOff(BarelyInstrumentHandle instrument, double pitch);

/// Sets the note off event of an instrument.
///
/// @param instrument Instrument handle.
/// @param definition Note off event definition.
/// @param user_data Pointer to user data.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_SetNoteOffEvent(BarelyInstrumentHandle instrument,
                                                    BarelyNoteOffEventDefinition definition,
                                                    void* user_data);

/// Sets an instrument note on.
///
/// @param instrument Instrument handle.
/// @param pitch Note pitch.
/// @param intensity Note intensity.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_SetNoteOn(BarelyInstrumentHandle instrument, double pitch,
                                              double intensity);

/// Sets the note on event of an instrument.
///
/// @param instrument Instrument handle.
/// @param definition Note on event definition.
/// @param user_data Pointer to user data.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_SetNoteOnEvent(BarelyInstrumentHandle instrument,
                                                   BarelyNoteOnEventDefinition definition,
                                                   void* user_data);

/// Creates a new musician.
///
/// @param out_musician Output musician handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_Create(BarelyMusicianHandle* out_musician);

/// Creates a new effect.
///
/// @param musician Musician handle.
/// @param definition Effect definition.
/// @param frame_rate Frame rate in hertz.
/// @param out_effect Output effect handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_CreateEffect(BarelyMusicianHandle musician,
                                               BarelyEffectDefinition definition,
                                               int32_t frame_rate, BarelyEffectHandle* out_effect);

/// Creates a new instrument.
///
/// @param musician Musician handle.
/// @param definition Instrument definition.
/// @param frame_rate Frame rate in hertz.
/// @param out_instrument Output instrument handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_CreateInstrument(BarelyMusicianHandle musician,
                                                   BarelyInstrumentDefinition definition,
                                                   int32_t frame_rate,
                                                   BarelyInstrumentHandle* out_instrument);

/// Creates a new performer.
///
/// @param musician Musician handle.
/// @param process_order Process order.
/// @param out_performer Output performer handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_CreatePerformer(BarelyMusicianHandle musician,
                                                  int32_t process_order,
                                                  BarelyPerformerHandle* out_performer);

/// Destroys a musician.
///
/// @param musician Musician handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_Destroy(BarelyMusicianHandle musician);

/// Destroys an effect.
///
/// @param musician Musician handle.
/// @param effect Effect handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_DestroyEffect(BarelyMusicianHandle musician,
                                                BarelyEffectHandle effect);

/// Destroys an instrument.
///
/// @param musician Musician handle.
/// @param instrument Instrument handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_DestroyInstrument(BarelyMusicianHandle musician,
                                                    BarelyInstrumentHandle instrument);

/// Destroys a performer.
///
/// @param musician Musician handle.
/// @param performer Performer handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_DestroyPerformer(BarelyMusicianHandle musician,
                                                   BarelyPerformerHandle performer);

/// Gets the corresponding number of musician beats for a given number of seconds.
///
/// @param musician Musician handle.
/// @param seconds Number of seconds.
/// @param out_beats Output number of musician beats.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_GetBeatsFromSeconds(BarelyMusicianHandle musician, double seconds,
                                                      double* out_beats);

/// Gets the corresponding number of seconds for a given number of musician beats.
///
/// @param musician Musician handle.
/// @param beats Number of musician beats.
/// @param out_seconds Output number of seconds.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_GetSecondsFromBeats(BarelyMusicianHandle musician, double beats,
                                                      double* out_seconds);

/// Gets the tempo of a musician.
///
/// @param musician Musician handle.
/// @param out_tempo Output tempo in beats per minute.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_GetTempo(BarelyMusicianHandle musician, double* out_tempo);

/// Gets the timestamp of a musician.
///
/// @param musician Musician handle.
/// @param out_timestamp Output timestamp in seconds.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_GetTimestamp(BarelyMusicianHandle musician,
                                               double* out_timestamp);

/// Sets the tempo of a musician.
///
/// @param musician Musician handle.
/// @param tempo Tempo in beats per minute.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_SetTempo(BarelyMusicianHandle musician, double tempo);

/// Updates a musician at timestamp.
///
/// @param musician Musician handle.
/// @param timestamp Timestamp in seconds.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_Update(BarelyMusicianHandle musician, double timestamp);

/// Cancels all one-off performer tasks.
///
/// @param musician Musician handle.
/// @param out_performer Output performer handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_CancelAllOneOffTasks(BarelyPerformerHandle performer);

/// Creates a new performer task.
///
/// @param performer Performer handle.
/// @param definition Task definition.
/// @param position Task position in beats.
/// @param user_data Pointer to user data.
/// @param out_task Output task handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_CreateTask(BarelyPerformerHandle performer,
                                              BarelyTaskDefinition definition, double position,
                                              void* user_data, BarelyTaskHandle* out_task);

/// Destroys a performer task.
///
/// @param performer Performer handle.
/// @param task Task handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_DestroyTask(BarelyPerformerHandle performer,
                                               BarelyTaskHandle task);

/// Gets the loop begin position of a performer.
///
/// @param performer Performer handle.
/// @param out_loop_begin_position Output loop begin position in beats.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_GetLoopBeginPosition(BarelyPerformerHandle performer,
                                                        double* out_loop_begin_position);

/// Gets the loop length of a performer.
///
/// @param performer Performer handle.
/// @param out_loop_length Output loop length.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_GetLoopLength(BarelyPerformerHandle performer,
                                                 double* out_loop_length);

/// Gets the position of a performer.
///
/// @param performer Performer handle.
/// @param out_position Output position in beats.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_GetPosition(BarelyPerformerHandle performer,
                                               double* out_position);

/// Gets whether a performer is looping or not.
///
/// @param performer Performer handle.
/// @param out_is_looping Output true if looping, false otherwise.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_IsLooping(BarelyPerformerHandle performer, bool* out_is_looping);

/// Gets whether a performer is playing or not.
///
/// @param performer Performer handle.
/// @param out_is_playing Output true if playing, false otherwise.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_IsPlaying(BarelyPerformerHandle performer, bool* out_is_playing);

/// Schedules a one-off task.
///
/// @param performer Performer handle.
/// @param definition Task definition.
/// @param position Task position in beats.
/// @param user_data Pointer to user data.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_ScheduleOneOffTask(BarelyPerformerHandle performer,
                                                      BarelyTaskDefinition definition,
                                                      double position, void* user_data);

/// Sets the loop begin position of a performer.
///
/// @param performer Performer handle.
/// @param loop_begin_position Loop begin position in beats.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_SetLoopBeginPosition(BarelyPerformerHandle performer,
                                                        double loop_begin_position);

/// Sets the loop length of a performer.
///
/// @param performer Performer handle.
/// @param loop_length Loop length in beats.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_SetLoopLength(BarelyPerformerHandle performer,
                                                 double loop_length);

/// Sets whether a performer is looping or not.
///
/// @param performer Performer handle.
/// @param is_looping True if looping, false otherwise.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_SetLooping(BarelyPerformerHandle performer, bool is_looping);

/// Sets the position of a performer.
///
/// @param performer Performer handle.
/// @param position Position in beats.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_SetPosition(BarelyPerformerHandle performer, double position);

/// Starts a performer.
///
/// @param performer Performer handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_Start(BarelyPerformerHandle performer);

/// Stops a performer.
///
/// @param performer Performer handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_Stop(BarelyPerformerHandle performer);

/// Gets the position of a task.
///
/// @param task Task handle.
/// @param out_position Output position in beats.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyTask_GetPosition(BarelyTaskHandle task, double* out_position);

/// Sets the position of a task.
///
/// @param task Task handle.
/// @param position Position in beats.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyTask_SetPosition(BarelyTaskHandle task, double position);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
// NOLINTEND

#ifdef __cplusplus
#include <cassert>
#include <cstdint>
#include <functional>
#include <limits>
#include <new>
#include <span>
#include <type_traits>
#include <utility>

namespace barely {

/// Control definition.
struct ControlDefinition : public BarelyControlDefinition {
  /// Default constructor.
  ControlDefinition() noexcept = default;

  /// Constructs a new `ControlDefinition`.
  ///
  /// @param id Identifier.
  /// @param default_value Default value.
  /// @param min_value Minimum value.
  /// @param max_value Maximum value.
  template <typename IdType, typename ValueType>
  ControlDefinition(IdType id, ValueType default_value,
                    ValueType min_value = std::numeric_limits<ValueType>::lowest(),
                    ValueType max_value = std::numeric_limits<ValueType>::max()) noexcept
      : ControlDefinition(BarelyControlDefinition{
            static_cast<int32_t>(id),
            static_cast<double>(default_value),
            static_cast<double>(min_value),
            static_cast<double>(max_value),
        }) {
    static_assert(std::is_integral<IdType>::value || std::is_enum<IdType>::value,
                  "IdType is not supported");
    static_assert(std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
                  "ValueType is not supported");
  }

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
  explicit EffectDefinition(CreateCallback create_callback, DestroyCallback destroy_callback,
                            ProcessCallback process_callback,
                            SetControlCallback set_control_callback,
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
  using SetNoteControlCallback = BarelyInstrumentDefinition_SetNoteControlCallback;

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
      SetDataCallback set_data_callback, SetNoteControlCallback set_note_control_callback,
      SetNoteOffCallback set_note_off_callback, SetNoteOnCallback set_note_on_callback,
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

  /// Returns a new `NoteOffEventDefinition` with `Callback`.
  ///
  /// @return Note off event definition.
  static NoteOffEventDefinition WithCallback() noexcept {
    return NoteOffEventDefinition(
        [](void** state, void* user_data) noexcept {
          *state = new (std::nothrow) Callback(std::move(*static_cast<Callback*>(user_data)));
          assert(*state);
        },
        [](void** state) noexcept { delete static_cast<Callback*>(*state); },
        [](void** state, double pitch) noexcept {
          if (const auto& callback = *static_cast<Callback*>(*state); callback) {
            callback(pitch);
          }
        });
  }

  /// Constructs a new `NoteOffEventDefinition`.
  ///
  /// @param create_callback Create callback.
  /// @param destroy_callback Destroy callback.
  /// @param process_callback Process callback.
  explicit NoteOffEventDefinition(CreateCallback create_callback, DestroyCallback destroy_callback,
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

  /// Returns a new `NoteOnEventDefinition` with `Callback`.
  ///
  /// @return Note on event definition.
  static NoteOnEventDefinition WithCallback() noexcept {
    return NoteOnEventDefinition(
        [](void** state, void* user_data) noexcept {
          *state = new (std::nothrow) Callback(std::move(*static_cast<Callback*>(user_data)));
          assert(*state);
        },
        [](void** state) noexcept { delete static_cast<Callback*>(*state); },
        [](void** state, double pitch, double intensity) noexcept {
          if (const auto& callback = *static_cast<Callback*>(*state); callback) {
            callback(pitch, intensity);
          }
        });
  }

  /// Constructs a new `NoteOnEventDefinition`.
  ///
  /// @param create_callback Create callback.
  /// @param destroy_callback Destroy callback.
  /// @param process_callback Process callback.
  explicit NoteOnEventDefinition(CreateCallback create_callback, DestroyCallback destroy_callback,
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

  /// Returns a new `TaskDefinition` with `Callback`.
  ///
  /// @return Task definition.
  static TaskDefinition WithCallback() noexcept {
    return TaskDefinition(
        [](void** state, void* user_data) noexcept {
          *state = new (std::nothrow) Callback(std::move(*static_cast<Callback*>(user_data)));
          assert(*state);
        },
        [](void** state) noexcept { delete static_cast<Callback*>(*state); },
        [](void** state) noexcept {
          if (const auto& callback = *static_cast<Callback*>(*state); callback) {
            callback();
          }
        });
  }

  /// Constructs a new `TaskDefinition`.
  ///
  /// @param create_callback Create callback.
  /// @param destroy_callback Destroy callback.
  /// @param process_callback Process callback.
  explicit TaskDefinition(CreateCallback create_callback, DestroyCallback destroy_callback,
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
  TaskDefinition(BarelyTaskDefinition definition) noexcept : BarelyTaskDefinition{definition} {}
};

/// Wrapper template.
template <typename HandleType>
class Wrapper {
 public:
  /// Default constructor.
  Wrapper() noexcept = default;

  /// Constructs a new `Wrapper`.
  ///
  /// @param handle Raw handle.
  explicit Wrapper(HandleType handle) noexcept : handle_(handle) { assert(handle); }

  /// Returns the raw handle.
  ///
  /// @return Handle.
  operator HandleType() const noexcept { return handle_; }

 private:
  // Raw handle.
  HandleType handle_ = nullptr;
};

/// Class that wraps a control.
class Control : public Wrapper<BarelyControlHandle> {
 public:
  /// Default constructor.
  Control() noexcept = default;

  /// Creates a new `Control` from a raw control handle.
  ///
  /// @param control Raw control handle.
  explicit Control(BarelyControlHandle control) noexcept : Wrapper(control) {}

  /// Returns the value.
  ///
  /// @return Value.
  template <typename ValueType>
  [[nodiscard]] ValueType GetValue() const noexcept {
    static_assert(std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
                  "ValueType is not supported");
    double value = 0.0;
    [[maybe_unused]] const bool success = BarelyControl_GetValue(*this, &value);
    assert(success);
    return static_cast<ValueType>(value);
  }

  /// Resets the value.
  void ResetValue() noexcept {
    [[maybe_unused]] const bool success = BarelyControl_ResetValue(*this);
    assert(success);
  }

  /// Sets the value.
  ///
  /// @param value Value.
  template <typename ValueType>
  void SetValue(ValueType value) noexcept {
    static_assert(std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
                  "ValueType is not supported");
    [[maybe_unused]] const bool success = BarelyControl_SetValue(*this, static_cast<double>(value));
    assert(success);
  }
};

/// Class that wraps an effect.
class Effect : public Wrapper<BarelyEffectHandle> {
 public:
  /// Default constructor.
  Effect() noexcept = default;

  /// Creates a new `Effect` from a raw effect handle.
  ///
  /// @param effect Raw effect handle.
  explicit Effect(BarelyEffectHandle effect) noexcept : Wrapper(effect) {}

  /// Returns a control.
  ///
  /// @param id Control identifier.
  /// @return Control.
  template <typename IdType>
  [[nodiscard]] Control GetControl(IdType id) const noexcept {
    static_assert(std::is_integral<IdType>::value || std::is_enum<IdType>::value,
                  "IdType is not supported");
    BarelyControlHandle control = nullptr;
    [[maybe_unused]] const bool success =
        BarelyEffect_GetControl(*this, static_cast<int>(id), &control);
    assert(success);
    return Control(control);
  }

  /// Processes output samples at timestamp.
  ///
  /// @param output_samples Interleaved array of output samples.
  /// @param output_channel_count Number of output channels.
  /// @param output_frame_count Number of output frames.
  /// @param timestamp Timestamp in seconds.
  void Process(double* output_samples, int output_channel_count, int output_frame_count,
               double timestamp) noexcept {
    [[maybe_unused]] const bool success = BarelyEffect_Process(
        *this, output_samples, output_channel_count, output_frame_count, timestamp);
    assert(success);
  }

  /// Sets data of type.
  ///
  /// @param data Immutable data.
  template <typename DataType, std::enable_if<std::is_trivially_copyable<DataType>::value>>
  void SetData(const DataType& data) noexcept {
    SetData(static_cast<const void*>(&data), sizeof(decltype(data)));
  }

  /// Sets data with a container.
  ///
  /// @param container Immutable container.
  template <typename ContainerType, typename ValueType = typename ContainerType::value_type>
  void SetData(const ContainerType& container) noexcept {
    SetData(static_cast<const void*>(container.data()),
            static_cast<int>(container.size() * sizeof(ValueType)));
  }

  /// Sets data.
  ///
  /// @param data Pointer to immutable data.
  /// @param size Data size in bytes.
  void SetData(const void* data, int size) noexcept {
    [[maybe_unused]] const bool success = BarelyEffect_SetData(*this, data, size);
    assert(success);
  }
};

/// Class that wraps an instrument.
class Instrument : public Wrapper<BarelyInstrumentHandle> {
 public:
  /// Default constructor.
  Instrument() noexcept = default;

  // Creates a new `Instrument` from a raw instrument handle.
  ///
  /// @param instrument Raw instrument handle.
  explicit Instrument(BarelyInstrumentHandle instrument) noexcept : Wrapper(instrument) {}

  /// Returns a control.
  ///
  /// @param id Control identifier.
  /// @return Control.
  template <typename IdType>
  [[nodiscard]] Control GetControl(IdType id) const noexcept {
    static_assert(std::is_integral<IdType>::value || std::is_enum<IdType>::value,
                  "IdType is not supported");
    BarelyControlHandle control = nullptr;
    [[maybe_unused]] const bool success =
        BarelyInstrument_GetControl(*this, static_cast<int>(id), &control);
    assert(success);
    return Control(control);
  }

  /// Returns a note control.
  ///
  /// @param pitch Note pitch.
  /// @param id Note control identifier.
  /// @return Note control.
  template <typename IdType>
  [[nodiscard]] Control GetNoteControl(double pitch, IdType id) const noexcept {
    static_assert(std::is_integral<IdType>::value || std::is_enum<IdType>::value,
                  "IdType is not supported");
    BarelyControlHandle note_control = nullptr;
    [[maybe_unused]] const bool success =
        BarelyInstrument_GetNoteControl(*this, pitch, id, &note_control);
    assert(success);
    return Control(note_control);
  }

  /// Returns whether a note is on or not.
  ///
  /// @param pitch Note pitch.
  /// @return True if active, false otherwise.
  [[nodiscard]] bool IsNoteOn(double pitch) const noexcept {
    bool is_note_on = false;
    [[maybe_unused]] const bool success = BarelyInstrument_IsNoteOn(*this, pitch, &is_note_on);
    assert(success);
    return is_note_on;
  }

  /// Processes output samples at timestamp.
  ///
  /// @param output_samples Interleaved array of output samples.
  /// @param output_channel_count Number of output channels.
  /// @param output_frame_count Number of output frames.
  /// @param timestamp Timestamp in seconds.
  void Process(double* output_samples, int output_channel_count, int output_frame_count,
               double timestamp) noexcept {
    [[maybe_unused]] const bool success = BarelyInstrument_Process(
        *this, output_samples, output_channel_count, output_frame_count, timestamp);
    assert(success);
  }

  /// Sets all notes off.
  void SetAllNotesOff() noexcept {
    [[maybe_unused]] const bool success = BarelyInstrument_SetAllNotesOff(*this);
    assert(success);
  }

  /// Sets data of type.
  ///
  /// @param data Immutable data.
  template <typename DataType, std::enable_if<std::is_trivially_copyable<DataType>::value>>
  void SetData(const DataType& data) noexcept {
    SetData(static_cast<const void*>(&data), sizeof(decltype(data)));
  }

  /// Sets data with a container.
  ///
  /// @param container Immutable container.
  template <typename ContainerType, typename ValueType = typename ContainerType::value_type>
  void SetData(const ContainerType& container) noexcept {
    SetData(static_cast<const void*>(container.data()),
            static_cast<int>(container.size() * sizeof(ValueType)));
  }

  /// Sets data.
  ///
  /// @param data Pointer to immutable data.
  /// @param size Data size in bytes.
  void SetData(const void* data, int size) noexcept {
    [[maybe_unused]] const bool success = BarelyInstrument_SetData(*this, data, size);
    assert(success);
  }

  /// Sets a note off.
  ///
  /// @param pitch Note pitch.
  void SetNoteOff(double pitch) noexcept {
    [[maybe_unused]] const bool success = BarelyInstrument_SetNoteOff(*this, pitch);
    assert(success);
  }

  /// Sets the note off event.
  ///
  /// @param definition Note off event definition.
  /// @param user_data Pointer to user data.
  void SetNoteOffEvent(NoteOffEventDefinition definition, void* user_data = nullptr) noexcept {
    [[maybe_unused]] const bool success =
        BarelyInstrument_SetNoteOffEvent(*this, definition, user_data);
    assert(success);
  }

  /// Sets the note off event with a callback.
  ///
  /// @param callback Note off event callback.
  void SetNoteOffEvent(NoteOffEventDefinition::Callback callback) noexcept {
    SetNoteOffEvent(NoteOffEventDefinition::WithCallback(), static_cast<void*>(&callback));
  }

  /// Sets a note on.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  void SetNoteOn(double pitch, double intensity = 1.0) noexcept {
    [[maybe_unused]] const bool success = BarelyInstrument_SetNoteOn(*this, pitch, intensity);
    assert(success);
  }

  /// Sets the note on event.
  ///
  /// @param definition Note on event definition.
  /// @param user_data Pointer to user data.
  void SetNoteOnEvent(NoteOnEventDefinition definition, void* user_data = nullptr) noexcept {
    [[maybe_unused]] const bool success =
        BarelyInstrument_SetNoteOnEvent(*this, definition, user_data);
    assert(success);
  }

  /// Sets the note off event with a callback.
  ///
  /// @param callback Note off event callback.
  void SetNoteOnEvent(NoteOnEventDefinition::Callback callback) noexcept {
    SetNoteOnEvent(NoteOnEventDefinition::WithCallback(), static_cast<void*>(&callback));
  }
};

/// Class that wraps a task.
class Task : public Wrapper<BarelyTaskHandle> {
 public:
  /// Default constructor.
  Task() noexcept = default;

  /// Creates a new `Task` from a raw task handle.
  ///
  /// @param task Raw task handle.
  explicit Task(BarelyTaskHandle task) noexcept : Wrapper(task) {}

  /// Returns the position.
  ///
  /// @return Position in beats.
  [[nodiscard]] double GetPosition() const noexcept {
    double position = 0.0;
    [[maybe_unused]] const bool success = BarelyTask_GetPosition(*this, &position);
    assert(success);
    return position;
  }

  /// Sets the position.
  ///
  /// @param position Position in beats.
  void SetPosition(double position) noexcept {
    [[maybe_unused]] const bool success = BarelyTask_SetPosition(*this, position);
    assert(success);
  }
};

/// Class that wraps a performer.
class Performer : public Wrapper<BarelyPerformerHandle> {
 public:
  /// Default constructor.
  Performer() noexcept = default;

  /// Creates a new `Performer` from a raw performer handle.
  ///
  /// @param performer Raw performer handle.
  explicit Performer(BarelyPerformerHandle performer) noexcept : Wrapper(performer) {}

  /// Cancels all one-off tasks.
  void CancelAllOneOffTasks() noexcept {
    [[maybe_unused]] const bool success = BarelyPerformer_CancelAllOneOffTasks(*this);
    assert(success);
  }

  /// Creates a new task.
  ///
  /// @param definition Task definition.
  /// @param position Task position in beats.
  /// @param user_data Pointer to user data.
  /// @return Task.
  [[nodiscard]] Task CreateTask(TaskDefinition definition, double position,
                                void* user_data = nullptr) noexcept {
    BarelyTaskHandle task;
    [[maybe_unused]] const bool success =
        BarelyPerformer_CreateTask(*this, definition, position, user_data, &task);
    assert(success);
    return Task(task);
  }

  /// Creates a new task with a callback.
  ///
  /// @param callback Task callback.
  /// @param position Task position in beats.
  /// @return Task.
  [[nodiscard]] Task CreateTask(TaskDefinition::Callback callback, double position) noexcept {
    return CreateTask(TaskDefinition::WithCallback(), position, static_cast<void*>(&callback));
  }

  /// Destroys a task.
  ///
  /// @param task Task.
  void DestroyTask(Task task) noexcept { BarelyPerformer_DestroyTask(*this, task); }

  /// Returns the loop begin position.
  ///
  /// @return Loop begin position in beats.
  [[nodiscard]] double GetLoopBeginPosition() const noexcept {
    double loop_begin_position = 0.0;
    [[maybe_unused]] const bool success =
        BarelyPerformer_GetLoopBeginPosition(*this, &loop_begin_position);
    assert(success);
    return loop_begin_position;
  }

  /// Returns the loop length.
  ///
  /// @return Loop length in beats.
  [[nodiscard]] double GetLoopLength() const noexcept {
    double loop_length = 0.0;
    [[maybe_unused]] const bool success = BarelyPerformer_GetLoopLength(*this, &loop_length);
    assert(success);
    return loop_length;
  }

  /// Returns the position.
  ///
  /// @return Position in beats.
  [[nodiscard]] double GetPosition() const noexcept {
    double position = 0.0;
    [[maybe_unused]] const bool success = BarelyPerformer_GetPosition(*this, &position);
    assert(success);
    return position;
  }

  /// Returns whether the performer is looping or not.
  ///
  /// @return True if looping, false otherwise.
  [[nodiscard]] bool IsLooping() const noexcept {
    bool is_looping = false;
    [[maybe_unused]] const bool success = BarelyPerformer_IsLooping(*this, &is_looping);
    assert(success);
    return is_looping;
  }

  /// Returns whether the performer is playing or not.
  ///
  /// @return True if playing, false otherwise.
  [[nodiscard]] bool IsPlaying() const noexcept {
    bool is_playing = false;
    [[maybe_unused]] const bool success = BarelyPerformer_IsPlaying(*this, &is_playing);
    assert(success);
    return is_playing;
  }

  /// Schedules a one-off task.
  ///
  /// @param definition Task definition.
  /// @param position Task position in beats.
  /// @param user_data Pointer to user data.
  void ScheduleOneOffTask(TaskDefinition definition, double position,
                          void* user_data = nullptr) noexcept {
    [[maybe_unused]] const bool success =
        BarelyPerformer_ScheduleOneOffTask(*this, definition, position, user_data);
    assert(success);
  }

  /// Schedules a one-off task with a callback.
  ///
  /// @param callback Task callback.
  /// @param position Task position in beats.
  void ScheduleOneOffTask(TaskDefinition::Callback callback, double position) noexcept {
    ScheduleOneOffTask(TaskDefinition::WithCallback(), position, static_cast<void*>(&callback));
  }

  /// Sets the loop begin position.
  ///
  /// @param loop_begin_position Loop begin position in beats.
  void SetLoopBeginPosition(double loop_begin_position) noexcept {
    [[maybe_unused]] const bool success =
        BarelyPerformer_SetLoopBeginPosition(*this, loop_begin_position);
    assert(success);
  }

  /// Sets the loop length.
  ///
  /// @param loop_length Loop length in beats.
  void SetLoopLength(double loop_length) noexcept {
    [[maybe_unused]] const bool success = BarelyPerformer_SetLoopLength(*this, loop_length);
    assert(success);
  }

  /// Sets whether the performer is looping or not.
  ///
  /// @param is_looping True if looping, false otherwise.
  void SetLooping(bool is_looping) noexcept {
    [[maybe_unused]] const bool success = BarelyPerformer_SetLooping(*this, is_looping);
    assert(success);
  }

  /// Sets the position.
  ///
  /// @param position Position in beats.
  void SetPosition(double position) noexcept {
    [[maybe_unused]] const bool success = BarelyPerformer_SetPosition(*this, position);
    assert(success);
  }

  /// Starts the performer.
  void Start() noexcept {
    [[maybe_unused]] const bool success = BarelyPerformer_Start(*this);
    assert(success);
  }

  /// Stops the performer.
  void Stop() noexcept {
    [[maybe_unused]] const bool success = BarelyPerformer_Stop(*this);
    assert(success);
  }
};

/// Class that wraps a musician.
class Musician : public Wrapper<BarelyMusicianHandle> {
 public:
  /// Default constructor.
  Musician() noexcept = default;

  /// Creates a new `Musician` from a raw musician handle.
  ///
  /// @param musician Raw musician handle.
  explicit Musician(BarelyMusicianHandle musician) noexcept : Wrapper(musician) {}

  /// Creates a new musician.
  ///
  /// @return Musician.
  [[nodiscard]] static Musician Create() noexcept {
    BarelyMusicianHandle musician = nullptr;
    [[maybe_unused]] const bool success = BarelyMusician_Create(&musician);
    assert(success);
    return Musician(musician);
  }

  /// Destroys a musician.
  ///
  /// @param musician Musician.
  static void Destroy(Musician musician) noexcept { BarelyMusician_Destroy(musician); }

  /// Creates a new component of type.
  ///
  /// @param args Component arguments.
  /// @return Component.
  template <class ComponentType, typename... ComponentArgs>
  [[nodiscard]] ComponentType CreateComponent(ComponentArgs&&... args) noexcept {
    return ComponentType(*this, args...);
  }

  /// Creates a new effect of type.
  ///
  /// @param frame_rate Frame rate in hertz.
  /// @return Effect.
  template <class EffectType>
  [[nodiscard]] Effect CreateEffect(int frame_rate) noexcept {
    return CreateEffect(EffectType::GetDefinition(), frame_rate);
  }

  /// Creates a new effect.
  ///
  /// @param definition Effect definition.
  /// @param frame_rate Frame rate in hertz.
  /// @return Effect.
  [[nodiscard]] Effect CreateEffect(EffectDefinition definition, int frame_rate) noexcept {
    BarelyEffectHandle effect;
    [[maybe_unused]] const bool success =
        BarelyMusician_CreateEffect(*this, definition, frame_rate, &effect);
    assert(success);
    return Effect(effect);
  }

  /// Creates a new instrument of type.
  ///
  /// @param frame_rate Frame rate in hertz.
  /// @return Instrument.
  template <class InstrumentType>
  [[nodiscard]] Instrument CreateInstrument(int frame_rate) noexcept {
    return CreateInstrument(InstrumentType::GetDefinition(), frame_rate);
  }

  /// Creates a new instrument.
  ///
  /// @param definition Instrument definition.
  /// @param frame_rate Frame rate in hertz.
  /// @return Instrument.
  [[nodiscard]] Instrument CreateInstrument(InstrumentDefinition definition,
                                            int frame_rate) noexcept {
    BarelyInstrumentHandle instrument;
    [[maybe_unused]] const bool success =
        BarelyMusician_CreateInstrument(*this, definition, frame_rate, &instrument);
    assert(success);
    return Instrument(instrument);
  }

  /// Creates a new performer.
  ///
  /// @param process_order Process order.
  /// @return Performer.
  [[nodiscard]] Performer CreatePerformer(int process_order = 0) noexcept {
    BarelyPerformerHandle performer;
    [[maybe_unused]] const bool success =
        BarelyMusician_CreatePerformer(*this, process_order, &performer);
    assert(success);
    return Performer(performer);
  }

  /// Destroys an effect.
  ///
  /// @param effect Effect.
  void DestroyEffect(Effect effect) noexcept { BarelyMusician_DestroyEffect(*this, effect); }

  /// Destroys an instrument.
  ///
  /// @param instrument Instrument.
  void DestroyInstrument(Instrument instrument) noexcept {
    BarelyMusician_DestroyInstrument(*this, instrument);
  }

  /// Destroys a performer.
  ///
  /// @param performer Performer.
  void DestroyPerformer(Performer performer) noexcept {
    BarelyMusician_DestroyPerformer(*this, performer);
  }

  /// Returns the corresponding number of beats for a given number of seconds.
  ///
  /// @param seconds Number of seconds.
  /// @return Number of beats.
  [[nodiscard]] double GetBeatsFromSeconds(double seconds) {
    double beats = 0.0;
    [[maybe_unused]] const bool success =
        BarelyMusician_GetBeatsFromSeconds(*this, seconds, &beats);
    assert(success);
    return beats;
  }

  /// Returns the corresponding number of seconds for a given number of beats.
  ///
  /// @param beats Number of beats.
  /// @return Number of seconds.
  [[nodiscard]] double GetSecondsFromBeats(double beats) {
    double seconds = 0.0;
    [[maybe_unused]] const bool success =
        BarelyMusician_GetSecondsFromBeats(*this, beats, &seconds);
    assert(success);
    return seconds;
  }

  /// Returns the tempo.
  ///
  /// @return Tempo in beats per minute.
  [[nodiscard]] double GetTempo() const noexcept {
    double tempo = 0.0;
    [[maybe_unused]] const bool success = BarelyMusician_GetTempo(*this, &tempo);
    assert(success);
    return tempo;
  }

  /// Returns the timestamp.
  ///
  /// @return Timestamp in seconds.
  [[nodiscard]] double GetTimestamp() const noexcept {
    double timestamp = 0.0;
    [[maybe_unused]] const bool success = BarelyMusician_GetTimestamp(*this, &timestamp);
    assert(success);
    return timestamp;
  }

  /// Sets the tempo.
  ///
  /// @param tempo Tempo in beats per minute.
  void SetTempo(double tempo) noexcept {
    [[maybe_unused]] const bool success = BarelyMusician_SetTempo(*this, tempo);
    assert(success);
  }

  /// Updates the musician at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void Update(double timestamp) noexcept {
    [[maybe_unused]] const bool success = BarelyMusician_Update(*this, timestamp);
    assert(success);
  }
};

/// Class that wraps a musician within a scope.
class ScopedMusician : public Musician {
 public:
  /// Creates a new `ScopedMusician`.
  ScopedMusician() noexcept : ScopedMusician(Musician::Create()) {}

  /// Creates a new `ScopedMusician` from a raw musician handle.
  ///
  /// @param musician Raw musician handle.
  explicit ScopedMusician(Musician musician) noexcept : Musician(musician) {}

  /// Destroys `ScopedMusician`.
  ~ScopedMusician() noexcept { Musician::Destroy(*this); }

  /// Non-copyable.
  ScopedMusician(const ScopedMusician& other) noexcept = delete;
  ScopedMusician& operator=(const ScopedMusician& other) noexcept = delete;

  /// Default move constructor.
  ScopedMusician(ScopedMusician&& other) noexcept = default;

  /// Assigns `ScopedMusician` via move.
  ///
  /// @param other Other musician.
  /// @return Scoped musician.
  ScopedMusician& operator=(ScopedMusician&& other) noexcept {
    if (this != &other) {
      Musician::Destroy(*this);
      Musician::operator=(std::move(other));
    }
    return *this;
  }
};

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_BARELYMUSICIAN_H_
