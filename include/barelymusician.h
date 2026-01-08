/// ================================================================================================
/// barelymusician
/// ================================================================================================
///
/// barelymusician is a real-time music engine for interactive systems.
/// It is used to generate and perform musical sounds from scratch with sample accurate timing.
///
/// ------------------------------------------------------------------------------------------------
/// Example C++ Usage
/// ------------------------------------------------------------------------------------------------
///
/// - Engine:
///
///   @code{.cpp}
///   #include <barelymusician.h>
///
///   // Create.
///   barely::Engine engine(/*sample_rate=*/48000);
///
///   // Set the tempo.
///   engine.SetTempo(/*tempo=*/124.0);
///
///   // Update the timestamp.
///   //
///   // Timestamp updates must occur before processing the engine with the respective timestamps.
///   // Otherwise, `Process` calls may be *late* in receiving relevant changes to the engine. To
///   // address this, `Update` should typically be called from the main thread update callback
///   // using a lookahead to prevent potential thread synchronization issues in real-time audio
///   // applications.
///   constexpr double kLookahead = 0.1;
///   double timestamp = 1.0;
///   engine.Update(timestamp + kLookahead);
///
///   // Process.
///   //
///   // The engine processes output samples synchronously. Therefore, `Process` should typically be
///   // called from an audio thread process callback in real-time audio applications.
///   constexpr int kChannelCount = 2;
///   constexpr int kFrameCount = 512;
///   float output_samples[kChannelCount * kFrameCount];
///   engine.Process(output_samples, kChannelCount, kFrameCount, timestamp);
///   @endcode
///
/// - Instrument:
///
///   @code{.cpp}
///   // Create.
///   auto instrument = engine.CreateInstrument();
///
///   // Set a note on.
///   //
///   // The note pitch is expressed in octaves relative to middle C as the center frequency.
///   // Fractional note values adjust the frequency logarithmically to ensure equally perceived
///   // pitch intervals within each octave.
///   constexpr float kC3Pitch = -1.0f;
///   instrument.SetNoteOn(kC3Pitch);
///
///   // Check if the note is on.
///   const bool is_note_on = instrument.IsNoteOn(kC3Pitch);
///
///   // Set the instrument to use full oscillator mix.
///   instrument.SetControl(barely::InstrumentControlType::kOscMix, /*value=*/1.0f);
///
///   // Destroy.
///   engine.DestroyInstrument(instrument);
///   @endcode
///
/// - Performer:
///
///   @code{.cpp}
///   // Create.
///   auto performer = engine.CreatePerformer();
///
///   // Create a task.
///   auto task = engine.CreateTask(performer, /*position=*/0.0, /*duration=*/1.0,
///                                 [](barely::TaskEventType type) { /*populate this*/ });
///
///   // Set to looping.
///   performer.SetLooping(/*is_looping=*/true);
///
///   // Start.
///   performer.Start();
///
///   // Check if started playing.
///   const bool is_playing = performer.IsPlaying();
///
///   // Destroy task.
///   engine.DestroyTask(task);
///
///   // Destroy.
///   engine.DestroyPerformer(performer);
///   @endcode
///
/// ------------------------------------------------------------------------------------------------
/// Example C Usage
/// ------------------------------------------------------------------------------------------------
///
/// - Engine:
///
///   @code{.cpp}
///   #include <barelymusician.h>
///
///   // Create.
///   BarelyEngine* engine = nullptr;
///   BarelyEngine_Create(/*sample_rate=*/48000, &engine);
///
///   // Set the tempo.
///   BarelyEngine_SetTempo(engine, /*tempo=*/124.0);
///
///   // Update the timestamp.
///   //
///   // Timestamp updates must occur before processing the engine with the respective timestamps.
///   // Otherwise, `Process` calls may be *late* in receiving relevant changes to the engine. To
///   // address this, `Update` should typically be called from the main thread update callback
///   // using a lookahead to prevent potential thread synchronization issues in real-time audio
///   // applications.
///   double lookahead = 0.1;
///   double timestamp = 0.0;
///   BarelyEngine_Update(engine, timestamp + lookahead);
///
///   // Process.
///   //
///   // Process the next output samples of the engine.
///   //
///   // The engine processes output samples synchronously. Therefore, `Process` should typically be
///   // called from an audio thread process callback in real-time audio applications.
///   float output_samples[2 * 512];
///   BarelyEngine_Process(engine, output_samples, 2, 512, timestamp);
///
///   // Destroy.
///   BarelyEngine_Destroy(engine);
///   @endcode
///
/// - Instrument:
///
///   @code{.cpp}
///   // Create.
///   BarelyRef instrument = {};
///   BarelyEngine_CreateInstrument(engine, /*control_overrides=*/nullptr,
///                                 /*control_override_count=*/0, &instrument);
///
///   // Set a note on.
///   //
///   // The note pitch is expressed in octaves relative to middle C as the center frequency.
///   // Fractional note values adjust the frequency logarithmically to ensure equally perceived
///   // pitch intervals within each octave.
///   float c3_pitch = -1.0f;
///   BarelyInstrument_SetNoteOn(engine, instrument, c3_pitch, /*note_control_overrides=*/nullptr,
///                              /*note_control_override_count=*/0);
///
///   // Check if the note is on.
///   bool is_note_on = false;
///   BarelyInstrument_IsNoteOn(engine, instrument, c3_pitch, &is_note_on);
///
///   // Set a control value.
///   BarelyInstrument_SetControl(engine, instrument, BarelyInstrumentControlType_kOscMix,
///                               /*value=*/1.0f);
///
///   // Destroy.
///   BarelyEngine_DestroyInstrument(engine, instrument);
///   @endcode
///
/// - Performer:
///
///   @code{.cpp}
///   // Create.
///   BarelyRef performer = {};
///   BarelyEngine_CreatePerformer(engine, &performer);
///
///   // Create a task.
///   BarelyRef task = {};
///   BarelyTaskEventCallback callback{ /*populate this*/ };
///   BarelyEngine_CreateTask(engine, performer, /*position=*/0.0, /*duration=*/1.0, /*priority=*/0,
///                           callback, &task);
///
///   // Set to looping.
///   BarelyPerformer_SetLooping(engine, performer, /*is_looping=*/true);
///
///   // Start.
///   BarelyPerformer_Start(engine, performer);
///
///   // Check if started playing.
///   bool is_playing = false;
///   BarelyPerformer_IsPlaying(engine, performer, &is_playing);
///
///   // Destroy the task.
///   BarelyEngine_DestroyTask(engine, task);
///
///   // Destroy.
///   BarelyEngine_DestroyPerformer(engine, performer);
///   @endcode

#ifndef BARELYMUSICIAN_BARELYMUSICIAN_H_
#define BARELYMUSICIAN_BARELYMUSICIAN_H_

/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// barelymusician C API
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// NOLINTBEGIN
#include <stdbool.h>
#include <stdint.h>

#if defined(_WIN32) || defined(__CYGWIN__)
#ifdef BARELYMUSICIAN_EXPORTS
#ifdef __GNUC__
#define BARELY_API __attribute__((dllexport))
#else  // __GNUC__
#define BARELY_API __declspec(dllexport)
#endif  // __GNUC__
#else   // BARELYMUSICIAN_EXPORTS
#ifdef __GNUC__
#define BARELY_API __attribute__((dllimport))
#else  // __GNUC__
#define BARELY_API __declspec(dllimport)
#endif  // __GNUC__
#endif  // BARELYMUSICIAN_EXPORTS
#else   // defined(_WIN32) || defined(__CYGWIN__)
#if __GNUC__ >= 4
#define BARELY_API __attribute__((visibility("default")))
#else  // __GNUC__ >= 4
#define BARELY_API
#endif  // __GNUC__ >= 4
#endif  // defined(_WIN32) || defined(__CYGWIN__)

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Engine control types.
typedef enum BarelyEngineControlType {
  /// Compressor mix.
  BarelyEngineControlType_kCompressorMix = 0,
  /// Compressor attack in seconds.
  BarelyEngineControlType_kCompressorAttack,
  /// Compressor release in seconds.
  BarelyEngineControlType_kCompressorRelease,
  /// Compressor threshold.
  BarelyEngineControlType_kCompressorThreshold,
  /// Compressor ratio.
  BarelyEngineControlType_kCompressorRatio,
  /// Delay mix.
  BarelyEngineControlType_kDelayMix,
  /// Delay time in seconds.
  BarelyEngineControlType_kDelayTime,
  /// Delay feedback.
  BarelyEngineControlType_kDelayFeedback,
  /// Delay low-pass frequency.
  BarelyEngineControlType_kDelayLowPassFrequency,
  /// Delay high-pass frequency.
  BarelyEngineControlType_kDelayHighPassFrequency,
  /// Sidechain mix.
  BarelyEngineControlType_kSidechainMix,
  /// Sidechain attack in seconds.
  BarelyEngineControlType_kSidechainAttack,
  /// Sidechain release in seconds.
  BarelyEngineControlType_kSidechainRelease,
  /// Sidechain threshold.
  BarelyEngineControlType_kSidechainThreshold,
  /// Sidechain ratio.
  BarelyEngineControlType_kSidechainRatio,
  // Number of engine control types.
  BarelyEngineControlType_kCount,
} BarelyEngineControlType;

/// Instrument control types.
typedef enum BarelyInstrumentControlType {
  /// Gain in linear amplitude.
  BarelyInstrumentControlType_kGain = 0,
  /// Pitch shift.
  BarelyInstrumentControlType_kPitchShift,
  /// Retrigger.
  BarelyInstrumentControlType_kRetrigger,
  /// Stereo pan.
  BarelyInstrumentControlType_kStereoPan,
  /// Number of voices.
  BarelyInstrumentControlType_kVoiceCount,
  /// Envelope attack in seconds.
  BarelyInstrumentControlType_kAttack,
  /// Envelope decay in seconds.
  BarelyInstrumentControlType_kDecay,
  /// Envelope sustain.
  BarelyInstrumentControlType_kSustain,
  /// Envelope release in seconds.
  BarelyInstrumentControlType_kRelease,
  /// Oscillator mix.
  BarelyInstrumentControlType_kOscMix,
  /// Oscillator mode.
  BarelyInstrumentControlType_kOscMode,
  /// Oscillator noise mix.
  BarelyInstrumentControlType_kOscNoiseMix,
  /// Oscillator pitch shift.
  BarelyInstrumentControlType_kOscPitchShift,
  /// Oscillator shape.
  BarelyInstrumentControlType_kOscShape,
  /// Oscillator skew.
  BarelyInstrumentControlType_kOscSkew,
  /// Slice mode.
  BarelyInstrumentControlType_kSliceMode,
  /// Bit crusher depth.
  BarelyInstrumentControlType_kBitCrusherDepth,
  /// Normalized bit crusher rate.
  BarelyInstrumentControlType_kBitCrusherRate,
  /// Distortion amount.
  BarelyInstrumentControlType_kDistortionAmount,
  /// Distortion drive.
  BarelyInstrumentControlType_kDistortionDrive,
  /// Filter type.
  BarelyInstrumentControlType_kFilterType,
  /// Filter frequency in hertz.
  BarelyInstrumentControlType_kFilterFrequency,
  /// Filter Q factor.
  BarelyInstrumentControlType_kFilterQ,
  /// Delay send.
  BarelyInstrumentControlType_kDelaySend,
  /// Sidechain send.
  BarelyInstrumentControlType_kSidechainSend,
  /// Arpeggiator mode.
  BarelyInstrumentControlType_kArpMode,
  /// Arpeggiator gate ratio.
  BarelyInstrumentControlType_kArpGateRatio,
  /// Arpeggiator rate.
  BarelyInstrumentControlType_kArpRate,
  /// Number of instrument control types.
  BarelyInstrumentControlType_kCount,
} BarelyInstrumentControlType;

/// Note control types.
typedef enum BarelyNoteControlType {
  /// Gain in linear amplitude.
  BarelyNoteControlType_kGain = 0,
  /// Pitch shift.
  BarelyNoteControlType_kPitchShift,
  /// Number of note control types.
  BarelyNoteControlType_kCount,
} BarelyNoteControlType;

/// Arpeggiator modes.
typedef enum BarelyArpMode {
  /// None.
  BarelyArpMode_kNone = 0,
  /// Up.
  BarelyArpMode_kUp,
  /// Down.
  BarelyArpMode_kDown,
  /// Random.
  BarelyArpMode_kRandom,
  /// Number of arpeggiator modes.
  BarelyArpMode_kCount,
} BarelyArpMode;

/// Filter types.
typedef enum BarelyFilterType {
  /// None.
  BarelyFilterType_kNone = 0,
  /// Low pass.
  BarelyFilterType_kLowPass,
  /// High pass.
  BarelyFilterType_kHighPass,
  /// Number of filters.
  BarelyFilterType_kCount,
} BarelyFilterType;

/// Oscillator modes.
typedef enum BarelyOscMode {
  /// Mix.
  BarelyOscMode_kMix = 0,
  /// Amplitude modulation by the oscillator applied to the slice.
  BarelyOscMode_kAm,
  /// Amplitude modulation by the slice applied to the oscillator.
  BarelyOscMode_kEnvelopeFollower,
  /// Frequency modulation using oscillator applied to slice.
  BarelyOscMode_kFm,
  /// Frequency modulation using slice applied to oscillator.
  BarelyOscMode_kMf,
  /// Ring modulation.
  BarelyOscMode_kRing,
  /// Number of oscillator modes.
  BarelyOscMode_kCount,
} BarelyOscMode;

/// Slice modes.
typedef enum BarelySliceMode {
  /// Sustain.
  BarelySliceMode_kSustain = 0,
  /// Loop.
  BarelySliceMode_kLoop,
  /// Once.
  BarelySliceMode_kOnce,
  /// Number of slice modes.
  BarelySliceMode_kCount,
} BarelySliceMode;

/// Note event types.
typedef enum BarelyNoteEventType {
  /// Begin.
  BarelyNoteEventType_kBegin = 0,
  /// End.
  BarelyNoteEventType_kEnd,
  /// Number of note event types.
  BarelyNoteEventType_kCount,
} BarelyNoteEventType;

/// Task event types.
typedef enum BarelyTaskEventType {
  /// Begin.
  BarelyTaskEventType_kBegin = 0,
  /// End.
  BarelyTaskEventType_kEnd,
  /// Number of task event types.
  BarelyTaskEventType_kCount,
} BarelyTaskEventType;

/// Engine.
typedef struct BarelyEngine BarelyEngine;

/// Reference.
typedef struct BarelyRef {
  /// Index.
  uint32_t index;

  /// Generation.
  uint16_t generation;
} BarelyRef;

/// Instrument control override.
typedef struct BarelyInstrumentControlOverride {
  /// Type.
  BarelyInstrumentControlType type;

  /// Value.
  float value;
} BarelyInstrumentControlOverride;

/// Note control override.
typedef struct BarelyNoteControlOverride {
  /// Type.
  BarelyNoteControlType type;

  /// Value.
  float value;
} BarelyNoteControlOverride;

/// Slice of sample data.
typedef struct BarelySlice {
  /// Root note pitch.
  float root_pitch;

  /// Sampling rate in hertz.
  int32_t sample_rate;

  /// Array of mono samples.
  const float* samples;

  /// Number of mono samples.
  int32_t sample_count;
} BarelySlice;

/// A musical quantization.
typedef struct BarelyQuantization {
  /// Subdivision of a beat.
  int32_t subdivision;

  /// Amount.
  float amount;
} BarelyQuantization;

/// A musical scale.
typedef struct BarelyScale {
  /// Array of note pitches relative to the root note pitch.
  const float* pitches;

  /// Number of note pitches.
  int32_t pitch_count;

  /// Root note pitch of the scale.
  float root_pitch;

  /// Mode index.
  int32_t mode;
} BarelyScale;

/// Note event callback.
///
/// @param type Note event type.
/// @param pitch Note pitch.
/// @param user_data Pointer to user data.
typedef void (*BarelyNoteEventCallback)(BarelyNoteEventType type, float pitch, void* user_data);

/// Task event callback.
///
/// @param type Task event type.
/// @param user_data Pointer to user data.
typedef void (*BarelyTaskEventCallback)(BarelyTaskEventType type, void* user_data);

/// Creates a new engine.
///
/// @param sample_rate Sampling rate in hertz.
/// @param out_engine Output pointer to engine.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_Create(int32_t sample_rate, BarelyEngine** out_engine);

/// Creates a new instrument.
///
/// @param engine Pointer to engine.
/// @param control_overrides Array of instrument control overrides.
/// @param control_override_count Number of instrument control overrides.
/// @param out_instrument Output instrument reference.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_CreateInstrument(
    BarelyEngine* engine, const BarelyInstrumentControlOverride* control_overrides,
    int32_t control_override_count, BarelyRef* out_instrument);

/// Creates a new performer.
///
/// @param engine Pointer to engine.
/// @param out_performer Output performer reference.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_CreatePerformer(BarelyEngine* engine, BarelyRef* out_performer);

/// Creates a new task.
///
/// @param performer Performer reference.
/// @param position Task position in beats.
/// @param duration Task duration in beats.
/// @param priority Task priority.
/// @param callback Task event callback.
/// @param user_data Pointer to user data.
/// @param out_task Output task reference.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_CreateTask(BarelyEngine* engine, BarelyRef performer, double position,
                                        double duration, int32_t priority,
                                        BarelyTaskEventCallback callback, void* user_data,
                                        BarelyRef* out_task);

/// Destroys an engine.
///
/// @param engine Pointer to engine.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_Destroy(BarelyEngine* engine);

/// Destroys an instrument.
///
/// @param engine Pointer to engine.
/// @param instrument Instrument reference.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_DestroyInstrument(BarelyEngine* engine, BarelyRef instrument);

/// Destroys a performer.
///
/// @param performer Performer reference.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_DestroyPerformer(BarelyEngine* engine, BarelyRef performer);

/// Destroys a task.
///
/// @param task Task reference.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_DestroyTask(BarelyEngine* engine, BarelyRef task);

/// Generates a new random number with uniform distribution in the normalized range [0, 1).
///
/// @param engine Pointer to engine.
/// @param out_number Output random number.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_GenerateRandomNumber(BarelyEngine* engine, double* out_number);

/// Gets a control value of an engine.
///
/// @param engine Pointer to engine.
/// @param type Engine control type.
/// @param out_value Output engine control value.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_GetControl(const BarelyEngine* engine, BarelyEngineControlType type,
                                        float* out_value);

/// Gets the random number generator seed of an engine.
///
/// @param engine Pointer to engine.
/// @param out_seed Output seed value.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_GetSeed(const BarelyEngine* engine, int32_t* out_seed);

/// Gets the tempo of an engine.
///
/// @param engine Pointer to engine.
/// @param out_tempo Output tempo in beats per minute.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_GetTempo(const BarelyEngine* engine, double* out_tempo);

/// Gets the timestamp of an engine.
///
/// @param engine Pointer to engine.
/// @param out_timestamp Output timestamp in seconds.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_GetTimestamp(const BarelyEngine* engine, double* out_timestamp);

/// Processes the next output samples of an engine at timestamp.
///
/// @param engine Pointer to engine.
/// @param output_samples Array of interleaved output samples.
/// @param output_channel_count Number of output channels.
/// @param output_frame_count Number of output frames.
/// @param timestamp Timestamp in seconds.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_Process(BarelyEngine* engine, float* output_samples,
                                     int32_t output_channel_count, int32_t output_frame_count,
                                     double timestamp);

/// Sets a control value of an engine.
///
/// @param engine Pointer to engine.
/// @param type Engine control type.
/// @param value Engine control value.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_SetControl(BarelyEngine* engine, BarelyEngineControlType type,
                                        float value);

/// Sets the random number generator seed of an engine.
///
/// @param engine Pointer to engine.
/// @param seed Seed value.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_SetSeed(BarelyEngine* engine, int32_t seed);

/// Sets the tempo of an engine.
///
/// @param engine Pointer to engine.
/// @param tempo Tempo in beats per minute.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_SetTempo(BarelyEngine* engine, double tempo);

/// Updates an engine at timestamp.
///
/// @param engine Pointer to engine.
/// @param timestamp Timestamp in seconds.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_Update(BarelyEngine* engine, double timestamp);

/// Gets an instrument control value.
///
/// @param engine Pointer to engine.
/// @param instrument Instrument reference.
/// @param type Instrument control type.
/// @param out_value Output instrument control value.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_GetControl(const BarelyEngine* engine, BarelyRef instrument,
                                            BarelyInstrumentControlType type, float* out_value);

/// Gets an instrument note control value.
///
/// @param engine Pointer to engine.
/// @param instrument Instrument reference.
/// @param pitch Note pitch.
/// @param type Note control type.
/// @param out_value Output note control value.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_GetNoteControl(const BarelyEngine* engine, BarelyRef instrument,
                                                float pitch, BarelyNoteControlType type,
                                                float* out_value);

/// Gets whether an instrument note is on or not.
///
/// @param engine Pointer to engine.
/// @param instrument Instrument reference.
/// @param pitch Note pitch.
/// @param out_is_note_on Output true if on, false otherwise.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_IsNoteOn(const BarelyEngine* engine, BarelyRef instrument,
                                          float pitch, bool* out_is_note_on);

/// Sets all instrument notes off.
///
/// @param engine Pointer to engine.
/// @param instrument Instrument reference.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_SetAllNotesOff(BarelyEngine* engine, BarelyRef instrument);

/// Sets an instrument control value.
///
/// @param engine Pointer to engine.
/// @param instrument Instrument reference.
/// @param type Instrument control type.
/// @param value Instrument control value.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_SetControl(BarelyEngine* engine, BarelyRef instrument,
                                            BarelyInstrumentControlType type, float value);

/// Sets an instrument note control value.
///
/// @param engine Pointer to engine.
/// @param instrument Instrument reference.
/// @param pitch Note pitch.
/// @param type Note control type.
/// @param value Note control value.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_SetNoteControl(BarelyEngine* engine, BarelyRef instrument,
                                                float pitch, BarelyNoteControlType type,
                                                float value);

/// Sets the note event callback of an instrument.
///
/// @param engine Pointer to engine.
/// @param instrument Instrument reference.
/// @param callback Note event callback.
/// @param user_data Pointer to user data.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_SetNoteEventCallback(BarelyEngine* engine, BarelyRef instrument,
                                                      BarelyNoteEventCallback callback,
                                                      void* user_data);

/// Sets an instrument note off.
///
/// @param engine Pointer to engine.
/// @param instrument Instrument reference.
/// @param pitch Note pitch.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_SetNoteOff(BarelyEngine* engine, BarelyRef instrument,
                                            float pitch);

/// Sets an instrument note on.
///
/// @param engine Pointer to engine.
/// @param instrument Instrument reference.
/// @param pitch Note pitch.
/// @param note_control_overrides Array of note control overrides.
/// @param note_control_override_count Number of note control overrides.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_SetNoteOn(BarelyEngine* engine, BarelyRef instrument, float pitch,
                                           const BarelyNoteControlOverride* note_control_overrides,
                                           int32_t note_control_override_count);

/// Sets instrument sample data.
///
/// @param engine Pointer to engine.
/// @param instrument Instrument reference.
/// @param slices Array of slices.
/// @param slice_count Number of slices.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_SetSampleData(BarelyEngine* engine, BarelyRef instrument,
                                               const BarelySlice* slices, int32_t slice_count);

/// Gets the loop begin position of a performer.
///
/// @param engine Pointer to engine.
/// @param performer Performer reference.
/// @param out_loop_begin_position Output loop begin position in beats.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_GetLoopBeginPosition(const BarelyEngine* engine,
                                                     BarelyRef performer,
                                                     double* out_loop_begin_position);

/// Gets the loop length of a performer.
///
/// @param engine Pointer to engine.
/// @param performer Performer reference.
/// @param out_loop_length Output loop length.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_GetLoopLength(const BarelyEngine* engine, BarelyRef performer,
                                              double* out_loop_length);

/// Gets the position of a performer.
///
/// @param engine Pointer to engine.
/// @param performer Performer reference.
/// @param out_position Output position in beats.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_GetPosition(const BarelyEngine* engine, BarelyRef performer,
                                            double* out_position);

/// Gets whether a performer is looping or not.
///
/// @param engine Pointer to engine.
/// @param performer Performer reference.
/// @param out_is_looping Output true if looping, false otherwise.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_IsLooping(const BarelyEngine* engine, BarelyRef performer,
                                          bool* out_is_looping);

/// Gets whether a performer is playing or not.
///
/// @param engine Pointer to engine.
/// @param performer Performer reference.
/// @param out_is_playing Output true if playing, false otherwise.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_IsPlaying(const BarelyEngine* engine, BarelyRef performer,
                                          bool* out_is_playing);

/// Sets the loop begin position of a performer.
///
/// @param engine Pointer to engine.
/// @param performer Performer reference.
/// @param loop_begin_position Loop begin position in beats.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_SetLoopBeginPosition(BarelyEngine* engine, BarelyRef performer,
                                                     double loop_begin_position);

/// Sets the loop length of a performer.
///
/// @param engine Pointer to engine.
/// @param performer Performer reference.
/// @param loop_length Loop length in beats.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_SetLoopLength(BarelyEngine* engine, BarelyRef performer,
                                              double loop_length);

/// Sets whether a performer is looping or not.
///
/// @param engine Pointer to engine.
/// @param performer Performer reference.
/// @param is_looping True if looping, false otherwise.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_SetLooping(BarelyEngine* engine, BarelyRef performer,
                                           bool is_looping);

/// Sets the position of a performer.
///
/// @param engine Pointer to engine.
/// @param performer Performer reference.
/// @param position Position in beats.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_SetPosition(BarelyEngine* engine, BarelyRef performer,
                                            double position);

/// Starts a performer.
///
/// @param engine Pointer to engine.
/// @param performer Performer reference.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_Start(BarelyEngine* engine, BarelyRef performer);

/// Stops a performer.
///
/// @param engine Pointer to engine.
/// @param performer Performer reference.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_Stop(BarelyEngine* engine, BarelyRef performer);

/// Gets the duration of a task.
///
/// @param engine Pointer to engine.
/// @param task Task reference.
/// @param out_duration Output duration in beats.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyTask_GetDuration(const BarelyEngine* engine, BarelyRef task,
                                       double* out_duration);

/// Gets the position of a task.
///
/// @param engine Pointer to engine.
/// @param task Task reference.
/// @param out_position Output position in beats.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyTask_GetPosition(const BarelyEngine* engine, BarelyRef task,
                                       double* out_position);

/// Gets the priority of a task.
///
/// @param engine Pointer to engine.
/// @param task Task reference.
/// @param out_priority Output priority.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyTask_GetPriority(const BarelyEngine* engine, BarelyRef task,
                                       int32_t* out_priority);

/// Gets whether the task is active or not.
///
/// @param engine Pointer to engine.
/// @param task Task reference.
/// @param out_is_active Output true if active, false otherwise.
BARELY_API bool BarelyTask_IsActive(const BarelyEngine* engine, BarelyRef task,
                                    bool* out_is_active);

/// Sets the duration of a task.
///
/// @param engine Pointer to engine.
/// @param task Task reference.
/// @param duration Duration in beats.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyTask_SetDuration(BarelyEngine* engine, BarelyRef task, double duration);

/// Sets the event callback of a task.
///
/// @param engine Pointer to engine.
/// @param task Task reference.
/// @param callback Event callback.
/// @param user_data Pointer to user data.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyTask_SetEventCallback(BarelyEngine* engine, BarelyRef task,
                                            BarelyTaskEventCallback callback, void* user_data);

/// Sets the position of a task.
///
/// @param engine Pointer to engine.
/// @param task Task reference.
/// @param position Position in beats.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyTask_SetPosition(BarelyEngine* engine, BarelyRef task, double position);

/// Sets the priority of a task.
///
/// @param engine Pointer to engine.
/// @param task Task reference.
/// @param priority Priority.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyTask_SetPriority(BarelyEngine* engine, BarelyRef task, int32_t priority);

/// Gets a quantized position.
///
/// @param quantization Pointer to quantization.
/// @param position Position.
/// @param out_position Output position.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyQuantization_GetPosition(const BarelyQuantization* quantization,
                                               double position, double* out_position);

/// Gets a scale note pitch for a given degree.
///
/// @param scale Pointer to scale.
/// @param degree Scale degree.
/// @param out_pitch Output note pitch.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyScale_GetPitch(const BarelyScale* scale, int32_t degree, float* out_pitch);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
// NOLINTEND

/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// barelymusician C++ API
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef __cplusplus
#include <array>
#include <cassert>
#include <cstdint>
#include <functional>
#include <optional>
#include <random>
#include <span>
#include <type_traits>
#include <utility>

namespace barely {

/// Instrument control types.
enum class InstrumentControlType {
  /// Gain in linear amplitude.
  kGain = BarelyInstrumentControlType_kGain,
  /// Pitch shift.
  kPitchShift = BarelyInstrumentControlType_kPitchShift,
  /// Retrigger.
  kRetrigger = BarelyInstrumentControlType_kRetrigger,
  /// Stereo pan.
  kStereoPan = BarelyInstrumentControlType_kStereoPan,
  /// Number of voices.
  kVoiceCount = BarelyInstrumentControlType_kVoiceCount,
  /// Envelope attack in seconds.
  kAttack = BarelyInstrumentControlType_kAttack,
  /// Envelope decay in seconds.
  kDecay = BarelyInstrumentControlType_kDecay,
  /// Envelope sustain.
  kSustain = BarelyInstrumentControlType_kSustain,
  /// Envelope release in seconds.
  kRelease = BarelyInstrumentControlType_kRelease,
  /// Oscillator mix.
  kOscMix = BarelyInstrumentControlType_kOscMix,
  /// Oscillator mode.
  kOscMode = BarelyInstrumentControlType_kOscMode,
  /// Oscillator noise mix.
  kOscNoiseMix = BarelyInstrumentControlType_kOscNoiseMix,
  /// Oscillator pitch shift.
  kOscPitchShift = BarelyInstrumentControlType_kOscPitchShift,
  /// Oscillator shape.
  kOscShape = BarelyInstrumentControlType_kOscShape,
  /// Oscillator skew.
  kOscSkew = BarelyInstrumentControlType_kOscSkew,
  /// Slice mode.
  kSliceMode = BarelyInstrumentControlType_kSliceMode,
  /// Bit crusher depth.
  kBitCrusherDepth = BarelyInstrumentControlType_kBitCrusherDepth,
  /// Normalized bit crusher rate.
  kBitCrusherRate = BarelyInstrumentControlType_kBitCrusherRate,
  /// Distortion amount.
  kDistortionAmount = BarelyInstrumentControlType_kDistortionAmount,
  /// Distortion drive.
  kDistortionDrive = BarelyInstrumentControlType_kDistortionDrive,
  /// Filter type.
  kFilterType = BarelyInstrumentControlType_kFilterType,
  /// Filter frequency in hertz.
  kFilterFrequency = BarelyInstrumentControlType_kFilterFrequency,
  /// Filter Q factor.
  kFilterQ = BarelyInstrumentControlType_kFilterQ,
  /// Delay send.
  kDelaySend = BarelyInstrumentControlType_kDelaySend,
  /// Sidechain send.
  kSidechainSend = BarelyInstrumentControlType_kSidechainSend,
  /// Arpeggiator mode.
  kArpMode = BarelyInstrumentControlType_kArpMode,
  /// Arpeggiator gate ratio.
  kArpGateRatio = BarelyInstrumentControlType_kArpGateRatio,
  /// Arpeggiator rate.
  kArpRate = BarelyInstrumentControlType_kArpRate,
};

/// Engine control types.
enum class EngineControlType {
  /// Compressor mix.
  kCompressorMix = BarelyEngineControlType_kCompressorMix,
  /// Compressor attack in seconds.
  kCompressorAttack = BarelyEngineControlType_kCompressorAttack,
  /// Compressor release in seconds.
  kCompressorRelease = BarelyEngineControlType_kCompressorRelease,
  /// Compressor threshold.
  kCompressorThreshold = BarelyEngineControlType_kCompressorThreshold,
  /// Compressor ratio.
  kCompressorRatio = BarelyEngineControlType_kCompressorRatio,
  /// Delay mix.
  kDelayMix = BarelyEngineControlType_kDelayMix,
  /// Delay time in seconds.
  kDelayTime = BarelyEngineControlType_kDelayTime,
  /// Delay feedback.
  kDelayFeedback = BarelyEngineControlType_kDelayFeedback,
  /// Delay low-pass frequency.
  kDelayLowPassFrequency = BarelyEngineControlType_kDelayLowPassFrequency,
  /// Delay high-pass frequency.
  kDelayHighPassFrequency = BarelyEngineControlType_kDelayHighPassFrequency,
  /// Sidechain mix.
  kSidechainMix = BarelyEngineControlType_kSidechainMix,
  /// Sidechain attack in seconds.
  kSidechainAttack = BarelyEngineControlType_kSidechainAttack,
  /// Sidechain release in seconds.
  kSidechainRelease = BarelyEngineControlType_kSidechainRelease,
  /// Sidechain threshold.
  kSidechainThreshold = BarelyEngineControlType_kSidechainThreshold,
  /// Sidechain ratio.
  kSidechainRatio = BarelyEngineControlType_kSidechainRatio,
};

/// Note control types.
enum class NoteControlType {
  /// Gain in linear amplitude.
  kGain = BarelyNoteControlType_kGain,
  /// Pitch shift.
  kPitchShift = BarelyNoteControlType_kPitchShift,
};

/// Arpeggiator modes.
enum class ArpMode {
  /// None.
  kNone = BarelyArpMode_kNone,
  /// Up.
  kUp = BarelyArpMode_kUp,
  /// Down.
  kDown = BarelyArpMode_kDown,
  /// Random.
  kRandom = BarelyArpMode_kRandom,
};

/// Filter types.
enum class FilterType {
  /// None.
  kNone = BarelyFilterType_kNone,
  /// Low pass.
  kLowPass = BarelyFilterType_kLowPass,
  /// High pass.
  kHighPass = BarelyFilterType_kHighPass,
};

/// Oscillator modes.
enum class OscMode {
  /// Mix.
  kMix = BarelyOscMode_kMix,
  /// Amplitude modulation by the oscillator applied to the slice.
  kAm = BarelyOscMode_kAm,
  /// Amplitude modulation by the slice applied to the oscillator.
  kEnvelopeFollower = BarelyOscMode_kEnvelopeFollower,
  /// Frequency modulation by the oscillator applied to the slice.
  kFm = BarelyOscMode_kFm,
  /// Frequency modulation by the slice applied to the oscillator.
  kMf = BarelyOscMode_kMf,
  /// Ring modulation.
  kRing = BarelyOscMode_kRing,
};

/// Slice modes.
enum class SliceMode {
  /// Sustain.
  kSustain = BarelySliceMode_kSustain,
  /// Loop.
  kLoop = BarelySliceMode_kLoop,
  /// Once.
  kOnce = BarelySliceMode_kOnce,
};

/// Note event types.
enum class NoteEventType {
  /// Begin.
  kBegin = BarelyNoteEventType_kBegin,
  /// End.
  kEnd = BarelyNoteEventType_kEnd,
};

/// Task event types.
enum class TaskEventType {
  /// Begin.
  kBegin = BarelyTaskEventType_kBegin,
  /// End.
  kEnd = BarelyTaskEventType_kEnd,
};

/// Instrument control override.
struct InstrumentControlOverride : public BarelyInstrumentControlOverride {
  /// Default constructor.
  InstrumentControlOverride() noexcept = default;

  /// Constructs a new `InstrumentControlOverride`.
  ///
  /// @param type Instrument control type.
  /// @param value Instrument control value.
  template <typename ValueType>
  InstrumentControlOverride(InstrumentControlType type, ValueType value) noexcept
      : BarelyInstrumentControlOverride{static_cast<BarelyInstrumentControlType>(type),
                                        static_cast<float>(value)} {
    static_assert(std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
                  "ValueType is not supported");
  }
};

/// Note control override.
struct NoteControlOverride : public BarelyNoteControlOverride {
  /// Default constructor.
  NoteControlOverride() noexcept = default;

  /// Constructs a new `NoteControlOverride`.
  ///
  /// @param type Note control type.
  /// @param value Note control value.
  template <typename ValueType>
  NoteControlOverride(NoteControlType type, ValueType value) noexcept
      : BarelyNoteControlOverride{static_cast<BarelyNoteControlType>(type),
                                  static_cast<float>(value)} {
    static_assert(std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
                  "ValueType is not supported");
  }
};

/// Slice of sample data.
struct Slice : public BarelySlice {
  /// Constructs a new `Slice`.
  ///
  /// @param root_pitch Root pitch.
  /// @param sample_rate Sampling rate in hertz.
  /// @param samples Span of mono samples.
  explicit constexpr Slice(float root_pitch, int sample_rate,
                           std::span<const float> samples) noexcept
      : Slice({root_pitch, sample_rate, samples.data(), static_cast<int32_t>(samples.size())}) {
    assert(sample_rate >= 0);
  }

  /// Constructs a new `Slice` from a raw type.
  ///
  /// @param slice Raw slice.
  // NOLINTNEXTLINE(google-explicit-constructor)
  constexpr Slice(BarelySlice slice) noexcept : BarelySlice{slice} {}
};

/// Note event callback function.
///
/// @param type Note event type.
/// @param pitch Note pitch.
using NoteEventCallback = std::function<void(NoteEventType type, float pitch)>;

/// Task event callback function.
///
/// @param type Task event type.
using TaskEventCallback = std::function<void(TaskEventType type)>;

/// Class that wraps an instrument reference.
class Instrument {
 public:
  /// Constructs a new `Instrument` from a raw reference.
  ///
  /// @param engine Raw engine handle.
  /// @param instrument Raw instrument reference.
  /// @param note_event_callback Pointer to note event callback.
  Instrument(BarelyEngine* engine, BarelyRef instrument,
             NoteEventCallback* note_event_callback) noexcept
      : engine_(engine), instrument_(instrument), note_event_callback_(note_event_callback) {}

  /// Returns the raw reference.
  ///
  /// @return Raw reference.
  [[nodiscard]] constexpr operator BarelyRef() const noexcept { return instrument_; }

  /// Returns a control value.
  ///
  /// @param type Instrument control type.
  /// @return Instrument control value.
  template <typename ValueType>
  [[nodiscard]] ValueType GetControl(InstrumentControlType type) const noexcept {
    static_assert(std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
                  "ValueType is not supported");
    float value = 0.0f;
    [[maybe_unused]] const bool success = BarelyInstrument_GetControl(
        engine_, instrument_, static_cast<BarelyInstrumentControlType>(type), &value);
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
        engine_, instrument_, pitch, static_cast<BarelyNoteControlType>(type), &value);
    assert(success);
    return static_cast<ValueType>(value);
  }

  /// Returns whether a note is on or not.
  ///
  /// @param pitch Note pitch.
  /// @return True if on, false otherwise.
  [[nodiscard]] bool IsNoteOn(float pitch) const noexcept {
    bool is_note_on = false;
    [[maybe_unused]] const bool success =
        BarelyInstrument_IsNoteOn(engine_, instrument_, pitch, &is_note_on);
    assert(success);
    return is_note_on;
  }

  /// Sets all notes off.
  void SetAllNotesOff() noexcept {
    [[maybe_unused]] const bool success = BarelyInstrument_SetAllNotesOff(engine_, instrument_);
    assert(success);
  }

  /// Sets a control value.
  ///
  /// @param type Instrument control type.
  /// @param value Instrument control value.
  template <typename ValueType>
  void SetControl(InstrumentControlType type, ValueType value) noexcept {
    static_assert(std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
                  "ValueType is not supported");
    [[maybe_unused]] const bool success = BarelyInstrument_SetControl(
        engine_, instrument_, static_cast<BarelyInstrumentControlType>(type),
        static_cast<float>(value));
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
        engine_, instrument_, pitch, static_cast<BarelyNoteControlType>(type),
        static_cast<float>(value));
    assert(success);
  }

  /// Sets the note event callback.
  ///
  /// @param callback Note event callback.
  void SetNoteEventCallback(NoteEventCallback callback) noexcept {
    assert(note_event_callback_ != nullptr);
    *note_event_callback_ = std::move(callback);
    [[maybe_unused]] const bool success =
        (*note_event_callback_)
            ? BarelyInstrument_SetNoteEventCallback(
                  engine_, instrument_,
                  [](BarelyNoteEventType type, float pitch, void* user_data) noexcept {
                    assert(user_data != nullptr && "Invalid note event callback user data");
                    if (const auto& callback = *static_cast<NoteEventCallback*>(user_data);
                        callback) {
                      callback(static_cast<NoteEventType>(type), pitch);
                    }
                  },
                  note_event_callback_)
            : BarelyInstrument_SetNoteEventCallback(engine_, instrument_, nullptr, nullptr);
    assert(success);
  }

  /// Sets a note off.
  ///
  /// @param pitch Note pitch.
  void SetNoteOff(float pitch) noexcept {
    [[maybe_unused]] const bool success = BarelyInstrument_SetNoteOff(engine_, instrument_, pitch);
    assert(success);
  }

  /// Sets a note on.
  ///
  /// @param pitch Note pitch.
  /// @param note_control_overrides Span of note control overrides.
  void SetNoteOn(float pitch,
                 std::span<const NoteControlOverride> note_control_overrides = {}) noexcept {
    static_assert(sizeof(BarelyNoteControlOverride) == sizeof(NoteControlOverride));
    [[maybe_unused]] const bool success = BarelyInstrument_SetNoteOn(
        engine_, instrument_, pitch,
        reinterpret_cast<const BarelyNoteControlOverride*>(note_control_overrides.data()),
        static_cast<int32_t>(note_control_overrides.size()));
    assert(success);
  }

  /// Sets a note on.
  ///
  /// @param pitch Note pitch.
  /// @param gain Note gain.
  void SetNoteOn(float pitch, float gain) noexcept {
    return SetNoteOn(pitch, {{{NoteControlType::kGain, gain}}});
  }

  /// Sets a note on.
  ///
  /// @param pitch Note pitch.
  /// @param gain Note gain.
  /// @param pitch_shift Note pitch shift.
  void SetNoteOn(float pitch, float gain, float pitch_shift) noexcept {
    return SetNoteOn(pitch, {{
                                {NoteControlType::kGain, gain},
                                {NoteControlType::kPitchShift, pitch_shift},
                            }});
  }

  /// Sets the sample data.
  ///
  /// @param slices Span of slices.
  void SetSampleData(std::span<const Slice> slices) noexcept {
    [[maybe_unused]] const bool success = BarelyInstrument_SetSampleData(
        engine_, instrument_, reinterpret_cast<const BarelySlice*>(slices.data()),
        static_cast<int32_t>(slices.size()));
    assert(success);
  }

 private:
  // Raw engine pointer.
  BarelyEngine* engine_ = nullptr;

  // Raw instrument reference.
  BarelyRef instrument_ = {};

  // Pointer to note event callback.
  NoteEventCallback* note_event_callback_ = nullptr;
};

/// Class that wraps a task reference.
class Task {
 public:
  /// Constructs a new `Task` from a raw reference.
  ///
  /// @param engine Raw engine pointer.
  /// @param task Raw task reference.
  /// @param event_callback Pointer to task event callback.
  Task(BarelyEngine* engine, BarelyRef task, TaskEventCallback* event_callback) noexcept
      : engine_(engine), task_(task), event_callback_(event_callback) {}

  /// Returns the raw reference.
  ///
  /// @return Raw reference.
  [[nodiscard]] constexpr operator BarelyRef() const noexcept { return task_; }

  /// Returns the duration.
  ///
  /// @return Duration in beats.
  [[nodiscard]] double GetDuration() const noexcept {
    double duration = 0.0;
    [[maybe_unused]] const bool success = BarelyTask_GetDuration(engine_, task_, &duration);
    assert(success);
    return duration;
  }

  /// Returns the position.
  ///
  /// @return Position in beats.
  [[nodiscard]] double GetPosition() const noexcept {
    double position = 0.0;
    [[maybe_unused]] const bool success = BarelyTask_GetPosition(engine_, task_, &position);
    assert(success);
    return position;
  }

  /// Returns the priority.
  ///
  /// @return Priority.
  [[nodiscard]] int GetPriority() const noexcept {
    int32_t priority = 0;
    [[maybe_unused]] const bool success = BarelyTask_GetPriority(engine_, task_, &priority);
    assert(success);
    return static_cast<int>(priority);
  }

  /// Returns whether the task is active or not.
  ///
  /// @return True if active, false otherwise.
  [[nodiscard]] bool IsActive() const noexcept {
    bool is_active = false;
    [[maybe_unused]] const bool success = BarelyTask_IsActive(engine_, task_, &is_active);
    assert(success);
    return is_active;
  }

  /// Sets the duration.
  ///
  /// @param duration Duration in beats.
  void SetDuration(double duration) noexcept {
    [[maybe_unused]] const bool success = BarelyTask_SetDuration(engine_, task_, duration);
    assert(success);
  }

  /// Sets the event callback.
  ///
  /// @param callback Event callback.
  void SetEventCallback(TaskEventCallback callback) noexcept {
    assert(event_callback_ != nullptr);
    if (*event_callback_) {
      [[maybe_unused]] const bool success =
          BarelyTask_SetEventCallback(engine_, task_, nullptr, nullptr);
      assert(success);
    }
    *event_callback_ = std::move(callback);
    [[maybe_unused]] const bool success =
        (*event_callback_)
            ? BarelyTask_SetEventCallback(
                  engine_, task_,
                  [](BarelyTaskEventType type, void* user_data) noexcept {
                    assert(user_data != nullptr && "Invalid task event callback user data");
                    if (const auto& callback = *static_cast<TaskEventCallback*>(user_data);
                        callback) {
                      callback(static_cast<TaskEventType>(type));
                    }
                  },
                  event_callback_)
            : BarelyTask_SetEventCallback(engine_, task_, nullptr, nullptr);
    assert(success);
  }

  /// Sets the position.
  ///
  /// @param position Position in beats.
  void SetPosition(double position) noexcept {
    [[maybe_unused]] const bool success = BarelyTask_SetPosition(engine_, task_, position);
    assert(success);
  }

  /// Sets the priority.
  ///
  /// @param priority Priority.
  void SetPriority(int priority) noexcept {
    [[maybe_unused]] const bool success =
        BarelyTask_SetPriority(engine_, task_, static_cast<int32_t>(priority));
    assert(success);
  }

 private:
  // Pointer to engine.
  BarelyEngine* engine_ = nullptr;

  // Raw task reference.
  BarelyRef task_ = {};

  // Pointer to event callback.
  TaskEventCallback* event_callback_ = nullptr;
};

/// Class that wraps a performer reference.
class Performer {
 public:
  /// Constructs a new `Performer` from a raw reference.
  ///
  /// @param engine Raw engine pointer.
  /// @param performer Raw performer reference.
  Performer(BarelyEngine* engine, BarelyRef performer) noexcept
      : engine_(engine), performer_(performer) {}

  /// Returns the raw reference.
  ///
  /// @return Raw reference.
  [[nodiscard]] constexpr operator BarelyRef() const noexcept { return performer_; }

  /// Returns the loop begin position.
  ///
  /// @return Loop begin position in beats.
  [[nodiscard]] double GetLoopBeginPosition() const noexcept {
    double loop_begin_position = 0.0;
    [[maybe_unused]] const bool success =
        BarelyPerformer_GetLoopBeginPosition(engine_, performer_, &loop_begin_position);
    assert(success);
    return loop_begin_position;
  }

  /// Returns the loop length.
  ///
  /// @return Loop length in beats.
  [[nodiscard]] double GetLoopLength() const noexcept {
    double loop_length = 0.0;
    [[maybe_unused]] const bool success =
        BarelyPerformer_GetLoopLength(engine_, performer_, &loop_length);
    assert(success);
    return loop_length;
  }

  /// Returns the position.
  ///
  /// @return Position in beats.
  [[nodiscard]] double GetPosition() const noexcept {
    double position = 0.0;
    [[maybe_unused]] const bool success =
        BarelyPerformer_GetPosition(engine_, performer_, &position);
    assert(success);
    return position;
  }

  /// Returns whether the performer is looping or not.
  ///
  /// @return True if looping, false otherwise.
  [[nodiscard]] bool IsLooping() const noexcept {
    bool is_looping = false;
    [[maybe_unused]] const bool success =
        BarelyPerformer_IsLooping(engine_, performer_, &is_looping);
    assert(success);
    return is_looping;
  }

  /// Returns whether the performer is playing or not.
  ///
  /// @return True if playing, false otherwise.
  [[nodiscard]] bool IsPlaying() const noexcept {
    bool is_playing = false;
    [[maybe_unused]] const bool success =
        BarelyPerformer_IsPlaying(engine_, performer_, &is_playing);
    assert(success);
    return is_playing;
  }

  /// Sets the loop begin position.
  ///
  /// @param loop_begin_position Loop begin position in beats.
  void SetLoopBeginPosition(double loop_begin_position) noexcept {
    [[maybe_unused]] const bool success =
        BarelyPerformer_SetLoopBeginPosition(engine_, performer_, loop_begin_position);
    assert(success);
  }

  /// Sets the loop length.
  ///
  /// @param loop_length Loop length in beats.
  void SetLoopLength(double loop_length) noexcept {
    [[maybe_unused]] const bool success =
        BarelyPerformer_SetLoopLength(engine_, performer_, loop_length);
    assert(success);
  }

  /// Sets whether the performer is looping or not.
  ///
  /// @param is_looping True if looping, false otherwise.
  void SetLooping(bool is_looping) noexcept {
    [[maybe_unused]] const bool success =
        BarelyPerformer_SetLooping(engine_, performer_, is_looping);
    assert(success);
  }

  /// Sets the position.
  ///
  /// @param position Position in beats.
  void SetPosition(double position) noexcept {
    [[maybe_unused]] const bool success =
        BarelyPerformer_SetPosition(engine_, performer_, position);
    assert(success);
  }

  /// Starts the performer.
  void Start() noexcept {
    [[maybe_unused]] const bool success = BarelyPerformer_Start(engine_, performer_);
    assert(success);
  }

  /// Stops the performer.
  void Stop() noexcept {
    [[maybe_unused]] const bool success = BarelyPerformer_Stop(engine_, performer_);
    assert(success);
  }

 private:
  // Raw engine pointer.
  BarelyEngine* engine_;

  // Raw performer reference.
  BarelyRef performer_;
};

/// A class that wraps an engine handle.
class Engine {
 public:
  /// Constructs a new `Engine`.
  ///
  /// @param sample_rate Sampling rate in hertz.
  explicit Engine(int sample_rate) noexcept {
    [[maybe_unused]] const bool success =
        BarelyEngine_Create(static_cast<int32_t>(sample_rate), &engine_);
    assert(success);
  }

  /// Constructs a new `Engine` from a raw handle.
  ///
  /// @param engine Raw engine pointer.
  explicit Engine(BarelyEngine* engine) noexcept : engine_(engine) { assert(engine != nullptr); }

  /// Destroys `Engine`.
  ~Engine() noexcept { BarelyEngine_Destroy(engine_); }

  /// Non-copyable.
  Engine(const Engine& other) noexcept = delete;
  Engine& operator=(const Engine& other) noexcept = delete;

  /// Constructs a new `Engine` via move.
  ///
  /// @param other Other engine.
  Engine(Engine&& other) noexcept
      : engine_(std::exchange(other.engine_, nullptr)),
        note_event_callbacks_(std::exchange(other.note_event_callbacks_, {})),
        task_event_callbacks_(std::exchange(other.task_event_callbacks_, {})) {}

  /// Assigns `Engine` via move.
  ///
  /// @param other Other engine.
  /// @return Engine.
  Engine& operator=(Engine&& other) noexcept {
    if (this != &other) {
      BarelyEngine_Destroy(engine_);
      engine_ = std::exchange(other.engine_, nullptr);
      note_event_callbacks_ = std::exchange(other.note_event_callbacks_, {});
      task_event_callbacks_ = std::exchange(other.task_event_callbacks_, {});
    }
    return *this;
  }

  /// Returns the raw handle.
  ///
  /// @return Raw handle.
  [[nodiscard]] constexpr operator BarelyEngine*() const noexcept { return engine_; }

  /// Creates a new instrument.
  ///
  /// @param control_overrides Span of instrument control overrides.
  /// @return Instrument reference.
  [[nodiscard]] Instrument CreateInstrument(
      std::span<const InstrumentControlOverride> control_overrides = {}) noexcept {
    BarelyRef instrument = {};
    [[maybe_unused]] const bool success = BarelyEngine_CreateInstrument(
        engine_, reinterpret_cast<const BarelyInstrumentControlOverride*>(control_overrides.data()),
        static_cast<int32_t>(control_overrides.size()), &instrument);
    assert(success);
    note_event_callbacks_.get()[instrument.index] = {};
    return Instrument(engine_, instrument, &note_event_callbacks_.get()[instrument.index]);
  }

  /// Creates a new performer.
  ///
  /// @return Performer reference.
  [[nodiscard]] Performer CreatePerformer() noexcept {
    BarelyRef performer = {};
    [[maybe_unused]] const bool success = BarelyEngine_CreatePerformer(engine_, &performer);
    assert(success);
    return Performer(engine_, performer);
  }

  /// Creates a new task.
  ///
  /// @param performer Performer reference.
  /// @param position Task position in beats.
  /// @param duration Task duration in beats.
  /// @param priority Task priority.
  /// @param callback Task event callback.
  /// @return Task reference.
  [[nodiscard]] Task CreateTask(Performer performer, double position, double duration, int priority,
                                TaskEventCallback callback) noexcept {
    BarelyRef task = {};
    [[maybe_unused]] bool success = BarelyEngine_CreateTask(engine_, performer, position, duration,
                                                            priority, nullptr, nullptr, &task);
    assert(success);
    task_event_callbacks_.get()[task.index] = std::move(callback);
    success = BarelyTask_SetEventCallback(
        engine_, task,
        [](BarelyTaskEventType type, void* user_data) noexcept {
          if (user_data != nullptr) {
            (*static_cast<TaskEventCallback*>(user_data))(static_cast<TaskEventType>(type));
          }
        },
        &task_event_callbacks_.get()[task.index]);
    assert(success);
    return Task(engine_, task, &task_event_callbacks_[task.index]);
  }

  /// Destroys an instrument.
  ///
  /// @param instrument Instrument reference.
  void DestroyInstrument(Instrument instrument) {
    [[maybe_unused]] const bool success = BarelyEngine_DestroyInstrument(engine_, instrument);
    assert(success);
  }

  /// Destroys a performer.
  ///
  /// @param performer Performer reference.
  void DestroyPerformer(Performer performer) {
    [[maybe_unused]] const bool success = BarelyEngine_DestroyPerformer(engine_, performer);
    assert(success);
  }

  /// Destroys a task.
  ///
  /// @param task Task reference.
  void DestroyTask(Task task) {
    [[maybe_unused]] const bool success = BarelyEngine_DestroyTask(engine_, task);
    assert(success);
  }

  /// Generates a random number with uniform distribution in the normalized range [0, 1).
  ///
  /// @return Random number.
  [[nodiscard]] double GenerateRandomNumber() noexcept {
    double number = 0.0;
    [[maybe_unused]] const bool success = BarelyEngine_GenerateRandomNumber(engine_, &number);
    assert(success);
    return number;
  }

  /// Generates a random number with uniform distribution in the range [min, max).
  ///
  /// @param min Minimum value (inclusive).
  /// @param max Maximum value (exclusive).
  /// @return Random number.
  template <typename NumberType>
  [[nodiscard]] NumberType GenerateRandomNumber(NumberType min, NumberType max) noexcept {
    static_assert(std::is_arithmetic<NumberType>::value, "NumberType is not supported");
    return min + static_cast<NumberType>(GenerateRandomNumber() * static_cast<double>(max - min));
  }

  /// Returns a control value.
  ///
  /// @param type Engine control type.
  /// @return Engine control value.
  template <typename ValueType>
  [[nodiscard]] ValueType GetControl(EngineControlType type) const noexcept {
    static_assert(std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
                  "ValueType is not supported");
    float value = 0.0f;
    [[maybe_unused]] const bool success =
        BarelyEngine_GetControl(engine_, static_cast<BarelyEngineControlType>(type), &value);
    assert(success);
    return static_cast<ValueType>(value);
  }

  /// Returns the random number generator seed.
  ///
  /// @return Seed value.
  [[nodiscard]] int GetSeed() const noexcept {
    int32_t seed = 0;
    [[maybe_unused]] const bool success = BarelyEngine_GetSeed(engine_, &seed);
    assert(success);
    return static_cast<int>(seed);
  }

  /// Returns the tempo.
  ///
  /// @return Tempo in beats per minute.
  [[nodiscard]] double GetTempo() const noexcept {
    double tempo = 0.0;
    [[maybe_unused]] const bool success = BarelyEngine_GetTempo(engine_, &tempo);
    assert(success);
    return tempo;
  }

  /// Returns the timestamp.
  ///
  /// @return Timestamp in seconds.
  [[nodiscard]] double GetTimestamp() const noexcept {
    double timestamp = 0.0;
    [[maybe_unused]] const bool success = BarelyEngine_GetTimestamp(engine_, &timestamp);
    assert(success);
    return timestamp;
  }

  /// Processes the next output samples at timestamp.
  ///
  /// @param output_samples Array of interleaved output samples.
  /// @param output_channel_count Number of output channels.
  /// @param output_frame_count Number of output frames.
  /// @param timestamp Timestamp in seconds.
  void Process(float* output_samples, int output_channel_count, int output_frame_count,
               double timestamp) noexcept {
    [[maybe_unused]] const bool success =
        BarelyEngine_Process(engine_, output_samples, static_cast<int32_t>(output_channel_count),
                             static_cast<int32_t>(output_frame_count), timestamp);
    assert(success);
  }

  /// Sets a control value.
  ///
  /// @param type Engine control type.
  /// @param value Engine control value.
  template <typename ValueType>
  void SetControl(EngineControlType type, ValueType value) noexcept {
    static_assert(std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
                  "ValueType is not supported");
    [[maybe_unused]] const bool success = BarelyEngine_SetControl(
        engine_, static_cast<BarelyEngineControlType>(type), static_cast<float>(value));
    assert(success);
  }

  /// Sets the random number generator seed.
  void SetSeed(int seed) noexcept {
    [[maybe_unused]] const bool success = BarelyEngine_SetSeed(engine_, static_cast<int32_t>(seed));
    assert(success);
  }

  /// Sets the tempo.
  ///
  /// @param tempo Tempo in beats per minute.
  void SetTempo(double tempo) noexcept {
    [[maybe_unused]] const bool success = BarelyEngine_SetTempo(engine_, tempo);
    assert(success);
  }

  /// Updates the engine at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void Update(double timestamp) noexcept {
    [[maybe_unused]] const bool success = BarelyEngine_Update(engine_, timestamp);
    assert(success);
  }

 private:
  // Raw pointer.
  BarelyEngine* engine_ = nullptr;

  // Heap allocated array of note event callbacks (for pointer stability on move).
  std::unique_ptr<NoteEventCallback[]> note_event_callbacks_ =
      std::make_unique<NoteEventCallback[]>(BARELYMUSICIAN_MAX_INSTRUMENT_COUNT + 1);

  // Heap allocated array of task event callbacks (for pointer stability on move).
  std::unique_ptr<TaskEventCallback[]> task_event_callbacks_ =
      std::make_unique<TaskEventCallback[]>(BARELYMUSICIAN_MAX_TASK_COUNT + 1);
};

/// A musical quantization.
struct Quantization : public BarelyQuantization {
 public:
  /// Default constructor.
  constexpr Quantization() noexcept = default;

  /// Constructs a new `Quantization`.
  ///
  /// @param subdivision Subdivision of a beat.
  /// @param amount Amount.
  constexpr Quantization(int subdivision, float amount = 1.0f) noexcept
      : Quantization(BarelyQuantization{static_cast<int32_t>(subdivision), amount}) {}

  /// Constructs a new `Quantization` from a raw type.
  ///
  /// @param quantization Raw quantization.
  // NOLINTNEXTLINE(google-explicit-constructor)
  constexpr Quantization(BarelyQuantization quantization) noexcept
      : BarelyQuantization{quantization} {
    assert(subdivision > 0);
    assert(amount >= 0.0 && amount <= 1.0);
  }

  /// Returns the quantized position.
  ///
  /// @param position Position.
  /// @return Quantized position.
  [[nodiscard]] double GetPosition(double position) const noexcept {
    double quantized_position = 0.0;
    [[maybe_unused]] const bool success =
        BarelyQuantization_GetPosition(this, position, &quantized_position);
    assert(success);
    return quantized_position;
  }
};

/// A musical scale.
struct Scale : public BarelyScale {
 public:
  /// Default constructor.
  constexpr Scale() noexcept = default;

  /// Constructs a new `Scale`.
  ///
  /// @param pitches Span of pitches.
  /// @param root_pitch Root pitch.
  /// @param mode Mode.
  constexpr Scale(std::span<const float> pitches, float root_pitch = 0.0f, int mode = 0) noexcept
      : Scale(BarelyScale{pitches.data(), static_cast<int32_t>(pitches.size()), root_pitch,
                          static_cast<int32_t>(mode)}) {}

  /// Constructs a new `Scale` from a raw type.
  ///
  /// @param scale Raw scale.
  // NOLINTNEXTLINE(google-explicit-constructor)
  constexpr Scale(BarelyScale scale) noexcept : BarelyScale{scale} {
    assert(pitches != nullptr);
    assert(pitch_count > 0);
    assert(mode >= 0 && mode < pitch_count);
  }

  /// Returns the pitch for a given degree.
  ///
  /// @param degree Degree.
  /// @return Pitch.
  [[nodiscard]] float GetPitch(int degree) const noexcept {
    float pitch = 0.0f;
    [[maybe_unused]] const bool success = BarelyScale_GetPitch(this, degree, &pitch);
    assert(success);
    return pitch;
  }

  /// Returns the number of pitches in the scale.
  ///
  /// @return Number of pitches.
  [[nodiscard]] constexpr int GetPitchCount() const noexcept {
    return static_cast<int>(pitch_count);
  }
};

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_BARELYMUSICIAN_H_
