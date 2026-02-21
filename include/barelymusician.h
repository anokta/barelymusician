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
///   // Create a new engine.
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
///   // Process the next output samples.
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
///   // Create a new instrument.
///   auto instrument = engine.CreateInstrument();
///
///   // Set an instrument note on.
///   //
///   // The note pitch is expressed in octaves relative to middle C as the center frequency.
///   // Fractional note values adjust the frequency logarithmically to ensure equally perceived
///   // pitch intervals within each octave.
///   constexpr float kC3Pitch = -1.0f;
///   instrument.SetNoteOn(kC3Pitch);
///
///   // Check if the instrument note is on.
///   const bool is_note_on = instrument.IsNoteOn(kC3Pitch);
///
///   // Set the instrument to use full oscillator mix.
///   instrument.SetControl(barely::InstrumentControlType::kOscMix, /*value=*/1.0f);
///
///   // Destroy the instrument.
///   engine.DestroyInstrument(instrument);
///   @endcode
///
/// - Performer:
///
///   @code{.cpp}
///   // Create a new performer.
///   auto performer = engine.CreatePerformer();
///
///   // Create a new task.
///   auto task = engine.CreateTask(performer, /*position=*/0.0, /*duration=*/1.0,
///                                 [](barely::TaskEventType type) { /*populate this*/ });
///
///   // Set the performer to looping.
///   performer.SetLooping(/*is_looping=*/true);
///
///   // Start the performer playback.
///   performer.Start();
///
///   // Check if the performer started playing.
///   const bool is_playing = performer.IsPlaying();
///
///   // Destroy the task.
///   engine.DestroyTask(task);
///
///   // Destroy the performer.
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
///   // Create a new engine.
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
///   // Process the next output samples.
///   //
///   // The engine processes output samples synchronously. Therefore, `Process` should typically be
///   // called from an audio thread process callback in real-time audio applications.
///   float output_samples[2 * 512];
///   BarelyEngine_Process(engine, output_samples, 2, 512, timestamp);
///
///   // Destroy the engine.
///   BarelyEngine_Destroy(engine);
///   @endcode
///
/// - Instrument:
///
///   @code{.cpp}
///   // Create a new instrument.
///   uint32_t instrument_id = 0;
///   BarelyEngine_CreateInstrument(engine, /*control_overrides=*/nullptr,
///                                 /*control_override_count=*/0, &instrument_id);
///
///   // Set an instrument note on.
///   //
///   // The note pitch is expressed in octaves relative to middle C as the center frequency.
///   // Fractional note values adjust the frequency logarithmically to ensure equally perceived
///   // pitch intervals within each octave.
///   float c3_pitch = -1.0f;
///   BarelyInstrument_SetNoteOn(engine, instrument_id, c3_pitch,
///                              /*note_control_overrides=*/nullptr,
///                              /*note_control_override_count=*/0);
///
///   // Check if the instrument note is on.
///   bool is_note_on = false;
///   BarelyInstrument_IsNoteOn(engine, instrument_id, c3_pitch, &is_note_on);
///
///   // Set the instrument to use full oscillator mix.
///   BarelyInstrument_SetControl(engine, instrument_id, BarelyInstrumentControlType_kOscMix,
///                               /*value=*/1.0f);
///
///   // Destroy the instrument.
///   BarelyEngine_DestroyInstrument(engine, instrument_id);
///   @endcode
///
/// - Performer:
///
///   @code{.cpp}
///   // Create a new performer.
///   uint32_t performer_id = 0;
///   BarelyEngine_CreatePerformer(engine, &performer_id);
///
///   // Create a new task.
///   uint32_t task_id = 0;
///   BarelyTaskEventCallback callback{ /*populate this*/ };
///   BarelyEngine_CreateTask(engine, performer_id, /*position=*/0.0, /*duration=*/1.0,
///                           /*priority=*/0, callback, &task_id);
///
///   // Set the performer to looping.
///   BarelyPerformer_SetLooping(engine, performer_id, /*is_looping=*/true);
///
///   // Start the performer playback.
///   BarelyPerformer_Start(engine, performer_id);
///
///   // Check if the performer started playing.
///   bool is_playing = false;
///   BarelyPerformer_IsPlaying(engine, performer_id, &is_playing);
///
///   // Destroy the task.
///   BarelyEngine_DestroyTask(engine, task_id);
///
///   // Destroy the performer.
///   BarelyEngine_DestroyPerformer(engine, performer_id);
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
#ifdef BARELY_EXPORT
#ifdef __GNUC__
#define BARELY_API __attribute__((dllexport))
#else  // __GNUC__
#define BARELY_API __declspec(dllexport)
#endif  // __GNUC__
#else   // BARELY_EXPORT
#ifdef __GNUC__
#define BARELY_API __attribute__((dllimport))
#else  // __GNUC__
#define BARELY_API __declspec(dllimport)
#endif  // __GNUC__
#endif  // BARELY_EXPORT
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
  BarelyEngineControlType_CompMix = 0,
  /// Compressor attack in seconds.
  BarelyEngineControlType_CompAttack,
  /// Compressor release in seconds.
  BarelyEngineControlType_CompRelease,
  /// Compressor threshold.
  BarelyEngineControlType_CompThreshold,
  /// Compressor ratio.
  BarelyEngineControlType_CompRatio,
  /// Delay mix.
  BarelyEngineControlType_kDelayMix,
  /// Delay time in seconds.
  BarelyEngineControlType_kDelayTime,
  /// Delay feedback.
  BarelyEngineControlType_kDelayFeedback,
  /// Delay low-pass cutoff.
  BarelyEngineControlType_kDelayLowPassCutoff,
  /// Delay high-pass cutoff.
  BarelyEngineControlType_kDelayHighPassCutoff,
  /// Delay reverb send.
  BarelyEngineControlType_kDelayReverbSend,
  /// Reverb mix.
  BarelyEngineControlType_kReverbMix,
  /// Reverb damping ratio.
  BarelyEngineControlType_kReverbDampingRatio,
  /// Reverb room size.
  BarelyEngineControlType_kReverbRoomSize,
  /// Reverb stereo width.
  BarelyEngineControlType_kReverbStereoWidth,
  /// Reverb freeze.
  BarelyEngineControlType_kReverbFreeze,
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
  /// Normalized gain in logarithmic scale.
  BarelyInstrumentControlType_kGain = 0,
  /// Pitch shift.
  BarelyInstrumentControlType_kPitchShift,
  /// Stereo pan.
  BarelyInstrumentControlType_kStereoPan,
  /// Retrigger.
  BarelyInstrumentControlType_kRetrigger,
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
  /// Bit crusher rate.
  BarelyInstrumentControlType_kBitCrusherRate,
  /// Distortion mix.
  BarelyInstrumentControlType_kDistortionMix,
  /// Distortion drive.
  BarelyInstrumentControlType_kDistortionDrive,
  /// Filter type.
  BarelyInstrumentControlType_kFilterType,
  /// Filter cutoff.
  BarelyInstrumentControlType_kFilterCutoff,
  /// Filter Q factor.
  BarelyInstrumentControlType_kFilterQ,
  /// Delay send.
  BarelyInstrumentControlType_kDelaySend,
  /// Reverb send.
  BarelyInstrumentControlType_kReverbSend,
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
  /// Normalized gain in logarithmic scale.
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
  /// Linear crossfade between the slice and the oscillator.
  BarelyOscMode_kCrossfade = 0,
  /// Amplitude modulation applied to the slice by the oscillator.
  BarelyOscMode_kAm,
  /// Frequency modulation applied to the slice by the oscillator.
  BarelyOscMode_kFm,
  /// Amplitude modulation applied to the oscillator by the slice.
  BarelyOscMode_kMa,
  /// Frequency modulation applied to the oscillator by the slice.
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

/// Engine handle.
typedef struct BarelyEngine BarelyEngine;

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
  /// Array of mono samples.
  const float* samples;

  /// Number of mono samples.
  int32_t sample_count;

  /// Sampling rate in hertz.
  int32_t sample_rate;

  /// Root note pitch.
  float root_pitch;
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
/// @param out_instrument_id Output instrument identifier.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_CreateInstrument(
    BarelyEngine* engine, const BarelyInstrumentControlOverride* control_overrides,
    int32_t control_override_count, uint32_t* out_instrument_id);

/// Creates a new performer.
///
/// @param engine Pointer to engine.
/// @param out_performer_id Output performer identifier.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_CreatePerformer(BarelyEngine* engine, uint32_t* out_performer_id);

/// Creates a new task.
///
/// @param performer_id Performer identifier.
/// @param position Task position in beats.
/// @param duration Task duration in beats.
/// @param priority Task priority.
/// @param callback Task event callback.
/// @param user_data Pointer to user data.
/// @param out_task_id Output task identifier.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_CreateTask(BarelyEngine* engine, uint32_t performer_id,
                                        double position, double duration, int32_t priority,
                                        BarelyTaskEventCallback callback, void* user_data,
                                        uint32_t* out_task_id);

/// Destroys an engine.
///
/// @param engine Pointer to engine.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_Destroy(BarelyEngine* engine);

/// Destroys an instrument.
///
/// @param engine Pointer to engine.
/// @param instrument_id Instrument identifier.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_DestroyInstrument(BarelyEngine* engine, uint32_t instrument_id);

/// Destroys a performer.
///
/// @param performer_id Performer identifier.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_DestroyPerformer(BarelyEngine* engine, uint32_t performer_id);

/// Destroys a task.
///
/// @param task_id Task identifier.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_DestroyTask(BarelyEngine* engine, uint32_t task_id);

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
/// @param instrument_id Instrument identifier.
/// @param type Instrument control type.
/// @param out_value Output instrument control value.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_GetControl(const BarelyEngine* engine, uint32_t instrument_id,
                                            BarelyInstrumentControlType type, float* out_value);

/// Gets an instrument note control value.
///
/// @param engine Pointer to engine.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @param type Note control type.
/// @param out_value Output note control value.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_GetNoteControl(const BarelyEngine* engine, uint32_t instrument_id,
                                                float pitch, BarelyNoteControlType type,
                                                float* out_value);

/// Gets whether an instrument note is on or not.
///
/// @param engine Pointer to engine.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @param out_is_note_on Output true if on, false otherwise.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_IsNoteOn(const BarelyEngine* engine, uint32_t instrument_id,
                                          float pitch, bool* out_is_note_on);

/// Sets all instrument notes off.
///
/// @param engine Pointer to engine.
/// @param instrument_id Instrument identifier.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_SetAllNotesOff(BarelyEngine* engine, uint32_t instrument_id);

/// Sets an instrument control value.
///
/// @param engine Pointer to engine.
/// @param instrument_id Instrument identifier.
/// @param type Instrument control type.
/// @param value Instrument control value.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_SetControl(BarelyEngine* engine, uint32_t instrument_id,
                                            BarelyInstrumentControlType type, float value);

/// Sets an instrument note control value.
///
/// @param engine Pointer to engine.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @param type Note control type.
/// @param value Note control value.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_SetNoteControl(BarelyEngine* engine, uint32_t instrument_id,
                                                float pitch, BarelyNoteControlType type,
                                                float value);

/// Sets the note event callback of an instrument.
///
/// @param engine Pointer to engine.
/// @param instrument_id Instrument identifier.
/// @param callback Note event callback.
/// @param user_data Pointer to user data.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_SetNoteEventCallback(BarelyEngine* engine, uint32_t instrument_id,
                                                      BarelyNoteEventCallback callback,
                                                      void* user_data);

/// Sets an instrument note off.
///
/// @param engine Pointer to engine.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_SetNoteOff(BarelyEngine* engine, uint32_t instrument_id,
                                            float pitch);

/// Sets an instrument note on.
///
/// @param engine Pointer to engine.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @param note_control_overrides Array of note control overrides.
/// @param note_control_override_count Number of note control overrides.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_SetNoteOn(BarelyEngine* engine, uint32_t instrument_id,
                                           float pitch,
                                           const BarelyNoteControlOverride* note_control_overrides,
                                           int32_t note_control_override_count);

/// Sets instrument sample data.
///
/// @param engine Pointer to engine.
/// @param instrument_id Instrument identifier.
/// @param slices Array of slices.
/// @param slice_count Number of slices.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_SetSampleData(BarelyEngine* engine, uint32_t instrument_id,
                                               const BarelySlice* slices, int32_t slice_count);

/// Gets the loop begin position of a performer.
///
/// @param engine Pointer to engine.
/// @param performer_id Performer identifier.
/// @param out_loop_begin_position Output loop begin position in beats.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_GetLoopBeginPosition(const BarelyEngine* engine,
                                                     uint32_t performer_id,
                                                     double* out_loop_begin_position);

/// Gets the loop length of a performer.
///
/// @param engine Pointer to engine.
/// @param performer_id Performer identifier.
/// @param out_loop_length Output loop length.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_GetLoopLength(const BarelyEngine* engine, uint32_t performer_id,
                                              double* out_loop_length);

/// Gets the position of a performer.
///
/// @param engine Pointer to engine.
/// @param performer_id Performer identifier.
/// @param out_position Output position in beats.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_GetPosition(const BarelyEngine* engine, uint32_t performer_id,
                                            double* out_position);

/// Gets whether a performer is looping or not.
///
/// @param engine Pointer to engine.
/// @param performer_id Performer identifier.
/// @param out_is_looping Output true if looping, false otherwise.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_IsLooping(const BarelyEngine* engine, uint32_t performer_id,
                                          bool* out_is_looping);

/// Gets whether a performer is playing or not.
///
/// @param engine Pointer to engine.
/// @param performer_id Performer identifier.
/// @param out_is_playing Output true if playing, false otherwise.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_IsPlaying(const BarelyEngine* engine, uint32_t performer_id,
                                          bool* out_is_playing);

/// Sets the loop begin position of a performer.
///
/// @param engine Pointer to engine.
/// @param performer_id Performer identifier.
/// @param loop_begin_position Loop begin position in beats.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_SetLoopBeginPosition(BarelyEngine* engine, uint32_t performer_id,
                                                     double loop_begin_position);

/// Sets the loop length of a performer.
///
/// @param engine Pointer to engine.
/// @param performer_id Performer identifier.
/// @param loop_length Loop length in beats.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_SetLoopLength(BarelyEngine* engine, uint32_t performer_id,
                                              double loop_length);

/// Sets whether a performer is looping or not.
///
/// @param engine Pointer to engine.
/// @param performer_id Performer identifier.
/// @param is_looping True if looping, false otherwise.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_SetLooping(BarelyEngine* engine, uint32_t performer_id,
                                           bool is_looping);

/// Sets the position of a performer.
///
/// @param engine Pointer to engine.
/// @param performer_id Performer identifier.
/// @param position Position in beats.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_SetPosition(BarelyEngine* engine, uint32_t performer_id,
                                            double position);

/// Starts the playback of a performer.
///
/// @param engine Pointer to engine.
/// @param performer_id Performer identifier.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_Start(BarelyEngine* engine, uint32_t performer_id);

/// Stops the playback of a performer.
///
/// @param engine Pointer to engine.
/// @param performer_id Performer identifier.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_Stop(BarelyEngine* engine, uint32_t performer_id);

/// Gets the duration of a task.
///
/// @param engine Pointer to engine.
/// @param task_id Task identifier.
/// @param out_duration Output duration in beats.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyTask_GetDuration(const BarelyEngine* engine, uint32_t task_id,
                                       double* out_duration);

/// Gets the position of a task.
///
/// @param engine Pointer to engine.
/// @param task_id Task identifier.
/// @param out_position Output position in beats.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyTask_GetPosition(const BarelyEngine* engine, uint32_t task_id,
                                       double* out_position);

/// Gets the priority of a task.
///
/// @param engine Pointer to engine.
/// @param task_id Task identifier.
/// @param out_priority Output priority.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyTask_GetPriority(const BarelyEngine* engine, uint32_t task_id,
                                       int32_t* out_priority);

/// Gets whether the task is active or not.
///
/// @param engine Pointer to engine.
/// @param task_id Task identifier.
/// @param out_is_active Output true if active, false otherwise.
BARELY_API bool BarelyTask_IsActive(const BarelyEngine* engine, uint32_t task_id,
                                    bool* out_is_active);

/// Sets the duration of a task.
///
/// @param engine Pointer to engine.
/// @param task_id Task identifier.
/// @param duration Duration in beats.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyTask_SetDuration(BarelyEngine* engine, uint32_t task_id, double duration);

/// Sets the event callback of a task.
///
/// @param engine Pointer to engine.
/// @param task_id Task identifier.
/// @param callback Event callback.
/// @param user_data Pointer to user data.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyTask_SetEventCallback(BarelyEngine* engine, uint32_t task_id,
                                            BarelyTaskEventCallback callback, void* user_data);

/// Sets the position of a task.
///
/// @param engine Pointer to engine.
/// @param task_id Task identifier.
/// @param position Position in beats.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyTask_SetPosition(BarelyEngine* engine, uint32_t task_id, double position);

/// Sets the priority of a task.
///
/// @param engine Pointer to engine.
/// @param task_id Task identifier.
/// @param priority Priority.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyTask_SetPriority(BarelyEngine* engine, uint32_t task_id, int32_t priority);

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
#include <memory>
#include <optional>
#include <random>
#include <span>
#include <type_traits>
#include <utility>

namespace barely {

/// Engine control types.
enum class EngineControlType {
  /// Compressor mix.
  CompMix = BarelyEngineControlType_CompMix,
  /// Compressor attack in seconds.
  CompAttack = BarelyEngineControlType_CompAttack,
  /// Compressor release in seconds.
  CompRelease = BarelyEngineControlType_CompRelease,
  /// Compressor threshold.
  CompThreshold = BarelyEngineControlType_CompThreshold,
  /// Compressor ratio.
  CompRatio = BarelyEngineControlType_CompRatio,
  /// Delay mix.
  kDelayMix = BarelyEngineControlType_kDelayMix,
  /// Delay time in seconds.
  kDelayTime = BarelyEngineControlType_kDelayTime,
  /// Delay feedback.
  kDelayFeedback = BarelyEngineControlType_kDelayFeedback,
  /// Delay low-pass cutoff.
  kDelayLowPassCutoff = BarelyEngineControlType_kDelayLowPassCutoff,
  /// Delay high-pass cutoff.
  kDelayHighPassCutoff = BarelyEngineControlType_kDelayHighPassCutoff,
  /// Delay reverb send.
  kDelayReverbSend = BarelyEngineControlType_kDelayReverbSend,
  /// Reverb mix.
  kReverbMix = BarelyEngineControlType_kReverbMix,
  /// Reverb damping ratio.
  kReverbDampingRatio = BarelyEngineControlType_kReverbDampingRatio,
  /// Reverb room size.
  kReverbRoomSize = BarelyEngineControlType_kReverbRoomSize,
  /// Reverb stereo width.
  kReverbStereoWidth = BarelyEngineControlType_kReverbStereoWidth,
  /// Reverb freeze.
  kReverbFreeze = BarelyEngineControlType_kReverbFreeze,
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

/// Instrument control types.
enum class InstrumentControlType {
  /// Normalized gain in logarithmic scale.
  kGain = BarelyInstrumentControlType_kGain,
  /// Pitch shift.
  kPitchShift = BarelyInstrumentControlType_kPitchShift,
  /// Stereo pan.
  kStereoPan = BarelyInstrumentControlType_kStereoPan,
  /// Retrigger.
  kRetrigger = BarelyInstrumentControlType_kRetrigger,
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
  /// Bit crusher rate.
  kBitCrusherRate = BarelyInstrumentControlType_kBitCrusherRate,
  /// Distortion mix.
  kDistortionMix = BarelyInstrumentControlType_kDistortionMix,
  /// Distortion drive.
  kDistortionDrive = BarelyInstrumentControlType_kDistortionDrive,
  /// Filter type.
  kFilterType = BarelyInstrumentControlType_kFilterType,
  /// Filter cutoff.
  kFilterCutoff = BarelyInstrumentControlType_kFilterCutoff,
  /// Filter Q factor.
  kFilterQ = BarelyInstrumentControlType_kFilterQ,
  /// Delay send.
  kDelaySend = BarelyInstrumentControlType_kDelaySend,
  /// Reverb send.
  kReverbSend = BarelyInstrumentControlType_kReverbSend,
  /// Sidechain send.
  kSidechainSend = BarelyInstrumentControlType_kSidechainSend,
  /// Arpeggiator mode.
  kArpMode = BarelyInstrumentControlType_kArpMode,
  /// Arpeggiator gate ratio.
  kArpGateRatio = BarelyInstrumentControlType_kArpGateRatio,
  /// Arpeggiator rate.
  kArpRate = BarelyInstrumentControlType_kArpRate,
};

/// Note control types.
enum class NoteControlType {
  /// Normalized gain in logarithmic scale.
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
  /// Linear crossfade between the slice and the oscillator.
  kCrossfade = BarelyOscMode_kCrossfade,
  /// Amplitude modulation applied to the slice by the oscillator.
  kAm = BarelyOscMode_kAm,
  /// Frequency modulation applied to the slice by the oscillator.
  kFm = BarelyOscMode_kFm,
  /// Amplitude modulation applied to the oscillator by the slice.
  kMa = BarelyOscMode_kMa,
  /// Frequency modulation applied to the oscillator by the slice.
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
  /// Default constructor.
  Slice() noexcept = default;

  /// Constructs a new `Slice`.
  ///
  /// @param samples Span of mono samples.
  /// @param sample_rate Sampling rate in hertz.
  /// @param root_pitch Root pitch.
  explicit constexpr Slice(std::span<const float> samples, int sample_rate,
                           float root_pitch) noexcept
      : Slice({samples.data(), static_cast<int32_t>(samples.size()), sample_rate, root_pitch}) {
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

/// Class that wraps an instrument.
class Instrument {
 public:
  /// Default constructor.
  Instrument() noexcept = default;

  /// Constructs a new `Instrument`.
  ///
  /// @param engine Pointer to raw engine.
  /// @param instrument_id Instrument identifier.
  /// @param note_event_callback Pointer to note event callback.
  Instrument(BarelyEngine* engine, uint32_t instrument_id,
             NoteEventCallback* note_event_callback) noexcept
      : engine_(engine), instrument_id_(instrument_id), note_event_callback_(note_event_callback) {}

  /// Returns the identifier.
  ///
  /// @return Identifier.
  [[nodiscard]] constexpr operator uint32_t() const noexcept { return instrument_id_; }

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
        engine_, instrument_id_, static_cast<BarelyInstrumentControlType>(type), &value);
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
        engine_, instrument_id_, pitch, static_cast<BarelyNoteControlType>(type), &value);
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
        BarelyInstrument_IsNoteOn(engine_, instrument_id_, pitch, &is_note_on);
    assert(success);
    return is_note_on;
  }

  /// Sets all notes off.
  void SetAllNotesOff() noexcept {
    [[maybe_unused]] const bool success = BarelyInstrument_SetAllNotesOff(engine_, instrument_id_);
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
        engine_, instrument_id_, static_cast<BarelyInstrumentControlType>(type),
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
        engine_, instrument_id_, pitch, static_cast<BarelyNoteControlType>(type),
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
                  engine_, instrument_id_,
                  [](BarelyNoteEventType type, float pitch, void* user_data) noexcept {
                    assert(user_data != nullptr && "Invalid note event callback user data");
                    if (const auto& callback = *static_cast<NoteEventCallback*>(user_data);
                        callback) {
                      callback(static_cast<NoteEventType>(type), pitch);
                    }
                  },
                  note_event_callback_)
            : BarelyInstrument_SetNoteEventCallback(engine_, instrument_id_, nullptr, nullptr);
    assert(success);
  }

  /// Sets a note off.
  ///
  /// @param pitch Note pitch.
  void SetNoteOff(float pitch) noexcept {
    [[maybe_unused]] const bool success =
        BarelyInstrument_SetNoteOff(engine_, instrument_id_, pitch);
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
        engine_, instrument_id_, pitch,
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
        engine_, instrument_id_, reinterpret_cast<const BarelySlice*>(slices.data()),
        static_cast<int32_t>(slices.size()));
    assert(success);
  }

 private:
  // Pointer to raw engine.
  BarelyEngine* engine_ = nullptr;

  // Instrument identifier.
  uint32_t instrument_id_ = 0;

  // Pointer to note event callback.
  NoteEventCallback* note_event_callback_ = nullptr;
};

/// Class that wraps a task.
class Task {
 public:
  /// Default constructor.
  Task() noexcept = default;

  /// Constructs a new `Task`.
  ///
  /// @param engine Pointer to raw engine.
  /// @param task_id Task identifier.
  /// @param event_callback Pointer to task event callback.
  Task(BarelyEngine* engine, uint32_t task_id, TaskEventCallback* event_callback) noexcept
      : engine_(engine), task_id_(task_id), event_callback_(event_callback) {}

  /// Returns the identifier.
  ///
  /// @return Identifier.
  [[nodiscard]] constexpr operator uint32_t() const noexcept { return task_id_; }

  /// Returns the duration.
  ///
  /// @return Duration in beats.
  [[nodiscard]] double GetDuration() const noexcept {
    double duration = 0.0;
    [[maybe_unused]] const bool success = BarelyTask_GetDuration(engine_, task_id_, &duration);
    assert(success);
    return duration;
  }

  /// Returns the position.
  ///
  /// @return Position in beats.
  [[nodiscard]] double GetPosition() const noexcept {
    double position = 0.0;
    [[maybe_unused]] const bool success = BarelyTask_GetPosition(engine_, task_id_, &position);
    assert(success);
    return position;
  }

  /// Returns the priority.
  ///
  /// @return Priority.
  [[nodiscard]] int GetPriority() const noexcept {
    int32_t priority = 0;
    [[maybe_unused]] const bool success = BarelyTask_GetPriority(engine_, task_id_, &priority);
    assert(success);
    return static_cast<int>(priority);
  }

  /// Returns whether the task is active or not.
  ///
  /// @return True if active, false otherwise.
  [[nodiscard]] bool IsActive() const noexcept {
    bool is_active = false;
    [[maybe_unused]] const bool success = BarelyTask_IsActive(engine_, task_id_, &is_active);
    assert(success);
    return is_active;
  }

  /// Sets the duration.
  ///
  /// @param duration Duration in beats.
  void SetDuration(double duration) noexcept {
    [[maybe_unused]] const bool success = BarelyTask_SetDuration(engine_, task_id_, duration);
    assert(success);
  }

  /// Sets the event callback.
  ///
  /// @param callback Event callback.
  void SetEventCallback(TaskEventCallback callback) noexcept {
    assert(event_callback_ != nullptr);
    *event_callback_ = std::move(callback);
    [[maybe_unused]] const bool success =
        (*event_callback_)
            ? BarelyTask_SetEventCallback(
                  engine_, task_id_,
                  [](BarelyTaskEventType type, void* user_data) noexcept {
                    assert(user_data != nullptr && "Invalid task event callback user data");
                    if (const auto& callback = *static_cast<TaskEventCallback*>(user_data);
                        callback) {
                      callback(static_cast<TaskEventType>(type));
                    }
                  },
                  event_callback_)
            : BarelyTask_SetEventCallback(engine_, task_id_, nullptr, nullptr);
    assert(success);
  }

  /// Sets the position.
  ///
  /// @param position Position in beats.
  void SetPosition(double position) noexcept {
    [[maybe_unused]] const bool success = BarelyTask_SetPosition(engine_, task_id_, position);
    assert(success);
  }

  /// Sets the priority.
  ///
  /// @param priority Priority.
  void SetPriority(int priority) noexcept {
    [[maybe_unused]] const bool success =
        BarelyTask_SetPriority(engine_, task_id_, static_cast<int32_t>(priority));
    assert(success);
  }

 private:
  // Pointer to engine.
  BarelyEngine* engine_ = nullptr;

  // Task identifier.
  uint32_t task_id_ = 0;

  // Pointer to event callback.
  TaskEventCallback* event_callback_ = nullptr;
};

/// Class that wraps a performer.
class Performer {
 public:
  /// Default constructor.
  Performer() noexcept = default;

  /// Constructs a new `Performer`.
  ///
  /// @param engine Pointer to raw engine.
  /// @param performer_id Performer identifier.
  Performer(BarelyEngine* engine, uint32_t performer_id) noexcept
      : engine_(engine), performer_id_(performer_id) {}

  /// Returns the identifier.
  ///
  /// @return Identifier.
  [[nodiscard]] constexpr operator uint32_t() const noexcept { return performer_id_; }

  /// Returns the loop begin position.
  ///
  /// @return Loop begin position in beats.
  [[nodiscard]] double GetLoopBeginPosition() const noexcept {
    double loop_begin_position = 0.0;
    [[maybe_unused]] const bool success =
        BarelyPerformer_GetLoopBeginPosition(engine_, performer_id_, &loop_begin_position);
    assert(success);
    return loop_begin_position;
  }

  /// Returns the loop length.
  ///
  /// @return Loop length in beats.
  [[nodiscard]] double GetLoopLength() const noexcept {
    double loop_length = 0.0;
    [[maybe_unused]] const bool success =
        BarelyPerformer_GetLoopLength(engine_, performer_id_, &loop_length);
    assert(success);
    return loop_length;
  }

  /// Returns the position.
  ///
  /// @return Position in beats.
  [[nodiscard]] double GetPosition() const noexcept {
    double position = 0.0;
    [[maybe_unused]] const bool success =
        BarelyPerformer_GetPosition(engine_, performer_id_, &position);
    assert(success);
    return position;
  }

  /// Returns whether the performer is looping or not.
  ///
  /// @return True if looping, false otherwise.
  [[nodiscard]] bool IsLooping() const noexcept {
    bool is_looping = false;
    [[maybe_unused]] const bool success =
        BarelyPerformer_IsLooping(engine_, performer_id_, &is_looping);
    assert(success);
    return is_looping;
  }

  /// Returns whether the performer is playing or not.
  ///
  /// @return True if playing, false otherwise.
  [[nodiscard]] bool IsPlaying() const noexcept {
    bool is_playing = false;
    [[maybe_unused]] const bool success =
        BarelyPerformer_IsPlaying(engine_, performer_id_, &is_playing);
    assert(success);
    return is_playing;
  }

  /// Sets the loop begin position.
  ///
  /// @param loop_begin_position Loop begin position in beats.
  void SetLoopBeginPosition(double loop_begin_position) noexcept {
    [[maybe_unused]] const bool success =
        BarelyPerformer_SetLoopBeginPosition(engine_, performer_id_, loop_begin_position);
    assert(success);
  }

  /// Sets the loop length.
  ///
  /// @param loop_length Loop length in beats.
  void SetLoopLength(double loop_length) noexcept {
    [[maybe_unused]] const bool success =
        BarelyPerformer_SetLoopLength(engine_, performer_id_, loop_length);
    assert(success);
  }

  /// Sets whether the performer is looping or not.
  ///
  /// @param is_looping True if looping, false otherwise.
  void SetLooping(bool is_looping) noexcept {
    [[maybe_unused]] const bool success =
        BarelyPerformer_SetLooping(engine_, performer_id_, is_looping);
    assert(success);
  }

  /// Sets the position.
  ///
  /// @param position Position in beats.
  void SetPosition(double position) noexcept {
    [[maybe_unused]] const bool success =
        BarelyPerformer_SetPosition(engine_, performer_id_, position);
    assert(success);
  }

  /// Starts the playback.
  void Start() noexcept {
    [[maybe_unused]] const bool success = BarelyPerformer_Start(engine_, performer_id_);
    assert(success);
  }

  /// Stops the playback.
  void Stop() noexcept {
    [[maybe_unused]] const bool success = BarelyPerformer_Stop(engine_, performer_id_);
    assert(success);
  }

 private:
  // Pointer to raw engine.
  BarelyEngine* engine_;

  // Performer identifier.
  uint32_t performer_id_;
};

/// A class that wraps an engine.
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

  /// Constructs a new `Engine`.
  ///
  /// @param engine Pointer to raw engine.
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

  /// Returns the pointer to raw engine.
  ///
  /// @return Pointer to raw engine.
  [[nodiscard]] constexpr operator BarelyEngine*() const noexcept { return engine_; }

  /// Creates a new instrument.
  ///
  /// @param control_overrides Span of instrument control overrides.
  /// @return Instrument.
  Instrument CreateInstrument(
      std::span<const InstrumentControlOverride> control_overrides = {}) noexcept {
    uint32_t instrument_id = 0;
    [[maybe_unused]] const bool success = BarelyEngine_CreateInstrument(
        engine_, reinterpret_cast<const BarelyInstrumentControlOverride*>(control_overrides.data()),
        static_cast<int32_t>(control_overrides.size()), &instrument_id);
    assert(success);
    NoteEventCallback& note_event_callback =
        note_event_callbacks_.get()[(instrument_id & ((1 << BARELY_ID_INDEX_BIT_COUNT) - 1)) - 1];
    note_event_callback = {};
    return Instrument(engine_, instrument_id, &note_event_callback);
  }

  /// Creates a new performer.
  ///
  /// @return Performer.
  Performer CreatePerformer() noexcept {
    uint32_t performer_id = 0;
    [[maybe_unused]] const bool success = BarelyEngine_CreatePerformer(engine_, &performer_id);
    assert(success);
    return Performer(engine_, performer_id);
  }

  /// Creates a new task.
  ///
  /// @param performer_id Performer identifier.
  /// @param position Task position in beats.
  /// @param duration Task duration in beats.
  /// @param priority Task priority.
  /// @param callback Task event callback.
  /// @return Task.
  Task CreateTask(Performer performer, double position, double duration, int priority,
                  TaskEventCallback callback) noexcept {
    uint32_t task_id = 0;
    [[maybe_unused]] bool success = BarelyEngine_CreateTask(engine_, performer, position, duration,
                                                            priority, nullptr, nullptr, &task_id);
    assert(success);
    TaskEventCallback& task_event_callback =
        task_event_callbacks_.get()[(task_id & ((1 << BARELY_ID_INDEX_BIT_COUNT) - 1)) - 1];
    task_event_callback = std::move(callback);
    success = BarelyTask_SetEventCallback(
        engine_, task_id,
        [](BarelyTaskEventType type, void* user_data) noexcept {
          if (user_data != nullptr) {
            (*static_cast<TaskEventCallback*>(user_data))(static_cast<TaskEventType>(type));
          }
        },
        &task_event_callback);
    assert(success);
    return Task(engine_, task_id, &task_event_callback);
  }

  /// Destroys an instrument.
  ///
  /// @param instrument Instrument.
  void DestroyInstrument(Instrument instrument) {
    [[maybe_unused]] const bool success = BarelyEngine_DestroyInstrument(engine_, instrument);
    assert(success);
  }

  /// Destroys a performer.
  ///
  /// @param performer Performer.
  void DestroyPerformer(Performer performer) {
    [[maybe_unused]] const bool success = BarelyEngine_DestroyPerformer(engine_, performer);
    assert(success);
  }

  /// Destroys a task.
  ///
  /// @param task Task.
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
  // Pointer to raw engine.
  BarelyEngine* engine_ = nullptr;

  // Heap allocated array of note event callbacks (for pointer stability on move).
  std::unique_ptr<NoteEventCallback[]> note_event_callbacks_ =
      std::make_unique<NoteEventCallback[]>(BARELY_MAX_INSTRUMENT_COUNT);

  // Heap allocated array of task event callbacks (for pointer stability on move).
  std::unique_ptr<TaskEventCallback[]> task_event_callbacks_ =
      std::make_unique<TaskEventCallback[]>(BARELY_MAX_TASK_COUNT);
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
