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

#include "barelymusician/common/id.h"

namespace barelyapi::unity {

extern "C" {

/// Debug callback signature.
using DebugCallback = void(int severity, const char* message);

/// Note off callback signature.
using NoteOffCallback = void(Id instrument_id, float note_pitch);

/// Note on callback signature.
using NoteOnCallback = void(Id instrument_id, float note_pitch,
                            float note_intensity);

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
///
/// @param barelymusician System handle.
BARELY_EXPORT void BarelyShutdown(BarelyMusician* barelymusician);

/// Creates new synth instrument.
///
/// @param barelymusician System handle.
/// @param timestamp Timestamp in seconds.
/// @return Instrument id.
BARELY_EXPORT Id BarelyCreateSynthInstrument(BarelyMusician* barelymusician,
                                             double timestamp);

/// Destroys instrument.
///
/// @param barelymusician System handle.
/// @param instrument_id Instrument id.
/// @param timestamp Timestamp in seconds.
BARELY_EXPORT bool BarelyDestroyInstrument(BarelyMusician* barelymusician,
                                           Id instrument_id, double timestamp);

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

/// Sets all instrument notes off.
///
/// @param barelymusician System handle.
/// @param instrument_id Instrument id.
/// @param timestamp Timestamp in seconds.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelySetAllInstrumentNotesOff(
    BarelyMusician* barelymusician, Id instrument_id, double timestamp);

/// Sets all instrument parameters to default value.
///
/// @param barelymusician System handle.
/// @param instrument_id Instrument id.
/// @param timestamp Timestamp in seconds.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelySetAllInstrumentParamsToDefault(
    BarelyMusician* barelymusician, Id instrument_id, double timestamp);

/// Sets instrument note off.
///
/// @param barelymusician System handle.
/// @param instrument_id Instrument id.
/// @param timestamp Timestamp in seconds.
/// @param note_pitch Note pitch.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelySetInstrumentNoteOff(BarelyMusician* barelymusician,
                                              Id instrument_id,
                                              double timestamp,
                                              float note_pitch);

/// Sets note off callback.
///
/// @param barelymusician System handle.
/// @param note_off_callback_ptr Pointer to note off callback.
BARELY_EXPORT void BarelySetInstrumentNoteOffCallback(
    BarelyMusician* barelymusician, NoteOffCallback* note_off_callback_ptr);

/// Sets instrument note off.
///
/// @param barelymusician System handle.
/// @param instrument_id Instrument id.
/// @param timestamp Timestamp in seconds.
/// @param note_pitch Note pitch.
/// @param note_intensity Note intensity.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelySetInstrumentNoteOn(BarelyMusician* barelymusician,
                                             Id instrument_id, double timestamp,
                                             float note_pitch,
                                             float note_intensity);

/// Sets note on callback.
///
/// @param barelymusician System handle.
/// @param note_on_callback_ptr Pointer to note on callback.
BARELY_EXPORT void BarelySetInstrumentNoteOnCallback(
    BarelyMusician* barelymusician, NoteOnCallback* note_on_callback_ptr);

/// Sets instrument parameter value.
///
/// @param barelymusician System handle.
/// @param instrument_id Instrument id.
/// @param timestamp Timestamp in seconds.
/// @param param_id Parameter id.
/// @param param_value Parameter value.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelySetInstrumentParam(BarelyMusician* barelymusician,
                                            Id instrument_id, double timestamp,
                                            int param_id, float param_value);

/// Sets instrument parameter to default value.
///
/// @param barelymusician System handle.
/// @param instrument_id Instrument id.
/// @param timestamp Timestamp in seconds.
/// @param param_id Parameter id.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelySetInstrumentParamToDefault(
    BarelyMusician* barelymusician, Id instrument_id, double timestamp,
    int param_id);

/// Sets sampling rate.
///
/// @param barelymusician System handle.
/// @param timestamp Timestamp in seconds.
/// @param sample_rate Sampling rate in Hz.
BARELY_EXPORT void BarelySetSampleRate(BarelyMusician* barelymusician,
                                       double timestamp, int sample_rate);

/// Updates the internal state.
///
/// @param barelymusician System handle.
/// @param timestamp Timestamp in seconds.
BARELY_EXPORT void BarelyUpdate(BarelyMusician* barelymusician,
                                double timestamp);

}  // extern "C"

}  // namespace barelyapi::unity

#endif  // UNITY_NATIVE_UNITY_H_
