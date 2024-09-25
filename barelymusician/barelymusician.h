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
///   Musician musician(/*frame_rate=*/48000);
///
///   // Set the tempo.
///   musician.SetTempo(/*tempo=*/124.0);
///
///   // Update the timestamp.
///   //
///   // Timestamp updates must happen before processing effects and instruments with their
///   // respective timestamps. Otherwise, such `Process` calls will be *late* to receive the
///   // relevant state changes. To compensate for this, `Update` should typically be called from a
///   // main thread update callback with an additional "lookahead" to avoid potential thread
///   // synchronization issues that could arise in real-time audio applications.
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
///   barely::Instrument instrument(musician, barely::SynthInstrument::GetDefinition());
///
///   // Set a note on.
///   //
///   // Note values for pitched instruments typically represent the frequencies of the
///   // corresponding notes. However, this is not a strict rule, as the `note` and `intensity`
///   // values can be interpreted in any desired way by a custom instrument.
///   instrument.SetNoteOn(/*note=*/220.0, /*intensity=*/0.25);
///
///   // Check if the note is on.
///   const bool is_note_on = instrument.IsNoteOn(/*note=*/220.0);
///
///   // Set a control value.
///   instrument.SetControl(barely::SynthInstrument::Control::kGain, /*value=*/0.5);
///
///   // Process.
///   //
///   // Instruments process raw PCM audio samples in a synchronous call. Therefore, `Process`
///   // should typically be called from an audio thread process callback in real-time audio
///   // applications.
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
///   barely::Effect effect(musician, barely::LowPassEffect::GetDefinition());
///
///   // Set a control value.
///   effect.SetControl(barely::LowPassEffect::Control::kCutoffFrequency, /*value=*/1000.0);
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
///   BarelyMusician_Create(/*frame_rate=*/48000, &musician);
///
///   // Set the tempo.
///   BarelyMusician_SetTempo(musician, /*tempo=*/124.0);
///
///   // Update the timestamp.
///   //
///   // Timestamp updates must occur before processing effects and instruments with their
///   // respective timestamps. Otherwise, such `Process` calls will be *late* to receive the
///   // relevant state changes. To compensate for this, `Update` should typically be called from a
///   // main thread update callback with an additional "lookahead" to avoid potential thread
///   // synchronization issues that could arise in real-time audio applications.
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
///   BarelyInstrument_Create(musician, BarelySynthInstrument_GetDefinition(), &instrument);
///
///   // Set a note on.
///   //
///   // Note values for pitched instruments typically represent the frequencies of the
///   // corresponding notes. However, this is not a strict rule, as the `note` and `intensity`
///   // values can be interpreted in any desired way by a custom instrument.
///   BarelyInstrument_SetNoteOn(instrument, /*note=*/220.0, /*intensity=*/0.25);
///
///   // Check if the note is on.
///   bool is_note_on = false;
///   BarelyInstrument_IsNoteOn(instrument, /*note=*/220.0, &is_note_on);
///
///   // Set a control value.
///   BarelyInstrument_SetControl(instrument, /*id=*/0, /*value=*/0.5);
///
///   // Process.
///   //
///   // Instruments process raw PCM audio samples in a synchronous call. Therefore, `Process`
///   // should typically be called from an audio thread process callback in real-time audio
///   // applications.
///   double output_samples[2 * 1024];
///   int output_channel_count = 2;
///   int output_frame_count = 1024;
///   double timestamp = 0.0;
///   BarelyInstrument_Process(instrument, output_samples, output_channel_count, output_frame_count,
///                            timestamp);
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
///   BarelyEffect_Create(musician, BarelyLowPassEffect_GetDefinition(), &effect);
///
///   // Set a control value.
///   BarelyEffect_SetControl(effect, /*id=*/0, /*value=*/1000.0);
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

/// Tuning definition alias.
typedef struct BarelyTuningDefinition BarelyTuningDefinition;

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

/// Control event definition create callback signature.
///
/// @param state Pointer to control event state.
/// @param user_data Pointer to user data.
typedef void (*BarelyControlEventDefinition_CreateCallback)(void** state, void* user_data);

/// Control event definition destroy callback signature.
///
/// @param state Pointer to control event state.
typedef void (*BarelyControlEventDefinition_DestroyCallback)(void** state);

/// Control event definition process callback signature.
///
/// @param state Pointer to control event state.
/// @param id Control identifier.
/// @param value Control value.
typedef void (*BarelyControlEventDefinition_ProcessCallback)(void** state, int32_t id,
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
/// @param note Note value.
/// @param id Note control identifier.
/// @param value Note control value.
typedef void (*BarelyInstrumentDefinition_SetNoteControlCallback)(void** state, double note,
                                                                  int32_t id, double value);

/// Instrument definition set note off callback signature.
///
/// @param state Pointer to instrument state.
/// @param note Note value.
typedef void (*BarelyInstrumentDefinition_SetNoteOffCallback)(void** state, double note);

/// Instrument definition set note on callback signature.
///
/// @param state Pointer to instrument state.
/// @param note Note value.
/// @param intensity Note intensity.
typedef void (*BarelyInstrumentDefinition_SetNoteOnCallback)(void** state, double note,
                                                             double intensity);

/// Instrument definition set tuning callback signature.
///
/// @param state Pointer to instrument state.
/// @param definition Pointer to tuning definition.
typedef void (*BarelyInstrumentDefinition_SetTuningCallback)(
    void** state, const BarelyTuningDefinition* definition);

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

  /// Set tuning callback.
  BarelyInstrumentDefinition_SetTuningCallback set_tuning_callback;

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
typedef void (*BarelyNoteControlEventDefinition_CreateCallback)(void** state, void* user_data);

/// Note control event definition destroy callback signature.
///
/// @param state Pointer to note control state.
typedef void (*BarelyNoteControlEventDefinition_DestroyCallback)(void** state);

/// Note control event definition process callback signature.
///
/// @param state Pointer to note control event state.
/// @param note Note value.
/// @param id Note control identifier.
/// @param value Note control value.
typedef void (*BarelyNoteControlEventDefinition_ProcessCallback)(void** state, double note,
                                                                 int32_t id, double value);

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
typedef void (*BarelyNoteOffEventDefinition_CreateCallback)(void** state, void* user_data);

/// Note off event definition destroy callback signature.
///
/// @param state Pointer to note off state.
typedef void (*BarelyNoteOffEventDefinition_DestroyCallback)(void** state);

/// Note off event definition process callback signature.
///
/// @param state Pointer to note off event state.
/// @param note Note value.
typedef void (*BarelyNoteOffEventDefinition_ProcessCallback)(void** state, double note);

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
/// @param note Note value.
/// @param intensity Note intensity.
typedef void (*BarelyNoteOnEventDefinition_ProcessCallback)(void** state, double note,
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

/// Definition of a musical tuning system.
#pragma pack(push, 1)
typedef struct BarelyTuningDefinition {
  /// Array of pitch ratios relative to, but excluding, the root pitch.
  const double* pitch_ratios;

  /// Number of pitch ratios.
  int32_t pitch_ratio_count;

  /// Root frequency in hertz.
  double root_frequency;

  /// Root pitch corresponding to the root frequency.
  int32_t root_pitch;
} BarelyTuningDefinition;
#pragma pack(pop)

/// Effect alias.
typedef struct BarelyEffect BarelyEffect;

/// Instrument alias.
typedef struct BarelyInstrument BarelyInstrument;

/// Musician alias.
typedef struct BarelyMusician BarelyMusician;

/// Performer alias.
typedef struct BarelyPerformer BarelyPerformer;

/// Task alias.
typedef struct BarelyTask BarelyTask;

/// Creates a new effect.
///
/// @param musician Pointer to musician.
/// @param definition Effect definition.
/// @param out_effect Output pointer to effect.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyEffect_Create(BarelyMusician* musician, BarelyEffectDefinition definition,
                                       BarelyEffect** out_effect);

/// Destroys an effect.
///
/// @param effect Pointer to effect.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyEffect_Destroy(BarelyEffect* effect);

/// Gets an effect control value.
///
/// @param effect Pointer to effect.
/// @param id Control identifier.
/// @param out_control Output pointer to control.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyEffect_GetControl(const BarelyEffect* effect, int32_t id,
                                           double* out_value);

/// Processes effect output samples at timestamp.
/// @note This is *not* thread-safe during a corresponding `BarelyEffect_Destroy` call.
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

/// Resets all control values of an effect.
///
/// @param effect Pointer to effect.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyEffect_ResetAllControls(BarelyEffect* effect);

/// Resets an effect control value.
///
/// @param control Pointer to control.
/// @param id Control identifier.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyEffect_ResetControl(BarelyEffect* effect, int32_t id);

/// Sets an effect control value.
///
/// @param effect Pointer to effect.
/// @param id Control identifier.
/// @param value Control value.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyEffect_SetControl(BarelyEffect* effect, int32_t id, double value);

/// Sets the control event of an effect.
///
/// @param effect Pointer to effect.
/// @param definition Control event definition.
/// @param user_data Pointer to user data.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyEffect_SetControlEvent(BarelyEffect* effect,
                                                BarelyControlEventDefinition definition,
                                                void* user_data);

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
/// @param out_instrument Output pointer to instrument.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_Create(BarelyMusician* musician,
                                           BarelyInstrumentDefinition definition,
                                           BarelyInstrument** out_instrument);

/// Destroys an instrument.
///
/// @param instrument Pointer to instrument.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_Destroy(BarelyInstrument* instrument);

/// Gets an instrument control value.
///
/// @param instrument Pointer to instrument.
/// @param id Control identifier.
/// @param out_value Output control value.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_GetControl(const BarelyInstrument* instrument, int32_t id,
                                               double* out_value);

/// Gets an instrument note control value.
///
/// @param instrument Pointer to instrument.
/// @param note Note value.
/// @param id Note control identifier.
/// @param out_value Output note control value.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_GetNoteControl(const BarelyInstrument* instrument, double note,
                                                   int32_t id, double* out_value);

/// Gets whether an instrument note is on or not.
///
/// @param instrument Instrument handle.
/// @param note Note value.
/// @param out_is_note_on Output true if on, false otherwise.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_IsNoteOn(const BarelyInstrument* instrument, double note,
                                             bool* out_is_note_on);

/// Processes instrument output samples at timestamp.
/// @note This is *not* thread-safe during a corresponding `BarelyInstrument_Destroy` call.
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

/// Resets all control values of an instrument.
///
/// @param instrument Pointer to instrument.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_ResetAllControls(BarelyInstrument* instrument);

/// Resets all control values of an instrument note.
///
/// @param instrument Pointer to instrument.
/// @param note Note value.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_ResetAllNoteControls(BarelyInstrument* instrument, double note);

/// Resets an instrument control value.
///
/// @param instrument Pointer to instrument.
/// @param id Control identifier.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_ResetControl(BarelyInstrument* instrument, int32_t id);

/// Resets an instrument note control value.
///
/// @param instrument Pointer to instrument.
/// @param note Note value.
/// @param id Control identifier.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_ResetNoteControl(BarelyInstrument* instrument, double note,
                                                     int32_t id);

/// Sets all instrument notes off.
///
/// @param instrument Pointer to instrument.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_SetAllNotesOff(BarelyInstrument* instrument);

/// Sets an instrument control value.
///
/// @param instrument Pointer to instrument.
/// @param id Control identifier.
/// @param value Control value.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_SetControl(BarelyInstrument* instrument, int32_t id,
                                               double value);

/// Sets the control event of an instrument.
///
/// @param instrument Pointer to instrument.
/// @param definition Control event definition.
/// @param user_data Pointer to user data.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_SetControlEvent(BarelyInstrument* instrument,
                                                    BarelyControlEventDefinition definition,
                                                    void* user_data);

/// Sets instrument data.
///
/// @param instrument Pointer to instrument.
/// @param data Pointer to immutable data.
/// @param size Data size in bytes.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_SetData(BarelyInstrument* instrument, const void* data,
                                            int32_t size);

/// Sets an instrument note control value.
///
/// @param instrument Pointer to instrument.
/// @param note Note value.
/// @param id Note control identifier.
/// @param value Note control value.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_SetNoteControl(BarelyInstrument* instrument, double note,
                                                   int32_t id, double value);

/// Sets the note control event of an instrument.
///
/// @param instrument Pointer to instrument.
/// @param definition Note control event definition.
/// @param user_data Pointer to user data.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_SetNoteControlEvent(BarelyInstrument* instrument,
                                                        BarelyNoteControlEventDefinition definition,
                                                        void* user_data);

/// Sets an instrument note off.
///
/// @param instrument Pointer to instrument.
/// @param note Note value.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_SetNoteOff(BarelyInstrument* instrument, double note);

/// Sets the note off event of an instrument.
///
/// @param instrument Pointer to instrument.
/// @param definition Note off event definition.
/// @param user_data Pointer to user data.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_SetNoteOffEvent(BarelyInstrument* instrument,
                                                    BarelyNoteOffEventDefinition definition,
                                                    void* user_data);

/// Sets an instrument note on.
///
/// @param instrument Pointer to instrument.
/// @param note Note value.
/// @param intensity Note intensity.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_SetNoteOn(BarelyInstrument* instrument, double note,
                                              double intensity);

/// Sets the note on event of an instrument.
///
/// @param instrument Pointer to instrument.
/// @param definition Note on event definition.
/// @param user_data Pointer to user data.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_SetNoteOnEvent(BarelyInstrument* instrument,
                                                   BarelyNoteOnEventDefinition definition,
                                                   void* user_data);

/// Sets an instrument tuning.
///
/// @param instrument Pointer to instrument.
/// @param definition Tuning definition.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_SetTuning(BarelyInstrument* instrument,
                                              const BarelyTuningDefinition* definition);

/// Creates a new musician.
///
/// @param frame_rate Frame rate in hertz.
/// @param out_musician Output pointer to musician.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_Create(int32_t frame_rate, BarelyMusician** out_musician);

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

/// Gets the corresponding frequency for a given pitch.
///
/// @param definition Pointer to tuning definition.
/// @param pitch Pitch.
/// @param out_frequency Output frequency.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyTuningDefinition_GetFrequency(const BarelyTuningDefinition* definition,
                                                       int32_t pitch, double* out_frequency);

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

/// Tuning definition.
struct TuningDefinition;

/// Control definition.
struct ControlDefinition : public BarelyControlDefinition {
  /// Default constructor.
  constexpr ControlDefinition() noexcept = default;

  /// Constructs a new `ControlDefinition`.
  ///
  /// @param id Identifier.
  /// @param default_value Default value.
  /// @param min_value Minimum value.
  /// @param max_value Maximum value.
  template <typename IdType, typename ValueType>
  constexpr ControlDefinition(IdType id, ValueType default_value,
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
  constexpr ControlDefinition(BarelyControlDefinition definition) noexcept
      : BarelyControlDefinition{definition} {
    assert(default_value >= min_value && default_value <= max_value);
  }
};

/// Control event definition.
struct ControlEventDefinition : public BarelyControlEventDefinition {
  /// Callback signature.
  ///
  /// @param id Control identifier.
  /// @param value Control value.
  using Callback = std::function<void(int id, double value)>;

  /// Create callback signature.
  using CreateCallback = BarelyControlEventDefinition_CreateCallback;

  /// Destroy callback signature.
  using DestroyCallback = BarelyControlEventDefinition_DestroyCallback;

  /// Process callback signature.
  using ProcessCallback = BarelyControlEventDefinition_ProcessCallback;

  /// Returns a new `ControlEventDefinition` with `Callback`.
  ///
  /// @return Control event definition.
  static constexpr ControlEventDefinition WithCallback() noexcept {
    return ControlEventDefinition(
        [](void** state, void* user_data) noexcept {
          *state = new (std::nothrow) Callback(std::move(*static_cast<Callback*>(user_data)));
          assert(*state);
        },
        [](void** state) noexcept { delete static_cast<Callback*>(*state); },
        [](void** state, int32_t id, double value) noexcept {
          if (const auto& callback = *static_cast<Callback*>(*state); callback) {
            callback(id, value);
          }
        });
  }

  /// Constructs a new `ControlEventDefinition`.
  ///
  /// @param create_callback Create callback.
  /// @param destroy_callback Destroy callback.
  /// @param process_callback Process callback.
  explicit constexpr ControlEventDefinition(CreateCallback create_callback,
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
  constexpr ControlEventDefinition(BarelyControlEventDefinition definition) noexcept
      : BarelyControlEventDefinition{definition} {}
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
  explicit constexpr EffectDefinition(
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
  constexpr EffectDefinition(BarelyEffectDefinition definition) noexcept
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

  /// Set tuning callback signature.
  using SetTuningCallback = BarelyInstrumentDefinition_SetTuningCallback;

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
  explicit constexpr InstrumentDefinition(
      CreateCallback create_callback, DestroyCallback destroy_callback,
      ProcessCallback process_callback, SetControlCallback set_control_callback,
      SetDataCallback set_data_callback, SetNoteControlCallback set_note_control_callback,
      SetNoteOffCallback set_note_off_callback, SetNoteOnCallback set_note_on_callback,
      SetTuningCallback set_tuning_callback, std::span<const ControlDefinition> control_definitions,
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
            set_tuning_callback,
            control_definitions.data(),
            static_cast<int>(control_definitions.size()),
            note_control_definitions.data(),
            static_cast<int>(note_control_definitions.size()),
        }) {}

  /// Constructs a new `InstrumentDefinition` from a raw type.
  ///
  /// @param definition Raw instrument definition.
  // NOLINTNEXTLINE(google-explicit-constructor)
  constexpr InstrumentDefinition(BarelyInstrumentDefinition definition) noexcept
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
  /// @param note Note value.
  /// @param id Note control identifier.
  /// @param value Note control value.
  using Callback = std::function<void(double note, int id, double value)>;

  /// Create callback signature.
  using CreateCallback = BarelyNoteControlEventDefinition_CreateCallback;

  /// Destroy callback signature.
  using DestroyCallback = BarelyNoteControlEventDefinition_DestroyCallback;

  /// Process callback signature.
  using ProcessCallback = BarelyNoteControlEventDefinition_ProcessCallback;

  /// Returns a new `NoteControlEventDefinition` with `Callback`.
  ///
  /// @return Note control event definition.
  static constexpr NoteControlEventDefinition WithCallback() noexcept {
    return NoteControlEventDefinition(
        [](void** state, void* user_data) noexcept {
          *state = new (std::nothrow) Callback(std::move(*static_cast<Callback*>(user_data)));
          assert(*state);
        },
        [](void** state) noexcept { delete static_cast<Callback*>(*state); },
        [](void** state, double note, int32_t id, double value) noexcept {
          if (const auto& callback = *static_cast<Callback*>(*state); callback) {
            callback(note, id, value);
          }
        });
  }

  /// Constructs a new `NoteControlEventDefinition`.
  ///
  /// @param create_callback Create callback.
  /// @param destroy_callback Destroy callback.
  /// @param process_callback Process callback.
  explicit constexpr NoteControlEventDefinition(CreateCallback create_callback,
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
  constexpr NoteControlEventDefinition(BarelyNoteControlEventDefinition definition) noexcept
      : BarelyNoteControlEventDefinition{definition} {}
};

/// Note off event definition.
struct NoteOffEventDefinition : public BarelyNoteOffEventDefinition {
  /// Callback signature.
  ///
  /// @param note Note value.
  using Callback = std::function<void(double note)>;

  /// Create callback signature.
  using CreateCallback = BarelyNoteOffEventDefinition_CreateCallback;

  /// Destroy callback signature.
  using DestroyCallback = BarelyNoteOffEventDefinition_DestroyCallback;

  /// Process callback signature.
  using ProcessCallback = BarelyNoteOffEventDefinition_ProcessCallback;

  /// Returns a new `NoteOffEventDefinition` with `Callback`.
  ///
  /// @return Note off event definition.
  static constexpr NoteOffEventDefinition WithCallback() noexcept {
    return NoteOffEventDefinition(
        [](void** state, void* user_data) noexcept {
          *state = new (std::nothrow) Callback(std::move(*static_cast<Callback*>(user_data)));
          assert(*state);
        },
        [](void** state) noexcept { delete static_cast<Callback*>(*state); },
        [](void** state, double note) noexcept {
          if (const auto& callback = *static_cast<Callback*>(*state); callback) {
            callback(note);
          }
        });
  }

  /// Constructs a new `NoteOffEventDefinition`.
  ///
  /// @param create_callback Create callback.
  /// @param destroy_callback Destroy callback.
  /// @param process_callback Process callback.
  explicit constexpr NoteOffEventDefinition(CreateCallback create_callback,
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
  constexpr NoteOffEventDefinition(BarelyNoteOffEventDefinition definition) noexcept
      : BarelyNoteOffEventDefinition{definition} {}
};

/// Note on event definition.
struct NoteOnEventDefinition : public BarelyNoteOnEventDefinition {
  /// Callback signature.
  ///
  /// @param note Note value.
  /// @param intensity Note intensity.
  using Callback = std::function<void(double note, double intensity)>;

  /// Create callback signature.
  using CreateCallback = BarelyNoteOnEventDefinition_CreateCallback;

  /// Destroy callback signature.
  using DestroyCallback = BarelyNoteOnEventDefinition_DestroyCallback;

  /// Process callback signature.
  using ProcessCallback = BarelyNoteOnEventDefinition_ProcessCallback;

  /// Returns a new `NoteOnEventDefinition` with `Callback`.
  ///
  /// @return Note on event definition.
  static constexpr NoteOnEventDefinition WithCallback() noexcept {
    return NoteOnEventDefinition(
        [](void** state, void* user_data) noexcept {
          *state = new (std::nothrow) Callback(std::move(*static_cast<Callback*>(user_data)));
          assert(*state);
        },
        [](void** state) noexcept { delete static_cast<Callback*>(*state); },
        [](void** state, double note, double intensity) noexcept {
          if (const auto& callback = *static_cast<Callback*>(*state); callback) {
            callback(note, intensity);
          }
        });
  }

  /// Constructs a new `NoteOnEventDefinition`.
  ///
  /// @param create_callback Create callback.
  /// @param destroy_callback Destroy callback.
  /// @param process_callback Process callback.
  explicit constexpr NoteOnEventDefinition(CreateCallback create_callback,
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
  constexpr NoteOnEventDefinition(BarelyNoteOnEventDefinition definition) noexcept
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
  static constexpr TaskDefinition WithCallback() noexcept {
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
  explicit constexpr TaskDefinition(CreateCallback create_callback,
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
  constexpr TaskDefinition(BarelyTaskDefinition definition) noexcept
      : BarelyTaskDefinition{definition} {}
};

/// Pointer wrapper template.
template <typename RawType>
class PtrWrapper {
 public:
  /// Default constructor.
  constexpr PtrWrapper() noexcept = default;

  /// Constructs a new `PtrWrapper`.
  ///
  /// @param ptr Raw pointer.
  explicit constexpr PtrWrapper(RawType* ptr) noexcept : ptr_(ptr) { assert(ptr != nullptr); }

  /// Default destructor.
  constexpr ~PtrWrapper() noexcept = default;

  /// Copyable.
  constexpr PtrWrapper(const PtrWrapper& other) noexcept = default;
  constexpr PtrWrapper& operator=(const PtrWrapper& other) noexcept = default;

  /// Constructs a new `PtrWrapper` via move.
  ///
  /// @param other Other.
  constexpr PtrWrapper(PtrWrapper&& other) noexcept : ptr_(std::exchange(other.ptr_, nullptr)) {}

  /// Assigns `PtrWrapper` via move.
  ///
  /// @param other Other.
  /// @return Pointer wrapper.
  constexpr PtrWrapper& operator=(PtrWrapper&& other) noexcept {
    if (this != &other) {
      ptr_ = std::exchange(other.ptr_, nullptr);
    }
    return *this;
  }

  /// Returns the raw pointer.
  ///
  /// @return Raw pointer.
  constexpr operator RawType*() const noexcept { return ptr_; }

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

/// Class that wraps a musician pointer.
class MusicianPtr : public PtrWrapper<BarelyMusician> {
 public:
  /// Creates a new `MusicianPtr`.
  ///
  /// @param frame_rate Frame rate in hertz.
  /// @return Musician pointer.
  [[nodiscard]] static MusicianPtr Create(int frame_rate) noexcept {
    BarelyMusician* musician = nullptr;
    [[maybe_unused]] const bool success = BarelyMusician_Create(frame_rate, &musician);
    assert(success);
    return MusicianPtr(musician);
  }

  /// Destroys a `MusicianPtr`.
  ///
  /// @param musician Musician.
  static void Destroy(MusicianPtr musician) noexcept { BarelyMusician_Destroy(musician); }

  /// Default constructor.
  constexpr MusicianPtr() noexcept = default;

  /// Constructs a new `Musician` from a raw pointer.
  ///
  /// @param musician Raw pointer to musician.
  explicit constexpr MusicianPtr(BarelyMusician* musician) noexcept : PtrWrapper(musician) {}

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
  /// @return Effect pointer.
  [[nodiscard]] static EffectPtr Create(MusicianPtr musician,
                                        EffectDefinition definition) noexcept {
    BarelyEffect* effect;
    [[maybe_unused]] const bool success = BarelyEffect_Create(musician, definition, &effect);
    assert(success);
    return EffectPtr(effect);
  }

  /// Destroys an `EffectPtr`.
  ///
  /// @param effect Effect pointer.
  static void Destroy(EffectPtr effect) noexcept { BarelyEffect_Destroy(effect); }

  /// Default constructor.
  constexpr EffectPtr() noexcept = default;

  /// Creates a new `EffectPtr` from a raw pointer.
  ///
  /// @param effect Raw pointer to effect.
  explicit constexpr EffectPtr(BarelyEffect* effect) noexcept : PtrWrapper(effect) {}

  /// Returns a control value.
  ///
  /// @param id Control identifier.
  /// @return Control value.
  template <typename IdType, typename ValueType>
  [[nodiscard]] ValueType GetControl(IdType id) const noexcept {
    static_assert(std::is_integral<IdType>::value || std::is_enum<IdType>::value,
                  "IdType is not supported");
    static_assert(std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
                  "ValueType is not supported");
    double value = 0.0;
    [[maybe_unused]] const bool success =
        BarelyEffect_GetControl(*this, static_cast<int32_t>(id), &value);
    assert(success);
    return static_cast<ValueType>(value);
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

  void ResetAllControls() noexcept {
    [[maybe_unused]] const bool success = BarelyEffect_ResetAllControls(*this);
    assert(success);
  }

  /// Resets a control value.
  ///
  /// @param id Control identifier.
  template <typename IdType>
  void ResetControl(IdType id) noexcept {
    static_assert(std::is_integral<IdType>::value || std::is_enum<IdType>::value,
                  "IdType is not supported");
    [[maybe_unused]] const bool success =
        BarelyEffect_ResetControl(*this, static_cast<int32_t>(id));
    assert(success);
  }

  /// Sets a control value.
  ///
  /// @param id Control identifier.
  /// @param value Control value.
  template <typename IdType, typename ValueType>
  void SetControl(IdType id, ValueType value) noexcept {
    static_assert(std::is_integral<IdType>::value || std::is_enum<IdType>::value,
                  "IdType is not supported");
    static_assert(std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
                  "ValueType is not supported");
    [[maybe_unused]] const bool success =
        BarelyEffect_SetControl(*this, static_cast<int32_t>(id), static_cast<double>(value));
    assert(success);
  }

  /// Sets the control event.
  ///
  /// @param definition Control event definition.
  /// @param user_data Pointer to user data.
  void SetControlEvent(ControlEventDefinition definition, void* user_data = nullptr) noexcept {
    [[maybe_unused]] const bool success =
        BarelyEffect_SetControlEvent(*this, definition, user_data);
    assert(success);
  }

  /// Sets the control event with a callback.
  ///
  /// @param callback Control event callback.
  void SetControlEvent(ControlEventDefinition::Callback callback) noexcept {
    SetControlEvent(ControlEventDefinition::WithCallback(), static_cast<void*>(&callback));
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
  /// @return Instrument pointer.
  [[nodiscard]] static InstrumentPtr Create(MusicianPtr musician,
                                            InstrumentDefinition definition) noexcept {
    BarelyInstrument* instrument;
    [[maybe_unused]] const bool success =
        BarelyInstrument_Create(musician, definition, &instrument);
    assert(success);
    return InstrumentPtr(instrument);
  }

  /// Destroys an `InstrumentPtr`.
  ///
  /// @param instrument Instrument pointer.
  static void Destroy(InstrumentPtr instrument) noexcept { BarelyInstrument_Destroy(instrument); }

  /// Default constructor.
  constexpr InstrumentPtr() noexcept = default;

  /// Creates a new `InstrumentPtr` from a raw pointer.
  ///
  /// @param instrument Raw pointer to instrument.
  explicit constexpr InstrumentPtr(BarelyInstrument* instrument) noexcept
      : PtrWrapper(instrument) {}

  /// Returns a control value.
  ///
  /// @param id Control identifier.
  /// @return Control value.
  template <typename IdType, typename ValueType>
  [[nodiscard]] ValueType GetControl(IdType id) const noexcept {
    static_assert(std::is_integral<IdType>::value || std::is_enum<IdType>::value,
                  "IdType is not supported");
    static_assert(std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
                  "ValueType is not supported");
    double value = 0.0;
    [[maybe_unused]] const bool success =
        BarelyInstrument_GetControl(*this, static_cast<int32_t>(id), &value);
    assert(success);
    return static_cast<ValueType>(value);
  }

  /// Returns a control value.
  ///
  /// @param note Note value.
  /// @param id Note control identifier.
  /// @return Note control value.
  template <typename IdType, typename ValueType>
  [[nodiscard]] ValueType GetNoteControl(double note, IdType id) const noexcept {
    static_assert(std::is_integral<IdType>::value || std::is_enum<IdType>::value,
                  "IdType is not supported");
    static_assert(std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
                  "ValueType is not supported");
    double value = 0.0;
    [[maybe_unused]] const bool success =
        BarelyInstrument_GetNoteControl(*this, note, static_cast<int32_t>(id), &value);
    assert(success);
    return static_cast<ValueType>(value);
  }

  /// Returns whether a note is on or not.
  ///
  /// @param note Note value.
  /// @return True if active, false otherwise.
  [[nodiscard]] bool IsNoteOn(double note) const noexcept {
    bool is_note_on = false;
    [[maybe_unused]] const bool success = BarelyInstrument_IsNoteOn(*this, note, &is_note_on);
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

  /// Resets all control values.
  void ResetAllControls() noexcept {
    [[maybe_unused]] const bool success = BarelyInstrument_ResetAllControls(*this);
    assert(success);
  }

  /// Resets all note control values.
  ///
  /// @param note Note value.
  void ResetAllNoteControls(double note) noexcept {
    [[maybe_unused]] const bool success = BarelyInstrument_ResetAllNoteControls(*this, note);
    assert(success);
  }

  /// Resets a control value.
  ///
  /// @param id Control identifier.
  template <typename IdType>
  void ResetControl(IdType id) noexcept {
    static_assert(std::is_integral<IdType>::value || std::is_enum<IdType>::value,
                  "IdType is not supported");
    [[maybe_unused]] const bool success =
        BarelyInstrument_ResetControl(*this, static_cast<int32_t>(id));
    assert(success);
  }

  /// Resets a note control value.
  ///
  /// @param note Note value.
  /// @param id Control identifier.
  template <typename IdType>
  void ResetNoteControl(double note, IdType id) noexcept {
    static_assert(std::is_integral<IdType>::value || std::is_enum<IdType>::value,
                  "IdType is not supported");
    [[maybe_unused]] const bool success =
        BarelyInstrument_ResetNoteControl(*this, note, static_cast<int32_t>(id));
    assert(success);
  }

  /// Sets all notes off.
  void SetAllNotesOff() noexcept {
    [[maybe_unused]] const bool success = BarelyInstrument_SetAllNotesOff(*this);
    assert(success);
  }

  /// Sets a control value.
  ///
  /// @param id Control identifier.
  /// @param value Control value.
  template <typename IdType, typename ValueType>
  void SetControl(IdType id, ValueType value) noexcept {
    static_assert(std::is_integral<IdType>::value || std::is_enum<IdType>::value,
                  "IdType is not supported");
    static_assert(std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
                  "ValueType is not supported");
    [[maybe_unused]] const bool success =
        BarelyInstrument_SetControl(*this, static_cast<int32_t>(id), static_cast<double>(value));
    assert(success);
  }

  /// Sets the control event.
  ///
  /// @param definition Control event definition.
  /// @param user_data Pointer to user data.
  void SetControlEvent(ControlEventDefinition definition, void* user_data = nullptr) noexcept {
    [[maybe_unused]] const bool success =
        BarelyInstrument_SetControlEvent(*this, definition, user_data);
    assert(success);
  }

  /// Sets the control event with a callback.
  ///
  /// @param callback Control event callback.
  void SetControlEvent(ControlEventDefinition::Callback callback) noexcept {
    SetControlEvent(ControlEventDefinition::WithCallback(), static_cast<void*>(&callback));
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

  /// Sets a control value.
  ///
  /// @param note Note value.
  /// @param id Control identifier.
  /// @param value Control value.
  template <typename IdType, typename ValueType>
  void SetNoteControl(double note, IdType id, ValueType value) noexcept {
    static_assert(std::is_integral<IdType>::value || std::is_enum<IdType>::value,
                  "IdType is not supported");
    static_assert(std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
                  "ValueType is not supported");
    [[maybe_unused]] const bool success = BarelyInstrument_SetNoteControl(
        *this, note, static_cast<int32_t>(id), static_cast<double>(value));
    assert(success);
  }

  /// Sets the note control event.
  ///
  /// @param definition Note control event definition.
  /// @param user_data Pointer to user data.
  void SetNoteControlEvent(NoteControlEventDefinition definition,
                           void* user_data = nullptr) noexcept {
    [[maybe_unused]] const bool success =
        BarelyInstrument_SetNoteControlEvent(*this, definition, user_data);
    assert(success);
  }

  /// Sets the note control event with a callback.
  ///
  /// @param callback Note control event callback.
  void SetNoteControlEvent(NoteControlEventDefinition::Callback callback) noexcept {
    SetNoteControlEvent(NoteControlEventDefinition::WithCallback(), static_cast<void*>(&callback));
  }

  /// Sets a note off.
  ///
  /// @param note Note value.
  void SetNoteOff(double note) noexcept {
    [[maybe_unused]] const bool success = BarelyInstrument_SetNoteOff(*this, note);
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
  /// @param note Note value.
  /// @param intensity Note intensity.
  void SetNoteOn(double note, double intensity = 1.0) noexcept {
    [[maybe_unused]] const bool success = BarelyInstrument_SetNoteOn(*this, note, intensity);
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

  /// Sets the tuning.
  ///
  /// @param definition Tuning definition.
  void SetTuning(const TuningDefinition* definition) noexcept {
    [[maybe_unused]] const bool success = BarelyInstrument_SetTuning(
        *this, reinterpret_cast<const BarelyTuningDefinition*>(definition));
    assert(success);
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
  constexpr PerformerPtr() noexcept = default;

  /// Creates a new `PerformerPtr` from a raw pointer.
  ///
  /// @param performer Raw pointer to performer.
  explicit constexpr PerformerPtr(BarelyPerformer* performer) noexcept : PtrWrapper(performer) {}

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
  constexpr TaskPtr() noexcept = default;

  /// Constructs a new `TaskPtr` from a raw pointer.
  ///
  /// @param task Raw pointer to task.
  explicit constexpr TaskPtr(BarelyTask* task) noexcept : PtrWrapper(task) {}

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

/// Definition of a musical tuning system.
struct TuningDefinition : public BarelyTuningDefinition {
 public:
  /// Default constructor.
  constexpr TuningDefinition() noexcept = default;

  /// Constructs a new `TuningDefinition`.
  ///
  /// @param pitch_ratios Span of pitch ratios.
  /// @param root_frequency Root frequency.
  /// @param root_pitch Root pitch.
  constexpr TuningDefinition(std::span<const double> pitch_ratios, double root_frequency,
                             int root_pitch) noexcept
      : TuningDefinition(
            BarelyTuningDefinition{pitch_ratios.data(), static_cast<int32_t>(pitch_ratios.size()),
                                   root_frequency, static_cast<int32_t>(root_pitch)}) {}

  /// Constructs a new `TuningDefinition` from a raw type.
  ///
  /// @param definition Raw tuning definition.
  // NOLINTNEXTLINE(google-explicit-BarelyTuningDefinition)
  constexpr TuningDefinition(BarelyTuningDefinition definition) noexcept
      : BarelyTuningDefinition{definition} {
    assert(definition.pitch_ratios != nullptr);
    assert(definition.pitch_ratio_count > 0);
  }

  /// Returns the corresponding frequency for a given pitch.
  ///
  /// @param pitch Pitch.
  /// @return Frequency.
  [[nodiscard]] double GetFrequency(int pitch) const noexcept {
    double frequency = 0.0;
    [[maybe_unused]] const bool success =
        BarelyTuningDefinition_GetFrequency(this, static_cast<int32_t>(pitch), &frequency);
    return frequency;
  }

  /// Returns the number of pitches in the tuning system.
  ///
  /// @return Number of pitches.
  [[nodiscard]] constexpr int GetPitchCount() const noexcept {
    return static_cast<int>(pitch_ratio_count);
  }
};

/// Scoped effect alias.
using Effect = ScopedWrapper<EffectPtr>;

/// Scoped instrument alias.
using Instrument = ScopedWrapper<InstrumentPtr>;

/// Scoped musician alias.
using Musician = ScopedWrapper<MusicianPtr>;

/// Scoped performer alias.
using Performer = ScopedWrapper<PerformerPtr>;

/// Scoped task alias.
using Task = ScopedWrapper<TaskPtr>;

}  // namespace barely

#endif  // __cplusplus

#endif  // BARELYMUSICIAN_BARELYMUSICIAN_H_
