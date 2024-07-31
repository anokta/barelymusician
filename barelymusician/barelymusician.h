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
///   // Create.
///   barely::Instrument instrument =
///       musician.CreateInstrument<barely::SynthInstrument>(/*frame_rate=*/48000);
///
///   // Create a note.
///   //
///   // Pitch values are normalized, where each `1.0` shifts one octave, and `0.0` represents the
///   // middle A (A4) for a typical instrument definition. However, this is not a strict rule,
///   // since `pitch` and `intensity` can be interpreted in any desired way by a custom instrument.
///   barely::Note note = instrument.CreateNote(/*pitch=*/-1.0, /*intensity=*/0.25);
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
///   barely::Effect effect = musician.CreateEffect<barely::LowPassEffect>(/*frame_rate=*/48000);
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
///   barely::Performer performer = musician.CreatePerformer(/*process_order=*/0);
///
///   // Create a task.
///   barely::Task task = performer.CreateTask([]() { /*populate this*/ }, /*position=*/0.0);
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
///   BarelyMusician_CreateInstrument(musician, BarelySynthInstrument_GetDefinition(),
///                                   /*frame_rate=*/48000, &instrument);
///
///   // Create a note.
///   //
///   // Pitch values are normalized, where each `1.0` shifts one octave, and `0.0` represents the
///   // middle A (A4) for a typical instrument definition. However, this is not a strict rule,
///   // since `pitch` and `intensity` can be interpreted in any desired way by a custom instrument.
///   BarelyNote* note = nullptr;
///   BarelyInstrument_CreateNote(instrument, /*pitch=*/-1.0, /*intensity=*/0.25, &note);
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
///   BarelyEffect* effect = nullptr;
///   BarelyMusician_CreateEffect(musician, BarelyLowPassEffect_GetDefinition(),
///                               /*frame_rate=*/48000, &effect);
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
///   BarelyMusician_DestroyEffect(musician, effect);
///   @endcode
///
/// - Performer:
///
///   @code{.cpp}
///   // Create.
///   BarelyPerformer* performer = nullptr;
///   BarelyMusician_CreatePerformer(musician, /*process_order=*/0, &performer);
///
///   // Create a task.
///   BarelyTask* task = nullptr;
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

/// Creates an instrument note.
///
/// @param instrument Pointer to instrument.
/// @param pitch Note pitch.
/// @param intensity Note intensity.
/// @param out_note Output pointer to note.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_CreateNote(BarelyInstrument* instrument, double pitch,
                                               double intensity, BarelyNote** out_note);

/// Destroys an instrument note.
///
/// @param instrument Pointer to instrument.
/// @param note Pointer to note.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_DestroyNote(BarelyInstrument* instrument, BarelyNote* note);

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

/// Creates a new effect.
///
/// @param musician Pointer to musician.
/// @param definition Effect definition.
/// @param frame_rate Frame rate in hertz.
/// @param out_effect Output pointer to effect.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_CreateEffect(BarelyMusician* musician,
                                               BarelyEffectDefinition definition,
                                               int32_t frame_rate, BarelyEffect** out_effect);

/// Creates a new instrument.
///
/// @param musician Pointer to musician.
/// @param definition Instrument definition.
/// @param frame_rate Frame rate in hertz.
/// @param out_instrument Output pointer to instrument.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_CreateInstrument(BarelyMusician* musician,
                                                   BarelyInstrumentDefinition definition,
                                                   int32_t frame_rate,
                                                   BarelyInstrument** out_instrument);

/// Creates a new performer.
///
/// @param musician Pointer to musician.
/// @param process_order Process order.
/// @param out_performer Output pointer to performer.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_CreatePerformer(BarelyMusician* musician, int32_t process_order,
                                                  BarelyPerformer** out_performer);

/// Destroys a musician.
///
/// @param musician Pointer to musician.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_Destroy(BarelyMusician* musician);

/// Destroys an effect.
///
/// @param musician Pointer to musician.
/// @param effect Pointer to effect.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_DestroyEffect(BarelyMusician* musician, BarelyEffect* effect);

/// Destroys an instrument.
///
/// @param musician Pointer to musician.
/// @param instrument Pointer to instrument.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_DestroyInstrument(BarelyMusician* musician,
                                                    BarelyInstrument* instrument);

/// Destroys a performer.
///
/// @param musician Pointer to musician.
/// @param performer Pointer to performer.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_DestroyPerformer(BarelyMusician* musician,
                                                   BarelyPerformer* performer);

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

/// Creates a new performer task.
///
/// @param performer Pointer to performer.
/// @param definition Task definition.
/// @param position Task position in beats.
/// @param user_data Pointer to user data.
/// @param out_task Output pointer to task.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_CreateTask(BarelyPerformer* performer,
                                              BarelyTaskDefinition definition, double position,
                                              void* user_data, BarelyTask** out_task);

/// Destroys a performer task.
///
/// @param performer Pointer to performer.
/// @param task Pointer to task.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_DestroyTask(BarelyPerformer* performer, BarelyTask* task);

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

/// Wrapper template.
template <typename RawType>
class Wrapper {
 public:
  /// Default constructor.
  Wrapper() noexcept = default;

  /// Constructs a new `Wrapper`.
  ///
  /// @param ptr Raw pointer.
  explicit Wrapper(RawType* ptr) noexcept : ptr_(ptr) { assert(ptr != nullptr); }

  /// Returns the raw pointer.
  ///
  /// @return Raw pointer.
  operator RawType*() const noexcept { return ptr_; }

 private:
  // Raw pointer.
  RawType* ptr_ = nullptr;
};

/// Class that wraps a control.
class Control : public Wrapper<BarelyControl> {
 public:
  /// Default constructor.
  Control() noexcept = default;

  /// Creates a new `Control` from a raw pointer.
  ///
  /// @param control Raw pointer to control.
  explicit Control(BarelyControl* control) noexcept : Wrapper(control) {}

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
class Effect : public Wrapper<BarelyEffect> {
 public:
  /// Default constructor.
  Effect() noexcept = default;

  /// Creates a new `Effect` from a raw pointer.
  ///
  /// @param effect Raw pointer to effect.
  explicit Effect(BarelyEffect* effect) noexcept : Wrapper(effect) {}

  /// Returns a control.
  ///
  /// @param control_id Control identifier.
  /// @return Control.
  template <typename IdType>
  [[nodiscard]] Control GetControl(IdType control_id) const noexcept {
    static_assert(std::is_integral<IdType>::value || std::is_enum<IdType>::value,
                  "IdType is not supported");
    BarelyControl* control = nullptr;
    [[maybe_unused]] const bool success =
        BarelyEffect_GetControl(*this, static_cast<int>(control_id), &control);
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

/// Class that wraps a note.
class Note : public Wrapper<BarelyNote> {
 public:
  /// Default constructor.
  Note() noexcept = default;

  /// Creates a new `Note` from a raw pointer.
  ///
  /// @param note Raw pointer to note.
  explicit Note(BarelyNote* note) noexcept : Wrapper(note) {}

  /// Returns a control.
  ///
  /// @param control_id Control identifier.
  /// @return Control.
  template <typename IdType>
  [[nodiscard]] Control GetControl(IdType control_id) const noexcept {
    static_assert(std::is_integral<IdType>::value || std::is_enum<IdType>::value,
                  "IdType is not supported");
    BarelyControl* control = nullptr;
    [[maybe_unused]] const bool success =
        BarelyNote_GetControl(*this, static_cast<int>(control_id), &control);
    assert(success);
    return Control(control);
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

/// Class that wraps an instrument.
class Instrument : public Wrapper<BarelyInstrument> {
 public:
  /// Default constructor.
  Instrument() noexcept = default;

  // Creates a new `Instrument` from a raw pointer.
  ///
  /// @param instrument Raw pointer to instrument.
  explicit Instrument(BarelyInstrument* instrument) noexcept : Wrapper(instrument) {}

  /// Creates a note.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  /// @return note Note.
  Note CreateNote(double pitch, double intensity = 1.0) noexcept {
    BarelyNote* note = nullptr;
    [[maybe_unused]] const bool success =
        BarelyInstrument_CreateNote(*this, pitch, intensity, &note);
    assert(success);
    return Note(note);
  }

  /// Destroys a note.
  ///
  /// @param note Note.
  void DestroyNote(Note note) noexcept {
    [[maybe_unused]] const bool success = BarelyInstrument_DestroyNote(*this, note);
    assert(success);
  }

  /// Returns a control.
  ///
  /// @param control_id Control identifier.
  /// @return Control.
  template <typename IdType>
  [[nodiscard]] Control GetControl(IdType control_id) const noexcept {
    static_assert(std::is_integral<IdType>::value || std::is_enum<IdType>::value,
                  "IdType is not supported");
    BarelyControl* control = nullptr;
    [[maybe_unused]] const bool success =
        BarelyInstrument_GetControl(*this, static_cast<int>(control_id), &control);
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

/// Class that wraps a task.
class Task : public Wrapper<BarelyTask> {
 public:
  /// Default constructor.
  Task() noexcept = default;

  /// Creates a new `Task` from a raw pointer.
  ///
  /// @param task Raw pointer to task.
  explicit Task(BarelyTask* task) noexcept : Wrapper(task) {}

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
class Performer : public Wrapper<BarelyPerformer> {
 public:
  /// Default constructor.
  Performer() noexcept = default;

  /// Creates a new `Performer` from a raw pointer.
  ///
  /// @param performer Raw pointer to performer.
  explicit Performer(BarelyPerformer* performer) noexcept : Wrapper(performer) {}

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
    BarelyTask* task;
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
class Musician : public Wrapper<BarelyMusician> {
 public:
  /// Default constructor.
  Musician() noexcept = default;

  /// Creates a new `Musician` from a raw pointer.
  ///
  /// @param musician Raw pointer to musician.
  explicit Musician(BarelyMusician* musician) noexcept : Wrapper(musician) {}

  /// Creates a new musician.
  ///
  /// @return Musician.
  [[nodiscard]] static Musician Create() noexcept {
    BarelyMusician* musician = nullptr;
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
    BarelyEffect* effect;
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
    BarelyInstrument* instrument;
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
    BarelyPerformer* performer;
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

  /// Creates a new `ScopedMusician` from a raw pointer to musician.
  ///
  /// @param musician Raw pointer to musician.
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

namespace std {

/// Effect hash specialization.
template <>
struct hash<::barely::Effect> {
  std::size_t operator()(const barely::Effect& effect) const noexcept {
    return std::hash<BarelyEffect*>()(effect);
  }
};

/// Instrument hash specialization.
template <>
struct hash<::barely::Instrument> {
  std::size_t operator()(const barely::Instrument& instrument) const noexcept {
    return std::hash<BarelyInstrument*>()(instrument);
  }
};

/// Musician hash specialization.
template <>
struct hash<::barely::Musician> {
  std::size_t operator()(const barely::Musician& musician) const noexcept {
    return std::hash<BarelyMusician*>()(musician);
  }
};

/// Note hash specialization.
template <>
struct hash<::barely::Note> {
  std::size_t operator()(const barely::Note& note) const noexcept {
    return std::hash<BarelyNote*>()(note);
  }
};

/// Performer hash specialization.
template <>
struct hash<::barely::Performer> {
  std::size_t operator()(const barely::Note& note) const noexcept {
    return std::hash<BarelyNote*>()(note);
  }
};

/// Task hash specialization.
template <>
struct hash<::barely::Task> {
  std::size_t operator()(const barely::Task& task) const noexcept {
    return std::hash<BarelyTask*>()(task);
  }
};

}  // namespace std

#endif  // __cplusplus

#endif  // BARELYMUSICIAN_BARELYMUSICIAN_H_
