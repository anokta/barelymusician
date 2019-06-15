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
using BeatCallback = void(int, int, int);
using NoteOffCallback = void(float);
using NoteOnCallback = void(float, float);

// Instrument function signatures.
using ClearFn = void();
using NoteOffFn = void(float);
using NoteOnFn = void(float, float);
using ProcessFn = void(float*, int, int);

// Initializes the system.
//
// @param sample_rate System sampling rate.
// @param num_channels System number of channels.
// @param num_frames System number of frames per buffer.
void EXPORT_API Initialize(int sample_rate, int num_channels, int num_frames);

// Shuts down the system.
void EXPORT_API Shutdown();

// Updates audio thread state.
void EXPORT_API UpdateAudioThread();

// Updates main thread state.
void EXPORT_API UpdateMainThread();

// Creates new sequencer.
//
// @param beat_callback_ptr Sequencer beat callback.
int EXPORT_API CreateSequencer(BeatCallback* beat_callback_ptr);

// Destroys sequencer.
//
// @param sequencer_id Sequencer ID.
void EXPORT_API DestroySequencer(int sequencer_id);

// Sets sequencer number of bars per section.
//
// @param sequencer_id Sequencer ID.
// @param num_bars Sequencer number of bars per section.
void EXPORT_API SetSequencerNumBars(int sequencer_id, int num_bars);

// Sets sequencer number of beats per bar.
//
// @param sequencer_id Sequencer ID.
// @param num_beats Sequencer number of beats per bar.
void EXPORT_API SetSequencerNumBeats(int sequencer_id, int num_beats);

// Sets sequencer position.
//
// @param sequencer_id Sequencer ID.
// @param section Section.
// @param bar Bar.
// @param beat Beat.
void EXPORT_API SetSequencerPosition(int sequencer_id, int section, int bar,
                                     int beat);

// Sets sequencer's tempo.
//
// @param sequencer_id Sequencer ID.
// @param tempo Sequencer tempo.
void EXPORT_API SetSequencerTempo(int sequencer_id, float tempo);

// Starts sequencer.
//
// @param sequencer_id Sequencer ID.
void EXPORT_API StartSequencer(int sequencer_id);

// Stops sequencer.
//
// @param sequencer_id Sequencer ID.
void EXPORT_API StopSequencer(int sequencer_id);

// Creates new instrument.
//
// @param clear_fn_ptr Clear function.
// @param note_off_fn_ptr Note off function.
// @param note_on_fn_ptr Note on function.
// @param process_fn_ptr Process function.
// @return Instrument ID.
int EXPORT_API CreateInstrument(ClearFn* clear_fn_ptr,
                                NoteOffFn* note_off_fn_ptr,
                                NoteOnFn* note_on_fn_ptr,
                                ProcessFn* process_fn_ptr);

// Destroys instrument.
//
// @param instrument_id Instrument ID.
void EXPORT_API DestroyInstrument(int instrument_id);

// Processes instrument.
//
// @param instrument_id Instrument ID.
// @param output Output buffer.
void EXPORT_API ProcessInstrument(int instrument_id, float* output);

// Resets instrument.
//
// @param instrument_id Instrument ID.
void EXPORT_API SetInstrumentClear(int instrument_id);

// Stops playing instrument's note.
//
// @param instrument_id Instrument ID.
// @param index Note index.
void EXPORT_API SetInstrumentNoteOff(int instrument_id, float index);

// Starts playing instrument's note.
//
// @param instrument_id Instrument ID.
// @param index Note index.
// @param intensity Note intensity.
void EXPORT_API SetInstrumentNoteOn(int instrument_id, float index,
                                    float intensity);

}  // extern "C"

}  // namespace unity
}  // namespace barelyapi

#endif  // BARELYMUSICIAN_PLATFORM_UNITY_UNITY_H_
