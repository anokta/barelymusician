#ifndef PLATFORMS_UNITY_NATIVE_UNITY_H_
#define PLATFORMS_UNITY_NATIVE_UNITY_H_

#include "platforms/capi/barelymusician.h"
#include "platforms/capi/visibility.h"

extern "C" {

/// Adds new synth instrument.
///
/// @param handle BarelyMusician handle.
/// @return Instrument id.
BARELY_EXPORT BarelyId BarelyAddSynthInstrument(BarelyHandle handle);

}  // extern "C"

#endif  // PLATFORMS_UNITY_NATIVE_UNITY_H_
