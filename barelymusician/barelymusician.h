#ifndef BARELYMUSICIAN_BARELYMUSICIAN_H_
#define BARELYMUSICIAN_BARELYMUSICIAN_H_

#include <stdint.h>

#ifdef BARELYMUSICIAN_EXPORTS
#define BARELY_EXPORT __declspec(dllexport)
#else
#define BARELY_EXPORT __declspec(dllimport)
#endif  // BARELYMUSICIAN_EXPORTS

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Constants.
enum BarelyConstants {
  /// Invalid id.
  kBarelyInvalidId = 0,

  /// Maximum name string size in bytes.
  kBarelyMaxNameStringSize = 32,
};

/// Status codes.
enum BarelyStatusValues {
  /// Success.
  kBarelyStatus_Ok = 0,

  /// Unimplemented error.
  kBarelyStatus_Unimplemented = 1,

  /// Internal error.
  kBarelyStatus_Internal = 2,

  /// Invalid argument error.
  kBarelyStatus_InvalidArgument = 3,
};
typedef uint32_t BarelyStatus;

/// System handle type.
typedef struct BarelySystem* BarelyHandle;

/// Id type.
typedef uint64_t BarelyId;

/// Instrument id type.
typedef BarelyId BarelyInstrumentId;

/// Instrument control id type.
typedef BarelyId BarelyInstrumentControlId;

/// Performer id type.
typedef BarelyId BarelyPerformerId;

/// Position type.
typedef double BarelyPosition;

/// Timestamp type.
typedef double BarelyTimestamp;

/// Audio buffer.
typedef struct BarelyBuffer {
  /// Interleaved PCM data.
  float* data;

  /// Number of channels.
  int32_t num_channels;

  /// Number of frames.
  int32_t num_frames;
} BarelyBuffer;

/// Musical note.
typedef struct BarelyNote {
  /// Note pitch.
  float pitch;

  /// Note intensity.
  float intensity;

  /// Note position.
  BarelyPosition position;

  /// Note duration.
  BarelyPosition duration;
} BarelyNote;

/// Beat callback signature.
typedef void (*BarelyBeatCallback)(int32_t beat);

/// Note on callback signature.
typedef void (*BarelyNoteOnCallback)(BarelyInstrumentId instrument_id,
                                     float pitch, float intensity);

/// Note off callback signature.
typedef void (*BarelyNoteOffCallback)(BarelyInstrumentId instrument_id,
                                      float pitch);

// TODO(anokta): Callback or Fn or Handler or Func?

/// Conductor set energy callback signature.
typedef void (*BarelyConductorSetEnergyCallback)(float energy);

/// Conductor set stress callback signature.
typedef void (*BarelyConductorSetStressCallback)(float stress);

/// Conductor transform note callback signature.
typedef void (*BarelyConductorTransformNoteCallback)(BarelyNote* note);

/// Instrument control callback type.
typedef void (*BarelyInstrumentControlCallback)(BarelyInstrumentControlId id,
                                                float value);

/// Instrument note on callback type.
typedef void (*BarelyInstrumentNoteOnCallback)(float pitch, float intensity);

/// Instrument note off callback type.
typedef void (*BarelyInstrumentNoteOffCallback)(float pitch);

/// Instrument process callback type.
typedef void (*BarelyInstrumentProcessCallback)(float* output,
                                                int32_t num_channels,
                                                int32_t num_frames);

/// Conductor definition.
typedef struct BarelyConductorDefinition {
  /// Set energy callback function.
  BarelyConductorSetEnergyCallback set_energy_callback;

  /// Set stress callback function.
  BarelyConductorSetStressCallback set_stress_callback;

  /// Transform note callback function.
  BarelyConductorTransformNoteCallback transform_note_callback;
} BarelyConductorDefinition;

/// Instrument definition.
typedef struct BarelyInstrumentDefinition {
  /// Instrument name.
  char name[kBarelyMaxNameStringSize];

  // TODO(anokta): Create/destroy + control callbacks.

  /// Note on callback function.
  BarelyInstrumentProcessCallback note_on_callback;

  /// Note off callback function.
  BarelyInstrumentProcessCallback note_off_callback;

  /// Process callback function.
  BarelyInstrumentProcessCallback process_callback;
} BarelyInstrumentDefinition;

/// Creates new BarelyMusician system.
///
/// @return System handle.
BarelyHandle BarelyCreateSystem();

/// Destroys BarelyMusician system.
///
/// @param handle System to destroy.
void BarelyDestroySystem(BarelyHandle handle);

/// Creates new instrument.
///
/// @param handle System handle.
/// @param definition Instrument definition.
/// @return Instrument id.
BarelyInstrumentId BarelyCreateInstrument(
    BarelyHandle handle, BarelyInstrumentDefinition definition);

/// Creates new performer.
///
/// @param handle System handle.
/// @return Performer id.
BarelyPerformerId BarelyCreatePerformer(BarelyHandle handle);

/// Destroys instrument.
///
/// @param handle System handle.
/// @param id Instrument id to destroy.
BarelyStatus BarelyDestroyInstrument(BarelyHandle handle,
                                     BarelyInstrumentId instrument_id);

/// Destroys performer.
///
/// @param handle System handle.
/// @param id Performer id to destroy.
BarelyStatus BarelyDestroyPerformer(BarelyHandle handle, BarelyPerformerId id);

/// Gets instrument control value.
///
/// @param handle System handle.
/// @param instrument_id Instrument id.
/// @param control_id Control id.
/// @param control_value Control value.
BarelyStatus BarelyGetInstrumentControl(const BarelyHandle handle,
                                        BarelyInstrumentId instrument_id,
                                        BarelyInstrumentControlId control_id,
                                        float* control_value);

/// Gets performer instrument.
///
/// @param handle System handle.
/// @param performer_id Performer id.
/// @param instrument_id Instrument id.
BarelyStatus BarelyGetPerformerInstrument(const BarelyHandle handle,
                                          BarelyPerformerId performer_id,
                                          BarelyInstrumentId* instrument_id);

/// Gets playback position.
///
/// @param handle System handle.
/// @param position Playback position.
BarelyStatus BarelyGetPlaybackPosition(const BarelyHandle handle,
                                       BarelyPosition* position);

/// Gets playback tempo.
///
/// @param handle System handle.
/// @param tempo Playback tempo in BPM.
BarelyStatus BarelyGetPlaybackTempo(const BarelyHandle handle, double* tempo);

/// Gets energy.
///
/// @param handle System handle.
/// @param energy System energy.
BarelyStatus BarelyGetEnergy(const BarelyHandle handle, float* energy);

/// Gets stress.
///
/// @param handle System handle.
/// @param stress System stress.
BarelyStatus BarelyGetStress(const BarelyHandle handle, float* stress);

/// Returns whether instrument note is on or not.
///
/// @param handle System handle.
/// @param instrument_id Instrument id.
/// @param pitch Pitch.
/// @param is_note_on Denotes whether note is on or not.
BarelyStatus BarelyIsInstrumentNoteOn(const BarelyHandle handle,
                                      BarelyInstrumentId instrument_id,
                                      float pitch, bool* is_note_on);

/// Returns whether playback is active or not.
///
/// @param handle System handle.
/// @param is_playing Denotes whether playback is active or not.
BarelyStatus BarelyIsPlaying(const BarelyHandle handle, bool* is_playing);

/// Performs note.
///
/// @param handle System handle.
/// @param performer_id Performer id.
/// @param note Note.
BarelyStatus BarelyPerformNote(BarelyHandle handle,
                               BarelyPerformerId performer_id, BarelyNote note);

/// Processes instrument note.
///
/// @param handle System handle.
/// @param instrument_id Instrument id.
/// @param timestamp Process timestamp.
/// @param output Output buffer.
BarelyStatus BarelyProcessInstrument(BarelyHandle handle,
                                     BarelyInstrumentId instrument_id,
                                     BarelyTimestamp timestamp,
                                     BarelyBuffer output);

/// Resets all instrument controls to their default values.
///
/// @param handle System handle.
/// @param instrument_id Instrument id.
BarelyStatus BarelyResetAllInstrumentControls(BarelyHandle handle,
                                              BarelyInstrumentId instrument_id);

/// Starts all instrument notes off.
///
/// @param handle System handle.
/// @param instrument_id Instrument id.
BarelyStatus BarelySetAllInstrumentNotesOff(BarelyHandle handle,
                                            BarelyInstrumentId instrument_id);

/// Sets beat callback.
///
/// @param handle System handle.
/// @param beat_callback Beat callback.
BarelyStatus BarelySetBeatCallback(BarelyHandle handle,
                                   BarelyBeatCallback beat_callback);

/// Sets conductor.
///
/// @param handle System handle.
/// @param definition Conductor definition.
BarelyStatus BarelySetConductor(BarelyHandle handle,
                                BarelyConductorDefinition definition);

/// Sets instrument control value.
///
/// @param handle System handle.
/// @param instrument_id Instrument id.
/// @param control_id Control id.
/// @param control_value Control value.
BarelyStatus BarelySetInstrumentControl(BarelyHandle handle,
                                        BarelyInstrumentId instrument_id,
                                        BarelyInstrumentControlId control_id,
                                        float control_value);

/// Starts playing instrument note.
///
/// @param handle System handle.
/// @param instrument_id Instrument id.
/// @param pitch Pitch.
/// @param intensity Note intensity.
BarelyStatus BarelySetInstrumentNoteOn(BarelyHandle handle,
                                       BarelyInstrumentId instrument_id,
                                       float pitch, float intensity);

/// Stops playing instrument note.
///
/// @param handle System handle.
/// @param instrument_id Instrument id.
/// @param pitch Pitch.
BarelyStatus BarelySetInstrumentNoteOff(BarelyHandle handle,
                                        BarelyInstrumentId instrument_id,
                                        float pitch);

/// Sets note on callback.
///
/// @param handle System handle.
/// @param note_on_callback Note on callback.
BarelyStatus BarelySetNoteOnCallback(BarelyHandle handle,
                                     BarelyNoteOnCallback note_on_callback);

/// Sets note off callback.
///
/// @param handle System handle.
/// @param note_off_callback Note off callback.
BarelyStatus BarelySetNoteOffCallback(BarelyHandle handle,
                                      BarelyNoteOffCallback note_off_callback);

/// Sets performer instrument.
///
/// @param handle System handle.
/// @param performer_id Performer id.
/// @param instrument_id Instrument id.
BarelyStatus BarelySetPerformerInstrument(BarelyHandle handle,
                                          BarelyPerformerId performer_id,
                                          BarelyInstrumentId instrument_id);

/// Sets playback position.
///
/// @param handle System handle.
/// @param position Playback position.
BarelyStatus BarelySetPlaybackPosition(BarelyHandle handle,
                                       BarelyPosition position);

/// Sets playback tempo.
///
/// @param handle System handle.
/// @param tempo Playback tempo in BPM.
BarelyStatus BarelySetPlaybackTempo(BarelyHandle handle, double tempo);

/// Sets energy.
///
/// @param handle System handle.
/// @param energy System energy.
BarelyStatus BarelySetEnergy(BarelyHandle handle, float energy);

/// Sets stress.
///
/// @param handle System handle.
/// @param stress System stress.
BarelyStatus BarelySetStress(BarelyHandle handle, float stress);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_BARELYMUSICIAN_H_
