#ifndef PLATFORMS_UNITY_UNITY_H_
#define PLATFORMS_UNITY_UNITY_H_

// NOLINTBEGIN
#include <stdint.h>

#include "barelymusician/barelymusician.h"

extern "C" {

/// Unity instrument type enum alias.
typedef int32_t BarelyUnityInstrumentType;

/// Unity instrument type enum values
enum BarelyUnityInstrumentType_Values {
  // Percussion instrumnet.
  BarelyUnityInstrumentType_kPercussion = 0,
  // Sampler instrument.
  BarelyUnityInstrumentType_kSampler = 1,
  // Synth instrument.
  BarelyUnityInstrumentType_kSynth = 2,
};

/// Creates new Unity instrument.
///
/// @param handle Engine handle.
/// @param type Unity instrument type.
/// @param frame_rate Frame rate in hz.
/// @param out_instrument_id Output instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyUnityInstrument_Create(
    BarelyEngineHandle handle, BarelyUnityInstrumentType type,
    int32_t frame_rate, BarelyId* out_instrument_id);

/// Sets Unity instrument note off callback.
///
/// @param handle Engine handle.
/// @param instrument_id Instrument identifier.
/// @param callback Note off callback.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyUnityInstrument_SetNoteOffCallback(
    BarelyEngineHandle handle, BarelyId instrument_id,
    BarelyInstrument_NoteOffCallback callback);

/// Sets Unity instrument note on callback.
///
/// @param handle Engine handle.
/// @param instrument_id Instrument identifier.
/// @param callback Note on callback.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyUnityInstrument_SetNoteOnCallback(
    BarelyEngineHandle handle, BarelyId instrument_id,
    BarelyInstrument_NoteOnCallback callback);

}  // extern "C"
// NOLINTEND

#endif  // PLATFORMS_UNITY_UNITY_H_
