#ifndef BARELYMUSICIAN_PLATFORM_UNITY_UNITY_H_
#define BARELYMUSICIAN_PLATFORM_UNITY_UNITY_H_

#ifdef BARELYMUSICIAN_UNITY_EXPORTS
#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API __declspec(dllimport)
#endif  // BARELYMUSICIAN_UNITY_EXPORTS

namespace barelyapi {
namespace unity {

extern "C" {

// Event callback signatures.
using BeatCallback = void(int beat);
using NoteOffCallback = void(int id, float index);
using NoteOnCallback = void(int id, float index, float intensity);

// Instrument function signatures.
using NoteOffFn = void(float index);
using NoteOnFn = void(float index, float intensity);
using ProcessFn = void(float* output, int size, int num_channels);

// Initializes the system.
//
// @param sample_rate System sampling rate.
void EXPORT_API Initialize(int sample_rate);

// Shuts down the system.
void EXPORT_API Shutdown();

// Creates new instrument.
//
// @param note_off_fn_ptr Note off function.
// @param note_on_fn_ptr Note on function.
// @param process_fn_ptr Process function.
// @return Instrument id.
int EXPORT_API Create(NoteOffFn* note_off_fn_ptr, NoteOnFn* note_on_fn_ptr,
                      ProcessFn* process_fn_ptr);

// Destroys instrument.
//
// @param id Performer id.
void EXPORT_API Destroy(int id);

// Returns playback position.
//
// @return Position in beats.
double EXPORT_API GetPosition();

// Returns playback tempo.
//
// @return Tempo in BPM.
double EXPORT_API GetTempo();

// Returns playback state.
//
// @return True if playing.
bool EXPORT_API IsPlaying();

// Stops instrument note.
//
// @param id Instrument id.
// @param index Note index.
void EXPORT_API NoteOff(int id, float index);

// Starts instrument note.
//
// @param id Instrument id.
// @param index Note index.
// @param intensity Note intensity.
void EXPORT_API NoteOn(int id, float index, float intensity);

// Processes instrument.
//
// @param id Instrument id.
// @param output Output buffer.
// @param num_channels Number of output channels.
// @param num_frames Number of output frames.
void EXPORT_API Process(int id, float* output, int num_channels,
                        int num_frames);

// Schedules instrument note off.
//
// @param id Instrument id.
// @param position Position in beats.
// @param index Note index.
void EXPORT_API ScheduleNoteOff(int id, double position, float index);

// Schedules instrument note on.
//
// @param id Instrument id.
// @param position Position in beats.
// @param index Note index.
// @param intensity Note intensity.
void EXPORT_API ScheduleNoteOn(int id, double position, float index,
                               float intensity);

// Sets beat callback.
//
// @param beat_callback_ptr Pointer to beat callback.
void EXPORT_API SetBeatCallback(BeatCallback* beat_callback_ptr);

// Sets note off callback.
//
// @param note_off_callback_ptr Pointer to note off callback.
void EXPORT_API SetNoteOffCallback(NoteOffCallback* note_off_callback_ptr);

// Sets note on callback.
//
// @param note_on_callback_ptr Pointer to note on callback.
void EXPORT_API SetNoteOnCallback(NoteOnCallback* note_on_callback_ptr);

// Sets playback position.
//
// @param position Position in beats.
void EXPORT_API SetPosition(double position);

// Sets playback tempo.
//
// @param tempo Tempo (BPM).
void EXPORT_API SetTempo(double tempo);

// Starts playback.
void EXPORT_API Start();

// Stops playback.
void EXPORT_API Stop();

// Updates audio thread state.
//
// @param num_frames Number of frames to iterate.
void EXPORT_API UpdateAudioThread(int num_frames);

// Updates main thread state.
void EXPORT_API UpdateMainThread();

}  // extern "C"

}  // namespace unity
}  // namespace barelyapi

#endif  // BARELYMUSICIAN_PLATFORM_UNITY_UNITY_H_
