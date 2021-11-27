#include "platforms/unity/native/unity.h"

#include "platforms/capi/barelymusician.h"

BarelyId BarelyAddSynthInstrument(BarelyHandle handle) {
  return BarelyAddInstrument(handle, kBarelySynthInstrument);
}
