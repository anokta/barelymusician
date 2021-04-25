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

#include "barelymusician/common/id_generator.h"

namespace barelyapi::unity {

extern "C" {

/// Event callback signatures.
using NoteOffCallback = void(Id instrument_id, float note_pitch);
using NoteOnCallback = void(Id instrument_id, float note_pitch,
                            float note_intensity);

/// Debug callback signature.
using DebugCallback = void(int severity, const char* message);

/// System handle.
struct BarelyMusician;

/// Initializes the system.
///
/// @param sample_rate System sampling rate.
/// @param debug_callback_ptr Pointer to debug callback.
/// @return System handle.
BARELY_EXPORT BarelyMusician* BarelyInitialize(
    int sample_rate, DebugCallback* debug_callback_ptr);

/// Shuts down the system.
BARELY_EXPORT void BarelyShutdown(BarelyMusician* barelymusician);

/// Creates new synth instrument.
///
/// @param barelymusician System handle.
/// @return Instrument id.
BARELY_EXPORT Id BarelyCreateSynthInstrument(BarelyMusician* barelymusician);

/// Destroys instrument.
///
/// @param barelymusician System handle.
/// @param instrument_id Instrument id.
BARELY_EXPORT bool BarelyDestroyInstrument(BarelyMusician* barelymusician,
                                           Id instrument_id);

/// Returns instrument parameter value.
///
/// @param barelymusician System handle.
/// @param instrument_id Instrument id.
/// @param param_id Param id.
/// @return Param value.
BARELY_EXPORT float BarelyGetInstrumentParam(BarelyMusician* barelymusician,
                                             Id instrument_id, int param_id);

/// Returns whether the note is active or not.
///
/// @param barelymusician System handle.
/// @param instrument_id Instrument id.
/// @param note_pitch Note pitch.
/// @return True if active.
BARELY_EXPORT bool BarelyIsInstrumentNoteOn(BarelyMusician* barelymusician,
                                            Id instrument_id, float note_pitch);

/// Processes instrument.
///
/// @param barelymusician System handle.
/// @param instrument_id Instrument id.
/// @param timestamp Timestamp in seconds.
/// @param output Output buffer.
/// @param num_channels Number of output channels.
/// @param num_frames Number of output frames.
BARELY_EXPORT void BarelyProcessInstrument(BarelyMusician* barelymusician,
                                           Id instrument_id, double timestamp,
                                           float* output, int num_channels,
                                           int num_frames);

/// Resets all params.
///
/// @param barelymusician System handle.
/// @param instrument_id Instrument id.
/// @param timestamp Timestamp in seconds.
BARELY_EXPORT void BarelyResetAllInstrumentParams(
    BarelyMusician* barelymusician, Id instrument_id, double timestamp);

/// Stops all notes.
///
/// @param barelymusician System handle.
/// @param instrument_id Instrument id.
/// @param timestamp Timestamp in seconds.
BARELY_EXPORT void BarelySetAllInstrumentNotesOff(
    BarelyMusician* barelymusician, Id instrument_id, double timestamp);

/// Stops instrument note.
///
/// @param barelymusician System handle.
/// @param instrument_id Instrument id.
/// @param timestamp Timestamp in seconds.
/// @param note_pitch Note pitch.
BARELY_EXPORT void BarelySetInstrumentNoteOff(BarelyMusician* barelymusician,
                                              Id instrument_id,
                                              double timestamp,
                                              float note_pitch);

/// Sets note off callback.
///
/// @param barelymusician System handle.
/// @param note_off_callback_ptr Pointer to note off callback.
BARELY_EXPORT void BarelySetInstrumentNoteOffCallback(
    BarelyMusician* barelymusician, NoteOffCallback* note_off_callback_ptr);

/// Starts instrument note.
///
/// @param barelymusician System handle.
/// @param instrument_id Instrument id.
/// @param timestamp Timestamp in seconds.
/// @param note_pitch Note pitch.
/// @param note_intensity Note intensity.
BARELY_EXPORT void BarelySetInstrumentNoteOn(BarelyMusician* barelymusician,
                                             Id instrument_id, double timestamp,
                                             float note_pitch,
                                             float note_intensity);

/// Sets note on callback.
///
/// @param barelymusician System handle.
/// @param note_on_callback_ptr Pointer to note on callback.
BARELY_EXPORT void BarelySetInstrumentNoteOnCallback(
    BarelyMusician* barelymusician, NoteOnCallback* note_on_callback_ptr);

/// Sets instrument param value.
///
/// @param barelymusician System handle.
/// @param instrument_id Instrument id.
/// @param timestamp Timestamp in seconds.
/// @param param_id Param id.
/// @param param_value Param param_value.
BARELY_EXPORT void BarelySetInstrumentParam(BarelyMusician* barelymusician,
                                            Id instrument_id, double timestamp,
                                            int param_id, float param_value);

/// Updates the internal state.
///
/// @param barelymusician System handle.
/// @param timestamp Timestamp in seconds.
BARELY_EXPORT void BarelyUpdate(BarelyMusician* barelymusician,
                                double timestamp);

}  // extern "C"

}  // namespace barelyapi::unity

#endif  // UNITY_NATIVE_UNITY_H_
