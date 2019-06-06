#ifndef BARELYMUSICIAN_PLATFORM_UNITY_UNITY_H_
#define BARELYMUSICIAN_PLATFORM_UNITY_UNITY_H_

#ifdef BARELYMUSICIAN_UNITY_EXPORTS
#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API __declspec(dllimport)
#endif  // BARELYMUSICIAN_UNITY_EXPORTS

namespace barelyapi {
namespace unity {

// TODO(#49): Refactor below using barelymusician api.
extern "C" {

// Sequencer beat callback.
using BeatCallback = void(int, int, int, double);

// Instrument callback signatures.
using NoteOffCallback = void(float);
using NoteOnCallback = void(float, float);
using ProcessCallback = void(float*, int, int);
using ResetCallback = void();

// Initializes the system.
//
// @param sample_rate System sampling rate.
// @param num_channels System number of channels.
// @param num_frames System number of frames per buffer.
void EXPORT_API Initialize(int sample_rate, int num_channels, int num_frames);

// Shuts down the system.
void EXPORT_API Shutdown();

// Creates new sequencer.
//
// @param beat_callback_ptr Sequencer beat callback.
// @return Sequencer ID.
int EXPORT_API CreateSequencer(BeatCallback* beat_callback_ptr);

// Destroys sequencer.
//
// @param Sequencer ID.
void EXPORT_API DestroySequencer(int sequencer_id);

// Processes sequencer.
//
// @param sequencer_id Sequencer ID.
// @param dsp_time Process start DSP time in seconds.
void EXPORT_API ProcessSequencer(int sequencer_id, double dsp_time);

// Sets sequencer's number of bars per section.
//
// @param num_bars Sequencer number of bars per section.
void EXPORT_API SetSequencerNumBars(int sequencer_id, int num_bars);

// Sets sequencer's number of beats per bar.
//
// @param num_beats Sequencer number of beats per bar.
void EXPORT_API SetSequencerNumBeats(int sequencer_id, int num_beats);

// Sets sequencer's position.
//
// @param section Section.
// @param bar Bar.
// @param beat Beat.
void EXPORT_API SetSequencerPosition(int sequencer_id, int section, int bar,
                                     int beat);

// Sets sequencer's tempo.
//
// @param tempo Sequencer tempo.
void EXPORT_API SetSequencerTempo(int sequencer_id, float tempo);

// Creates new instrument.
//
// @param note_off_callback_ptr Note off callback.
// @param note_on_callback_ptr Note on callback.
// @param process_callback_ptr Process callback.
// @param reset_callback_ptr Reset callback.
// @return Instrument ID.
int EXPORT_API CreateInstrument(NoteOffCallback* note_off_callback_ptr,
                                NoteOnCallback* note_on_callback_ptr,
                                ProcessCallback* process_callback_ptr,
                                ResetCallback* reset_callback_ptr);

// Destroys instrument.
//
// @param instrument_id Instrument ID.
void EXPORT_API DestroyInstrument(int instrument_id);

// Stops playing instrument's note.
//
// @param instrument_id Instrument ID.
// @param index Note index.
void EXPORT_API NoteOffInstrument(int instrument_id, float index);

// Starts playing instrument's note.
//
// @param instrument_id Instrument ID.
// @param index Note index.
// @param intensity Note intensity.
void EXPORT_API NoteOnInstrument(int instrument_id, float index,
                                 float intensity);

// Processes instrument.
//
// @param instrument_id Instrument ID.
// @param output Output buffer.
void EXPORT_API ProcessInstrument(int instrument_id, float* output);

// Resets instrument.
//
// @param instrument_id Instrument ID.
void EXPORT_API ResetInstrument(int instrument_id);

}  // extern "C"

}  // namespace unity
}  // namespace barelyapi

#endif  // BARELYMUSICIAN_PLATFORM_UNITY_UNITY_H_
