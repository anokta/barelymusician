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

// Global constants.
enum BarelyConstants {
  // Invalid id.
  kBarelyInvalidId = 0,

  // Maximum name string size in bytes.
  kBarelyMaxNameStringSize = 32,
};

// Status codes.
enum BarelyStatusValues {
  // Success.
  kBarelyStatus_Ok = 0,

  // Internal error.
  kBarelyStatus_Internal = 1,

  // Unimplemented error.
  kBarelyStatus_Unimplemented = 2,
};
typedef uint32_t BarelyStatus;

// System handle type.
typedef struct BarelySystem* BarelyHandle;

// Id type.
typedef uint64_t BarelyId;

// Instrument id type.
typedef BarelyId BarelyInstrumentId;

// Instrument control id type.
typedef BarelyId BarelyInstrumentControlId;

// Timestamp type.
typedef double BarelyTimestamp;

// TODO(anokta): Callback or Fn or Handler or Func?
// Instrument control callback type.
typedef void (*BarelyInstrumentControlCallback)(BarelyInstrumentControlId id,
                                                float value);

// Instrument note on callback type.
typedef void (*BarelyInstrumentNoteOnCallback)(float index, float intensity);

// Instrument note off callback type.
typedef void (*BarelyInstrumentNoteOffCallback)(float index);

// Instrument process callback type.
typedef void (*BarelyInstrumentProcessCallback)(float* output,
                                                int32_t num_channels,
                                                int32_t num_frames);

// Audio buffer.
typedef struct BarelyBuffer {
  // Interleaved PCM data.
  float* data;

  // Number of channels.
  int32_t num_channels;

  // Number of frames.
  int32_t num_frames;
} BarelyBuffer;

// Instrument definition.
typedef struct BarelyInstrumentDefinition {
  // Instrument name.
  char name[kBarelyMaxNameStringSize];

  // TODO(anokta): Create/destroy + control callbacks.

  // Note on callback function.
  BarelyInstrumentProcessCallback note_on_callback;

  // Note off callback function.
  BarelyInstrumentProcessCallback note_off_callback;

  // Process callback function.
  BarelyInstrumentProcessCallback process_callback;
} BarelyInstrumentDefinition;

// Creates new BarelyMusician system.
//
// @return System handle.
BarelyHandle BarelyCreateSystem();

// Destroys BarelyMusician system.
//
// @param handle System to destroy.
void BarelyDestroySystem(BarelyHandle handle);

// Creates new instrument.
//
// @param handle System handle.
// @param definition Instrument definition.
// @return Instrument id.
BarelyInstrumentId BarelyCreateInstrument(
    BarelyHandle handle, BarelyInstrumentDefinition definition);

// Destroys instrument.
//
// @param handle System handle.
// @param id Instrument id to destroy.
BarelyStatus BarelyDestroyInstrument(BarelyHandle handle,
                                     BarelyInstrumentId id);

// Gets instrument control value.
//
// @param handle System handle.
// @param id Instrument id.
// @param control_id Control id.
// @param control_value Control value.
BarelyStatus BarelyGetInstrumentControl(const BarelyHandle handle,
                                        BarelyInstrumentId id,
                                        BarelyInstrumentControlId control_id,
                                        float* control_value);

// Gets playback position.
//
// @param handle System handle.
// @param position Playback position.
BarelyStatus BarelyGetPlaybackPosition(BarelyHandle handle, double* position);

// Gets playback tempo.
//
// @param handle System handle.
// @param tempo Playback tempo in BPM.
BarelyStatus BarelyGetPlaybackTempo(BarelyHandle handle, double* tempo);

// Returns whether instrument note is on or not.
//
// @param handle System handle.
// @param id Instrument id.
// @param note_index Note index.
// @param is_note_on Denotes whether note is on or not.
BarelyStatus BarelyIsInstrumentNoteOn(const BarelyHandle handle,
                                      BarelyInstrumentId id, float note_index,
                                      bool* is_note_on);

// Processes instrument note.
//
// @param handle System handle.
// @param id Instrument id.
// @param timestamp Process timestamp.
// @param output Output buffer.
BarelyStatus BarelyProcessInstrument(BarelyHandle handle, BarelyInstrumentId id,
                                     BarelyTimestamp timestamp,
                                     BarelyBuffer output);

// Sets instrument control value.
//
// @param handle System handle.
// @param id Instrument id.
// @param control_id Control id.
// @param control_value Control value.
BarelyStatus BarelySetInstrumentControl(BarelyHandle handle,
                                        BarelyInstrumentId id,
                                        BarelyInstrumentControlId control_id,
                                        float control_value);

// Starts playing instrument note.
//
// @param handle System handle.
// @param id Instrument id.
// @param note_index Note index.
// @param note_intensity Note intensity.
BarelyStatus BarelySetInstrumentNoteOn(BarelyHandle handle,
                                       BarelyInstrumentId id, float note_index,
                                       float note_intensity);

// Stops playing instrument note.
//
// @param handle System handle.
// @param id Instrument id.
// @param note_index Note index.
BarelyStatus BarelySetInstrumentNoteOff(BarelyHandle handle,
                                        BarelyInstrumentId id,
                                        float note_index);

// Sets playback position.
//
// @param handle System handle.
// @param position Playback position.
BarelyStatus BarelySetPlaybackPosition(BarelyHandle handle, double position);

// Sets playback tempo.
//
// @param handle System handle.
// @param tempo Playback tempo in BPM.
BarelyStatus BarelySetPlaybackTempo(BarelyHandle handle, double tempo);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_BARELYMUSICIAN_H_
