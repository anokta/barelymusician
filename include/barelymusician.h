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
///   #include "barelymusician.h"
///
///   // Create.
///   barely::Musician musician(/*frame_rate=*/48000);
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
///   // Add.
///   auto instrument = musician.AddInstrument();
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
///   instrument.SetControl(barely::ControlType::kGain, /*value=*/0.5);
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
///   // Add.
///   auto performer = musician.AddPerformer(/*process_order=*/0);
///
///   // Add a task.
///   auto task = performer.AddTask([]() { /*populate this*/ }, /*position=*/0.0);
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
///   #include "barelymusician.h"
///
///   // Create.
///   BarelyMusicianHandle musician = nullptr;
///   BarelyMusician_Create(/*frame_rate=*/48000, &musician);
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
///   // Add.
///   BarelyInstrumentHandle instrument = nullptr;
///   BarelyMusician_AddInstrument(musician, &instrument);
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
///   BarelyInstrument_SetControl(instrument, BarelyControlType_kGain, /*value=*/0.5);
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
///   // Remove.
///   BarelyMusician_RemoveInstrument(instrument);
///   @endcode
///
/// - Performer:
///
///   @code{.cpp}
///   // Add.
///   BarelyPerformerHandle performer = nullptr;
///   BarelyPerformer_Create(musician, /*process_order=*/0, &performer);
///
///   // Add a task.
///   BarelyTaskHandle task = nullptr;
///   BarelyPerformer_AddTask(performer, BarelyTaskEvent{/*populate this*/}, /*position=*/0.0,
///                           &task);
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
///   // Remove the task.
///   BarelyPerformer_RemoveTask(task);
///
///   // Remove.
///   BarelyMusician_RemoveTask(performer);
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

/// Control type enum alias.
typedef int32_t BarelyControlType;

/// Control type enum values.
enum BarelyControlType_Values {
  /// Gain.
  BarelyControlType_kGain = 0,
  /// Number of voices.
  BarelyControlType_kVoiceCount,
  /// Oscillator shape.
  BarelyControlType_kOscillatorShape,
  /// Sample playback mode.
  BarelyControlType_kSamplePlaybackMode,
  /// Envelope attack.
  BarelyControlType_kAttack,
  /// Envelope decay.
  BarelyControlType_kDecay,
  /// Envelope sustain.
  BarelyControlType_kSustain,
  /// Envelope release.
  BarelyControlType_kRelease,
  /// Pitch shift.
  BarelyControlType_kPitchShift,
  /// Retrigger.
  BarelyControlType_kRetrigger,
  /// Number of control types.
  BarelyControlType_kCount,
};

/// Note control type enum alias.
typedef int32_t BarelyNoteControlType;

/// Note control type enum values.
enum BarelyNoteControlType_Values {
  /// Pitch shift.
  BarelyNoteControlType_kPitchShift = 0,
  /// Number of note control types.
  BarelyNoteControlType_kCount,
};

/// Oscillator shape enum alias.
typedef int32_t BarelyOscillatorShape;

/// Oscillator shape enum value.
enum BarelyOscillatorShape_Values {
  /// None.
  BarelyOscillatorShape_kNone = 0,
  /// Sine wave.
  BarelyOscillatorShape_kSine,
  /// Sawtooth wave.
  BarelyOscillatorShape_kSaw,
  /// Square wave.
  BarelyOscillatorShape_kSquare,
  /// White noise.
  BarelyOscillatorShape_kNoise,
  /// Number of oscillator shapes.
  BarelyOscillatorShape_kCount,
};

/// Sample playback mode enum alias.
typedef int32_t BarelySamplePlaybackMode;

/// Sample playback mode enum values.
enum BarelySamplePlaybackMode_Values {
  /// None.
  BarelySamplePlaybackMode_kNone = 0,
  /// Once.
  BarelySamplePlaybackMode_kOnce,
  /// Sustain.
  BarelySamplePlaybackMode_kSustain,
  /// Loop.
  BarelySamplePlaybackMode_kLoop,
  /// Number of playback modes.
  BarelySamplePlaybackMode_kCount,
};

/// Slice of sample data.
typedef struct BarelySampleDataSlice {
  /// Root note pitch.
  double root_pitch;

  /// Sampling rate in hertz.
  int32_t sample_rate;

  /// Array of mono samples.
  const double* samples;

  /// Number of mono samples.
  int32_t sample_count;
} BarelySampleDataSlice;

/// Note off event create callback signature.
///
/// @param state Pointer to note off event state.
/// @param user_data Pointer to user data.
typedef void (*BarelyNoteOffEvent_CreateCallback)(void** state, void* user_data);

/// Note off event destroy callback signature.
///
/// @param state Pointer to note off event state.
typedef void (*BarelyNoteOffEvent_DestroyCallback)(void** state);

/// Note off event process callback signature.
///
/// @param state Pointer to note off event state.
/// @param pitch Note pitch.
typedef void (*BarelyNoteOffEvent_ProcessCallback)(void** state, double pitch);

/// Note off event.
typedef struct BarelyNoteOffEvent {
  /// Create callback.
  BarelyNoteOffEvent_CreateCallback create_callback;

  /// Destroy callback.
  BarelyNoteOffEvent_DestroyCallback destroy_callback;

  /// Process callback.
  BarelyNoteOffEvent_ProcessCallback process_callback;

  /// Pointer to user data.
  void* user_data;
} BarelyNoteOffEvent;

/// Note on event create callback signature.
///
/// @param state Pointer to note on event state.
/// @param user_data Pointer to user data.
typedef void (*BarelyNoteOnEvent_CreateCallback)(void** state, void* user_data);

/// Note on event destroy callback signature.
///
/// @param state Pointer to note on event state.
typedef void (*BarelyNoteOnEvent_DestroyCallback)(void** state);

/// Note on event process callback signature.
///
/// @param state Pointer to note on event state.
/// @param pitch Note pitch.
/// @param intensity Note intensity.
typedef void (*BarelyNoteOnEvent_ProcessCallback)(void** state, double pitch, double intensity);

/// Note on event.
typedef struct BarelyNoteOnEvent {
  /// Create callback.
  BarelyNoteOnEvent_CreateCallback create_callback;

  /// Destroy callback.
  BarelyNoteOnEvent_DestroyCallback destroy_callback;

  /// Process callback.
  BarelyNoteOnEvent_ProcessCallback process_callback;

  /// Pointer to user data.
  void* user_data;
} BarelyNoteOnEvent;

/// Task event create callback signature.
///
/// @param state Pointer to task event state.
/// @param user_data Pointer to user data.
typedef void (*BarelyTaskEvent_CreateCallback)(void** state, void* user_data);

/// Task event destroy callback signature.
///
/// @param state Pointer to task event state.
typedef void (*BarelyTaskEvent_DestroyCallback)(void** state);

/// Task event process callback signature.
///
/// @param state Pointer to task event state.
typedef void (*BarelyTaskEvent_ProcessCallback)(void** state);

/// Task event.
typedef struct BarelyTaskEvent {
  /// Create callback.
  BarelyTaskEvent_CreateCallback create_callback;

  /// Destroy callback.
  BarelyTaskEvent_DestroyCallback destroy_callback;

  /// Process callback.
  BarelyTaskEvent_ProcessCallback process_callback;

  /// Pointer to user data.
  void* user_data;
} BarelyTaskEvent;

/// Instrument handle alias.
typedef struct BarelyInstrument* BarelyInstrumentHandle;

/// Musician handle alias.
typedef struct BarelyMusician* BarelyMusicianHandle;

/// Performer handle alias.
typedef struct BarelyPerformer* BarelyPerformerHandle;

/// Task handle alias.
typedef struct BarelyTask* BarelyTaskHandle;

/// Gets an instrument control value.
///
/// @param instrument Instrument handle.
/// @param type Control type.
/// @param out_value Output control value.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_GetControl(BarelyInstrumentHandle instrument,
                                               BarelyControlType type, double* out_value);

/// Gets an instrument note control value.
///
/// @param instrument Instrument handle.
/// @param pitch Note pitch.
/// @param type Note control type.
/// @param out_value Output note control value.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_GetNoteControl(BarelyInstrumentHandle instrument, double pitch,
                                                   BarelyNoteControlType type, double* out_value);

/// Gets whether an instrument note is on or not.
///
/// @param instrument Instrument handle.
/// @param pitch Note pitch.
/// @param out_is_note_on Output true if on, false otherwise.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_IsNoteOn(BarelyInstrumentHandle instrument, double pitch,
                                             bool* out_is_note_on);

/// Processes instrument output samples at timestamp.
/// @note This is *not* thread-safe during a corresponding `BarelyMusician_RemoveInstrument` call.
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

/// Sets an instrument control value.
///
/// @param instrument Instrument handle.
/// @param type Control type.
/// @param value Control value.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_SetControl(BarelyInstrumentHandle instrument,
                                               BarelyControlType type, double value);

/// Sets an instrument note control value.
///
/// @param instrument Instrument handle.
/// @param pitch Note pitch.
/// @param type Note control type.
/// @param value Note control value.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_SetNoteControl(BarelyInstrumentHandle instrument, double pitch,
                                                   BarelyNoteControlType type, double value);

/// Sets an instrument note off.
///
/// @param instrument Instrument handle.
/// @param pitch Note pitch.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_SetNoteOff(BarelyInstrumentHandle instrument, double pitch);

/// Sets the note off event of an instrument.
///
/// @param instrument Instrument handle.
/// @param note_off_event Pointer to note off event.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_SetNoteOffEvent(BarelyInstrumentHandle instrument,
                                                    const BarelyNoteOffEvent* note_off_event);

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
/// @param note_on_event Pointer to note on event.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_SetNoteOnEvent(BarelyInstrumentHandle instrument,
                                                   const BarelyNoteOnEvent* note_on_event);

/// Sets instrument sample data.
///
/// @param instrument Instrument handle.
/// @param slices Array of sample data slices.
/// @param sample_data_count Number of sample data slices.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_SetSampleData(BarelyInstrumentHandle instrument,
                                                  const BarelySampleDataSlice* slices,
                                                  int32_t slice_count);

/// Adds an instrument.
///
/// @param musician Musician handle.
/// @param out_instrument Output instrument handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_AddInstrument(BarelyMusicianHandle musician,
                                                BarelyInstrumentHandle* out_instrument);

/// Adds a performer.
///
/// @param musician Musician handle.
/// @param process_order Process order.
/// @param out_performer Output performer handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_AddPerformer(BarelyMusicianHandle musician, int32_t process_order,
                                               BarelyPerformerHandle* out_performer);

/// Creates a new musician.
///
/// @param frame_rate Frame rate in hertz.
/// @param out_musician Output musician handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_Create(int32_t frame_rate, BarelyMusicianHandle* out_musician);

/// Destroys a musician.
///
/// @param musician Musician handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_Destroy(BarelyMusicianHandle musician);

/// Gets the reference frequency of a musician.
///
/// @param musician Musician handle.
/// @param out_reference_frequency Output reference frequency in hertz.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_GetReferenceFrequency(BarelyMusicianHandle musician,
                                                        double* out_reference_frequency);

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

/// Removes an instrument.
///
/// @param musician Musician handle.
/// @param instrument Instrument handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_RemoveInstrument(BarelyMusicianHandle musician,
                                                   BarelyInstrumentHandle instrument);

/// Removes a performer.
///
/// @param musician Musician handle.
/// @param performer Performer handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_RemovePerformer(BarelyMusicianHandle musician,
                                                  BarelyPerformerHandle performer);

/// Sets the reference frequency of a musician.
///
/// @param musician Musician handle.
/// @param reference_frequency Reference frequency in hertz.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_SetReferenceFrequency(BarelyMusicianHandle musician,
                                                        double reference_frequency);

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

/// Adds a task.
///
/// @param performer Performer handle.
/// @param task_event Pointer to task event.
/// @param position Task position in beats.
/// @param out_task Output task handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_AddTask(BarelyPerformerHandle performer,
                                           const BarelyTaskEvent* task_event, double position,
                                           BarelyTaskHandle* out_task);

/// Cancels all one-off tasks.
///
/// @param performer Performer handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_CancelAllOneOffTasks(BarelyPerformerHandle performer);

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

/// Removes a task.
///
/// @param performer Performer handle.
/// @param task Task handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_RemoveTask(BarelyPerformerHandle performer,
                                              BarelyTaskHandle task);

/// Schedules a one-off task.
///
/// @param performer Performer handle.
/// @param task_event Pointer to task event.
/// @param position Task position in beats.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_ScheduleOneOffTask(BarelyPerformerHandle performer,
                                                      const BarelyTaskEvent* task_event,
                                                      double position);

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

/// Control type enum.
enum class ControlType : BarelyControlType {
  /// Gain.
  kGain = BarelyControlType_kGain,
  /// Number of voices.
  kVoiceCount = BarelyControlType_kVoiceCount,
  /// Oscillator shape.
  kOscillatorShape = BarelyControlType_kOscillatorShape,
  /// Sample playback mode.
  kSamplePlaybackMode = BarelyControlType_kSamplePlaybackMode,
  /// Envelope attack.
  kAttack = BarelyControlType_kAttack,
  /// Envelope decay.
  kDecay = BarelyControlType_kDecay,
  /// Envelope sustain.
  kSustain = BarelyControlType_kSustain,
  /// Envelope release.
  kRelease = BarelyControlType_kRelease,
  /// Pitch shift.
  kPitchShift = BarelyControlType_kPitchShift,
  /// Retrigger.
  kRetrigger = BarelyControlType_kRetrigger,
};

/// Note control type enum.
enum class NoteControlType : BarelyNoteControlType {
  /// Pitch shift.
  kPitchShift = BarelyNoteControlType_kPitchShift,
};

/// Oscillator shape enum.
enum class OscillatorShape : BarelyOscillatorShape {
  /// None.
  kNone = BarelyOscillatorShape_kNone,
  /// Sine wave.
  kSine = BarelyOscillatorShape_kSine,
  /// Sawtooth wave.
  kSaw = BarelyOscillatorShape_kSaw,
  /// Square wave.
  kSquare = BarelyOscillatorShape_kSquare,
  /// White noise.
  kNoise = BarelyOscillatorShape_kNoise,
};

/// Sample playback mode enum.
enum class SamplePlaybackMode : BarelySamplePlaybackMode {
  /// None.
  kNone = BarelySamplePlaybackMode_kNone,
  /// Once.
  kOnce = BarelySamplePlaybackMode_kOnce,
  /// Sustain.
  kSustain = BarelySamplePlaybackMode_kSustain,
  /// Loop.
  kLoop = BarelySamplePlaybackMode_kLoop,
};

/// Slice of sample data.
struct SampleDataSlice : public BarelySampleDataSlice {
  /// Constructs a new `SampleDataSlice`.
  ///
  /// @param root_pitch Root pich.
  /// @param sample_rate Sampling rate in hertz.
  /// @param samples Span of mono samples.
  explicit constexpr SampleDataSlice(double root_pitch, int sample_rate,
                                     std::span<const double> samples) noexcept
      : SampleDataSlice(
            {root_pitch, sample_rate, samples.data(), static_cast<int>(samples.size())}) {
    assert(sample_rate >= 0);
  }

  /// Constructs a new `SampleDataSlice` from a raw type.
  ///
  /// @param sample_data_slice Raw sample data slice.
  // NOLINTNEXTLINE(google-explicit-constructor)
  constexpr SampleDataSlice(BarelySampleDataSlice sample_data_slice) noexcept
      : BarelySampleDataSlice{sample_data_slice} {}
};

/// Note off event.
struct NoteOffEvent : public BarelyNoteOffEvent {
  /// Callback signature.
  ///
  /// @param pitch Note pitch.
  using Callback = std::function<void(double pitch)>;

  /// Create callback signature.
  using CreateCallback = BarelyNoteOffEvent_CreateCallback;

  /// Destroy callback signature.
  using DestroyCallback = BarelyNoteOffEvent_DestroyCallback;

  /// Process callback signature.
  using ProcessCallback = BarelyNoteOffEvent_ProcessCallback;

  /// Constructs a new `NoteOffEvent`.
  ///
  /// @param create_callback Create callback.
  /// @param destroy_callback Destroy callback.
  /// @param process_callback Process callback.
  /// @param user_data Pointer to user data.
  explicit constexpr NoteOffEvent(CreateCallback create_callback, DestroyCallback destroy_callback,
                                  ProcessCallback process_callback,
                                  void* user_data = nullptr) noexcept
      : NoteOffEvent(
            BarelyNoteOffEvent{create_callback, destroy_callback, process_callback, user_data}) {}

  /// Constructs a new `NoteOffEvent` from a raw type.
  ///
  /// @param note_off_event Raw note off event.
  // NOLINTNEXTLINE(google-explicit-constructor)
  constexpr NoteOffEvent(BarelyNoteOffEvent note_off_event) noexcept
      : BarelyNoteOffEvent{note_off_event} {}
};

/// Note on event.
struct NoteOnEvent : public BarelyNoteOnEvent {
  /// Callback signature.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  using Callback = std::function<void(double pitch, double intensity)>;

  /// Create callback signature.
  using CreateCallback = BarelyNoteOnEvent_CreateCallback;

  /// Destroy callback signature.
  using DestroyCallback = BarelyNoteOnEvent_DestroyCallback;

  /// Process callback signature.
  using ProcessCallback = BarelyNoteOnEvent_ProcessCallback;

  /// Constructs a new `NoteOnEvent`.
  ///
  /// @param create_callback Create callback.
  /// @param destroy_callback Destroy callback.
  /// @param process_callback Process callback.
  /// @param user_data Pointer to user data.
  explicit constexpr NoteOnEvent(CreateCallback create_callback, DestroyCallback destroy_callback,
                                 ProcessCallback process_callback,
                                 void* user_data = nullptr) noexcept
      : NoteOnEvent(
            BarelyNoteOnEvent{create_callback, destroy_callback, process_callback, user_data}) {}

  /// Constructs a new `NoteOnEvent` from a raw type.
  ///
  /// @param note_on_event Raw note on event.
  // NOLINTNEXTLINE(google-explicit-constructor)
  constexpr NoteOnEvent(BarelyNoteOnEvent note_on_event) noexcept
      : BarelyNoteOnEvent{note_on_event} {}
};

/// Task event.
struct TaskEvent : public BarelyTaskEvent {
  /// Callback signature.
  using Callback = std::function<void()>;

  /// Create callback signature.
  using CreateCallback = BarelyTaskEvent_CreateCallback;

  /// Destroy callback signature.
  using DestroyCallback = BarelyTaskEvent_DestroyCallback;

  /// Process callback signature.
  using ProcessCallback = BarelyTaskEvent_ProcessCallback;

  /// Constructs a new `TaskEvent`.
  ///
  /// @param create_callback Create callback.
  /// @param destroy_callback Destroy callback.
  /// @param process_callback Process callback.
  /// @param user_data Pointer to user data.
  explicit constexpr TaskEvent(CreateCallback create_callback, DestroyCallback destroy_callback,
                               ProcessCallback process_callback, void* user_data = nullptr) noexcept
      : TaskEvent(BarelyTaskEvent{create_callback, destroy_callback, process_callback, user_data}) {
  }

  /// Constructs a new `Task` from a raw type.
  ///
  /// @param task_event Raw task event.
  // NOLINTNEXTLINE(google-explicit-constructor)
  constexpr TaskEvent(BarelyTaskEvent task_event) noexcept : BarelyTaskEvent{task_event} {}
};

/// Creates an event of type with a callback.
///
/// @param event_callback Event callback.
/// @return Event.
template <typename EventType, typename... EventArgs>
static constexpr EventType EventWithCallback(typename EventType::Callback& event_callback) {
  return EventType(
      [](void** state, void* user_data) noexcept {
        *state = new (std::nothrow) typename EventType::Callback(
            std::move(*static_cast<typename EventType::Callback*>(user_data)));
        assert(*state);
      },
      [](void** state) noexcept { delete static_cast<typename EventType::Callback*>(*state); },
      [](void** state, EventArgs... args) noexcept {
        if (const auto& callback = *static_cast<typename EventType::Callback*>(*state); callback) {
          callback(args...);
        }
      },
      static_cast<void*>(&event_callback));
}

/// Handle wrapper template.
template <typename HandleType>
class HandleWrapper {
 public:
  /// Default constructor.
  constexpr HandleWrapper() noexcept = default;

  /// Constructs a new `HandleWrapper`.
  ///
  /// @param handle Raw handle.
  explicit constexpr HandleWrapper(HandleType handle) noexcept : handle_(handle) {
    assert(handle != nullptr);
  }

  /// Default destructor.
  constexpr ~HandleWrapper() noexcept = default;

  /// Copyable.
  constexpr HandleWrapper(const HandleWrapper& other) noexcept = default;
  constexpr HandleWrapper& operator=(const HandleWrapper& other) noexcept = default;

  /// Constructs a new `HandleWrapper` via move.
  ///
  /// @param other Other handle wrapper.
  constexpr HandleWrapper(HandleWrapper&& other) noexcept
      : handle_(std::exchange(other.handle_, nullptr)) {}

  /// Assigns `HandleWrapper` via move.
  ///
  /// @param other Other.
  /// @return Handle wrapper.
  constexpr HandleWrapper& operator=(HandleWrapper&& other) noexcept {
    if (this != &other) {
      handle_ = std::exchange(other.handle_, nullptr);
    }
    return *this;
  }

  /// Returns the raw handle.
  ///
  /// @return Raw handle.
  constexpr operator HandleType() const noexcept { return handle_; }

 private:
  // Raw handle.
  HandleType handle_ = nullptr;
};

/// Scoped handle wrapper template.
template <typename HandleWrapperType>
class ScopedHandleWrapper : public HandleWrapperType {
 public:
  /// Constructs a new `ScopedHandleWrapper`.
  template <typename... Args>
  explicit ScopedHandleWrapper(Args&&... args) noexcept
      : ScopedHandleWrapper(HandleWrapperType::Create(args...)) {}

  /// Constructs a new `ScopedHandleWrapper` from an existing `HandleWrapper`.
  ///
  /// @param handle_wrapper Handle wrapper.
  explicit ScopedHandleWrapper(HandleWrapperType handle_wrapper) noexcept
      : HandleWrapperType(handle_wrapper) {}

  /// Destroys `ScopedHandleWrapper`.
  ~ScopedHandleWrapper() noexcept { HandleWrapperType::Destroy(*this); }

  /// Non-copyable.
  ScopedHandleWrapper(const ScopedHandleWrapper& other) noexcept = delete;
  ScopedHandleWrapper& operator=(const ScopedHandleWrapper& other) noexcept = delete;

  /// Default move constructor.
  ScopedHandleWrapper(ScopedHandleWrapper&& other) noexcept = default;

  /// Assigns `ScopedHandleWrapper` via move.
  ///
  /// @param other Other.
  /// @return Scoped handle wrapper.
  ScopedHandleWrapper& operator=(ScopedHandleWrapper&& other) noexcept {
    if (this != &other) {
      HandleWrapperType::Destroy(*this);
      HandleWrapperType::operator=(std::move(other));
    }
    return *this;
  }

  /// Releases the scope.
  ///
  /// @return Handle wrapper.
  [[nodiscard]] HandleWrapperType Release() noexcept { return std::move(*this); }
};

/// Class that wraps an instrument handle.
class InstrumentHandle : public HandleWrapper<BarelyInstrumentHandle> {
 public:
  /// Default constructor.
  constexpr InstrumentHandle() noexcept = default;

  /// Creates a new `InstrumentHandle` from a raw handle.
  ///
  /// @param instrument Raw handle to instrument.
  explicit constexpr InstrumentHandle(BarelyInstrumentHandle instrument) noexcept
      : HandleWrapper(instrument) {}

  /// Returns a control value.
  ///
  /// @param type Control type.
  /// @return Control value.
  template <typename ValueType>
  [[nodiscard]] ValueType GetControl(ControlType type) const noexcept {
    static_assert(std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
                  "ValueType is not supported");
    double value = 0.0;
    [[maybe_unused]] const bool success =
        BarelyInstrument_GetControl(*this, static_cast<BarelyControlType>(type), &value);
    assert(success);
    return static_cast<ValueType>(value);
  }

  /// Returns a control value.
  ///
  /// @param pitch Note pitch.
  /// @param type Note control type.
  /// @return Note control value.
  template <typename ValueType>
  [[nodiscard]] ValueType GetNoteControl(double pitch, NoteControlType type) const noexcept {
    static_assert(std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
                  "ValueType is not supported");
    double value = 0.0;
    [[maybe_unused]] const bool success = BarelyInstrument_GetNoteControl(
        *this, pitch, static_cast<BarelyNoteControlType>(type), &value);
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

  /// Sets all notes off.
  void SetAllNotesOff() noexcept {
    [[maybe_unused]] const bool success = BarelyInstrument_SetAllNotesOff(*this);
    assert(success);
  }

  /// Sets a control value.
  ///
  /// @param type Control type.
  /// @param value Control value.
  template <typename ValueType>
  void SetControl(ControlType type, ValueType value) noexcept {
    static_assert(std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
                  "ValueType is not supported");
    [[maybe_unused]] const bool success = BarelyInstrument_SetControl(
        *this, static_cast<BarelyControlType>(type), static_cast<double>(value));
    assert(success);
  }

  /// Sets a control value.
  ///
  /// @param pitch Note pitch.
  /// @param type Note control type.
  /// @param value Note control value.
  template <typename ValueType>
  void SetNoteControl(double pitch, NoteControlType type, ValueType value) noexcept {
    static_assert(std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
                  "ValueType is not supported");
    [[maybe_unused]] const bool success = BarelyInstrument_SetNoteControl(
        *this, pitch, static_cast<BarelyNoteControlType>(type), static_cast<double>(value));
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
  /// @param note_off_event Pointer to note off event.
  void SetNoteOffEvent(const NoteOffEvent* note_off_event) noexcept {
    [[maybe_unused]] const bool success = BarelyInstrument_SetNoteOffEvent(*this, note_off_event);
    assert(success);
  }

  /// Sets the note off event with a callback.
  ///
  /// @param callback Note off event callback.
  void SetNoteOffEvent(NoteOffEvent::Callback callback) noexcept {
    if (callback) {
      const auto note_off_event = EventWithCallback<NoteOffEvent, double>(callback);
      SetNoteOffEvent(&note_off_event);
    } else {
      SetNoteOffEvent(nullptr);
    }
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
  /// @param note_on_event Pointer to note on event.
  void SetNoteOnEvent(const NoteOnEvent* note_on_event) noexcept {
    [[maybe_unused]] const bool success = BarelyInstrument_SetNoteOnEvent(*this, note_on_event);
    assert(success);
  }

  /// Sets the note off event with a callback.
  ///
  /// @param callback Note off event callback.
  void SetNoteOnEvent(NoteOnEvent::Callback callback) noexcept {
    if (callback) {
      const auto note_on_event = EventWithCallback<NoteOnEvent, double, double>(callback);
      SetNoteOnEvent(&note_on_event);
    } else {
      SetNoteOnEvent(nullptr);
    }
  }

  /// Sets the sample data.
  ///
  /// @param sample_data Span of sample data slices.
  void SetSampleData(std::span<const SampleDataSlice> slices) noexcept {
    [[maybe_unused]] const bool success = BarelyInstrument_SetSampleData(
        *this, reinterpret_cast<const BarelySampleDataSlice*>(slices.data()),
        static_cast<int32_t>(slices.size()));
    assert(success);
  }
};

/// Class that wraps a task handle.
class TaskHandle : public HandleWrapper<BarelyTaskHandle> {
 public:
  /// Default constructor.
  constexpr TaskHandle() noexcept = default;

  /// Constructs a new `TaskHandle` from a raw handle.
  ///
  /// @param task Raw handle to task.
  explicit constexpr TaskHandle(BarelyTaskHandle task) noexcept : HandleWrapper(task) {}

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

/// Class that wraps a performer handle.
class PerformerHandle : public HandleWrapper<BarelyPerformerHandle> {
 public:
  /// Default constructor.
  constexpr PerformerHandle() noexcept = default;

  /// Creates a new `PerformerHandle` from a raw handle.
  ///
  /// @param performer Raw handle to performer.
  explicit constexpr PerformerHandle(BarelyPerformerHandle performer) noexcept
      : HandleWrapper(performer) {}

  /// Adds a task.
  ///
  /// @param task_event Task event.
  /// @param position Task position in beats.
  /// @return Task handle.
  TaskHandle AddTask(const TaskEvent& task_event, double position) noexcept {
    BarelyTaskHandle task;
    [[maybe_unused]] const bool success =
        BarelyPerformer_AddTask(*this, &task_event, position, &task);
    assert(success);
    return TaskHandle(task);
  }

  /// Adds a task with a callback.
  ///
  /// @param callback Task callback.
  /// @param position Task position in beats.
  /// @return Task handle.
  TaskHandle AddTask(TaskEvent::Callback callback, double position) noexcept {
    assert(callback);
    return AddTask(EventWithCallback<TaskEvent>(callback), position);
  }

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

  /// Removes a task.
  ///
  /// @param task Task handle.
  void RemoveTask(TaskHandle task) noexcept {
    [[maybe_unused]] const bool success = BarelyPerformer_RemoveTask(*this, task);
    assert(success);
  }

  /// Schedules a one-off task.
  ///
  /// @param task_event Task event.
  /// @param position Task position in beats.
  void ScheduleOneOffTask(const TaskEvent& task_event, double position) noexcept {
    [[maybe_unused]] const bool success =
        BarelyPerformer_ScheduleOneOffTask(*this, &task_event, position);
    assert(success);
  }

  /// Schedules a one-off task with a callback.
  ///
  /// @param callback Task callback.
  /// @param position Task position in beats.
  void ScheduleOneOffTask(TaskEvent::Callback callback, double position) noexcept {
    assert(callback);
    ScheduleOneOffTask(EventWithCallback<TaskEvent>(callback), position);
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

/// Class that wraps a musician handle.
class MusicianHandle : public HandleWrapper<BarelyMusicianHandle> {
 public:
  /// Creates a new `MusicianHandle`.
  ///
  /// @param frame_rate Frame rate in hertz.
  /// @return Musician handle.
  [[nodiscard]] static MusicianHandle Create(int frame_rate) noexcept {
    BarelyMusicianHandle musician = nullptr;
    [[maybe_unused]] const bool success = BarelyMusician_Create(frame_rate, &musician);
    assert(success);
    return MusicianHandle(musician);
  }

  /// Destroys a `MusicianHandle`.
  ///
  /// @param musician Musician handle.
  static void Destroy(MusicianHandle musician) noexcept { BarelyMusician_Destroy(musician); }

  /// Default constructor.
  constexpr MusicianHandle() noexcept = default;

  /// Constructs a new `Musician` from a raw handle.
  ///
  /// @param musician Raw handle to musician.
  explicit constexpr MusicianHandle(BarelyMusicianHandle musician) noexcept
      : HandleWrapper(musician) {}

  /// Adds an instrument.
  ///
  /// @return Instrument handle.
  InstrumentHandle AddInstrument() noexcept {
    BarelyInstrumentHandle instrument;
    [[maybe_unused]] const bool success = BarelyMusician_AddInstrument(*this, &instrument);
    assert(success);
    return InstrumentHandle(instrument);
  }

  /// Adds a performer.
  ///
  /// @param process_order Process order.
  /// @return Performer handle.
  PerformerHandle AddPerformer(int process_order = 0) noexcept {
    BarelyPerformerHandle performer;
    [[maybe_unused]] const bool success =
        BarelyMusician_AddPerformer(*this, process_order, &performer);
    assert(success);
    return PerformerHandle(performer);
  }

  /// Returns the reference frequency.
  ///
  /// @return Reference frequency in hertz.
  [[nodiscard]] double GetReferenceFrequency() const noexcept {
    double reference_frequency = 0.0;
    [[maybe_unused]] const bool success =
        BarelyMusician_GetReferenceFrequency(*this, &reference_frequency);
    assert(success);
    return reference_frequency;
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

  /// Removes an instrument.
  ///
  /// @param instrument Instrument handle.
  void RemoveInstrument(InstrumentHandle instrument) noexcept {
    [[maybe_unused]] const bool success = BarelyMusician_RemoveInstrument(*this, instrument);
    assert(success);
  }

  /// Removes a performer.
  ///
  /// @param instrument Instrument handle.
  void RemovePerformer(PerformerHandle performer) noexcept {
    [[maybe_unused]] const bool success = BarelyMusician_RemovePerformer(*this, performer);
    assert(success);
  }

  /// Sets the reference frequency.
  ///
  /// @param reference_frequency Reference frequency in hertz.
  void SetReferenceFrequency(double reference_frequency) noexcept {
    [[maybe_unused]] const bool success =
        BarelyMusician_SetReferenceFrequency(*this, reference_frequency);
    assert(success);
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

/// Scoped musician alias.
using Musician = ScopedHandleWrapper<MusicianHandle>;

}  // namespace barely

#endif  // __cplusplus

#endif  // BARELYMUSICIAN_BARELYMUSICIAN_H_
