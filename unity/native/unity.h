#ifndef UNITY_NATIVE_UNITY_H_
#define UNITY_NATIVE_UNITY_H_

#if defined(_WIN32) || defined(__CYGWIN__)
#ifdef BARELYMUSICIAN_EXPORTS
#define BARELY_EXPORT __declspec(dllexport)
#else  // BARELYMUSICIAN_EXPORTS
#define BARELY_EXPORT __declspec(dllimport)
#endif  // BARELYMUSICIAN_EXPORTS
#else   // defined(_WIN32) || defined(__CYGWIN__)
#define BARELY_EXPORT
#endif  // defined(_WIN32) || defined(__CYGWIN__)

namespace barelyapi::unity {

extern "C" {

/// Event callback signatures.
using BeatCallback = void(double timestamp, int beat);
using NoteOffCallback = void(double timestamp, int id, float pitch);
using NoteOnCallback = void(double timestamp, int id, float pitch,
                            float intensity);

/// Instrument function signatures.
using ProcessFn = void(float* output, int size, int num_channels);
using SetNoteOffFn = void(float pitch);
using SetNoteOnFn = void(float pitch, float intensity);
using SetParamFn = void(int id, float value);

/// Debug callback signature.
using DebugCallback = void(int severity, const char* message);

/// Initializes the system.
///
/// @param sample_rate System sampling rate.
BARELY_EXPORT void Initialize(int sample_rate);

/// Shuts down the system.
BARELY_EXPORT void Shutdown();

/// Creates new Unity instrument.
///
/// @param process_fn_ptr Process function.
/// @param set_note_off_fn_ptr Set note off function.
/// @param set_note_on_fn_ptr Set note on function.
/// @param set_param_fn_ptr Set parameter function.
/// @return Instrument id.
BARELY_EXPORT int CreateUnityInstrument(ProcessFn* process_fn_ptr,
                                        SetNoteOffFn* set_note_off_fn_ptr,
                                        SetNoteOnFn* set_note_on_fn_ptr,
                                        SetParamFn* set_param_fn);

/// Creates new synth instrument.
///
/// @return Instrument id.
BARELY_EXPORT int CreateSynthInstrument();

/// Destroys instrument.
///
/// @param id Performer id.
BARELY_EXPORT void Destroy(int id);

/// Returns instrument parameter value.
///
/// @param id Instrument id.
/// @param param_id Param id.
/// @return Param value.
BARELY_EXPORT float GetParam(int id, int param_id);

/// Returns playback position.
///
/// @return Position in beats.
BARELY_EXPORT double GetPosition();

/// Returns playback tempo.
///
/// @return Tempo in BPM.
BARELY_EXPORT double GetTempo();

/// Returns whether the note is active or not.
///
/// @param id Instrument id.
/// @param pitch Note pitch.
/// @return True if active.
BARELY_EXPORT bool IsNoteOn(int id, float pitch);

/// Returns playback state.
///
/// @return True if playing.
BARELY_EXPORT bool IsPlaying();

/// Processes instrument.
///
/// @param id Instrument id.
/// @param output Output buffer.
/// @param num_channels Number of output channels.
/// @param num_frames Number of output frames.
BARELY_EXPORT void Process(int id, double timestamp, float* output,
                           int num_channels, int num_frames);

/// Resets all params.
BARELY_EXPORT void ResetAllParams(int id);

/// Schedules instrument note.
///
/// @param id Instrument id.
/// @param position Note position in beats.
/// @param duration Note duration in beats.
/// @param pitch Note pitch.
/// @param intensity Note intensity.
BARELY_EXPORT void ScheduleNote(int id, double position, double duration,
                                float pitch, float intensity);

/// Sets beat callback.
///
/// @param beat_callback_ptr Pointer to beat callback.
BARELY_EXPORT void SetBeatCallback(BeatCallback* beat_callback_ptr);

/// Sets debug callback.
///
/// @param debug_callback_ptr Pointer to debug callback.
BARELY_EXPORT void SetDebugCallback(DebugCallback* debug_callback_ptr);

/// Sets note off callback.
///
/// @param note_off_callback_ptr Pointer to note off callback.
BARELY_EXPORT void SetNoteOffCallback(NoteOffCallback* note_off_callback_ptr);

/// Sets note on callback.
///
/// @param note_on_callback_ptr Pointer to note on callback.
BARELY_EXPORT void SetNoteOnCallback(NoteOnCallback* note_on_callback_ptr);

/// Stops all notes.
BARELY_EXPORT void SetAllNotesOff(int id);

/// Stops instrument note.
///
/// @param id Instrument id.
/// @param pitch Note pitch.
BARELY_EXPORT void SetNoteOff(int id, float pitch);

/// Starts instrument note.
///
/// @param id Instrument id.
/// @param pitch Note pitch.
/// @param intensity Note intensity.
BARELY_EXPORT void SetNoteOn(int id, float pitch, float intensity);

/// Sets instrument param value.
///
/// @param id Instrument id.
/// @param param_id Param id.
/// @param value Param value.
BARELY_EXPORT void SetParam(int id, int param_id, float value);

/// Sets playback position.
///
/// @param position Position in beats.
BARELY_EXPORT void SetPosition(double position);

/// Sets playback tempo.
///
/// @param tempo Tempo (BPM).
BARELY_EXPORT void SetTempo(double tempo);

/// Starts playback.
BARELY_EXPORT void Start(double timestamp);

/// Pauses playback.
BARELY_EXPORT void Pause();

/// Stops playback.
BARELY_EXPORT void Stop();

/// Updates internal state.
BARELY_EXPORT void Update(double timestamp);

}  // extern "C"

}  // namespace barelyapi::unity

#endif  // UNITY_NATIVE_UNITY_H_
