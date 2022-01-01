#include "platforms/capi/barelymusician_v2.h"

#include <stdint.h>

extern "C" {

/// BarelyMusician C API.
struct BarelyMusician {};

BarelyStatus BarelyApi_Create(BarelyApi* out_api) {
  *out_api = new BarelyMusician();

  return BarelyStatus_kOk;
}

BarelyStatus BarelyApi_Destroy(BarelyApi api) {
  if (!api) return BarelyStatus_kNotFound;

  delete api;
  return BarelyStatus_kOk;
}

}  // extern "C"
