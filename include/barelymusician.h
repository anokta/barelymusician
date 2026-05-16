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
///   BarelyEngine* engine = NULL;
///   BarelyEngine_Create(&config, &engine);
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
      .max_frame_count = 2048,                    \
      .max_event_count = 8192,                    \
      .max_instrument_count = 128,                \
      .max_note_count = 1024,                     \
      .max_slice_count = 512,                     \
      .max_voice_count = 256,                     \
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

/// Engine handle.
typedef struct BarelyEngine BarelyEngine;

/// Engine configuration.
typedef struct BarelyEngineConfig {
  /// Sampling rate in hertz.
  int32_t sample_rate;

  /// Maximum number of frames to process per call.
  int32_t max_frame_count;

  /// Maximum number of scheduled events.
  int32_t max_event_count;

  /// Maximum number of instruments.
  int32_t max_instrument_count;

  /// Maximum number of active notes.
  int32_t max_note_count;

  /// Maximum number of active slices.
  int32_t max_slice_count;

  /// Maximum number of active voices.
  int32_t max_voice_count;
} BarelyEngineConfig;

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
  /// Array of mono samples.
  const float* samples;

  /// Number of mono samples.
  int32_t sample_count;

  /// Sampling rate in hertz.
  int32_t sample_rate;

  /// Root note pitch.
  float root_pitch;
} BarelySlice;

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Note callback.
///
/// @param pitch Note pitch.
/// @param user_data Pointer to user data.
typedef void (*BarelyNoteCallback)(float pitch, void* user_data);

/// Trigger callback.
///
/// @param user_data Pointer to user data.
typedef void (*BarelyTriggerCallback)(void* user_data);

/// Cancels all scheduled events of an engine.
///
/// @param engine Pointer to engine.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_CancelAllEvents(BarelyEngine* engine);

/// Cancels a scheduled event of an engine.
///
/// @param engine Pointer to engine.
/// @param event_id Event identifier.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_CancelEvent(BarelyEngine* engine, uint32_t event_id);

/// Creates a new engine.
///
/// @param config Pointer to engine configuration.
/// @param out_engine Output pointer to engine.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_Create(const BarelyEngineConfig* config, BarelyEngine** out_engine);

/// Creates a new instrument.
///
/// @param engine Pointer to engine.
/// @param out_instrument_id Output instrument identifier.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_CreateInstrument(BarelyEngine* engine, uint32_t* out_instrument_id);

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

/// Resets the random number generator seed of an engine.
///
/// @param engine Pointer to engine.
/// @param seed Seed value.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_ResetSeed(BarelyEngine* engine, int32_t seed);

/// Schedules an engine control event.
///
/// @param engine Pointer to engine.
/// @param type Engine control type.
/// @param value Control value.
/// @param ramp_duration Ramp duration in beats.
/// @param offset Time offset in beats.
/// @param out_event_id Output event identifier.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_ScheduleControl(BarelyEngine* engine, BarelyEngineControlType type,
                                             float value, double ramp_duration, double offset,
                                             uint32_t* out_event_id);

/// Schedules an engine trigger event.
///
/// @param engine Pointer to engine.
/// @param callback Trigger callback.
/// @param user_data Pointer to user data.
/// @param interval Repeat interval in beats.
/// @param offset Time offset in beats.
/// @param out_event_id Output event identifier.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyEngine_ScheduleTrigger(BarelyEngine* engine, BarelyTriggerCallback callback,
                                             void* user_data, double interval, double offset,
                                             uint32_t* out_event_id);

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

/// Schedules an instrument control event.
///
/// @param engine Pointer to engine.
/// @param instrument_id Instrument identifier.
/// @param type Instrument control type.
/// @param value Control value.
/// @param ramp_duration Ramp duration in beats.
/// @param offset Time offset in beats.
/// @param out_event_id Output event identifier.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_ScheduleControl(BarelyEngine* engine, uint32_t instrument_id,
                                                 BarelyInstrumentControlType type, float value,
                                                 double ramp_duration, double offset,
                                                 uint32_t* out_event_id);

/// Schedules a note event for an instrument.
///
/// @param engine Pointer to engine.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @param duration Note duration in beats.
/// @param offset Time offset in beats.
/// @param out_event_id Output event identifier.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_ScheduleNote(BarelyEngine* engine, uint32_t instrument_id,
                                              float pitch, double duration, double offset,
                                              uint32_t* out_event_id);

/// Schedules a note control event for an instrument.
///
/// @param engine Pointer to engine.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @param type Note control type.
/// @param value Note control value.
/// @param ramp_duration Ramp duration in beats.
/// @param offset Time offset in beats.
/// @param out_event_id Output event identifier.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_ScheduleNoteControl(BarelyEngine* engine, uint32_t instrument_id,
                                                     float pitch, BarelyNoteControlType type,
                                                     float value, double ramp_duration,
                                                     double offset, uint32_t* out_event_id);

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

/// Sets an instrument note off.
///
/// @param engine Pointer to engine.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_SetNoteOff(BarelyEngine* engine, uint32_t instrument_id,
                                            float pitch);

/// Sets the note off callback of an instrument.
///
/// @param engine Pointer to engine.
/// @param instrument_id Instrument identifier.
/// @param user_data Pointer to user data.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_SetNoteOffCallback(BarelyEngine* engine, uint32_t instrument_id,
                                                    BarelyNoteCallback callback, void* user_data);

/// Sets an instrument note on.
///
/// @param engine Pointer to engine.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_SetNoteOn(BarelyEngine* engine, uint32_t instrument_id,
                                           float pitch);

/// Sets the note on callback of an instrument.
///
/// @param engine Pointer to engine.
/// @param instrument_id Instrument identifier.
/// @param user_data Pointer to user data.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_SetNoteOnCallback(BarelyEngine* engine, uint32_t instrument_id,
                                                   BarelyNoteCallback callback, void* user_data);

/// Sets instrument sample data.
///
/// @param engine Pointer to engine.
/// @param instrument_id Instrument identifier.
/// @param slices Array of slices.
/// @param slice_count Number of slices.
/// @return True if successful, false otherwise.
BARELY_API bool BarelyInstrument_SetSampleData(BarelyEngine* engine, uint32_t instrument_id,
                                               const BarelySlice* slices, int32_t slice_count);

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
#include <tuple>
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

/// Event callback function.
using TriggerCallback = std::function<void()>;

/// Note callback function.
///
/// @param pitch Note pitch.
using NoteCallback = std::function<void(float pitch)>;

/// Class that wraps an event.
class Event {
 public:
  /// Default constructor.
  Event() noexcept = default;

  /// Constructs a new `Event`.
  ///
  /// @param event_id Event identifier.
  /// @param trigger_callback Pointer to trigger callback.
  Event(uint32_t event_id, void* trigger_callback) noexcept
      : event_id_(event_id), trigger_callback_(trigger_callback) {}

  /// Returns the identifier.
  ///
  /// @return Identifier.
  // NOLINTNEXTLINE(google-explicit-constructor)
  [[nodiscard]] constexpr operator uint32_t() const noexcept { return event_id_; }

 private:
  friend class Engine;

  // Event identifier.
  uint32_t event_id_ = 0;

  // Pointer to trigger callback.
  void* trigger_callback_ = nullptr;
};

/// Class that wraps an instrument.
class Instrument {
 public:
  /// Default constructor.
  Instrument() noexcept = default;

  /// Constructs a new `Instrument`.
  ///
  /// @param engine Pointer to raw engine.
  /// @param instrument_id Instrument identifier.
  /// @param note_callback Pointer to note callback.
  Instrument(BarelyEngine* engine, uint32_t instrument_id,
             std::pair<NoteCallback, NoteCallback>* note_callback) noexcept
      : engine_(engine), instrument_id_(instrument_id), note_callback_(note_callback) {}

  /// Returns the identifier.
  ///
  /// @return Identifier.
  // NOLINTNEXTLINE(google-explicit-constructor)
  [[nodiscard]] constexpr operator uint32_t() const noexcept { return instrument_id_; }

  /// Returns a control value.
  ///
  /// @param type Instrument control type.
  /// @return Instrument control value.
  template <typename ValueType>
  [[nodiscard]] ValueType GetControl(InstrumentControlType type) const noexcept {
    static_assert(std::is_arithmetic_v<ValueType> || std::is_enum_v<ValueType>,
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
    static_assert(std::is_arithmetic_v<ValueType> || std::is_enum_v<ValueType>,
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

  /// Schedules a control event.
  ///
  /// @param type Instrument control type.
  /// @param value Instrument control value.
  /// @param ramp_duration Ramp duration in beats.
  /// @param offset Time offset in beats.
  /// @return Event.
  Event ScheduleControl(InstrumentControlType type, float value, double ramp_duration,
                        double offset) noexcept {
    uint32_t event_id = 0;
    [[maybe_unused]] const bool success = BarelyInstrument_ScheduleControl(
        engine_, instrument_id_, static_cast<BarelyInstrumentControlType>(type), value,
        ramp_duration, offset, &event_id);
    assert(success);
    return {event_id, nullptr};
  }

  /// Schedules a note event.
  ///
  /// @param pitch Note pitch.
  /// @param duration Note duration in beats.
  /// @param offset Time offset in beats.
  /// @return Event.
  Event ScheduleNote(float pitch, double duration, double offset = 0.0) noexcept {
    uint32_t event_id = 0;
    [[maybe_unused]] const bool success =
        BarelyInstrument_ScheduleNote(engine_, instrument_id_, pitch, duration, offset, &event_id);
    assert(success);
    return {event_id, nullptr};
  }

  /// Schedules a control event.
  ///
  /// @param pitch Note pitch.
  /// @param type Note control type.
  /// @param value Note control value.
  /// @param ramp_duration Ramp duration in beats.
  /// @param offset Time offset in beats.
  /// @return Event.
  Event ScheduleNoteControl(float pitch, NoteControlType type, float value,
                            double ramp_duration = 0.0, double offset = 0.0) noexcept {
    uint32_t event_id = 0;
    [[maybe_unused]] const bool success = BarelyInstrument_ScheduleNoteControl(
        engine_, instrument_id_, pitch, static_cast<BarelyNoteControlType>(type), value,
        ramp_duration, offset, &event_id);
    assert(success);
    return {event_id, nullptr};
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

  /// Sets the note off callback.
  ///
  /// @param callback Note callback.
  void SetNoteOffCallback(NoteCallback callback) noexcept {
    assert(note_callback_ != nullptr);
    note_callback_->first = std::move(callback);
    [[maybe_unused]] const bool success =
        (note_callback_->first)
            ? BarelyInstrument_SetNoteOffCallback(
                  engine_, instrument_id_,
                  [](float pitch, void* user_data) noexcept {
                    assert(user_data != nullptr && "Invalid note off callback user data");
                    if (const auto& callback = *static_cast<NoteCallback*>(user_data); callback) {
                      callback(pitch);
                    }
                  },
                  &note_callback_->first)
            : BarelyInstrument_SetNoteOffCallback(engine_, instrument_id_, nullptr, nullptr);
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

  /// Sets the note on callback.
  ///
  /// @param callback Note callback.
  void SetNoteOnCallback(NoteCallback callback) noexcept {
    assert(note_callback_ != nullptr);
    note_callback_->second = std::move(callback);
    [[maybe_unused]] const bool success =
        (note_callback_->second)
            ? BarelyInstrument_SetNoteOnCallback(
                  engine_, instrument_id_,
                  [](float pitch, void* user_data) noexcept {
                    assert(user_data != nullptr && "Invalid note on callback user data");
                    if (const auto& callback = *static_cast<NoteCallback*>(user_data); callback) {
                      callback(pitch);
                    }
                  },
                  &note_callback_->second)
            : BarelyInstrument_SetNoteOnCallback(engine_, instrument_id_, nullptr, nullptr);
    assert(success);
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

  // Pointer to raw engine.
  BarelyEngine* engine_ = nullptr;

  // Instrument identifier.
  uint32_t instrument_id_ = 0;

  // Pointer to note callback.
  std::pair<NoteCallback, NoteCallback>* note_callback_ = nullptr;
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
      : note_callbacks_(std::make_unique<CallbackPool<std::pair<NoteCallback, NoteCallback>>>(
            config.max_instrument_count)),
        trigger_callbacks_(
            std::make_unique<CallbackPool<TriggerCallbackData>>(config.max_event_count)) {
    [[maybe_unused]] const bool success = BarelyEngine_Create(&config, &engine_);
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
      : engine_(std::exchange(other.engine_, nullptr)),
        note_callbacks_(std::exchange(other.note_callbacks_, {})),
        trigger_callbacks_(std::exchange(other.trigger_callbacks_, {})) {}

  /// Assigns `Engine` via move.
  ///
  /// @param other Other engine.
  /// @return Engine.
  Engine& operator=(Engine&& other) noexcept {
    if (this != &other) {
      BarelyEngine_Destroy(engine_);
      engine_ = std::exchange(other.engine_, nullptr);
      note_callbacks_ = std::exchange(other.note_callbacks_, {});
      trigger_callbacks_ = std::exchange(other.trigger_callbacks_, {});
    }
    return *this;
  }

  /// Returns the pointer to raw engine.
  ///
  /// @return Pointer to raw engine.
  // NOLINTNEXTLINE(google-explicit-constructor)
  [[nodiscard]] constexpr operator BarelyEngine*() const noexcept { return engine_; }

  /// Cancels all scheduled events.
  void CancelAllEvents() {
    [[maybe_unused]] const bool success = BarelyEngine_CancelAllEvents(engine_);
    trigger_callbacks_->free_callbacks.resize(trigger_callbacks_->callbacks.size());
    for (int32_t i = 0; i < trigger_callbacks_->callbacks.size(); ++i) {
      trigger_callbacks_->free_callbacks[i] = &trigger_callbacks_->callbacks[i];
    }
    assert(success);
  }

  /// Cancels a scheduled event.
  ///
  /// @param event Event.
  void CancelEvent(Event event) {
    if (BarelyEngine_CancelEvent(engine_, event) && event.trigger_callback_ != nullptr) {
      auto* trigger_callback = static_cast<TriggerCallbackData*>(event.trigger_callback_);
      assert(trigger_callback->pool->free_callbacks.size() <
             trigger_callback->pool->callbacks.size());
      trigger_callbacks_->free_callbacks.push_back(trigger_callback);
    }
  }

  /// Creates a new instrument.
  ///
  /// @return Instrument.
  Instrument CreateInstrument() noexcept {
    uint32_t instrument_id = 0;
    [[maybe_unused]] bool success = BarelyEngine_CreateInstrument(engine_, &instrument_id);
    assert(success);
    assert(!note_callbacks_->free_callbacks.empty());
    auto* note_callbacks = note_callbacks_->free_callbacks.back();
    note_callbacks_->free_callbacks.pop_back();
    *note_callbacks = {};
    return {engine_, instrument_id, note_callbacks};
  }

  /// Destroys an instrument.
  ///
  /// @param instrument Instrument.
  void DestroyInstrument(Instrument instrument) {
    const bool success = BarelyEngine_DestroyInstrument(engine_, instrument);
    if (success) {
      assert(note_callbacks_->free_callbacks.size() < note_callbacks_->callbacks.size());
      note_callbacks_->free_callbacks.push_back(instrument.note_callback_);
    }
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
    static_assert(std::is_arithmetic_v<NumberType>, "NumberType is not supported");
    return min + static_cast<NumberType>(GenerateRandomNumber() * static_cast<double>(max - min));
  }

  /// Returns a control value.
  ///
  /// @param type Engine control type.
  /// @return Engine control value.
  template <typename ValueType>
  [[nodiscard]] ValueType GetControl(EngineControlType type) const noexcept {
    static_assert(std::is_arithmetic_v<ValueType> || std::is_enum_v<ValueType>,
                  "ValueType is not supported");
    float value = 0.0f;
    [[maybe_unused]] const bool success =
        BarelyEngine_GetControl(engine_, static_cast<BarelyEngineControlType>(type), &value);
    assert(success);
    return static_cast<ValueType>(value);
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

  /// Resets the random number generator seed.
  void ResetSeed(int seed) noexcept {
    [[maybe_unused]] const bool success =
        BarelyEngine_ResetSeed(engine_, static_cast<int32_t>(seed));
    assert(success);
  }

  /// Schedules a control event.
  ///
  /// @param type Engine control type.
  /// @param value Engine control value.
  /// @param ramp_duration Ramp duration in beats.
  /// @param offset Time offset in beats.
  /// @return Event.
  Event ScheduleControl(EngineControlType type, float value, double ramp_duration = 0.0,
                        double offset = 0.0) noexcept {
    uint32_t event_id = 0;
    [[maybe_unused]] const bool success =
        BarelyEngine_ScheduleControl(engine_, static_cast<BarelyEngineControlType>(type), value,
                                     ramp_duration, offset, &event_id);
    assert(success);
    return {event_id, nullptr};
  }

  /// Schedules a trigger event.
  ///
  /// @param callback Trigger callback.
  /// @param offset Time offset in beats.
  /// @param interval Repeat interval in beats.
  /// @return Event.
  Event ScheduleTrigger(TriggerCallback callback, double interval = 0.0,
                        double offset = 0.0) noexcept {
    uint32_t event_id = 0;
    assert(!trigger_callbacks_->free_callbacks.empty());
    auto* trigger_callback = trigger_callbacks_->free_callbacks.back();
    trigger_callbacks_->free_callbacks.pop_back();
    *trigger_callback = {std::move(callback), trigger_callbacks_.get(), interval > 0.0};
    [[maybe_unused]] const bool success = BarelyEngine_ScheduleTrigger(
        engine_,
        [](void* user_data) noexcept {
          assert(user_data != nullptr && "Invalid trigger callback user data");
          auto* trigger_callback = static_cast<TriggerCallbackData*>(user_data);
          const uint32_t event_id = trigger_callback->event_id;
          if (const auto& callback = trigger_callback->callback; callback) {
            callback();
          }
          if (trigger_callback->event_id == event_id && !trigger_callback->repeat) {
            assert(trigger_callback->pool->free_callbacks.size() <
                   trigger_callback->pool->callbacks.size());
            trigger_callback->pool->free_callbacks.push_back(trigger_callback);
          }
        },
        trigger_callback, interval, offset, &event_id);
    assert(success);
    return {event_id, trigger_callback};
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
  // Heap allocated callback pools for pointer stability on move.
  template <typename T>
  struct CallbackPool {
    explicit CallbackPool(int32_t capacity) noexcept
        : callbacks(capacity), free_callbacks(capacity) {
      for (int32_t i = 0; i < capacity; ++i) {
        free_callbacks[i] = &callbacks[i];
      }
    }
    std::vector<T> callbacks;
    std::vector<T*> free_callbacks;
  };
  struct TriggerCallbackData {
    TriggerCallback callback;
    CallbackPool<TriggerCallbackData>* pool = nullptr;
    uint32_t event_id = 0;
    bool repeat = false;
  };
  std::unique_ptr<CallbackPool<std::pair<NoteCallback, NoteCallback>>> note_callbacks_ = nullptr;
  std::unique_ptr<CallbackPool<TriggerCallbackData>> trigger_callbacks_ = nullptr;

  // Pointer to raw engine.
  BarelyEngine* engine_ = nullptr;
};

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_BARELYMUSICIAN_H_
