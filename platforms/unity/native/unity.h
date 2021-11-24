#ifndef PLATFORMS_UNITY_NATIVE_UNITY_H_
#define PLATFORMS_UNITY_NATIVE_UNITY_H_

#include <cstdint>

#include "platforms/capi/barelymusician.h"
#include "platforms/common/visibility.h"

extern "C" {

/// Debug callback signature.
typedef void (*DebugCallback)(int32_t severity, const char* message);

/// Adds new synth instrument.
///
/// @param handle BarelyMusician handle.
/// @return Instrument id.
BARELY_EXPORT BarelyId BarelyAddSynthInstrument(BarelyHandle handle);

/// Creates new BarelyMusician for Unity.
///
/// @param sample_rate System sampling rate.
/// @param debug_callback_ptr Pointer to debug callback.
/// @return BarelyMusician handle.
BARELY_EXPORT BarelyHandle BarelyCreateUnity(std::int32_t sample_rate,
                                             DebugCallback debug_callback_ptr);

/// Destroys BarelyMusician for Unity.
///
/// @param handle BarelyMusician handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyDestroyUnity(BarelyHandle handle);

}  // extern "C"

#endif  // PLATFORMS_UNITY_NATIVE_UNITY_H_
