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
using BeatCallback = void(int beat);
using NoteOffCallback = void(int id, float pitch);
using NoteOnCallback = void(int id, float pitch, float intensity);

/// Instrument function signatures.
using ProcessFn = void(float* output, int size, int num_channels);
using SetNoteOffFn = void(float pitch);
using SetNoteOnFn = void(float pitch, float intensity);
using SetParamFn = void(int id, float value);

/// Debug callback signature.
using DebugCallback = void(int severity, const char* message);

/// System handle.
struct BarelyMusician;

/// Initializes the system.
///
/// @param sample_rate System sampling rate.
BARELY_EXPORT BarelyMusician* Initialize(int sample_rate);

/// Shuts down the system.
BARELY_EXPORT void Shutdown(BarelyMusician* barelymusician);

/// Creates new Unity instrument.
///
/// @param process_fn_ptr Process function.
/// @param set_note_off_fn_ptr Set note off function.
/// @param set_note_on_fn_ptr Set note on function.
/// @param set_param_fn_ptr Set parameter function.
/// @return Instrument id.
BARELY_EXPORT int CreateUnityInstrument(BarelyMusician* barelymusician,
                                        ProcessFn* process_fn_ptr,
                                        SetNoteOffFn* set_note_off_fn_ptr,
                                        SetNoteOnFn* set_note_on_fn_ptr,
                                        SetParamFn* set_param_fn);

/// Creates new synth instrument.
///
/// @return Instrument id.
BARELY_EXPORT int CreateSynthInstrument(BarelyMusician* barelymusician);

/// Destroys instrument.
///
/// @param id Performer id.
BARELY_EXPORT void Destroy(BarelyMusician* barelymusician, int id);

/// Returns instrument parameter value.
///
/// @param id Instrument id.
/// @param param_id Param id.
/// @return Param value.
BARELY_EXPORT float GetParam(BarelyMusician* barelymusician, int id,
                             int param_id);

/// Returns playback position.
///
/// @return Position in beats.
BARELY_EXPORT double GetPosition(BarelyMusician* barelymusician);

/// Returns playback tempo.
///
/// @return Tempo in BPM.
BARELY_EXPORT double GetTempo(BarelyMusician* barelymusician);

/// Returns whether the note is active or not.
///
/// @param id Instrument id.
/// @param pitch Note pitch.
/// @return True if active.
BARELY_EXPORT bool IsNoteOn(BarelyMusician* barelymusician, int id,
                            float pitch);

/// Returns playback state.
///
/// @return True if playing.
BARELY_EXPORT bool IsPlaying(BarelyMusician* barelymusician);

/// Processes instrument.
///
/// @param id Instrument id.
/// @param output Output buffer.
/// @param num_channels Number of output channels.
/// @param num_frames Number of output frames.
BARELY_EXPORT void Process(BarelyMusician* barelymusician, int id,
                           double timestamp, float* output, int num_channels,
                           int num_frames);

/// Resets all params.
BARELY_EXPORT void ResetAllParams(BarelyMusician* barelymusician, int id);

/// Schedules instrument note.
///
/// @param id Instrument id.
/// @param position Note position in beats.
/// @param duration Note duration in beats.
/// @param pitch Note pitch.
/// @param intensity Note intensity.
BARELY_EXPORT void ScheduleNote(BarelyMusician* barelymusician, int id,
                                double position, double duration, float pitch,
                                float intensity);

/// Sets beat callback.
///
/// @param beat_callback_ptr Pointer to beat callback.
BARELY_EXPORT void SetBeatCallback(BarelyMusician* barelymusician,
                                   BeatCallback* beat_callback_ptr);

/// Sets debug callback.
///
/// @param debug_callback_ptr Pointer to debug callback.
BARELY_EXPORT void SetDebugCallback(BarelyMusician* barelymusician,
                                    DebugCallback* debug_callback_ptr);

/// Sets note off callback.
///
/// @param note_off_callback_ptr Pointer to note off callback.
BARELY_EXPORT void SetNoteOffCallback(BarelyMusician* barelymusician,
                                      NoteOffCallback* note_off_callback_ptr);

/// Sets note on callback.
///
/// @param note_on_callback_ptr Pointer to note on callback.
BARELY_EXPORT void SetNoteOnCallback(BarelyMusician* barelymusician,
                                     NoteOnCallback* note_on_callback_ptr);

/// Stops all notes.
BARELY_EXPORT void SetAllNotesOff(BarelyMusician* barelymusician, int id);

/// Stops instrument note.
///
/// @param id Instrument id.
/// @param pitch Note pitch.
BARELY_EXPORT void SetNoteOff(BarelyMusician* barelymusician, int id,
                              float pitch);

/// Starts instrument note.
///
/// @param id Instrument id.
/// @param pitch Note pitch.
/// @param intensity Note intensity.
BARELY_EXPORT void SetNoteOn(BarelyMusician* barelymusician, int id,
                             float pitch, float intensity);

/// Sets instrument param value.
///
/// @param id Instrument id.
/// @param param_id Param id.
/// @param value Param value.
BARELY_EXPORT void SetParam(BarelyMusician* barelymusician, int id,
                            int param_id, float value);

/// Sets playback position.
///
/// @param position Position in beats.
BARELY_EXPORT void SetPosition(BarelyMusician* barelymusician, double position);

/// Sets playback tempo.
///
/// @param tempo Tempo (BPM).
BARELY_EXPORT void SetTempo(BarelyMusician* barelymusician, double tempo);

/// Starts playback.
BARELY_EXPORT void Start(BarelyMusician* barelymusician);

/// Pauses playback.
BARELY_EXPORT void Pause(BarelyMusician* barelymusician);

/// Stops playback.
BARELY_EXPORT void Stop(BarelyMusician* barelymusician);

/// Updates internal state.
BARELY_EXPORT void Update(BarelyMusician* barelymusician, double timestamp);

}  // extern "C"

}  // namespace barelyapi::unity

#endif  // UNITY_NATIVE_UNITY_H_
