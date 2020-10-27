#ifndef BARELYMUSICIAN_PLATFORM_UNITY_UNITY_H_
#define BARELYMUSICIAN_PLATFORM_UNITY_UNITY_H_

#ifdef BARELYMUSICIAN_UNITY_EXPORTS
#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API __declspec(dllimport)
#endif  // BARELYMUSICIAN_UNITY_EXPORTS

#include "barelymusician/base/constants.h"

namespace barelyapi {
namespace unity {

extern "C" {

// Event callback signatures.
using BeatCallback = void(double timestamp, int beat);
using NoteOffCallback = void(double timestamp, Id id, float index);
using NoteOnCallback = void(double timestamp, Id id, float index,
                            float intensity);

// Instrument function signatures.
using NoteOffFn = void(float index);
using NoteOnFn = void(float index, float intensity);
using ProcessFn = void(float* output, int size, int num_channels);

// Debug callback signature.
using DebugCallback = void(int severity, const char* message);

// Initializes the system.
//
// @param sample_rate System sampling rate.
void EXPORT_API Initialize(int sample_rate);

// Shuts down the system.
void EXPORT_API Shutdown();

// Creates new Unity instrument.
//
// @param note_off_fn_ptr Note off function.
// @param note_on_fn_ptr Note on function.
// @param process_fn_ptr Process function.
// @return Instrument id.
Id EXPORT_API CreateUnityInstrument(NoteOffFn* note_off_fn_ptr,
                                    NoteOnFn* note_on_fn_ptr,
                                    ProcessFn* process_fn_ptr);

// Creates new synth instrument.
//
// @return Instrument id.
Id EXPORT_API CreateBasicSynthInstrument();

// Destroys instrument.
//
// @param id Performer id.
void EXPORT_API Destroy(Id id);

// Returns instrument parameter value.
//
// @param id Instrument id.
// @param param_id Param id.
// @return Param value.
float EXPORT_API GetParam(Id id, int param_id);

// Returns playback position.
//
// @return Position in beats.
double EXPORT_API GetPosition();

// Returns playback tempo.
//
// @return Tempo in BPM.
double EXPORT_API GetTempo();

// Returns whether the note is active or not.
//
// @param id Instrument id.
// @param index Note index.
// @return True if active.
bool EXPORT_API IsNoteOn(Id id, float index);

// Returns playback state.
//
// @return True if playing.
bool EXPORT_API IsPlaying();

// Stops all notes.
void EXPORT_API AllNotesOff(Id id);

// Stops instrument note.
//
// @param id Instrument id.
// @param index Note index.
void EXPORT_API NoteOff(Id id, float index);

// Starts instrument note.
//
// @param id Instrument id.
// @param index Note index.
// @param intensity Note intensity.
void EXPORT_API NoteOn(Id id, float index, float intensity);

// Processes instrument.
//
// @param id Instrument id.
// @param output Output buffer.
// @param num_channels Number of output channels.
// @param num_frames Number of output frames.
void EXPORT_API Process(Id id, double timestamp, float* output,
                        int num_channels, int num_frames);

// Resets all params.
void EXPORT_API ResetAllParams(Id id);

// Schedules instrument note.
//
// @param id Instrument id.
// @param position Note position in beats.
// @param duration Note duration in beats.
// @param index Note index.
// @param intensity Note intensity.
void EXPORT_API ScheduleNote(Id id, double position, double duration,
                             float index, float intensity);

// Schedules instrument note off.
//
// @param id Instrument id.
// @param position Note position in beats.
// @param index Note index.
void EXPORT_API ScheduleNoteOff(Id id, double position, float index);

// Schedules instrument note on.
//
// @param id Instrument id.
// @param position Note position in beats.
// @param index Note index.
// @param intensity Note intensity.
void EXPORT_API ScheduleNoteOn(Id id, double position, float index,
                               float intensity);

// Sets beat callback.
//
// @param beat_callback_ptr Pointer to beat callback.
void EXPORT_API SetBeatCallback(BeatCallback* beat_callback_ptr);

// Sets debug callback.
//
// @param debug_callback_ptr Pointer to debug callback.
void EXPORT_API SetDebugCallback(DebugCallback* debug_callback_ptr);

// Sets note off callback.
//
// @param note_off_callback_ptr Pointer to note off callback.
void EXPORT_API SetNoteOffCallback(NoteOffCallback* note_off_callback_ptr);

// Sets note on callback.
//
// @param note_on_callback_ptr Pointer to note on callback.
void EXPORT_API SetNoteOnCallback(NoteOnCallback* note_on_callback_ptr);

// Sets instrument param value.
//
// @param id Instrument id.
// @param param_id Param id.
// @param value Param value.
void EXPORT_API SetParam(Id id, int param_id, float value);

// Sets playback position.
//
// @param position Position in beats.
void EXPORT_API SetPosition(double position);

// Sets playback tempo.
//
// @param tempo Tempo (BPM).
void EXPORT_API SetTempo(double tempo);

// Starts playback.
void EXPORT_API Start(double timestamp);

// Pauses playback.
void EXPORT_API Pause();

// Stops playback.
void EXPORT_API Stop();

// Updates internal state.
void EXPORT_API Update(double timestamp);

}  // extern "C"

}  // namespace unity
}  // namespace barelyapi

#endif  // BARELYMUSICIAN_PLATFORM_UNITY_UNITY_H_
