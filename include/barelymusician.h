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
///   barely::Engine engine(/*sample_rate=*/48000, /*max_frame_count=*/512);
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
///   @endcode
///
/// - Performer:
///
///   @code{.cpp}
///   // Create.
///   auto performer_ref = engine.CreatePerformer();
///
///   // Create a task.
///   auto task = performer_ref.CreateTask(/*position=*/0.0, /*duration=*/1.0,
///                                        [](barely::TaskEventType type) { /*populate this*/ });
///
///   // Set to looping.
///   performer_ref.SetLooping(/*is_looping=*/true);
///
///   // Start.
///   performer_ref.Start();
///
///   // Check if started playing.
///   const bool is_playing = performer_ref.IsPlaying();
///
///   // Destroy.
///   engine.DestroyPerformer(performer_ref);
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
///   BarelyEngineHandle engine = nullptr;
///   BarelyEngine_Create(/*sample_rate=*/48000, /*max_frame_count=*/512,
///                       BARELY_DEFAULT_REFERENCE_FREQUENCY, &engine);
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
///   BarelyInstrumentHandle instrument = nullptr;
///   BarelyInstrument_Create(engine, /*control_overrides=*/nullptr, /*control_override_count=*/0,
///                           &instrument);
///
///   // Set a note on.
///   //
///   // The note pitch is expressed in octaves relative to middle C as the center frequency.
///   // Fractional note values adjust the frequency logarithmically to ensure equally perceived
///   // pitch intervals within each octave.
///   float c3_pitch = -1.0f;
///   BarelyInstrument_SetNoteOn(instrument, c3_pitch, /*note_control_overrides=*/nullptr,
///                              /*note_control_override_count=*/0);
///
///   // Check if the note is on.
///   bool is_note_on = false;
///   BarelyInstrument_IsNoteOn(instrument, c3_pitch, &is_note_on);
///
///   // Set a control value.
///   BarelyInstrument_SetControl(instrument, BarelyInstrumentControlType_kOscMix, /*value=*/1.0f);
///
///   // Destroy.
///   BarelyInstrument_Destroy(instrument);
///   @endcode
///
/// - Performer:
///
///   @code{.cpp}
///   // Create.
///   BarelyPerformerRef performer_ref;
///   BarelyEngine_CreatePerformer(engine, &performer_ref);
///
///   // Create a task.
///   BarelyTaskHandle task = nullptr;
///   BarelyTaskEventCallback callback{ /*populate this*/ };
///   BarelyTask_Create(engine, performer_ref, /*position=*/0.0, /*duration=*/1.0, /*priority=*/0,
///                     callback, &task);
///
///   // Set to looping.
///   BarelyPerformer_SetLooping(engine, performer_ref, /*is_looping=*/true);
///
///   // Start.
///   BarelyPerformer_Start(engine, performer_ref);
///
///   // Check if started playing.
///   bool is_playing = false;
///   BarelyPerformer_IsPlaying(engine, performer_ref, &is_playing);
///
///   // Destroy the task.
///   BarelyTask_Destroy(task);
///
///   // Destroy.
///   BarelyEngine_DestroyPerformer(engine, engine, performer_ref);
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

/// Note event types.
typedef enum BarelyNoteEventType {
  /// Begin.
  BarelyNoteEventType_kBegin = 0,
  /// End.
  BarelyNoteEventType_kEnd,
  /// Number of note event types.
  BarelyNoteEventType_kCount,
} BarelyNoteEventType;

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

/// Task event types.
typedef enum BarelyTaskEventType {
  /// Begin.
  BarelyTaskEventType_kBegin = 0,
  /// End.
  BarelyTaskEventType_kEnd,
  /// Number of task event types.
  BarelyTaskEventType_kCount,
} BarelyTaskEventType;

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

/// Engine handle.
typedef struct BarelyEngine* BarelyEngineHandle;

/// Instrument handle.
typedef struct BarelyInstrument* BarelyInstrumentHandle;

/// Performer reference.
typedef uint32_t BarelyPerformerRef;

/// Task handle.
typedef struct BarelyTask* BarelyTaskHandle;

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
/// @param max_frame_count Maximum number of frames.
/// @param out_engine Output engine handle.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_Create(int32_t sample_rate, int32_t max_frame_count,
                                    BarelyEngineHandle* out_engine);

/// Creates a new performer.
///
/// @param engine Engine handle.
/// @param out_performer_ref Output performer reference.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_CreatePerformer(BarelyEngineHandle engine,
                                             BarelyPerformerRef* out_performer_ref);

/// Destroys an engine.
///
/// @param engine Engine handle.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_Destroy(BarelyEngineHandle engine);

/// Destroys a performer.
///
/// @param performer_ref Performer reference.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_DestroyPerformer(BarelyEngineHandle engine,
                                              BarelyPerformerRef performer_ref);

/// Generates a new random number with uniform distribution in the normalized range [0, 1).
///
/// @param engine Engine handle.
/// @param out_number Output random number.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_GenerateRandomNumber(BarelyEngineHandle engine, double* out_number);

/// Gets a control value of an engine.
///
/// @param engine Engine handle.
/// @param type Engine control type.
/// @param out_value Output engine control value.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_GetControl(BarelyEngineHandle engine, BarelyEngineControlType type,
                                        float* out_value);

/// Gets the random number generator seed of an engine.
///
/// @param engine Engine handle.
/// @param out_seed Output seed value.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_GetSeed(BarelyEngineHandle engine, int32_t* out_seed);

/// Gets the tempo of an engine.
///
/// @param engine Engine handle.
/// @param out_tempo Output tempo in beats per minute.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_GetTempo(BarelyEngineHandle engine, double* out_tempo);

/// Gets the timestamp of an engine.
///
/// @param engine Engine handle.
/// @param out_timestamp Output timestamp in seconds.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_GetTimestamp(BarelyEngineHandle engine, double* out_timestamp);

/// Processes the next output samples of an engine at timestamp.
///
/// @param engine Engine handle.
/// @param output_samples Array of interleaved output samples.
/// @param output_channel_count Number of output channels.
/// @param output_frame_count Number of output frames.
/// @param timestamp Timestamp in seconds.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_Process(BarelyEngineHandle engine, float* output_samples,
                                     int32_t output_channel_count, int32_t output_frame_count,
                                     double timestamp);

/// Sets a control value of an engine.
///
/// @param engine Engine handle.
/// @param type Engine control type.
/// @param value Engine control value.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_SetControl(BarelyEngineHandle engine, BarelyEngineControlType type,
                                        float value);

/// Sets the random number generator seed of an engine.
///
/// @param engine Engine handle.
/// @param seed Seed value.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_SetSeed(BarelyEngineHandle engine, int32_t seed);

/// Sets the tempo of an engine.
///
/// @param engine Engine handle.
/// @param tempo Tempo in beats per minute.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_SetTempo(BarelyEngineHandle engine, double tempo);

/// Updates an engine at timestamp.
///
/// @param engine Engine handle.
/// @param timestamp Timestamp in seconds.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_Update(BarelyEngineHandle engine, double timestamp);

/// Creates a new instrument.
///
/// @param engine Engine handle.
/// @param control_overrides Array of instrument control overrides.
/// @param control_override_count Number of instrument control overrides.
/// @param out_instrument Output instrument handle.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_Create(BarelyEngineHandle engine,
                                        const BarelyInstrumentControlOverride* control_overrides,
                                        int32_t control_override_count,
                                        BarelyInstrumentHandle* out_instrument);

/// Destroys an instrument.
///
/// @param instrument Instrument handle.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_Destroy(BarelyInstrumentHandle instrument);

/// Gets an instrument control value.
///
/// @param instrument Instrument handle.
/// @param type Instrument control type.
/// @param out_value Output instrument control value.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_GetControl(BarelyInstrumentHandle instrument,
                                            BarelyInstrumentControlType type, float* out_value);

/// Gets an instrument note control value.
///
/// @param instrument Instrument handle.
/// @param pitch Note pitch.
/// @param type Note control type.
/// @param out_value Output note control value.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_GetNoteControl(BarelyInstrumentHandle instrument, float pitch,
                                                BarelyNoteControlType type, float* out_value);

/// Gets whether an instrument note is on or not.
///
/// @param instrument Instrument handle.
/// @param pitch Note pitch.
/// @param out_is_note_on Output true if on, false otherwise.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_IsNoteOn(BarelyInstrumentHandle instrument, float pitch,
                                          bool* out_is_note_on);

/// Sets all instrument notes off.
///
/// @param instrument Instrument handle.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_SetAllNotesOff(BarelyInstrumentHandle instrument);

/// Sets an instrument control value.
///
/// @param instrument Instrument handle.
/// @param type Instrument control type.
/// @param value Instrument control value.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_SetControl(BarelyInstrumentHandle instrument,
                                            BarelyInstrumentControlType type, float value);

/// Sets an instrument note control value.
///
/// @param instrument Instrument handle.
/// @param pitch Note pitch.
/// @param type Note control type.
/// @param value Note control value.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_SetNoteControl(BarelyInstrumentHandle instrument, float pitch,
                                                BarelyNoteControlType type, float value);

/// Sets the note event callback of an instrument.
///
/// @param instrument Instrument handle.
/// @param callback Note event callback.
/// @param user_data Pointer to user data.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_SetNoteEventCallback(BarelyInstrumentHandle instrument,
                                                      BarelyNoteEventCallback callback,
                                                      void* user_data);

/// Sets an instrument note off.
///
/// @param instrument Instrument handle.
/// @param pitch Note pitch.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_SetNoteOff(BarelyInstrumentHandle instrument, float pitch);

/// Sets an instrument note on.
///
/// @param instrument Instrument handle.
/// @param pitch Note pitch.
/// @param note_control_overrides Array of note control overrides.
/// @param note_control_override_count Number of note control overrides.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_SetNoteOn(BarelyInstrumentHandle instrument, float pitch,
                                           const BarelyNoteControlOverride* note_control_overrides,
                                           int32_t note_control_override_count);

/// Sets instrument sample data.
///
/// @param instrument Instrument handle.
/// @param slices Array of slices.
/// @param slice_count Number of slices.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_SetSampleData(BarelyInstrumentHandle instrument,
                                               const BarelySlice* slices, int32_t slice_count);

/// Gets the loop begin position of a performer.
///
/// @param performer_ref Performer reference.
/// @param out_loop_begin_position Output loop begin position in beats.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_GetLoopBeginPosition(BarelyEngineHandle engine,
                                                     BarelyPerformerRef performer_ref,
                                                     double* out_loop_begin_position);

/// Gets the loop length of a performer.
///
/// @param performer_ref Performer reference.
/// @param out_loop_length Output loop length.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_GetLoopLength(BarelyEngineHandle engine,
                                              BarelyPerformerRef performer_ref,
                                              double* out_loop_length);

/// Gets the position of a performer.
///
/// @param performer_ref Performer reference.
/// @param out_position Output position in beats.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_GetPosition(BarelyEngineHandle engine,
                                            BarelyPerformerRef performer_ref, double* out_position);

/// Gets whether a performer is looping or not.
///
/// @param performer_ref Performer reference.
/// @param out_is_looping Output true if looping, false otherwise.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_IsLooping(BarelyEngineHandle engine,
                                          BarelyPerformerRef performer_ref, bool* out_is_looping);

/// Gets whether a performer is playing or not.
///
/// @param performer_ref Performer reference.
/// @param out_is_playing Output true if playing, false otherwise.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_IsPlaying(BarelyEngineHandle engine,
                                          BarelyPerformerRef performer_ref, bool* out_is_playing);

/// Sets the loop begin position of a performer.
///
/// @param performer_ref Performer reference.
/// @param loop_begin_position Loop begin position in beats.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_SetLoopBeginPosition(BarelyEngineHandle engine,
                                                     BarelyPerformerRef performer_ref,
                                                     double loop_begin_position);

/// Sets the loop length of a performer.
///
/// @param performer_ref Performer reference.
/// @param loop_length Loop length in beats.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_SetLoopLength(BarelyEngineHandle engine,
                                              BarelyPerformerRef performer_ref, double loop_length);

/// Sets whether a performer is looping or not.
///
/// @param performer_ref Performer reference.
/// @param is_looping True if looping, false otherwise.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_SetLooping(BarelyEngineHandle engine,
                                           BarelyPerformerRef performer_ref, bool is_looping);

/// Sets the position of a performer.
///
/// @param performer_ref Performer reference.
/// @param position Position in beats.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_SetPosition(BarelyEngineHandle engine,
                                            BarelyPerformerRef performer_ref, double position);

/// Starts a performer.
///
/// @param performer_ref Performer reference.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_Start(BarelyEngineHandle engine, BarelyPerformerRef performer_ref);

/// Stops a performer.
///
/// @param performer_ref Performer reference.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_Stop(BarelyEngineHandle engine, BarelyPerformerRef performer_ref);

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

/// Creates a new task.
///
/// @param performer_ref Performer reference.
/// @param position Task position in beats.
/// @param duration Task duration in beats.
/// @param priority Task priority.
/// @param callback Task event callback.
/// @param user_data Pointer to user data.
/// @param out_task Output task handle.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyTask_Create(BarelyEngineHandle engine, BarelyPerformerRef performer_ref,
                                  double position, double duration, int32_t priority,
                                  BarelyTaskEventCallback callback, void* user_data,
                                  BarelyTaskHandle* out_task);

/// Destroys a task.
///
/// @param task Task handle.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyTask_Destroy(BarelyTaskHandle task);

/// Gets the duration of a task.
///
/// @param task Task handle.
/// @param out_duration Output duration in beats.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyTask_GetDuration(BarelyTaskHandle task, double* out_duration);

/// Gets the position of a task.
///
/// @param task Task handle.
/// @param out_position Output position in beats.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyTask_GetPosition(BarelyTaskHandle task, double* out_position);

/// Gets the priority of a task.
///
/// @param task Task handle.
/// @param out_priority Output priority.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyTask_GetPriority(BarelyTaskHandle task, int32_t* out_priority);

/// Gets whether the task is active or not.
///
/// @param task Task handle.
/// @param out_is_active Output true if active, false otherwise.
BARELY_API bool BarelyTask_IsActive(BarelyTaskHandle task, bool* out_is_active);

/// Sets the duration of a task.
///
/// @param task Task handle.
/// @param duration Duration in beats.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyTask_SetDuration(BarelyTaskHandle task, double duration);

/// Sets the event callback of a task.
///
/// @param task Task handle.
/// @param callback Event callback.
/// @param user_data Pointer to user data.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyTask_SetEventCallback(BarelyTaskHandle task, BarelyTaskEventCallback callback,
                                            void* user_data);

/// Sets the position of a task.
///
/// @param task Task handle.
/// @param position Position in beats.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyTask_SetPosition(BarelyTaskHandle task, double position);

/// Sets the priority of a task.
///
/// @param task Task handle.
/// @param priority Priority.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyTask_SetPriority(BarelyTaskHandle task, int32_t priority);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
// NOLINTEND

/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// barelymusician C++ API
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef __cplusplus
#include <cassert>
#include <cstdint>
#include <functional>
#include <optional>
#include <random>
#include <span>
#include <type_traits>
#include <utility>

namespace barely {

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

/// Filter types.
enum class FilterType {
  /// None.
  kNone = BarelyFilterType_kNone,
  /// Low pass.
  kLowPass = BarelyFilterType_kLowPass,
  /// High pass.
  kHighPass = BarelyFilterType_kHighPass,
};

/// Note control types.
enum class NoteControlType {
  /// Gain in linear amplitude.
  kGain = BarelyNoteControlType_kGain,
  /// Pitch shift.
  kPitchShift = BarelyNoteControlType_kPitchShift,
};

/// Note event types.
enum class NoteEventType {
  /// Begin.
  kBegin = BarelyNoteEventType_kBegin,
  /// End.
  kEnd = BarelyNoteEventType_kEnd,
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

  /// Non-copyable.
  constexpr HandleWrapper(const HandleWrapper& other) noexcept = delete;
  constexpr HandleWrapper& operator=(const HandleWrapper& other) noexcept = delete;

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
  [[nodiscard]] constexpr operator HandleType() const noexcept { return handle_; }

 protected:
  // Helper functions to set a callback.
  template <typename CallbackFn, typename SetCallbackFn, typename... CallbackArgs>
  void SetCallback(SetCallbackFn set_callback_fn, std::function<void(CallbackArgs...)>& callback,
                   CallbackFn callback_fn) {
    [[maybe_unused]] const bool success = callback
                                              ? set_callback_fn(handle_, callback_fn, &callback)
                                              : set_callback_fn(handle_, nullptr, nullptr);
    assert(success && "HandleWrapper::SetCallback failed");
  }

 private:
  // Raw handle.
  HandleType handle_ = nullptr;
};

/// Class that wraps an instrument handle.
class Instrument : public HandleWrapper<BarelyInstrumentHandle> {
 public:
  /// Constructs a new `Instrument`.
  ///
  /// @param engine Raw engine handle.
  /// @param control_overrides Span of instrument control overrides.
  Instrument(BarelyEngineHandle engine,
             std::span<const InstrumentControlOverride> control_overrides) noexcept
      : HandleWrapper([&]() {
          BarelyInstrumentHandle instrument = nullptr;
          [[maybe_unused]] const bool success = BarelyInstrument_Create(
              engine,
              reinterpret_cast<const BarelyInstrumentControlOverride*>(control_overrides.data()),
              static_cast<int32_t>(control_overrides.size()), &instrument);
          assert(success);
          return instrument;
        }()) {}

  /// Constructs a new `Instrument` from a raw handle.
  ///
  /// @param instrument Raw handle to instrument.
  explicit Instrument(BarelyInstrumentHandle instrument) noexcept : HandleWrapper(instrument) {}

  /// Destroys `Instrument`.
  ~Instrument() noexcept { BarelyInstrument_Destroy(*this); }

  /// Non-copyable.
  Instrument(const Instrument& other) noexcept = delete;
  Instrument& operator=(const Instrument& other) noexcept = delete;

  /// Constructs a new `Instrument` via move.
  ///
  /// @param other Other instrument.
  /// @return Instrument.
  Instrument(Instrument&& other) noexcept
      : HandleWrapper(std::move(other)),
        note_event_callback_(std::exchange(other.note_event_callback_, {})) {
    if (note_event_callback_) {
      SetNoteEventCallback();
    }
  }

  /// Assigns `Instrument` via move.
  ///
  /// @param other Other instrument.
  /// @return Instrument.
  Instrument& operator=(Instrument&& other) noexcept {
    if (this != &other) {
      BarelyInstrument_Destroy(*this);
      HandleWrapper::operator=(std::move(other));
      note_event_callback_ = std::exchange(other.note_event_callback_, {});
      if (note_event_callback_) {
        SetNoteEventCallback();
      }
    }
    return *this;
  }

  /// Returns a control value.
  ///
  /// @param type Instrument control type.
  /// @return Instrument control value.
  template <typename ValueType>
  [[nodiscard]] ValueType GetControl(InstrumentControlType type) const noexcept {
    static_assert(std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
                  "ValueType is not supported");
    float value = 0.0f;
    [[maybe_unused]] const bool success =
        BarelyInstrument_GetControl(*this, static_cast<BarelyInstrumentControlType>(type), &value);
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
  /// @return True if on, false otherwise.
  [[nodiscard]] bool IsNoteOn(float pitch) const noexcept {
    bool is_note_on = false;
    [[maybe_unused]] const bool success = BarelyInstrument_IsNoteOn(*this, pitch, &is_note_on);
    assert(success);
    return is_note_on;
  }

  /// Sets all notes off.
  void SetAllNotesOff() noexcept {
    [[maybe_unused]] const bool success = BarelyInstrument_SetAllNotesOff(*this);
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
        *this, static_cast<BarelyInstrumentControlType>(type), static_cast<float>(value));
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

  /// Sets the note event callback.
  ///
  /// @param callback Note event callback.
  void SetNoteEventCallback(NoteEventCallback callback) noexcept {
    note_event_callback_ = std::move(callback);
    SetNoteEventCallback();
  }

  /// Sets a note off.
  ///
  /// @param pitch Note pitch.
  void SetNoteOff(float pitch) noexcept {
    [[maybe_unused]] const bool success = BarelyInstrument_SetNoteOff(*this, pitch);
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
        *this, pitch,
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
    [[maybe_unused]] const bool success =
        BarelyInstrument_SetSampleData(*this, reinterpret_cast<const BarelySlice*>(slices.data()),
                                       static_cast<int32_t>(slices.size()));
    assert(success);
  }

 private:
  // Helper function to set the note event callback.
  void SetNoteEventCallback() noexcept {
    SetCallback(BarelyInstrument_SetNoteEventCallback, note_event_callback_,
                [](BarelyNoteEventType type, float pitch, void* user_data) noexcept {
                  assert(user_data != nullptr && "Invalid note event callback user data");
                  if (const auto& callback = *static_cast<NoteEventCallback*>(user_data);
                      callback) {
                    callback(static_cast<NoteEventType>(type), pitch);
                  }
                });
  }

  // Note event callback.
  NoteEventCallback note_event_callback_;
};

/// Class that wraps a task handle.
class Task : public HandleWrapper<BarelyTaskHandle> {
 public:
  /// Constructs a new `Task`.
  ///
  /// @param performer_ref Raw performer reference.
  /// @param position Task position in beats.
  /// @param duration Task duration in beats.
  /// @param priority Task priority.
  /// @param callback Task event callback.
  Task(BarelyEngineHandle engine, BarelyPerformerRef performer_ref, double position,
       double duration, int priority, TaskEventCallback callback) noexcept
      : HandleWrapper([&]() {
          BarelyTaskHandle task = nullptr;
          [[maybe_unused]] const bool success = BarelyTask_Create(
              engine, performer_ref, position, duration, priority,
              [](BarelyTaskEventType type, void* user_data) noexcept {
                if (user_data != nullptr) {
                  (*static_cast<TaskEventCallback*>(user_data))(static_cast<TaskEventType>(type));
                }
              },
              &event_callback_, &task);
          assert(success);
          return task;
        }()),
        event_callback_(std::move(callback)) {}

  /// Constructs a new `Task` from a raw handle.
  ///
  /// @param task Raw handle to task.
  explicit Task(BarelyTaskHandle task) noexcept : HandleWrapper(task) {}

  /// Destroys `Task`.
  ~Task() noexcept { BarelyTask_Destroy(*this); }

  /// Non-copyable.
  Task(const Task& other) noexcept = delete;
  Task& operator=(const Task& other) noexcept = delete;

  /// Constructs a new `Task` via move.
  ///
  /// @param other Other task.
  /// @return Task.
  Task(Task&& other) noexcept
      : HandleWrapper(std::move(other)), event_callback_(std::exchange(other.event_callback_, {})) {
    if (event_callback_) {
      SetEventCallback();
    }
  }

  /// Assigns `Task` via move.
  ///
  /// @param other Other task.
  /// @return Task.
  Task& operator=(Task&& other) noexcept {
    if (this != &other) {
      BarelyTask_Destroy(*this);
      HandleWrapper::operator=(std::move(other));
      event_callback_ = std::exchange(other.event_callback_, {});
      if (event_callback_) {
        SetEventCallback();
      }
    }
    return *this;
  }

  /// Returns the duration.
  ///
  /// @return Duration in beats.
  [[nodiscard]] double GetDuration() const noexcept {
    double duration = 0.0;
    [[maybe_unused]] const bool success = BarelyTask_GetDuration(*this, &duration);
    assert(success);
    return duration;
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

  /// Returns the priority.
  ///
  /// @return Priority.
  [[nodiscard]] int GetPriority() const noexcept {
    int32_t priority = 0;
    [[maybe_unused]] const bool success = BarelyTask_GetPriority(*this, &priority);
    assert(success);
    return static_cast<int>(priority);
  }

  /// Returns whether the task is active or not.
  ///
  /// @return True if active, false otherwise.
  [[nodiscard]] bool IsActive() const noexcept {
    bool is_active = false;
    [[maybe_unused]] const bool success = BarelyTask_IsActive(*this, &is_active);
    assert(success);
    return is_active;
  }

  /// Sets the duration.
  ///
  /// @param duration Duration in beats.
  void SetDuration(double duration) noexcept {
    [[maybe_unused]] const bool success = BarelyTask_SetDuration(*this, duration);
    assert(success);
  }

  /// Sets the event callback.
  ///
  /// @param callback Event callback.
  void SetEventCallback(TaskEventCallback callback) noexcept {
    BarelyTask_SetEventCallback(*this, nullptr, nullptr);
    event_callback_ = std::move(callback);
    SetEventCallback();
  }

  /// Sets the position.
  ///
  /// @param position Position in beats.
  void SetPosition(double position) noexcept {
    [[maybe_unused]] const bool success = BarelyTask_SetPosition(*this, position);
    assert(success);
  }

  /// Sets the priority.
  ///
  /// @param priority Priority.
  void SetPriority(int priority) noexcept {
    [[maybe_unused]] const bool success =
        BarelyTask_SetPriority(*this, static_cast<int32_t>(priority));
    assert(success);
  }

 private:
  // Helper function to set the event callback.
  void SetEventCallback() noexcept {
    SetCallback(BarelyTask_SetEventCallback, event_callback_,
                [](BarelyTaskEventType type, void* user_data) noexcept {
                  assert(user_data != nullptr && "Invalid task event callback user data");
                  if (const auto& callback = *static_cast<TaskEventCallback*>(user_data);
                      callback) {
                    callback(static_cast<TaskEventType>(type));
                  }
                });
  }

  // Event callback.
  TaskEventCallback event_callback_;
};

/// Class that wraps a performer reference.
class PerformerRef {
 public:
  /// Constructs a new `PerformerRef` from a raw reference.
  ///
  /// @param engine Raw engine handle.
  /// @param performer_ref Raw performer reference.
  PerformerRef(BarelyEngineHandle engine, BarelyPerformerRef performer_ref) noexcept
      : engine_(engine), performer_ref_(performer_ref) {}

  /// Returns the raw reference.
  ///
  /// @return Raw reference.
  [[nodiscard]] constexpr operator BarelyPerformerRef() const noexcept { return performer_ref_; }

  /// Creates a new task.
  ///
  /// @param position Task position in beats.
  /// @param duration Task duration in beats.
  /// @param priority Task priority.
  /// @param callback Task event callback.
  /// @return Task.
  [[nodiscard]] Task CreateTask(double position, double duration, int priority,
                                TaskEventCallback callback) noexcept {
    return Task(engine_, performer_ref_, position, duration, priority, std::move(callback));
  }

  /// Returns the loop begin position.
  ///
  /// @return Loop begin position in beats.
  [[nodiscard]] double GetLoopBeginPosition() const noexcept {
    double loop_begin_position = 0.0;
    [[maybe_unused]] const bool success =
        BarelyPerformer_GetLoopBeginPosition(engine_, performer_ref_, &loop_begin_position);
    assert(success);
    return loop_begin_position;
  }

  /// Returns the loop length.
  ///
  /// @return Loop length in beats.
  [[nodiscard]] double GetLoopLength() const noexcept {
    double loop_length = 0.0;
    [[maybe_unused]] const bool success =
        BarelyPerformer_GetLoopLength(engine_, performer_ref_, &loop_length);
    assert(success);
    return loop_length;
  }

  /// Returns the position.
  ///
  /// @return Position in beats.
  [[nodiscard]] double GetPosition() const noexcept {
    double position = 0.0;
    [[maybe_unused]] const bool success =
        BarelyPerformer_GetPosition(engine_, performer_ref_, &position);
    assert(success);
    return position;
  }

  /// Returns whether the performer is looping or not.
  ///
  /// @return True if looping, false otherwise.
  [[nodiscard]] bool IsLooping() const noexcept {
    bool is_looping = false;
    [[maybe_unused]] const bool success =
        BarelyPerformer_IsLooping(engine_, performer_ref_, &is_looping);
    assert(success);
    return is_looping;
  }

  /// Returns whether the performer is playing or not.
  ///
  /// @return True if playing, false otherwise.
  [[nodiscard]] bool IsPlaying() const noexcept {
    bool is_playing = false;
    [[maybe_unused]] const bool success =
        BarelyPerformer_IsPlaying(engine_, performer_ref_, &is_playing);
    assert(success);
    return is_playing;
  }

  /// Sets the loop begin position.
  ///
  /// @param loop_begin_position Loop begin position in beats.
  void SetLoopBeginPosition(double loop_begin_position) noexcept {
    [[maybe_unused]] const bool success =
        BarelyPerformer_SetLoopBeginPosition(engine_, performer_ref_, loop_begin_position);
    assert(success);
  }

  /// Sets the loop length.
  ///
  /// @param loop_length Loop length in beats.
  void SetLoopLength(double loop_length) noexcept {
    [[maybe_unused]] const bool success =
        BarelyPerformer_SetLoopLength(engine_, performer_ref_, loop_length);
    assert(success);
  }

  /// Sets whether the performer is looping or not.
  ///
  /// @param is_looping True if looping, false otherwise.
  void SetLooping(bool is_looping) noexcept {
    [[maybe_unused]] const bool success =
        BarelyPerformer_SetLooping(engine_, performer_ref_, is_looping);
    assert(success);
  }

  /// Sets the position.
  ///
  /// @param position Position in beats.
  void SetPosition(double position) noexcept {
    [[maybe_unused]] const bool success =
        BarelyPerformer_SetPosition(engine_, performer_ref_, position);
    assert(success);
  }

  /// Starts the performer.
  void Start() noexcept {
    [[maybe_unused]] const bool success = BarelyPerformer_Start(engine_, performer_ref_);
    assert(success);
  }

  /// Stops the performer.
  void Stop() noexcept {
    [[maybe_unused]] const bool success = BarelyPerformer_Stop(engine_, performer_ref_);
    assert(success);
  }

 private:
  /// Engine handle.
  BarelyEngineHandle engine_;

  /// Raw performer reference.
  BarelyPerformerRef performer_ref_;
};

/// A class that wraps an engine handle.
class Engine : public HandleWrapper<BarelyEngineHandle> {
 public:
  /// Constructs a new `Engine`.
  ///
  /// @param sample_rate Sampling rate in hertz.
  /// @param max_frame_count Maximum number of frames.
  Engine(int sample_rate, int max_frame_count) noexcept
      : HandleWrapper([&]() {
          BarelyEngineHandle engine = nullptr;
          [[maybe_unused]] const bool success = BarelyEngine_Create(
              static_cast<int32_t>(sample_rate), static_cast<int32_t>(max_frame_count), &engine);
          assert(success);
          return engine;
        }()) {}

  /// Constructs a new `Engine` from a raw handle.
  ///
  /// @param engine Raw handle to engine.
  explicit Engine(BarelyEngineHandle engine) noexcept : HandleWrapper(engine) {}

  /// Destroys `Engine`.
  ~Engine() noexcept { BarelyEngine_Destroy(*this); }

  /// Non-copyable.
  Engine(const Engine& other) noexcept = delete;
  Engine& operator=(const Engine& other) noexcept = delete;

  /// Default move constructor.
  Engine(Engine&& other) noexcept = default;

  /// Assigns `Engine` via move.
  ///
  /// @param other Other engine.
  /// @return Engine.
  Engine& operator=(Engine&& other) noexcept {
    if (this != &other) {
      BarelyEngine_Destroy(*this);
      HandleWrapper::operator=(std::move(other));
    }
    return *this;
  }

  /// Creates a new instrument.
  ///
  /// @param control_overrides Span of instrument control overrides.
  /// @return Instrument.
  Instrument CreateInstrument(
      std::span<const InstrumentControlOverride> control_overrides = {}) noexcept {
    return Instrument(*this, control_overrides);
  }

  /// Creates a performer.
  ///
  /// @return PerformerRef.
  [[nodiscard]] PerformerRef CreatePerformer() noexcept {
    BarelyPerformerRef performer_ref = 0;
    [[maybe_unused]] const bool success = BarelyEngine_CreatePerformer(*this, &performer_ref);
    assert(success);
    return PerformerRef(*this, performer_ref);
  }

  /// Destroys a performer.
  ///
  /// @param performer_ref Performer reference.
  void DestroyPerformer(const PerformerRef& performer_ref) {
    [[maybe_unused]] const bool success = BarelyEngine_DestroyPerformer(*this, performer_ref);
    assert(success);
  }

  /// Generates a random number with uniform distribution in the normalized range [0, 1).
  ///
  /// @return Random number.
  [[nodiscard]] double GenerateRandomNumber() noexcept {
    double number = 0.0;
    [[maybe_unused]] const bool success = BarelyEngine_GenerateRandomNumber(*this, &number);
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
        BarelyEngine_GetControl(*this, static_cast<BarelyEngineControlType>(type), &value);
    assert(success);
    return static_cast<ValueType>(value);
  }

  /// Returns the random number generator seed.
  ///
  /// @return Seed value.
  [[nodiscard]] int GetSeed() const noexcept {
    int32_t seed = 0;
    [[maybe_unused]] const bool success = BarelyEngine_GetSeed(*this, &seed);
    assert(success);
    return static_cast<int>(seed);
  }

  /// Returns the tempo.
  ///
  /// @return Tempo in beats per minute.
  [[nodiscard]] double GetTempo() const noexcept {
    double tempo = 0.0;
    [[maybe_unused]] const bool success = BarelyEngine_GetTempo(*this, &tempo);
    assert(success);
    return tempo;
  }

  /// Returns the timestamp.
  ///
  /// @return Timestamp in seconds.
  [[nodiscard]] double GetTimestamp() const noexcept {
    double timestamp = 0.0;
    [[maybe_unused]] const bool success = BarelyEngine_GetTimestamp(*this, &timestamp);
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
        BarelyEngine_Process(*this, output_samples, static_cast<int32_t>(output_channel_count),
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
        *this, static_cast<BarelyEngineControlType>(type), static_cast<float>(value));
    assert(success);
  }

  /// Sets the random number generator seed.
  void SetSeed(int seed) noexcept {
    [[maybe_unused]] const bool success = BarelyEngine_SetSeed(*this, static_cast<int32_t>(seed));
    assert(success);
  }

  /// Sets the tempo.
  ///
  /// @param tempo Tempo in beats per minute.
  void SetTempo(double tempo) noexcept {
    [[maybe_unused]] const bool success = BarelyEngine_SetTempo(*this, tempo);
    assert(success);
  }

  /// Updates the engine at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void Update(double timestamp) noexcept {
    [[maybe_unused]] const bool success = BarelyEngine_Update(*this, timestamp);
    assert(success);
  }
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
