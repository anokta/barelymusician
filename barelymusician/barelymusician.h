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
///   Musician musician(/*frame_rate=*/48000, /*reference_frequency=/440.0);
///
///   // Set the tempo.
///   musician.SetTempo(/*tempo=*/124.0);
///
///   // Update the timestamp.
///   //
///   // Timestamp updates must happen before processing instruments with their respective
///   // timestamps. Otherwise, such `Process` calls will be *late* to receive the relevant state
///   // changes. To compensate for this, `Update` should typically be called from a main thread
///   // update callback with an additional "lookahead" to avoid potential thread synchronization
///   // issues that could arise in real-time audio applications.
///   double timestamp = 1.0;
///   musician.Update(timestamp);
///   @endcode
///
/// - Instrument:
///
///   @code{.cpp}
///   // Create.
///   barely::Instrument instrument(musician);
///
///   // Set a note on.
///   //
//    // Note pitch is centered around the reference frequency, and measured in octaves. Fractional
///   // values adjust the frequency logarithmically to maintain perceived pitch intervals in each
///   // octave.
///   const double c3_pitch = -1.0;
///   instrument.SetNoteOn(c3_pitch, /*intensity=*/0.25);
///
///   // Check if the note is on.
///   const bool is_note_on = instrument.IsNoteOn(c3_pitch);
///
///   // Set a control value.
///   instrument.SetControl(barely::InstrumentControl::kGain, /*value=*/0.5);
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
///   BarelyMusician_Create(/*frame_rate=*/48000, /*reference_frequency=/440.0, &musician);
///
///   // Set the tempo.
///   BarelyMusician_SetTempo(musician, /*tempo=*/124.0);
///
///   // Update the timestamp.
///   //
///   // Timestamp updates must happen before processing instruments with their respective
///   // timestamps. Otherwise, such `Process` calls will be *late* to receive the relevant state
///   // changes. To compensate for this, `Update` should typically be called from a main thread
///   // update callback with an additional "lookahead" to avoid potential thread synchronization
///   // issues that could arise in real-time audio applications.
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
///   // Create.
///   BarelyInstrument* instrument = nullptr;
///   BarelyInstrument_Create(musician, &instrument);
///
///   // Set a note on.
///   //
//    // Note pitch is centered around the reference frequency, and measured in octaves. Fractional
///   // values adjust the frequency logarithmically to maintain perceived pitch intervals in each
///   // octave.
///   double c3_pitch = -1.0;
///   BarelyInstrument_SetNoteOn(instrument, c3_pitch, /*intensity=*/0.25);
///
///   // Check if the note is on.
///   bool is_note_on = false;
///   BarelyInstrument_IsNoteOn(instrument, c3_pitch, &is_note_on);
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
/// @param pitch Note pitch.
/// @param id Note control identifier.
/// @param value Note control value.
typedef void (*BarelyNoteControlEventDefinition_ProcessCallback)(void** state, double pitch,
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

/// Instrument alias.
typedef struct BarelyInstrument BarelyInstrument;

/// Musician alias.
typedef struct BarelyMusician BarelyMusician;

/// Performer alias.
typedef struct BarelyPerformer BarelyPerformer;

/// Task alias.
typedef struct BarelyTask BarelyTask;

/// Creates a new instrument.
///
/// @param musician Pointer to musician.
/// @param out_instrument Output pointer to instrument.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_Create(BarelyMusician* musician,
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
/// @param pitch Note pitch.
/// @param id Note control identifier.
/// @param out_value Output note control value.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_GetNoteControl(const BarelyInstrument* instrument, double pitch,
                                                   int32_t id, double* out_value);

/// Gets whether an instrument note is on or not.
///
/// @param instrument Instrument handle.
/// @param pitch Note pitch.
/// @param out_is_note_on Output true if on, false otherwise.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_IsNoteOn(const BarelyInstrument* instrument, double pitch,
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
/// @param pitch Note pitch.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_ResetAllNoteControls(BarelyInstrument* instrument,
                                                         double pitch);

/// Resets an instrument control value.
///
/// @param instrument Pointer to instrument.
/// @param id Control identifier.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_ResetControl(BarelyInstrument* instrument, int32_t id);

/// Resets an instrument note control value.
///
/// @param instrument Pointer to instrument.
/// @param pitch Note pitch.
/// @param id Control identifier.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_ResetNoteControl(BarelyInstrument* instrument, double pitch,
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
/// @param pitch Note pitch.
/// @param id Note control identifier.
/// @param value Note control value.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_SetNoteControl(BarelyInstrument* instrument, double pitch,
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
/// @param pitch Note pitch.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_SetNoteOff(BarelyInstrument* instrument, double pitch);

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
/// @param pitch Note pitch.
/// @param intensity Note intensity.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_SetNoteOn(BarelyInstrument* instrument, double pitch,
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

/// Creates a new musician.
///
/// @param frame_rate Frame rate in hertz.
/// @param reference_frequency Reference frequency in hertz.
/// @param out_musician Output pointer to musician.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_Create(int32_t frame_rate, double reference_frequency,
                                         BarelyMusician** out_musician);

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

/// Note control event definition.
struct NoteControlEventDefinition : public BarelyNoteControlEventDefinition {
  /// Callback signature.
  ///
  /// @param pitch Note pitch.
  /// @param id Note control identifier.
  /// @param value Note control value.
  using Callback = std::function<void(double pitch, int id, double value)>;

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
        [](void** state, double pitch, int32_t id, double value) noexcept {
          if (const auto& callback = *static_cast<Callback*>(*state); callback) {
            callback(pitch, id, value);
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
  static constexpr NoteOffEventDefinition WithCallback() noexcept {
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
  static constexpr NoteOnEventDefinition WithCallback() noexcept {
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
  /// @param reference_frequency Reference frequency in hertz.
  /// @return Musician pointer.
  // TODO(#139): Move the C4 constant somewhere else - likely back to internal with a setter here.
  [[nodiscard]] static MusicianPtr Create(int frame_rate,
                                          double reference_frequency = 261.625565301) noexcept {
    BarelyMusician* musician = nullptr;
    [[maybe_unused]] const bool success =
        BarelyMusician_Create(frame_rate, reference_frequency, &musician);
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

/// Instrument control enum.
enum class InstrumentControl : int32_t {
  /// Gain.
  kGain = 0,
  /// Number of voices.
  kVoiceCount,
  /// Oscillator on.
  // TODO(#139): This could be replaced by a mix value between the oscillator and sample playback.
  kOscillatorOn,
  /// Oscillator type.
  kOscillatorType,
  /// Sample player loop.
  // TODO(#139): This could be replaced by `SamplePlaybackMode` with sustained and looped modes.
  kSamplePlayerLoop,
  /// Envelope attack.
  kAttack,
  /// Envelope decay.
  kDecay,
  /// Envelope sustain.
  kSustain,
  /// Envelope release.
  kRelease,
  /// Pitch shift.
  kPitchShift,
  /// Number of controls.
  kCount,
};

/// Class that wraps an instrument pointer.
class InstrumentPtr : public PtrWrapper<BarelyInstrument> {
 public:
  /// Creates a new `InstrumentPtr`.
  ///
  /// @param musician Musician pointer.
  /// @return Instrument pointer.
  [[nodiscard]] static InstrumentPtr Create(MusicianPtr musician) noexcept {
    BarelyInstrument* instrument;
    [[maybe_unused]] const bool success = BarelyInstrument_Create(musician, &instrument);
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
  /// @param pitch Note pitch.
  /// @param id Note control identifier.
  /// @return Note control value.
  template <typename IdType, typename ValueType>
  [[nodiscard]] ValueType GetNoteControl(double pitch, IdType id) const noexcept {
    static_assert(std::is_integral<IdType>::value || std::is_enum<IdType>::value,
                  "IdType is not supported");
    static_assert(std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
                  "ValueType is not supported");
    double value = 0.0;
    [[maybe_unused]] const bool success =
        BarelyInstrument_GetNoteControl(*this, pitch, static_cast<int32_t>(id), &value);
    assert(success);
    return static_cast<ValueType>(value);
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

  /// Resets all control values.
  void ResetAllControls() noexcept {
    [[maybe_unused]] const bool success = BarelyInstrument_ResetAllControls(*this);
    assert(success);
  }

  /// Resets all note control values.
  ///
  /// @param pitch Note pitch.
  void ResetAllNoteControls(double pitch) noexcept {
    [[maybe_unused]] const bool success = BarelyInstrument_ResetAllNoteControls(*this, pitch);
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
  /// @param pitch Note pitch.
  /// @param id Control identifier.
  template <typename IdType>
  void ResetNoteControl(double pitch, IdType id) noexcept {
    static_assert(std::is_integral<IdType>::value || std::is_enum<IdType>::value,
                  "IdType is not supported");
    [[maybe_unused]] const bool success =
        BarelyInstrument_ResetNoteControl(*this, pitch, static_cast<int32_t>(id));
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
  /// @param pitch Note pitch.
  /// @param id Control identifier.
  /// @param value Control value.
  template <typename IdType, typename ValueType>
  void SetNoteControl(double pitch, IdType id, ValueType value) noexcept {
    static_assert(std::is_integral<IdType>::value || std::is_enum<IdType>::value,
                  "IdType is not supported");
    static_assert(std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
                  "ValueType is not supported");
    [[maybe_unused]] const bool success = BarelyInstrument_SetNoteControl(
        *this, pitch, static_cast<int32_t>(id), static_cast<double>(value));
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
