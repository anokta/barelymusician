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
  // Synth instrument.
  BarelyUnityInstrumentType_kSynth = 1,
};

/// Creates new Unity instrument.
///
/// @param handle Musician handle.
/// @param type Unity instrument type.
/// @param frame_rate Frame rate in hz.
/// @param out_instrument_id Output instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyUnityInstrument_Create(
    BarelyMusicianHandle handle, BarelyUnityInstrumentType type,
    int32_t frame_rate, BarelyId* out_instrument_id);

}  // extern "C"
// NOLINTEND

#endif  // PLATFORMS_UNITY_UNITY_H_
