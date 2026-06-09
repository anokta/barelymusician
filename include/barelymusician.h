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
///   // Set the instrument to use full oscillator mix.
///   instrument.SetControl(barely::InstrumentControlType::kOscMix, /*value=*/1.0f);
///
///   // Destroy the instrument.
///   instrument.Destroy();
///   @endcode
///
/// - Performer:
///
///   @code{.cpp}
///   // Create a new performer.
///   auto performer = engine.CreatePerformer();
///
///   // Create a new task.
///   auto task = performer.CreateTask(/*position=*/0.0, /*duration=*/1.0,
///                                    [](barely::TaskEventType type) { /*populate this*/ });
///
///   // Set the performer to looping.
///   performer.SetLooping(/*is_looping=*/true);
///
///   // Start the performer playback.
///   performer.Start();
///
///   // Destroy the performer.
///   performer.Destroy();
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
///   const BarelyEngineConfig config = BARELY_ENGINE_CONFIG_DEFAULT(/*sample_rate=*/48000);
///   int32_t allocation_size = 0;
///   BarelyEngineConfig_GetRequiredAllocationSize(&config, &allocation_size);
///   void* allocation = malloc(allocation_size);
///   BarelyEngine* engine = NULL;
///   BarelyEngine_Create(&config, allocation, allocation_size, &engine);
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
///   const double lookahead = 0.1;
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
///   free(allocation);
///   @endcode
///
/// - Instrument:
///
///   @code{.cpp}
///   // Create a new instrument.
///   uint32_t instrument_id = 0;
///   BarelyEngine_CreateInstrument(engine, &instrument_id);
///
///   // Set an instrument note on.
///   //
///   // The note pitch is expressed in octaves relative to middle C as the center frequency.
///   // Fractional note values adjust the frequency logarithmically to ensure equally perceived
///   // pitch intervals within each octave.
///   const float c3_pitch = -1.0f;
///   BarelyInstrument_SetNoteOn(engine, instrument_id, c3_pitch);
///
///   // Set the instrument to use full oscillator mix.
///   BarelyInstrument_SetControl(engine, instrument_id, BarelyInstrumentControlType_kOscMix,
///                               /*value=*/1.0f);
///
///   // Destroy the instrument.
///   BarelyInstrument_Destroy(engine, instrument_id);
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
///   BarelyTaskCallback callback{ /*populate this*/ };
///   BarelyPerformer_CreateTask(engine, performer_id, /*position=*/0.0, /*duration=*/1.0,
///                              /*priority=*/0, callback, &task_id);
///
///   // Set the performer to looping.
///   BarelyPerformer_SetLooping(engine, performer_id, /*is_looping=*/true);
///
///   // Start the performer playback.
///   BarelyPerformer_Start(engine, performer_id);
///
///   // Destroy the performer.
///   BarelyPerformer_Destroy(engine, performer_id);
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

#define BARELY_C_ENUM_VALUE(EnumType, Name, ...) Barely##EnumType##_k##Name,
#define BARELY_C_ENUM(EnumType, X)                                \
  typedef enum Barely##EnumType{                                  \
      X(EnumType, BARELY_C_ENUM_VALUE) Barely##EnumType##_kCount, \
  } Barely##EnumType;
#ifdef __cplusplus
#define BARELY_CPP_ENUM_VALUE(EnumType, Name, ...) k##Name = Barely##EnumType##_k##Name,
#define BARELY_CPP_ENUM(EnumType, X)                          \
  namespace barely {                                          \
  enum class EnumType { X(EnumType, BARELY_CPP_ENUM_VALUE) }; \
  }
#define BARELY_ENUM(EnumType, X) BARELY_C_ENUM(EnumType, X) BARELY_CPP_ENUM(EnumType, X)
#else  // __cplusplus
#define BARELY_ENUM(EnumType, X) BARELY_C_ENUM(EnumType, X)
#endif  // __cplusplus

/// Default engine configuration.
#define BARELY_ENGINE_CONFIG_DEFAULT(sample_rate) \
  {                                               \
      .sample_##rate = sample_rate,               \
      .max_instrument_count = 100,                \
      .max_performer_count = 100,                 \
      .max_task_count = 5000,                     \
      .max_command_count = 8192,                  \
      .max_frame_count = 2048,                    \
      .max_slice_count = 1000,                    \
      .max_voice_count = 200,                     \
  }

/// Engine control types.
#define BARELY_ENGINE_CONTROL_TYPES(EngineControlType, X)                           \
  X(EngineControlType, Gain, 1.0f, 0.0f, 1.0f, "Gain")                              \
  X(EngineControlType, CompMix, 1.0f, 0.0f, 1.0f, "Compressor Mix")                 \
  X(EngineControlType, CompAttack, 0.0f, 0.0f, 8.0f, "Compressor Attack")           \
  X(EngineControlType, CompRelease, 0.0f, 0.0f, 8.0f, "Compressor Release")         \
  X(EngineControlType, CompThreshold, 1.0f, 0.0f, 1.0f, "Compressor Threshold")     \
  X(EngineControlType, CompRatio, 0.0f, 0.0f, 1.0f, "Compressor Ratio")             \
  X(EngineControlType, DelayMix, 1.0f, 0.0f, 1.0f, "Delay Mix")                     \
  X(EngineControlType, DelayTime, 0.0f, 0.0f, 8.0f, "Delay Time")                   \
  X(EngineControlType, DelayFeedback, 0.0f, 0.0f, 1.0f, "Delay Feedback")           \
  X(EngineControlType, DelayLpfCutoff, 1.0f, 0.0f, 1.0f, "Delay LPF Cutoff")        \
  X(EngineControlType, DelayHpfCutoff, 0.0f, 0.0f, 1.0f, "Delay HPF Cutoff")        \
  X(EngineControlType, DelayPingPong, 0.0f, 0.0f, 1.0f, "Delay Ping-Pong")          \
  X(EngineControlType, DelayReverbSend, 0.0f, 0.0f, 2.0f, "Delay Reverb Send")      \
  X(EngineControlType, ReverbMix, 1.0f, 0.0f, 1.0f, "Reverb Mix")                   \
  X(EngineControlType, ReverbDamping, 0.0f, 0.0f, 1.0f, "Reverb Damping")           \
  X(EngineControlType, ReverbRoomSize, 0.0f, 0.0f, 1.0f, "Reverb Room Size")        \
  X(EngineControlType, ReverbStereoWidth, 1.0f, 0.0f, 1.0f, "Reverb Stereo Width")  \
  X(EngineControlType, ReverbFreeze, 0, 0, 1, "Reverb Freeze")                      \
  X(EngineControlType, SidechainMix, 1.0f, 0.0f, 1.0f, "Sidechain Mix")             \
  X(EngineControlType, SidechainAttack, 0.0f, 0.0f, 8.0f, "Sidechain Attack")       \
  X(EngineControlType, SidechainRelease, 0.0f, 0.0f, 8.0f, "Sidechain Release")     \
  X(EngineControlType, SidechainThreshold, 1.0f, 0.0f, 1.0f, "Sidechain Threshold") \
  X(EngineControlType, SidechainRatio, 0.0f, 0.0f, 1.0f, "Sidechain Ratio")
BARELY_ENUM(EngineControlType, BARELY_ENGINE_CONTROL_TYPES)

/// Instrument control types.
#define BARELY_INSTRUMENT_CONTROL_TYPES(InstrumentControlType, X)                      \
  X(InstrumentControlType, Gain, 1.0f, 0.0f, 1.0f, "Gain")                             \
  X(InstrumentControlType, PitchShift, 0.0f, -2.0f, 2.0f, "Pitch Shift")               \
  X(InstrumentControlType, StereoPan, 0.0f, -1.0f, 1.0f, "Stereo Pan")                 \
  X(InstrumentControlType, Attack, 0.0f, 0.0f, 8.0f, "Envelope Attack")                \
  X(InstrumentControlType, Decay, 0.0f, 0.0f, 8.0f, "Envelope Decay")                  \
  X(InstrumentControlType, Sustain, 1.0f, 0.0f, 1.0f, "Envelope Sustain")              \
  X(InstrumentControlType, Release, 0.0f, 0.0f, 8.0f, "Envelope Release")              \
  X(InstrumentControlType, SliceMode, 0, 0, BarelySliceMode_kCount - 1, "Slice Mode")  \
  X(InstrumentControlType, OscMix, 0.0f, 0.0f, 1.0f, "Oscillator Mix")                 \
  X(InstrumentControlType, OscMode, 0, 0, BarelyOscMode_kCount - 1, "Oscillator Mode") \
  X(InstrumentControlType, OscNoiseMix, 0.0f, 0.0f, 1.0f, "Oscillator Noise Mix")      \
  X(InstrumentControlType, OscPitchShift, 0.0f, -2.0f, 2.0f, "Oscillator Pitch Shift") \
  X(InstrumentControlType, OscShape, 0.0f, 0.0f, 1.0f, "Oscillator Shape")             \
  X(InstrumentControlType, OscSkew, 0.0f, -1.0f, 1.0f, "Oscillator Skew")              \
  X(InstrumentControlType, CrushDepth, 0.0f, 0.0f, 1.0f, "Bit Crusher Depth")          \
  X(InstrumentControlType, CrushRate, 0.0f, 0.0f, 1.0f, "Bit Crusher Rate")            \
  X(InstrumentControlType, DistortionMix, 0.0f, 0.0f, 1.0f, "Distortion Mix")          \
  X(InstrumentControlType, DistortionDrive, 0.0f, 0.0f, 1.0f, "Distortion Drive")      \
  X(InstrumentControlType, FilterCutoff, 1.0f, 0.0f, 1.0f, "Filter Cutoff")            \
  X(InstrumentControlType, FilterResonance, 0.5f, 0.0f, 1.0f, "Filter Resonance")      \
  X(InstrumentControlType, FilterTone, 0.0f, -1.0f, 1.0f, "Filter Tone")               \
  X(InstrumentControlType, DelaySend, 0.0f, 0.0f, 1.0f, "Delay Send")                  \
  X(InstrumentControlType, ReverbSend, 0.0f, 0.0f, 2.0f, "Reverb Send")                \
  X(InstrumentControlType, SidechainSend, 0.0f, -1.0f, 1.0f, "Sidechain Send")         \
  X(InstrumentControlType, Retrigger, 0, 0, 1, "Retrigger")                            \
  X(InstrumentControlType, VoiceCount, 8, 1, 16, "Voice Count")
BARELY_ENUM(InstrumentControlType, BARELY_INSTRUMENT_CONTROL_TYPES)

/// Note control types.
#define BARELY_NOTE_CONTROL_TYPES(NoteControlType, X) \
  X(NoteControlType, Gain, 1.0f, 0.0f, 1.0f, "Gain")  \
  X(NoteControlType, PitchShift, 0.0f, -2.0f, 2.0f, "Pitch Shift")
BARELY_ENUM(NoteControlType, BARELY_NOTE_CONTROL_TYPES)

/// Oscillator modes.
#define BARELY_OSC_MODES(OscMode, X)                               \
  X(OscMode, Crossfade, "Linear Crossfade (slice <-> oscillator)") \
  X(OscMode, Am, "Amplitude Modulation (oscillator -> slice)")     \
  X(OscMode, Fm, "Frequency Modulation (oscillator -> slice)")     \
  X(OscMode, Ma, "Amplitude Modulation (slice -> oscillator)")     \
  X(OscMode, Mf, "Frequency Modulation (slice -> oscillator)")     \
  X(OscMode, Ring, "Ring Modulation")
BARELY_ENUM(OscMode, BARELY_OSC_MODES)

/// Slice modes.
#define BARELY_SLICE_MODES(SliceMode, X) \
  X(SliceMode, Sustain, "Sustain")       \
  X(SliceMode, Loop, "Loop")             \
  X(SliceMode, Once, "Once")
BARELY_ENUM(SliceMode, BARELY_SLICE_MODES)

/// Task event types.
#define BARELY_TASK_EVENT_TYPES(TaskEventType, X) \
  X(TaskEventType, Begin, "Begin")                \
  X(TaskEventType, End, "End")
BARELY_ENUM(TaskEventType, BARELY_TASK_EVENT_TYPES)

/// Engine handle.
typedef struct BarelyEngine BarelyEngine;

/// Engine configuration.
typedef struct BarelyEngineConfig {
  /// Sampling rate in hertz.
  int32_t sample_rate;

  /// Maximum number of instruments.
  int32_t max_instrument_count;

  /// Maximum number of performers.
  int32_t max_performer_count;

  /// Maximum number of tasks.
  int32_t max_task_count;

  /// Maximum number of commands to process per call.
  int32_t max_command_count;

  /// Maximum number of frames to process per call.
  int32_t max_frame_count;

  /// Maximum number of active slices.
  int32_t max_slice_count;

  /// Maximum number of active voices.
  int32_t max_voice_count;
} BarelyEngineConfig;

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

/// Musical quantization.
typedef struct BarelyQuantization {
  /// Subdivision of a beat.
  int32_t subdivision;

  /// Amount.
  float amount;
} BarelyQuantization;

/// Musical scale.
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

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Task callback.
///
/// @param type Task event type.
/// @param user_data Pointer to user data.
typedef void (*BarelyTaskCallback)(BarelyTaskEventType type, void* user_data);

/// Gets the required memory allocation size for an engine configuration.
///
/// @param config Pointer to engine configuration.
/// @param out_allocation_size Output required memory allocation size.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngineConfig_GetRequiredAllocationSize(const BarelyEngineConfig* config,
                                                             int32_t* out_allocation_size);

/// Creates a new engine.
///
/// @param config Pointer to engine configuration.
/// @param allocation Pointer to memory allocation.
/// @param allocation_size Memory allocation size.
/// @param out_engine Output pointer to engine.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_Create(const BarelyEngineConfig* config, void* allocation,
                                    int32_t allocation_size, BarelyEngine** out_engine);

/// Creates a new instrument.
///
/// @param engine Pointer to engine.
/// @param out_instrument_id Output instrument identifier.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_CreateInstrument(BarelyEngine* engine, uint32_t* out_instrument_id);

/// Creates a new performer.
///
/// @param engine Pointer to engine.
/// @param out_performer_id Output performer identifier.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_CreatePerformer(BarelyEngine* engine, uint32_t* out_performer_id);

/// Destroys an engine.
///
/// @param engine Pointer to engine.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_Destroy(BarelyEngine* engine);

/// Generates a new random number with uniform distribution in the normalized range [0, 1).
///
/// @param engine Pointer to engine.
/// @param out_number Output random number.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_GenerateRandomNumber(BarelyEngine* engine, double* out_number);

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

/// Resets the random number generator seed of an engine.
///
/// @param engine Pointer to engine.
/// @param seed Seed value.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_ResetSeed(BarelyEngine* engine, int32_t seed);

/// Sets a control value of an engine.
///
/// @param engine Pointer to engine.
/// @param type Engine control type.
/// @param value Engine control value.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_SetControl(BarelyEngine* engine, BarelyEngineControlType type,
                                        float value);

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

/// Destroys an instrument.
///
/// @param engine Pointer to engine.
/// @param instrument_id Instrument identifier.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_Destroy(BarelyEngine* engine, uint32_t instrument_id);

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
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_SetNoteOn(BarelyEngine* engine, uint32_t instrument_id,
                                           float pitch);

/// Sets instrument sample data.
///
/// @param engine Pointer to engine.
/// @param instrument_id Instrument identifier.
/// @param slices Array of slices.
/// @param slice_count Number of slices.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_SetSampleData(BarelyEngine* engine, uint32_t instrument_id,
                                               const BarelySlice* slices, int32_t slice_count);

/// Creates a new performer task.
///
/// @param engine Pointer to engine.
/// @param performer_id Performer identifier.
/// @param position Task position in beats.
/// @param duration Task duration in beats.
/// @param priority Task priority.
/// @param callback Task callback.
/// @param user_data Pointer to user data.
/// @param out_task_id Output task identifier.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_CreateTask(BarelyEngine* engine, uint32_t performer_id,
                                           double position, double duration, int32_t priority,
                                           BarelyTaskCallback callback, void* user_data,
                                           uint32_t* out_task_id);

/// Destroys a performer.
///
/// @param engine Pointer to engine.
/// @param performer_id Performer identifier.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_Destroy(BarelyEngine* engine, uint32_t performer_id);

/// Gets the position of a performer.
///
/// @param engine Pointer to engine.
/// @param performer_id Performer identifier.
/// @param out_position Output position in beats.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyPerformer_GetPosition(const BarelyEngine* engine, uint32_t performer_id,
                                            double* out_position);

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

/// Destroys a task.
///
/// @param engine Pointer to engine.
/// @param task_id Task identifier.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyTask_Destroy(BarelyEngine* engine, uint32_t task_id);

/// Gets whether the task is active or not.
///
/// @param engine Pointer to engine.
/// @param task_id Task identifier.
/// @param out_is_active Output true if active, false otherwise.
BARELY_API bool BarelyTask_IsActive(const BarelyEngine* engine, uint32_t task_id,
                                    bool* out_is_active);

/// Sets the callback of a task.
///
/// @param engine Pointer to engine.
/// @param task_id Task identifier.
/// @param callback Task callback.
/// @param user_data Pointer to user data.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyTask_SetCallback(BarelyEngine* engine, uint32_t task_id,
                                       BarelyTaskCallback callback, void* user_data);

/// Sets the duration of a task.
///
/// @param engine Pointer to engine.
/// @param task_id Task identifier.
/// @param duration Task duration in beats.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyTask_SetDuration(BarelyEngine* engine, uint32_t task_id, double duration);

/// Sets the position of a task.
///
/// @param engine Pointer to engine.
/// @param task_id Task identifier.
/// @param position Task position in beats.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyTask_SetPosition(BarelyEngine* engine, uint32_t task_id, double position);

/// Sets the priority of a task.
///
/// @param engine Pointer to engine.
/// @param task_id Task identifier.
/// @param priority Task priority.
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
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <random>
#include <span>
#include <type_traits>
#include <utility>

namespace barely {

/// Engine configuration.
struct EngineConfig : public BarelyEngineConfig {
 public:
  /// Default constructor.
  EngineConfig() noexcept = default;

  /// Constructs a new `EngineConfig`.
  ///
  /// @param sample_rate Sampling rate in hertz.
  constexpr explicit EngineConfig(int sample_rate)
      : EngineConfig(BARELY_ENGINE_CONFIG_DEFAULT(sample_rate)) {}

  /// Constructs a new `EngineConfig` from a raw type.
  ///
  /// @param config Raw engine configuration.
  // NOLINTNEXTLINE(google-explicit-constructor)
  constexpr EngineConfig(BarelyEngineConfig config) noexcept : BarelyEngineConfig{config} {}

  /// Returns the required memory allocation size.
  ///
  /// @return Required memory allocation size.
  [[nodiscard]] int32_t GetRequiredAllocationSize() const noexcept {
    int32_t allocation_size = 0;
    [[maybe_unused]] const bool success =
        BarelyEngineConfig_GetRequiredAllocationSize(this, &allocation_size);
    assert(success);
    return allocation_size;
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
  constexpr Slice(std::span<const float> samples, int sample_rate, float root_pitch) noexcept
      : Slice({samples.data(), static_cast<int32_t>(samples.size()), sample_rate, root_pitch}) {
    assert(sample_rate >= 0);
  }

  /// Constructs a new `Slice` from a raw type.
  ///
  /// @param slice Raw slice.
  // NOLINTNEXTLINE(google-explicit-constructor)
  constexpr Slice(BarelySlice slice) noexcept : BarelySlice{slice} {}
};

/// Musical quantization.
struct Quantization : public BarelyQuantization {
 public:
  /// Default constructor.
  constexpr Quantization() noexcept = default;

  /// Constructs a new `Quantization`.
  ///
  /// @param subdivision Subdivision of a beat.
  /// @param amount Amount.
  // NOLINTNEXTLINE(google-explicit-constructor)
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

/// Musical scale.
struct Scale : public BarelyScale {
 public:
  /// Default constructor.
  constexpr Scale() noexcept = default;

  /// Constructs a new `Scale`.
  ///
  /// @param pitches Span of pitches.
  /// @param root_pitch Root pitch.
  /// @param mode Mode.
  // NOLINTNEXTLINE(google-explicit-constructor)
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

/// Task callback function.
///
/// @param type Task event type.
using TaskCallback = std::function<void(TaskEventType type)>;

/// Class that wraps an instrument.
class Instrument {
 public:
  /// Default constructor.
  Instrument() noexcept = default;

  /// Destroys the instrument.
  void Destroy() noexcept {
    BarelyInstrument_Destroy(std::exchange(engine_, nullptr), std::exchange(instrument_id_, 0));
  }

  /// Returns the identifier.
  ///
  /// @return Identifier.
  // NOLINTNEXTLINE(google-explicit-constructor)
  [[nodiscard]] constexpr operator uint32_t() const noexcept { return instrument_id_; }

  /// Sets a control value.
  ///
  /// @param type Instrument control type.
  /// @param value Instrument control value.
  template <typename ValueType>
  void SetControl(InstrumentControlType type, ValueType value) noexcept {
    static_assert(std::is_arithmetic_v<ValueType> || std::is_enum_v<ValueType>,
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
    static_assert(std::is_arithmetic_v<ValueType> || std::is_enum_v<ValueType>,
                  "ValueType is not supported");
    [[maybe_unused]] const bool success = BarelyInstrument_SetNoteControl(
        engine_, instrument_id_, pitch, static_cast<BarelyNoteControlType>(type),
        static_cast<float>(value));
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
  /// @param gain Note gain.
  /// @param pitch_shift Note pitch shift.
  void SetNoteOn(float pitch, float gain = 1.0f, float pitch_shift = 0.0f) noexcept {
    [[maybe_unused]] const bool success =
        BarelyInstrument_SetNoteOn(engine_, instrument_id_, pitch);
    assert(success);
    if (gain != 1.0f) {
      SetNoteControl(pitch, NoteControlType::kGain, gain);
    }
    if (pitch_shift != 0.0f) {
      SetNoteControl(pitch, NoteControlType::kPitchShift, pitch_shift);
    }
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
  friend class Engine;
  Instrument(BarelyEngine* engine, uint32_t instrument_id) noexcept
      : engine_(engine), instrument_id_(instrument_id) {}
  BarelyEngine* engine_ = nullptr;
  uint32_t instrument_id_ = 0;
};

/// Class that wraps a task.
class Task {
 public:
  /// Default constructor.
  Task() noexcept = default;

  /// Returns the identifier.
  ///
  /// @return Identifier.
  // NOLINTNEXTLINE(google-explicit-constructor)
  [[nodiscard]] constexpr operator uint32_t() const noexcept { return task_id_; }

  /// Destroys the task.
  void Destroy() noexcept {
    if (BarelyTask_Destroy(std::exchange(engine_, nullptr), std::exchange(task_id_, 0))) {
      if (task_callback_ == *first_task_callback_) {
        *first_task_callback_ = task_callback_->next;
      }
      ReleaseTaskCallback(task_callbacks_, task_callback_);
    }
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

  /// Sets the callback.
  ///
  /// @param callback Callback.
  void SetCallback(TaskCallback callback) noexcept {
    assert(task_callback_ != nullptr);
    task_callback_->callback = std::move(callback);
    [[maybe_unused]] const bool success =
        (task_callback_->callback)
            ? BarelyTask_SetCallback(
                  engine_, task_id_,
                  [](BarelyTaskEventType type, void* user_data) noexcept {
                    assert(user_data != nullptr && "Invalid task callback user data");
                    if (const auto& callback = *static_cast<TaskCallback*>(user_data); callback) {
                      callback(static_cast<TaskEventType>(type));
                    }
                  },
                  &task_callback_->callback)
            : BarelyTask_SetCallback(engine_, task_id_, nullptr, nullptr);
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
  friend class Engine;
  friend class Performer;
  struct CallbackNode {
    TaskCallback callback = nullptr;
    CallbackNode* prev = nullptr;
    CallbackNode* next = nullptr;
  };
  template <typename T>
  struct Pool {
    Pool() noexcept = default;
    explicit Pool(int32_t capacity) noexcept
        : items_(std::make_unique<T[]>(capacity)),
          free_(std::make_unique<T*[]>(capacity)),
          free_count_(capacity) {
      for (int32_t i = 0; i < capacity; ++i) {
        free_[i] = &items_[i];
      }
    }
    [[nodiscard]] T* Acquire() noexcept { return free_[--free_count_]; }
    void Release(T* item) noexcept { free_[free_count_++] = item; }
    std::unique_ptr<T[]> items_;
    std::unique_ptr<T*[]> free_;
    int32_t free_count_ = 0;
  };
  static void ReleaseTaskCallback(Pool<CallbackNode>* task_callbacks,
                                  CallbackNode* task_callback) noexcept {
    if (task_callback->prev != nullptr) {
      task_callback->prev->next = task_callback->next;
    }
    if (task_callback->next != nullptr) {
      task_callback->next->prev = task_callback->prev;
    }
    task_callbacks->Release(task_callback);
  }
  Task(Pool<CallbackNode>* task_callbacks, CallbackNode** first_task_callback,
       CallbackNode* task_callback, BarelyEngine* engine, uint32_t task_id) noexcept
      : task_callbacks_(task_callbacks),
        first_task_callback_(first_task_callback),
        task_callback_(task_callback),
        engine_(engine),
        task_id_(task_id) {}
  Pool<CallbackNode>* task_callbacks_ = nullptr;
  CallbackNode** first_task_callback_ = nullptr;
  CallbackNode* task_callback_ = nullptr;
  BarelyEngine* engine_ = nullptr;
  uint32_t task_id_ = 0;
};

/// Class that wraps a performer.
class Performer {
 public:
  /// Default constructor.
  Performer() noexcept = default;

  /// Returns the identifier.
  ///
  /// @return Identifier.
  // NOLINTNEXTLINE(google-explicit-constructor)
  [[nodiscard]] constexpr operator uint32_t() const noexcept { return performer_id_; }

  /// Creates a new task.
  ///
  /// @param position Task position in beats.
  /// @param duration Task duration in beats.
  /// @param priority Task priority.
  /// @param callback Task callback.
  /// @return Task.
  Task CreateTask(double position, double duration, int priority, TaskCallback callback) noexcept {
    uint32_t task_id = 0;
    [[maybe_unused]] bool success = BarelyPerformer_CreateTask(
        engine_, performer_id_, position, duration, priority, nullptr, nullptr, &task_id);
    assert(success);

    Task::CallbackNode* task_callback = task_callbacks_->Acquire();
    *task_callback = {.callback = std::move(callback)};

    if (*first_task_callback_ != nullptr) {
      (*first_task_callback_)->prev = task_callback;
      task_callback->next = *first_task_callback_;
    }
    *first_task_callback_ = task_callback;

    success = BarelyTask_SetCallback(
        engine_, task_id,
        [](BarelyTaskEventType type, void* user_data) noexcept {
          if (user_data != nullptr) {
            (*static_cast<TaskCallback*>(user_data))(static_cast<TaskEventType>(type));
          }
        },
        &task_callback->callback);
    assert(success);
    return {task_callbacks_, first_task_callback_, task_callback, engine_, task_id};
  }

  /// Destroys the performer.
  void Destroy() noexcept {
    if (BarelyPerformer_Destroy(std::exchange(engine_, nullptr), std::exchange(performer_id_, 0))) {
      Task::CallbackNode* task_callback = *first_task_callback_;
      while (task_callback != nullptr) {
        Task::CallbackNode* next_task_callback = task_callback->next;
        Task::ReleaseTaskCallback(task_callbacks_, task_callback);
        task_callback = next_task_callback;
      }
      first_task_callbacks_->Release(first_task_callback_);
    }
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
  friend class Engine;
  Performer(Task::Pool<Task::CallbackNode>* task_callbacks,
            Task::Pool<Task::CallbackNode*>* first_task_callbacks, BarelyEngine* engine,
            uint32_t performer_id) noexcept
      : task_callbacks_(task_callbacks),
        first_task_callbacks_(first_task_callbacks),
        engine_(engine),
        performer_id_(performer_id) {
    first_task_callback_ = first_task_callbacks->Acquire();
    *first_task_callback_ = nullptr;
  }
  Task::Pool<Task::CallbackNode>* task_callbacks_ = nullptr;
  Task::Pool<Task::CallbackNode*>* first_task_callbacks_ = nullptr;
  Task::CallbackNode** first_task_callback_ = nullptr;
  BarelyEngine* engine_ = nullptr;
  uint32_t performer_id_ = 0;
};

/// A class that wraps an engine.
class Engine {
 public:
  /// Constructs a new `Engine`.
  ///
  /// @param sample_rate Sampling rate in hertz.
  explicit Engine(int sample_rate) noexcept : Engine(EngineConfig(sample_rate)) {}

  /// Constructs a new `Engine`.
  ///
  /// @param config Engine configuration.
  explicit Engine(const EngineConfig& config) noexcept
      : task_callbacks_(std::make_unique<Task::Pool<Task::CallbackNode>>(config.max_task_count)),
        first_task_callbacks_(
            std::make_unique<Task::Pool<Task::CallbackNode*>>(config.max_performer_count)),
        allocation_(config.GetRequiredAllocationSize()) {
    [[maybe_unused]] const bool success = BarelyEngine_Create(
        &config, allocation_.data(), static_cast<int32_t>(allocation_.size()), &engine_);
    assert(success);
  }

  /// Destroys `Engine`.
  ~Engine() noexcept { BarelyEngine_Destroy(engine_); }

  /// Non-copyable.
  Engine(const Engine& other) noexcept = delete;
  Engine& operator=(const Engine& other) noexcept = delete;

  /// Constructs a new `Engine` via move.
  ///
  /// @param other Other engine.
  Engine(Engine&& other) noexcept
      : task_callbacks_(std::exchange(other.task_callbacks_, {})),
        first_task_callbacks_(std::exchange(other.first_task_callbacks_, {})),
        allocation_(std::exchange(other.allocation_, {})),
        engine_(std::exchange(other.engine_, nullptr)) {}

  /// Assigns `Engine` via move.
  ///
  /// @param other Other engine.
  /// @return Engine.
  Engine& operator=(Engine&& other) noexcept {
    if (this != &other) {
      BarelyEngine_Destroy(engine_);
      task_callbacks_ = std::exchange(other.task_callbacks_, {});
      first_task_callbacks_ = std::exchange(other.first_task_callbacks_, {});
      allocation_ = std::exchange(other.allocation_, {});
      engine_ = std::exchange(other.engine_, nullptr);
    }
    return *this;
  }

  /// Returns the pointer to raw engine.
  ///
  /// @return Pointer to raw engine.
  // NOLINTNEXTLINE(google-explicit-constructor)
  [[nodiscard]] constexpr operator BarelyEngine*() const noexcept { return engine_; }

  /// Creates a new instrument.
  ///
  /// @return Instrument.
  Instrument CreateInstrument() noexcept {
    uint32_t instrument_id = 0;
    [[maybe_unused]] const bool success = BarelyEngine_CreateInstrument(engine_, &instrument_id);
    assert(success);
    return {engine_, instrument_id};
  }

  /// Creates a new performer.
  ///
  /// @return Performer.
  Performer CreatePerformer() noexcept {
    uint32_t performer_id = 0;
    [[maybe_unused]] const bool success = BarelyEngine_CreatePerformer(engine_, &performer_id);
    assert(success);
    return {task_callbacks_.get(), first_task_callbacks_.get(), engine_, performer_id};
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
    static_assert(std::is_arithmetic_v<NumberType>, "NumberType is not supported");
    return min + static_cast<NumberType>(GenerateRandomNumber() * static_cast<double>(max - min));
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

  /// Resets the random number generator seed.
  void ResetSeed(int seed) noexcept {
    [[maybe_unused]] const bool success =
        BarelyEngine_ResetSeed(engine_, static_cast<int32_t>(seed));
    assert(success);
  }

  /// Sets a control value.
  ///
  /// @param type Engine control type.
  /// @param value Engine control value.
  template <typename ValueType>
  void SetControl(EngineControlType type, ValueType value) noexcept {
    static_assert(std::is_arithmetic_v<ValueType> || std::is_enum_v<ValueType>,
                  "ValueType is not supported");
    [[maybe_unused]] const bool success = BarelyEngine_SetControl(
        engine_, static_cast<BarelyEngineControlType>(type), static_cast<float>(value));
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
  // Heap allocated fixed size buffers below (for pointer stability on move).
  std::unique_ptr<Task::Pool<Task::CallbackNode>> task_callbacks_;
  std::unique_ptr<Task::Pool<Task::CallbackNode*>> first_task_callbacks_;
  std::vector<std::byte> allocation_;
  BarelyEngine* engine_ = nullptr;
};

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_BARELYMUSICIAN_H_
