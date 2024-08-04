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
///   Musician musician;
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
///   // Create.
///   barely::Instrument instrument(musician, barely::SynthInstrument::GetDefinition(),
///                                 /*frame_rate=*/48000);
///
///   // Create a note.
///   //
///   // Pitch values are normalized, where each `1.0` shifts one octave, and `0.0` represents the
///   // middle A (A4) for a typical instrument definition. However, this is not a strict rule,
///   // since `pitch` and `intensity` can be interpreted in any desired way by a custom instrument.
///   barely::Note note(instrument, /*pitch=*/-1.0, /*intensity=*/0.25);
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
///   barely::Effect effect(musician, barely::LowPassEffect::GetDefinition(), /*frame_rate=*/48000);
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
///   barely::Performer performer(musician, /*process_order=*/0);
///
///   // Create a task.
///   barely::Task task(performer, []() { /*populate this*/ }, /*position=*/0.0);
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
///   BarelyMusician* musician = nullptr;
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
///   BarelyInstrument* instrument = nullptr;
///   BarelyInstrument_Create(musician, BarelySynthInstrument_GetDefinition(), /*frame_rate=*/48000,
///                           &instrument);
///
///   // Create a note.
///   //
///   // Pitch values are normalized, where each `1.0` shifts one octave, and `0.0` represents the
///   // middle A (A4) for a typical instrument definition. However, this is not a strict rule,
///   // since `pitch` and `intensity` can be interpreted in any desired way by a custom instrument.
///   BarelyNote* note = nullptr;
///   BarelyNote_Create(instrument, /*pitch=*/-1.0, /*intensity=*/0.25, &note);
///
///   // Set a control value.
///   BarelyControl* control = nullptr;
///   BarelyInstrument_GetControl(instrument, /*control_id=*/0, &control);
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
///   // Destroy the note.
///   BarelyNote_Destroy(note);
///
///   // Destroy.
///   BarelyInstrument_Destroy(instrument);
///   @endcode
///
/// - Effect:
///
///   @code{.cpp}
///   #include "barelymusician/effects/low_pass_effect.h"
///
///   // Create.
///   BarelyEffect* effect = nullptr;
///   BarelyEffect_Create(musician, BarelyLowPassEffect_GetDefinition(), /*frame_rate=*/48000,
///                       &effect);
///
///   // Set a control value.
///   BarelyControl* control = nullptr;
///   BarelyEffect_GetControl(effect, /*control_id=*/0, &control);
///   BarelyControl_SetValue(control, /*value=*/800.0);
///
///   // Process.
///   BarelyEffect_Process(effect, output_samples, output_channel_count, output_frame_count,
///                        timestamp);
///
///   // Destroy the effect.
///   BarelyEffect_Destroy(effect);
///   @endcode
///
/// - Performer:
///
///   @code{.cpp}
///   // Create.
///   BarelyPerformer* performer = nullptr;
///   BarelyPerformer_Create(musician, /*process_order=*/0, &performer);
///
///   // Create a task.
///   BarelyTask* task = nullptr;
///   BarelyTask_Create(performer, BarelyTaskDefinition{/*populate this*/}, /*position=*/0.0,
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
  int32_t control_id;

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
/// @param control_id Control identifier.
/// @param value Control value.
typedef void (*BarelyEffectDefinition_SetControlCallback)(void** state, int32_t control_id,
                                                          double value);

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
/// @param control_id Control identifier.
/// @param value Control value.
typedef void (*BarelyInstrumentDefinition_SetControlCallback)(void** state, int32_t control_id,
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
/// @param note_id Note identifier.
/// @param control_id Note control identifier.
/// @param value Note control value.
typedef void (*BarelyInstrumentDefinition_SetNoteControlCallback)(void** state, int32_t note_id,
                                                                  int32_t control_id, double value);

/// Instrument definition set note off callback signature.
///
/// @param state Pointer to instrument state.
/// @param note_id Note identifier.
typedef void (*BarelyInstrumentDefinition_SetNoteOffCallback)(void** state, int32_t note_id);

/// Instrument definition set note on callback signature.
///
/// @param state Pointer to instrument state.
/// @param note_id Note identifier.
/// @param pitch Note pitch.
/// @param intensity Note intensity.
typedef void (*BarelyInstrumentDefinition_SetNoteOnCallback)(void** state, int32_t note_id,
                                                             double pitch, double intensity);

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

/// Control alias.
typedef struct BarelyControl BarelyControl;

/// Effect alias.
typedef struct BarelyEffect BarelyEffect;

/// Instrument alias.
typedef struct BarelyInstrument BarelyInstrument;

/// Musician alias.
typedef struct BarelyMusician BarelyMusician;

/// Note alias.
typedef struct BarelyNote BarelyNote;

/// Performer alias.
typedef struct BarelyPerformer BarelyPerformer;

/// Task alias.
typedef struct BarelyTask BarelyTask;

/// Gets a control value.
///
/// @param control Pointer to control.
/// @param out_value Output value.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyControl_GetValue(const BarelyControl* control, double* out_value);

/// Resets a control value.
///
/// @param control Pointer to control.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyControl_ResetValue(BarelyControl* control);

/// Sets a control value.
///
/// @param control Pointer to control.
/// @param value Value.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyControl_SetValue(BarelyControl* control, double value);

/// Gets an effect control.
///
/// @param effect Pointer to effect.
/// @param control_id Control identifier.
/// @param out_control Output pointer to control.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyEffect_GetControl(BarelyEffect* effect, int32_t control_id,
                                           BarelyControl** out_control);

/// Creates a new effect.
///
/// @param musician Pointer to musician.
/// @param definition Effect definition.
/// @param frame_rate Frame rate in hertz.
/// @param out_effect Output pointer to effect.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyEffect_Create(BarelyMusician* musician, BarelyEffectDefinition definition,
                                       int32_t frame_rate, BarelyEffect** out_effect);

/// Destroys an effect.
///
/// @param effect Pointer to effect.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyEffect_Destroy(BarelyEffect* effect);

/// Processes effect output samples at timestamp.
/// @note This is *not* thread-safe during a corresponding
/// `BarelyMusician_DestroyEffect` call.
///
/// @param effect Pointer to effect.
/// @param output_samples Array of interleaved output samples.
/// @param output_channel_count Number of output channels.
/// @param output_frame_count Number of output frames.
/// @param timestamp Timestamp in seconds.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyEffect_Process(BarelyEffect* instrument, double* output_samples,
                                        int32_t output_channel_count, int32_t output_frame_count,
                                        double timestamp);

/// Sets effect data.
///
/// @param effect Pointer to effect.
/// @param data Pointer to immutable data.
/// @param size Data size in bytes.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyEffect_SetData(BarelyEffect* effect, const void* data, int32_t size);

/// Creates a new instrument.
///
/// @param musician Pointer to musician.
/// @param definition Instrument definition.
/// @param frame_rate Frame rate in hertz.
/// @param out_instrument Output pointer to instrument.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_Create(BarelyMusician* musician,
                                           BarelyInstrumentDefinition definition,
                                           int32_t frame_rate, BarelyInstrument** out_instrument);

/// Destroys an instrument.
///
/// @param instrument Pointer to instrument.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_Destroy(BarelyInstrument* instrument);

/// Gets an instrument control.
///
/// @param instrument Pointer to instrument.
/// @param control_id Control identifier.
/// @param out_control Output pointer to control.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_GetControl(BarelyInstrument* instrument, int32_t control_id,
                                               BarelyControl** out_control);

/// Processes instrument output samples at timestamp.
/// @note This is *not* thread-safe during a corresponding
/// `BarelyMusician_DestroyInstrument` call.
///
/// @param instrument Pointer to instrument.
/// @param output_samples Array of interleaved output samples.
/// @param output_channel_count Number of output channels.
/// @param output_frame_count Number of output frames.
/// @param timestamp Timestamp in seconds.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_Process(BarelyInstrument* instrument, double* output_samples,
                                            int32_t output_channel_count,
                                            int32_t output_frame_count, double timestamp);

/// Sets instrument data.
///
/// @param instrument Pointer to instrument.
/// @param data Pointer to immutable data.
/// @param size Data size in bytes.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_SetData(BarelyInstrument* instrument, const void* data,
                                            int32_t size);

/// Creates a new musician.
///
/// @param out_musician Output pointer to musician.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_Create(BarelyMusician** out_musician);

/// Destroys a musician.
///
/// @param musician Pointer to musician.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_Destroy(BarelyMusician* musician);

/// Gets the corresponding number of musician beats for a given number of
/// seconds.
///
/// @param musician Pointer to musician.
/// @param seconds Number of seconds.
/// @param out_beats Output number of musician beats.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_GetBeatsFromSeconds(const BarelyMusician* musician,
                                                      double seconds, double* out_beats);

/// Gets the corresponding number of seconds for a given number of musician
/// beats.
///
/// @param musician Pointer to musician.
/// @param beats Number of musician beats.
/// @param out_seconds Output number of seconds.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_GetSecondsFromBeats(const BarelyMusician* musician, double beats,
                                                      double* out_seconds);

/// Gets the tempo of a musician.
///
/// @param musician Pointer to musician.
/// @param out_tempo Output tempo in beats per minute.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_GetTempo(const BarelyMusician* musician, double* out_tempo);

/// Gets the timestamp of a musician.
///
/// @param musician Pointer to musician.
/// @param out_timestamp Output timestamp in seconds.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_GetTimestamp(const BarelyMusician* musician,
                                               double* out_timestamp);

/// Sets the tempo of a musician.
///
/// @param musician Pointer to musician.
/// @param tempo Tempo in beats per minute.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_SetTempo(BarelyMusician* musician, double tempo);

/// Updates a musician at timestamp.
///
/// @param musician Pointer to musician.
/// @param timestamp Timestamp in seconds.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_Update(BarelyMusician* musician, double timestamp);

/// Creates a note.
///
/// @param instrument Pointer to instrument.
/// @param pitch Note pitch.
/// @param intensity Note intensity.
/// @param out_note Output pointer to note.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyNote_Create(BarelyInstrument* instrument, double pitch, double intensity,
                                     BarelyNote** out_note);

/// Destroys a note.
///
/// @param note Pointer to note.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyNote_Destroy(BarelyNote* note);

/// Gets a note control.
///
/// @param note Pointer to note.
/// @param control_id Control identifier.
/// @param out_control Output pointer to control.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyNote_GetControl(BarelyNote* note, int32_t control_id,
                                         BarelyControl** out_control);

/// Gets a note intensity.
///
/// @param note Pointer to note.
/// @param control_id Control identifier.
/// @param out_intensity Output intensity.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyNote_GetIntensity(const BarelyNote* note, double* out_intensity);

/// Gets a note pitch.
///
/// @param note Pointer to note.
/// @param control_id Control identifier.
/// @param out_pitch Output pitch.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyNote_GetPitch(const BarelyNote* note, double* out_pitch);

/// Cancels all one-off performer tasks.
///
/// @param musician Pointer to musician.
/// @param out_performer Output pointer to performer.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_CancelAllOneOffTasks(BarelyPerformer* performer);

/// Creates a new performer.
///
/// @param musician Pointer to musician.
/// @param process_order Process order.
/// @param out_performer Output pointer to performer.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_Create(BarelyMusician* musician, int32_t process_order,
                                          BarelyPerformer** out_performer);

/// Destroys a performer.
///
/// @param performer Pointer to performer.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_Destroy(BarelyPerformer* performer);

/// Gets the loop begin position of a performer.
///
/// @param performer Pointer to performer.
/// @param out_loop_begin_position Output loop begin position in beats.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_GetLoopBeginPosition(const BarelyPerformer* performer,
                                                        double* out_loop_begin_position);

/// Gets the loop length of a performer.
///
/// @param performer Pointer to performer.
/// @param out_loop_length Output loop length.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_GetLoopLength(const BarelyPerformer* performer,
                                                 double* out_loop_length);

/// Gets the position of a performer.
///
/// @param performer Pointer to performer.
/// @param out_position Output position in beats.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_GetPosition(const BarelyPerformer* performer,
                                               double* out_position);

/// Gets whether a performer is looping or not.
///
/// @param performer Pointer to performer.
/// @param out_is_looping Output true if looping, false otherwise.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_IsLooping(const BarelyPerformer* performer,
                                             bool* out_is_looping);

/// Gets whether a performer is playing or not.
///
/// @param performer Pointer to performer.
/// @param out_is_playing Output true if playing, false otherwise.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_IsPlaying(const BarelyPerformer* performer,
                                             bool* out_is_playing);

/// Schedules a one-off task.
///
/// @param performer Pointer to performer.
/// @param definition Task definition.
/// @param position Task position in beats.
/// @param user_data Pointer to user data.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_ScheduleOneOffTask(BarelyPerformer* performer,
                                                      BarelyTaskDefinition definition,
                                                      double position, void* user_data);

/// Sets the loop begin position of a performer.
///
/// @param performer Pointer to performer.
/// @param loop_begin_position Loop begin position in beats.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_SetLoopBeginPosition(BarelyPerformer* performer,
                                                        double loop_begin_position);

/// Sets the loop length of a performer.
///
/// @param performer Pointer to performer.
/// @param loop_length Loop length in beats.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_SetLoopLength(BarelyPerformer* performer, double loop_length);

/// Sets whether a performer is looping or not.
///
/// @param performer Pointer to performer.
/// @param is_looping True if looping, false otherwise.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_SetLooping(BarelyPerformer* performer, bool is_looping);

/// Sets the position of a performer.
///
/// @param performer Pointer to performer.
/// @param position Position in beats.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_SetPosition(BarelyPerformer* performer, double position);

/// Starts a performer.
///
/// @param performer Pointer to performer.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_Start(BarelyPerformer* performer);

/// Stops a performer.
///
/// @param performer Pointer to performer.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_Stop(BarelyPerformer* performer);

/// Creates a new task.
///
/// @param performer Pointer to performer.
/// @param definition Task definition.
/// @param position Task position in beats.
/// @param user_data Pointer to user data.
/// @param out_task Output pointer to task.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyTask_Create(BarelyPerformer* performer, BarelyTaskDefinition definition,
                                     double position, void* user_data, BarelyTask** out_task);

/// Destroys a task.
///
/// @param task Pointer to task.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyTask_Destroy(BarelyTask* task);

/// Gets the position of a task.
///
/// @param task Pointer to task.
/// @param out_position Output position in beats.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyTask_GetPosition(const BarelyTask* task, double* out_position);

/// Sets the position of a task.
///
/// @param task Pointer to task.
/// @param position Position in beats.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyTask_SetPosition(BarelyTask* task, double position);

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
  /// @param control_id Identifier.
  /// @param default_value Default value.
  /// @param min_value Minimum value.
  /// @param max_value Maximum value.
  template <typename IdType, typename ValueType>
  ControlDefinition(IdType control_id, ValueType default_value,
                    ValueType min_value = std::numeric_limits<ValueType>::lowest(),
                    ValueType max_value = std::numeric_limits<ValueType>::max()) noexcept
      : ControlDefinition(BarelyControlDefinition{
            static_cast<int32_t>(control_id),
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

/// Pointer wrapper template.
template <typename RawType>
class PtrWrapper {
 public:
  /// Default constructor.
  PtrWrapper() noexcept = default;

  /// Constructs a new `PtrWrapper`.
  ///
  /// @param ptr Raw pointer.
  explicit PtrWrapper(RawType* ptr) noexcept : ptr_(ptr) { assert(ptr != nullptr); }

  /// Default destructor.
  ~PtrWrapper() noexcept = default;

  /// Copyable.
  PtrWrapper(const PtrWrapper& other) noexcept = default;
  PtrWrapper& operator=(const PtrWrapper& other) noexcept = default;

  /// Constructs a new `PtrWrapper` via move.
  ///
  /// @param other Other.
  PtrWrapper(PtrWrapper&& other) noexcept : ptr_(std::exchange(other.ptr_, nullptr)) {}

  /// Assigns `PtrWrapper` via move.
  ///
  /// @param other Other.
  /// @return Pointer wrapper.
  PtrWrapper& operator=(PtrWrapper&& other) noexcept {
    if (this != &other) {
      ptr_ = std::exchange(other.ptr_, nullptr);
    }
    return *this;
  }

  /// Returns the raw pointer.
  ///
  /// @return Raw pointer.
  operator RawType*() const noexcept { return ptr_; }

 private:
  // Raw pointer.
  RawType* ptr_ = nullptr;
};

/// Scoped wrapper template.
template <typename PtrWrapperType>
class ScopedWrapper : public PtrWrapperType {
 public:
  /// Constructs a new `ScopedWrapper`.
  template <typename... Args>
  explicit ScopedWrapper(Args&&... args) noexcept
      : ScopedWrapper(PtrWrapperType::Create(args...)) {}

  /// Constructs a new `ScopedWrapper` from an existing `PtrWrapper`.
  ///
  /// @param ptr_wrapper Pointer wrapper.
  explicit ScopedWrapper(PtrWrapperType ptr_wrapper) noexcept : PtrWrapperType(ptr_wrapper) {}

  /// Destroys `ScopedWrapper`.
  ~ScopedWrapper() noexcept { PtrWrapperType::Destroy(*this); }

  /// Non-copyable.
  ScopedWrapper(const ScopedWrapper& other) noexcept = delete;
  ScopedWrapper& operator=(const ScopedWrapper& other) noexcept = delete;

  /// Default move constructor.
  ScopedWrapper(ScopedWrapper&& other) noexcept = default;

  /// Assigns `ScopedWrapper` via move.
  ///
  /// @param other Other.
  /// @return Scoped wrapper.
  ScopedWrapper& operator=(ScopedWrapper&& other) noexcept {
    if (this != &other) {
      PtrWrapperType::Destroy(*this);
      PtrWrapperType::operator=(std::move(other));
    }
    return *this;
  }

  /// Releases the scope.
  ///
  /// @return Pointer wrapper.
  [[nodiscard]] PtrWrapperType Release() noexcept { return std::move(*this); }
};

/// Class that wraps a control pointer.
class ControlPtr : public PtrWrapper<BarelyControl> {
 public:
  /// Default constructor.
  ControlPtr() noexcept = default;

  /// Creates a new `ControlPtr` from a raw pointer.
  ///
  /// @param control Raw pointer to control.
  explicit ControlPtr(BarelyControl* control) noexcept : PtrWrapper(control) {}

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

/// Class that wraps a musician pointer.
class MusicianPtr : public PtrWrapper<BarelyMusician> {
 public:
  /// Creates a new `MusicianPtr`.
  ///
  /// @return Musician pointer.
  [[nodiscard]] static MusicianPtr Create() noexcept {
    BarelyMusician* musician = nullptr;
    [[maybe_unused]] const bool success = BarelyMusician_Create(&musician);
    assert(success);
    return MusicianPtr(musician);
  }

  /// Destroys a `MusicianPtr`.
  ///
  /// @param musician Musician.
  static void Destroy(MusicianPtr musician) noexcept { BarelyMusician_Destroy(musician); }

  /// Default constructor.
  MusicianPtr() noexcept = default;

  /// Constructs a new `Musician` from a raw pointer.
  ///
  /// @param musician Raw pointer to musician.
  explicit MusicianPtr(BarelyMusician* musician) noexcept : PtrWrapper(musician) {}

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

/// Class that wraps an effect pointer.
class EffectPtr : public PtrWrapper<BarelyEffect> {
 public:
  /// Creates a new `EffectPtr`.
  ///
  /// @param musician Musician pointer.
  /// @param definition Effect definition.
  /// @param frame_rate Frame rate in hertz.
  /// @return Effect pointer.
  [[nodiscard]] static EffectPtr Create(MusicianPtr musician, EffectDefinition definition,
                                        int frame_rate) noexcept {
    BarelyEffect* effect;
    [[maybe_unused]] const bool success =
        BarelyEffect_Create(musician, definition, frame_rate, &effect);
    assert(success);
    return EffectPtr(effect);
  }

  /// Destroys an `EffectPtr`.
  ///
  /// @param effect Effect pointer.
  static void Destroy(EffectPtr effect) noexcept { BarelyEffect_Destroy(effect); }

  /// Default constructor.
  EffectPtr() noexcept = default;

  /// Creates a new `EffectPtr` from a raw pointer.
  ///
  /// @param effect Raw pointer to effect.
  explicit EffectPtr(BarelyEffect* effect) noexcept : PtrWrapper(effect) {}

  /// Returns a control.
  ///
  /// @param control_id Control identifier.
  /// @return Control pointer.
  template <typename IdType>
  [[nodiscard]] ControlPtr GetControl(IdType control_id) const noexcept {
    static_assert(std::is_integral<IdType>::value || std::is_enum<IdType>::value,
                  "IdType is not supported");
    BarelyControl* control = nullptr;
    [[maybe_unused]] const bool success =
        BarelyEffect_GetControl(*this, static_cast<int>(control_id), &control);
    assert(success);
    return ControlPtr(control);
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

/// Class that wraps an instrument pointer.
class InstrumentPtr : public PtrWrapper<BarelyInstrument> {
 public:
  /// Creates a new `InstrumentPtr`.
  ///
  /// @param musician Musician pointer.
  /// @param definition Instrument definition.
  /// @param frame_rate Frame rate in hertz.
  /// @return Instrument pointer.
  [[nodiscard]] static InstrumentPtr Create(MusicianPtr musician, InstrumentDefinition definition,
                                            int frame_rate) noexcept {
    BarelyInstrument* instrument;
    [[maybe_unused]] const bool success =
        BarelyInstrument_Create(musician, definition, frame_rate, &instrument);
    assert(success);
    return InstrumentPtr(instrument);
  }

  /// Destroys an `InstrumentPtr`.
  ///
  /// @param instrument Instrument pointer.
  static void Destroy(InstrumentPtr instrument) noexcept { BarelyInstrument_Destroy(instrument); }

  /// Default constructor.
  InstrumentPtr() noexcept = default;

  // Creates a new `InstrumentPtr` from a raw pointer.
  ///
  /// @param instrument Raw pointer to instrument.
  explicit InstrumentPtr(BarelyInstrument* instrument) noexcept : PtrWrapper(instrument) {}

  /// Returns a control.
  ///
  /// @param control_id Control identifier.
  /// @return Control pointer.
  template <typename IdType>
  [[nodiscard]] ControlPtr GetControl(IdType control_id) const noexcept {
    static_assert(std::is_integral<IdType>::value || std::is_enum<IdType>::value,
                  "IdType is not supported");
    BarelyControl* control = nullptr;
    [[maybe_unused]] const bool success =
        BarelyInstrument_GetControl(*this, static_cast<int>(control_id), &control);
    assert(success);
    return ControlPtr(control);
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
};

/// Class that wraps a note pointer.
class NotePtr : public PtrWrapper<BarelyNote> {
 public:
  /// Creates a new `NotePtr`.
  ///
  /// @param instrument Instrument pointer.
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  /// @return Note pointer.
  static NotePtr Create(InstrumentPtr instrument, double pitch, double intensity = 1.0) noexcept {
    BarelyNote* note = nullptr;
    [[maybe_unused]] const bool success = BarelyNote_Create(instrument, pitch, intensity, &note);
    assert(success);
    return NotePtr(note);
  }

  /// Destroys a `NotePtr`.
  ///
  /// @param note Note.
  static void Destroy(NotePtr note) noexcept {
    [[maybe_unused]] const bool success = BarelyNote_Destroy(note);
    assert(note == nullptr || success);
  }

  /// Default constructor.
  NotePtr() noexcept = default;

  /// Constructs a new `NotePtr` from a raw pointer.
  ///
  /// @param note Raw pointer to note.
  explicit NotePtr(BarelyNote* note) noexcept : PtrWrapper(note) {}

  /// Returns a control.
  ///
  /// @param control_id Control identifier.
  /// @return Control pointer.
  template <typename IdType>
  [[nodiscard]] ControlPtr GetControl(IdType control_id) const noexcept {
    static_assert(std::is_integral<IdType>::value || std::is_enum<IdType>::value,
                  "IdType is not supported");
    BarelyControl* control = nullptr;
    [[maybe_unused]] const bool success =
        BarelyNote_GetControl(*this, static_cast<int>(control_id), &control);
    assert(success);
    return ControlPtr(control);
  }

  /// Returns the intensity.
  ///
  /// @return Intensity.
  [[nodiscard]] double GetIntensity() const noexcept {
    double intensity = 0.0;
    [[maybe_unused]] const bool success = BarelyNote_GetIntensity(*this, &intensity);
    assert(success);
    return intensity;
  }

  /// Returns the pitch.
  ///
  /// @return Pitch.
  [[nodiscard]] double GetPitch() const noexcept {
    double pitch = 0.0;
    [[maybe_unused]] const bool success = BarelyNote_GetPitch(*this, &pitch);
    assert(success);
    return pitch;
  }
};

/// Class that wraps a performer pointer.
class PerformerPtr : public PtrWrapper<BarelyPerformer> {
 public:
  /// Creates a new `PerformerPtr`.
  ///
  /// @param musician Musician pointer.
  /// @param process_order Process order.
  /// @return Performer pointer.
  [[nodiscard]] static PerformerPtr Create(MusicianPtr musician, int process_order = 0) noexcept {
    BarelyPerformer* performer;
    [[maybe_unused]] const bool success =
        BarelyPerformer_Create(musician, process_order, &performer);
    assert(success);
    return PerformerPtr(performer);
  }

  /// Destroys a `PerformerPtr`.
  ///
  /// @param performer Performer pointer.
  static void Destroy(PerformerPtr performer) noexcept { BarelyPerformer_Destroy(performer); }

  /// Default constructor.
  PerformerPtr() noexcept = default;

  /// Creates a new `PerformerPtr` from a raw pointer.
  ///
  /// @param performer Raw pointer to performer.
  explicit PerformerPtr(BarelyPerformer* performer) noexcept : PtrWrapper(performer) {}

  /// Cancels all one-off tasks.
  void CancelAllOneOffTasks() noexcept {
    [[maybe_unused]] const bool success = BarelyPerformer_CancelAllOneOffTasks(*this);
    assert(success);
  }

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

/// Class that wraps a task pointer.
class TaskPtr : public PtrWrapper<BarelyTask> {
 public:
  /// Creates a new `TaskPtr`.
  ///
  /// @param performer Performer pointer.
  /// @param definition Task definition.
  /// @param position Task position in beats.
  /// @param user_data Pointer to user data.
  /// @return Task pointer.
  [[nodiscard]] static TaskPtr Create(PerformerPtr performer, TaskDefinition definition,
                                      double position, void* user_data = nullptr) noexcept {
    BarelyTask* task;
    [[maybe_unused]] const bool success =
        BarelyTask_Create(performer, definition, position, user_data, &task);
    assert(success);
    return TaskPtr(task);
  }

  /// Creates a new `TaskPtr` with a callback.
  ///
  /// @param performer Performer pointer.
  /// @param callback Task callback.
  /// @param position Task position in beats.
  /// @return Task pointer.
  [[nodiscard]] static TaskPtr Create(PerformerPtr performer, TaskDefinition::Callback callback,
                                      double position) noexcept {
    return Create(performer, TaskDefinition::WithCallback(), position,
                  static_cast<void*>(&callback));
  }

  /// Destroys a `TaskPtr`.
  ///
  /// @param task Task pointer.
  static void Destroy(TaskPtr task) noexcept { BarelyTask_Destroy(task); }

  /// Default constructor.
  TaskPtr() noexcept = default;

  /// Constructs a new `TaskPtr` from a raw pointer.
  ///
  /// @param task Raw pointer to task.
  explicit TaskPtr(BarelyTask* task) noexcept : PtrWrapper(task) {}

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

/// Scoped effect alias.
using Effect = ScopedWrapper<EffectPtr>;

/// Scoped instrument alias.
using Instrument = ScopedWrapper<InstrumentPtr>;

/// Scoped musician alias.
using Musician = ScopedWrapper<MusicianPtr>;

/// Scoped note alias.
using Note = ScopedWrapper<NotePtr>;

/// Scoped performer alias.
using Performer = ScopedWrapper<PerformerPtr>;

/// Scoped task alias.
using Task = ScopedWrapper<TaskPtr>;

}  // namespace barely

#endif  // __cplusplus

#endif  // BARELYMUSICIAN_BARELYMUSICIAN_H_
