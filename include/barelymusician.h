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
///   barely::Musician musician(/*sample_rate=*/48000);
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
///   auto instrument = musician.CreateInstrument();
///
///   // Set a note on.
///   //
//    // Note pitch is centered around the reference frequency, and measured in octaves. Fractional
///   // values adjust the frequency logarithmically to maintain perceived pitch intervals in each
///   // octave.
///   const float c3_pitch = -1.0f;
///   instrument.SetNoteOn(c3_pitch, /*intensity=*/0.25f);
///
///   // Check if the note is on.
///   const bool is_note_on = instrument.IsNoteOn(c3_pitch);
///
///   // Set a control value.
///   instrument.SetControl(barely::ControlType::kGain, /*value=*/-6.0f);
///
///   // Process.
///   //
///   // Instruments process raw PCM audio samples in a synchronous call. Therefore, `Process`
///   // should typically be called from an audio thread process callback in real-time audio
///   // applications.
///   float output_samples[1024];
///   double timestamp = 0.0;
///   instrument.Process(output_samples, timestamp);
///   @endcode
///
/// - Performer:
///
///   @code{.cpp}
///   // Add.
///   auto performer = musician.CreatePerformer(/*process_order=*/0);
///
///   // Add a task.
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
///   #include "barelymusician.h"
///
///   // Create.
///   BarelyMusicianHandle musician = nullptr;
///   BarelyMusician_Create(/*sample_rate=*/48000, &musician);
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
///   BarelyInstrumentHandle instrument = nullptr;
///   BarelyInstrument_Create(musician, &instrument);
///
///   // Set a note on.
///   //
//    // Note pitch is centered around the reference frequency, and measured in octaves. Fractional
///   // values adjust the frequency logarithmically to maintain perceived pitch intervals in each
///   // octave.
///   float c3_pitch = -1.0f;
///   BarelyInstrument_SetNoteOn(instrument, c3_pitch, /*intensity=*/0.25f);
///
///   // Check if the note is on.
///   bool is_note_on = false;
///   BarelyInstrument_IsNoteOn(instrument, c3_pitch, &is_note_on);
///
///   // Set a control value.
///   BarelyInstrument_SetControl(instrument, BarelyControlType_kGain, /*value=*/-6.0f);
///
///   // Process.
///   //
///   // Instruments process raw PCM audio samples in a synchronous call. Therefore, `Process`
///   // should typically be called from an audio thread process callback in real-time audio
///   // applications.
///   float output_samples[1024];
///   double timestamp = 0.0;
///   BarelyInstrument_Process(instrument, output_samples, /*output_sample_count=*/1024, timestamp);
///
///   // Destroy.
///   BarelyInstrument_Destroy(instrument);
///   @endcode
///
/// - Performer:
///
///   @code{.cpp}
///   // Create.
///   BarelyPerformerHandle performer = nullptr;
///   BarelyPerformer_Create(musician, /*process_order=*/0, &performer);
///
///   // Create a task.
///   BarelyTaskHandle task = nullptr;
///   BarelyTask_Create(performer, BarelyTaskEvent{/*populate this*/}, /*position=*/0.0,
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

/// Control type enum alias.
typedef int32_t BarelyControlType;

/// Control type enum values.
enum BarelyControlType_Values {
  /// Gain in decibels.
  BarelyControlType_kGain = 0,
  /// Pitch shift.
  BarelyControlType_kPitchShift,
  /// Retrigger.
  BarelyControlType_kRetrigger,
  /// Number of voices.
  BarelyControlType_kVoiceCount,
  /// Envelope attack in seconds.
  BarelyControlType_kAttack,
  /// Envelope decay in seconds.
  BarelyControlType_kDecay,
  /// Envelope sustain.
  BarelyControlType_kSustain,
  /// Envelope release in seconds.
  BarelyControlType_kRelease,
  /// Oscillator mix.
  BarelyControlType_kOscillatorMix,
  /// Oscillator mode.
  BarelyControlType_kOscillatorMode,
  /// Oscillator pitch shift.
  BarelyControlType_kOscillatorPitchShift,
  /// Oscillator shape.
  BarelyControlType_kOscillatorShape,
  /// Pulse width.
  BarelyControlType_kPulseWidth,
  /// Sample playback mode.
  BarelyControlType_kSamplePlaybackMode,
  /// Filter type.
  BarelyControlType_kFilterType,
  /// Filter frequency in hertz.
  BarelyControlType_kFilterFrequency,
  /// Number of control types.
  BarelyControlType_kCount,
};

/// Filter type enum alias.
typedef int32_t BarelyFilterType;

/// Filter type enum values.
enum BarelyFilterType_Values {
  /// None.
  BarelyFilterType_kNone = 0,
  /// Low pass.
  BarelyFilterType_kLowPass,
  /// High pass.
  BarelyFilterType_kHighPass,
  /// Number of filters.
  BarelyFilterType_kCount,
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

/// Oscillator mode enum alias.
typedef int32_t BarelyOscillatorMode;

/// Oscillator mode enum value.
enum BarelyOscillatorMode_Values {
  /// Mix.
  BarelyOscillatorMode_kMix = 0,
  /// Amplitude modulation.
  BarelyOscillatorMode_kAm,
  /// Envelope follower.
  BarelyOscillatorMode_kEnvelopeFollower,
  // TODO(#146): Add FM support once voice pitch calculation is cleaned up.
  // /// Frequency modulation.
  // BarelyOscillatorMode_kFm,
  /// Ring modulation.
  BarelyOscillatorMode_kRing,
  /// Number of oscillator modes.
  BarelyOscillatorMode_kCount,
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
  float root_pitch;

  /// Sampling rate in hertz.
  int32_t sample_rate;

  /// Array of mono samples.
  const float* samples;

  /// Number of mono samples.
  int32_t sample_count;
} BarelySampleDataSlice;

/// Beat event callback signature.
///
/// @param position Position in beats.
/// @param user_data Pointer to user data.
typedef void (*BarelyBeatEvent_Callback)(double position, void* user_data);

/// Beat event.
typedef struct BarelyBeatEvent {
  /// Callback.
  BarelyBeatEvent_Callback callback;

  /// Pointer to user data.
  void* user_data;
} BarelyBeatEvent;

/// Note off event callback signature.
///
/// @param pitch Note pitch.
/// @param user_data Pointer to user data.
typedef void (*BarelyNoteOffEvent_Callback)(float pitch, void* user_data);

/// Note off event.
typedef struct BarelyNoteOffEvent {
  /// Callback.
  BarelyNoteOffEvent_Callback callback;

  /// Pointer to user data.
  void* user_data;
} BarelyNoteOffEvent;

/// Note on event callback signature.
///
/// @param pitch Note pitch.
/// @param intensity Note intensity.
/// @param user_data Pointer to user data.
typedef void (*BarelyNoteOnEvent_Callback)(float pitch, float intensity, void* user_data);

/// Note on event.
typedef struct BarelyNoteOnEvent {
  /// Callback.
  BarelyNoteOnEvent_Callback callback;

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

/// Creates a new instrument.
///
/// @param musician Musician handle.
/// @param out_instrument Output instrument handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_Create(BarelyMusicianHandle musician,
                                           BarelyInstrumentHandle* out_instrument);

/// Destroys an instrument.
///
/// @param instrument Instrument handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_Destroy(BarelyInstrumentHandle instrument);

/// Gets an instrument control value.
///
/// @param instrument Instrument handle.
/// @param type Control type.
/// @param out_value Output control value.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_GetControl(BarelyInstrumentHandle instrument,
                                               BarelyControlType type, float* out_value);

/// Gets an instrument note control value.
///
/// @param instrument Instrument handle.
/// @param pitch Note pitch.
/// @param type Note control type.
/// @param out_value Output note control value.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_GetNoteControl(BarelyInstrumentHandle instrument, float pitch,
                                                   BarelyNoteControlType type, float* out_value);

/// Gets whether an instrument note is on or not.
///
/// @param instrument Instrument handle.
/// @param pitch Note pitch.
/// @param out_is_note_on Output true if on, false otherwise.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_IsNoteOn(BarelyInstrumentHandle instrument, float pitch,
                                             bool* out_is_note_on);

/// Processes instrument output samples at timestamp.
/// @note This is *not* thread-safe during a corresponding `BarelyInstrument_Destroy` call.
///
/// @param instrument Instrument handle.
/// @param output_samples Array of mono output samples.
/// @param output_sample_count Number of output samples.
/// @param timestamp Timestamp in seconds.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_Process(BarelyInstrumentHandle instrument,
                                            float* output_samples, int32_t output_sample_count,
                                            double timestamp);

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
                                               BarelyControlType type, float value);

/// Sets an instrument note control value.
///
/// @param instrument Instrument handle.
/// @param pitch Note pitch.
/// @param type Note control type.
/// @param value Note control value.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_SetNoteControl(BarelyInstrumentHandle instrument, float pitch,
                                                   BarelyNoteControlType type, float value);

/// Sets an instrument note off.
///
/// @param instrument Instrument handle.
/// @param pitch Note pitch.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyInstrument_SetNoteOff(BarelyInstrumentHandle instrument, float pitch);

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
BARELY_EXPORT bool BarelyInstrument_SetNoteOn(BarelyInstrumentHandle instrument, float pitch,
                                              float intensity);

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

/// Creates a new musician.
///
/// @param sample_rate Sampling rate in hertz.
/// @param out_musician Output musician handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_Create(int32_t sample_rate, BarelyMusicianHandle* out_musician);

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
                                                        float* out_reference_frequency);

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

/// Sets the reference frequency of a musician.
///
/// @param musician Musician handle.
/// @param reference_frequency Reference frequency in hertz.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyMusician_SetReferenceFrequency(BarelyMusicianHandle musician,
                                                        float reference_frequency);

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

/// Creates a performer.
///
/// @param musician Musician handle.
/// @param process_order Process order.
/// @param out_performer Output performer handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_Create(BarelyMusicianHandle musician, int32_t process_order,
                                          BarelyPerformerHandle* out_performer);

/// Destroys a performer.
///
/// @param performer Performer handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_Destroy(BarelyPerformerHandle performer);

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

/// Sets the beat event of a performer.
///
/// @param performer Performer handle.
/// @param beat_event Pointer to beat event.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyPerformer_SetBeatEvent(BarelyPerformerHandle performer,
                                                const BarelyBeatEvent* beat_event);

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

/// Creates a new task.
///
/// @param performer Performer handle.
/// @param task_event Pointer to task event.
/// @param position Task position in beats.
/// @param out_task Output task handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyTask_Create(BarelyPerformerHandle performer,
                                     const BarelyTaskEvent* task_event, double position,
                                     BarelyTaskHandle* out_task);

/// Destroys a task.
///
/// @param task Task handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyTask_Destroy(BarelyTaskHandle task);

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
  /// Gain in decibels.
  kGain = BarelyControlType_kGain,
  /// Pitch shift.
  kPitchShift = BarelyControlType_kPitchShift,
  /// Retrigger.
  kRetrigger = BarelyControlType_kRetrigger,
  /// Number of voices.
  kVoiceCount = BarelyControlType_kVoiceCount,
  /// Envelope attack in seconds.
  kAttack = BarelyControlType_kAttack,
  /// Envelope decay in seconds.
  kDecay = BarelyControlType_kDecay,
  /// Envelope sustain.
  kSustain = BarelyControlType_kSustain,
  /// Envelope release in seconds.
  kRelease = BarelyControlType_kRelease,
  /// Oscillator mix.
  kOscillatorMix = BarelyControlType_kOscillatorMix,
  /// Oscillator mode.
  kOscillatorMode = BarelyControlType_kOscillatorMode,
  /// Oscillator pitch shift.
  kOscillatorPitchShift = BarelyControlType_kOscillatorPitchShift,
  /// Oscillator shape.
  kOscillatorShape = BarelyControlType_kOscillatorShape,
  /// Pulse width.
  kPulseWidth = BarelyControlType_kPulseWidth,
  /// Sample playback mode.
  kSamplePlaybackMode = BarelyControlType_kSamplePlaybackMode,
  /// Filter type.
  kFilterType = BarelyControlType_kFilterType,
  /// Filter frequency in hertz.
  kFilterFrequency = BarelyControlType_kFilterFrequency,
};

/// Filter type enum.
enum class FilterType : BarelyFilterType {
  /// None.
  kNone = BarelyFilterType_kNone,
  /// Low pass.
  kLowPass = BarelyFilterType_kLowPass,
  /// High pass.
  kHighPass = BarelyFilterType_kHighPass,
};

/// Note control type enum.
enum class NoteControlType : BarelyNoteControlType {
  /// Pitch shift.
  kPitchShift = BarelyNoteControlType_kPitchShift,
};

/// Oscillator mode enum.
enum class OscillatorMode : BarelyOscillatorMode {
  /// Mix.
  kMix = BarelyOscillatorMode_kMix,
  /// Amplitude modulation.
  kAm = BarelyOscillatorMode_kAm,
  /// Envelope follower.
  kEnvelopeFollower = BarelyOscillatorMode_kEnvelopeFollower,
  // TODO(#146): Add FM support once voice pitch calculation is cleaned up.
  // /// Frequency modulation.
  // kFm = BarelyOscillatorMode_kFm,
  /// Ring modulation.
  kRing = BarelyOscillatorMode_kRing,
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
  explicit constexpr SampleDataSlice(float root_pitch, int sample_rate,
                                     std::span<const float> samples) noexcept
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

/// Beat event.
struct BeatEvent : public BarelyBeatEvent {
  /// Callback signature.
  using Callback = BarelyBeatEvent_Callback;

  /// Constructs a new `BeatEvent`.
  ///
  /// @param callback Callback.
  /// @param user_data Pointer to user data.
  constexpr BeatEvent(Callback callback = nullptr, void* user_data = nullptr) noexcept
      : BeatEvent(BarelyBeatEvent{callback, user_data}) {}

  /// Constructs a new `BeatEvent` from a raw type.
  ///
  /// @param beat_event Raw beat event.
  // NOLINTNEXTLINE(google-explicit-constructor)
  constexpr BeatEvent(BarelyBeatEvent beat_event) noexcept : BarelyBeatEvent{beat_event} {}
};

/// Note off event.
struct NoteOffEvent : public BarelyNoteOffEvent {
  /// Callback signature.
  using Callback = BarelyNoteOffEvent_Callback;

  /// Constructs a new `NoteOffEvent`.
  ///
  /// @param callback Callback.
  /// @param user_data Pointer to user data.
  constexpr NoteOffEvent(Callback callback = nullptr, void* user_data = nullptr) noexcept
      : NoteOffEvent(BarelyNoteOffEvent{callback, user_data}) {}

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
  using Callback = BarelyNoteOnEvent_Callback;

  /// Constructs a new `NoteOnEvent`.
  ///
  /// @param callback Callback.
  /// @param user_data Pointer to user data.
  constexpr NoteOnEvent(Callback callback = nullptr, void* user_data = nullptr) noexcept
      : NoteOnEvent(BarelyNoteOnEvent{callback, user_data}) {}

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

/// Handle wrapper template 2.
template <typename HandleType>
class ScopedHandleWrapper {
 public:
  /// Default constructor.
  constexpr ScopedHandleWrapper() noexcept = default;

  /// Constructs a new `ScopedHandleWrapper`.
  ///
  /// @param handle Raw handle.
  explicit constexpr ScopedHandleWrapper(HandleType handle) noexcept : handle_(handle) {
    assert(handle != nullptr);
  }

  /// Default destructor.
  constexpr ~ScopedHandleWrapper() noexcept = default;

  /// Non-copyable.
  constexpr ScopedHandleWrapper(const ScopedHandleWrapper& other) noexcept = delete;
  constexpr ScopedHandleWrapper& operator=(const ScopedHandleWrapper& other) noexcept = delete;

  /// Constructs a new `ScopedHandleWrapper` via move.
  ///
  /// @param other Other handle wrapper.
  constexpr ScopedHandleWrapper(ScopedHandleWrapper&& other) noexcept
      : handle_(std::exchange(other.handle_, nullptr)) {}

  /// Assigns `ScopedHandleWrapper` via move.
  ///
  /// @param other Other.
  /// @return Handle wrapper.
  constexpr ScopedHandleWrapper& operator=(ScopedHandleWrapper&& other) noexcept {
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

/// Class that wraps an instrument handle.
class Instrument : public ScopedHandleWrapper<BarelyInstrumentHandle> {
 public:
  // TODO(#147): Clean the aliases up after task refactor.
  using NoteOnEventCallback = std::function<void(float pitch, float intensity)>;
  using NoteOffEventCallback = std::function<void(float pitch)>;

  /// Constructs a new `Instrument`.
  ///
  /// @param musician Raw musician handle.
  explicit Instrument(BarelyMusicianHandle musician) noexcept
      : ScopedHandleWrapper([&]() {
          BarelyInstrumentHandle instrument = nullptr;
          [[maybe_unused]] const bool success = BarelyInstrument_Create(musician, &instrument);
          assert(success);
          return instrument;
        }()) {}

  /// Constructs a new `Instrument` from a raw handle.
  ///
  /// @param instrument Raw handle to instrument.
  explicit Instrument(BarelyInstrumentHandle instrument) noexcept
      : ScopedHandleWrapper(instrument) {}

  /// Destroys `Instrument`.
  ~Instrument() noexcept { BarelyInstrument_Destroy(*this); }

  /// Non-copyable.
  Instrument(const Instrument& other) noexcept = delete;
  Instrument& operator=(const Instrument& other) noexcept = delete;

  /// Default move constructor.
  Instrument(Instrument&& other) noexcept = default;

  /// Assigns `Instrument` via move.
  ///
  /// @param other Other instrument.
  /// @return Instrument.
  Instrument& operator=(Instrument&& other) noexcept {
    if (this != &other) {
      BarelyInstrument_Destroy(*this);
      ScopedHandleWrapper::operator=(std::move(other));
    }
    return *this;
  }

  /// Returns a control value.
  ///
  /// @param type Control type.
  /// @return Control value.
  template <typename ValueType>
  [[nodiscard]] ValueType GetControl(ControlType type) const noexcept {
    static_assert(std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
                  "ValueType is not supported");
    float value = 0.0f;
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
  [[nodiscard]] ValueType GetNoteControl(float pitch, NoteControlType type) const noexcept {
    static_assert(std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
                  "ValueType is not supported");
    float value = 0.0f;
    [[maybe_unused]] const bool success = BarelyInstrument_GetNoteControl(
        *this, pitch, static_cast<BarelyNoteControlType>(type), &value);
    assert(success);
    return static_cast<ValueType>(value);
  }

  /// Returns whether a note is on or not.
  ///
  /// @param pitch Note pitch.
  /// @return True if active, false otherwise.
  [[nodiscard]] bool IsNoteOn(float pitch) const noexcept {
    bool is_note_on = false;
    [[maybe_unused]] const bool success = BarelyInstrument_IsNoteOn(*this, pitch, &is_note_on);
    assert(success);
    return is_note_on;
  }

  /// Processes output samples at timestamp.
  ///
  /// @param output_samples Span of mono output samples.
  /// @param timestamp Timestamp in seconds.
  void Process(std::span<float> output_samples, double timestamp) noexcept {
    [[maybe_unused]] const bool success = BarelyInstrument_Process(
        *this, output_samples.data(), static_cast<int32_t>(output_samples.size()), timestamp);
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
        *this, static_cast<BarelyControlType>(type), static_cast<float>(value));
    assert(success);
  }

  /// Sets a control value.
  ///
  /// @param pitch Note pitch.
  /// @param type Note control type.
  /// @param value Note control value.
  template <typename ValueType>
  void SetNoteControl(float pitch, NoteControlType type, ValueType value) noexcept {
    static_assert(std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
                  "ValueType is not supported");
    [[maybe_unused]] const bool success = BarelyInstrument_SetNoteControl(
        *this, pitch, static_cast<BarelyNoteControlType>(type), static_cast<float>(value));
    assert(success);
  }

  /// Sets a note off.
  ///
  /// @param pitch Note pitch.
  void SetNoteOff(float pitch) noexcept {
    [[maybe_unused]] const bool success = BarelyInstrument_SetNoteOff(*this, pitch);
    assert(success);
  }

  /// Sets the note off event.
  ///
  /// @param note_off_event Pointer to note off event.
  void SetNoteOffEvent(const NoteOffEvent& note_off_event) noexcept {
    [[maybe_unused]] const bool success = BarelyInstrument_SetNoteOffEvent(*this, &note_off_event);
    assert(success);
  }

  /// Sets the note off event with a callback.
  ///
  /// @param note_off_event_callback Note off event callback.
  void SetNoteOffEvent(NoteOffEventCallback note_off_event_callback) noexcept {
    note_off_event_callback_ = std::move(note_off_event_callback);
    return SetNoteOffEvent(NoteOffEvent(
        [](float pitch, void* user_data) {
          (*static_cast<NoteOffEventCallback*>(user_data))(pitch);
        },
        &note_off_event_callback_));
  }

  /// Sets a note on.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  void SetNoteOn(float pitch, float intensity = 1.0f) noexcept {
    [[maybe_unused]] const bool success = BarelyInstrument_SetNoteOn(*this, pitch, intensity);
    assert(success);
  }

  /// Sets the note on event.
  ///
  /// @param note_on_event Pointer to note on event.
  void SetNoteOnEvent(const NoteOnEvent& note_on_event) noexcept {
    [[maybe_unused]] const bool success = BarelyInstrument_SetNoteOnEvent(*this, &note_on_event);
    assert(success);
  }

  /// Sets the note on event with a callback.
  ///
  /// @param note_on_event_callback Note on event callback.
  void SetNoteOnEvent(NoteOnEventCallback note_on_event_callback) noexcept {
    note_on_event_callback_ = std::move(note_on_event_callback);
    return SetNoteOnEvent(NoteOnEvent(
        [](float pitch, float intensity, void* user_data) {
          (*static_cast<NoteOnEventCallback*>(user_data))(pitch, intensity);
        },
        &note_on_event_callback_));
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

 private:
  // Note off event callback.
  NoteOffEventCallback note_off_event_callback_;

  // Note on event callback.
  NoteOnEventCallback note_on_event_callback_;
};

/// Class that wraps a task handle.
class Task : public ScopedHandleWrapper<BarelyTaskHandle> {
 public:
  /// Constructs a new `Task`.
  ///
  /// @param performer Raw performer handle.
  /// @param task_event Task event.
  /// @param position Task position in beats.
  Task(BarelyPerformerHandle performer, const TaskEvent& task_event, double position) noexcept
      : ScopedHandleWrapper([&]() {
          BarelyTaskHandle task = nullptr;
          [[maybe_unused]] const bool success =
              BarelyTask_Create(performer, &task_event, position, &task);
          assert(success);
          return task;
        }()) {}

  /// Constructs a new `Task` from a raw handle.
  ///
  /// @param task Raw handle to task.
  explicit Task(BarelyTaskHandle task) noexcept : ScopedHandleWrapper(task) {}

  /// Destroys `Task`.
  ~Task() noexcept { BarelyTask_Destroy(*this); }

  /// Non-copyable.
  Task(const Task& other) noexcept = delete;
  Task& operator=(const Task& other) noexcept = delete;

  /// Default move constructor.
  Task(Task&& other) noexcept = default;

  /// Assigns `Task` via move.
  ///
  /// @param other Other task.
  /// @return Task.
  Task& operator=(Task&& other) noexcept {
    if (this != &other) {
      BarelyTask_Destroy(*this);
      ScopedHandleWrapper::operator=(std::move(other));
    }
    return *this;
  }

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
class Performer : public ScopedHandleWrapper<BarelyPerformerHandle> {
 public:
  /// Constructs a new `Performer`.
  ///
  /// @param musician Raw musician handle.
  /// @param process_order Process order.
  explicit Performer(BarelyMusicianHandle musician, int process_order = 0) noexcept
      : ScopedHandleWrapper([&]() {
          BarelyPerformerHandle performer = nullptr;
          [[maybe_unused]] const bool success =
              BarelyPerformer_Create(musician, static_cast<int32_t>(process_order), &performer);
          assert(success);
          return performer;
        }()) {}

  /// Constructs a new `Performer` from a raw handle.
  ///
  /// @param performer Raw handle to performer.
  explicit Performer(BarelyPerformerHandle performer) noexcept : ScopedHandleWrapper(performer) {}

  /// Destroys `Performer`.
  ~Performer() noexcept { BarelyPerformer_Destroy(*this); }

  /// Non-copyable.
  Performer(const Performer& other) noexcept = delete;
  Performer& operator=(const Performer& other) noexcept = delete;

  /// Default move constructor.
  Performer(Performer&& other) noexcept = default;

  /// Assigns `Performer` via move.
  ///
  /// @param other Other performer.
  /// @return Performer.
  Performer& operator=(Performer&& other) noexcept {
    if (this != &other) {
      BarelyPerformer_Destroy(*this);
      ScopedHandleWrapper::operator=(std::move(other));
    }
    return *this;
  }

  /// Creates a new task.
  ///
  /// @param task_event Task event.
  /// @param position Task position in beats.
  /// @return Task.
  [[nodiscard]] Task CreateTask(const TaskEvent& task_event, double position) noexcept {
    return Task(*this, task_event, position);
  }

  /// Creates a new task with a callback.
  ///
  /// @param callback Task callback.
  /// @param position Task position in beats.
  /// @return Task.
  [[nodiscard]] Task CreateTask(TaskEvent::Callback callback, double position) noexcept {
    assert(callback);
    return CreateTask(EventWithCallback<TaskEvent>(callback), position);
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

  /// Sets the beat event.
  ///
  /// @param beat_event Pointer to beat event.
  void SetBeatEvent(const BeatEvent& beat_event) noexcept {
    [[maybe_unused]] const bool success = BarelyPerformer_SetBeatEvent(*this, &beat_event);
    assert(success);
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

/// A class that wraps a musician handle.
class Musician : public ScopedHandleWrapper<BarelyMusicianHandle> {
 public:
  /// Constructs a new `Musician`.
  ///
  /// @param sample_rate Sampling rate in hertz.
  explicit Musician(int sample_rate) noexcept
      : ScopedHandleWrapper([&]() {
          BarelyMusicianHandle musician = nullptr;
          [[maybe_unused]] const bool success =
              BarelyMusician_Create(static_cast<int32_t>(sample_rate), &musician);
          assert(success);
          return musician;
        }()) {}

  /// Constructs a new `Musician` from a raw handle.
  ///
  /// @param musician Raw handle to musician.
  explicit Musician(BarelyMusicianHandle musician) noexcept : ScopedHandleWrapper(musician) {}

  /// Destroys `Musician`.
  ~Musician() noexcept { BarelyMusician_Destroy(*this); }

  /// Non-copyable.
  Musician(const Musician& other) noexcept = delete;
  Musician& operator=(const Musician& other) noexcept = delete;

  /// Default move constructor.
  Musician(Musician&& other) noexcept = default;

  /// Assigns `Musician` via move.
  ///
  /// @param other Other musician.
  /// @return Musician.
  Musician& operator=(Musician&& other) noexcept {
    if (this != &other) {
      BarelyMusician_Destroy(*this);
      ScopedHandleWrapper::operator=(std::move(other));
    }
    return *this;
  }

  /// Creates a new instrument.
  ///
  /// @return Instrument.
  Instrument CreateInstrument() noexcept { return Instrument(*this); }

  /// Creates a performer.
  ///
  /// @param process_order Process order.
  /// @return Performer.
  [[nodiscard]] Performer CreatePerformer(int process_order = 0) noexcept {
    return Performer(*this, process_order);
  }

  /// Returns the reference frequency.
  ///
  /// @return Reference frequency in hertz.
  [[nodiscard]] float GetReferenceFrequency() const noexcept {
    float reference_frequency = 0.0f;
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

  /// Sets the reference frequency.
  ///
  /// @param reference_frequency Reference frequency in hertz.
  void SetReferenceFrequency(float reference_frequency) noexcept {
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

}  // namespace barely

#endif  // __cplusplus

#endif  // BARELYMUSICIAN_BARELYMUSICIAN_H_
