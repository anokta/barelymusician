#include "platforms/capi/barelymusician_v2.h"

#include <stdint.h>

extern "C" {

/// BarelyMusician C API.
struct BarelyMusician {
  int sample_rate;
};

BarelyStatus BarelyApi_Create(BarelyApi* out_api) {
  *out_api = new BarelyMusician();

  return BarelyStatus_kOk;
}

BarelyStatus BarelyApi_Destroy(BarelyApi api) {
  if (!api) return BarelyStatus_kNotFound;

  delete api;
  return BarelyStatus_kOk;
}

BarelyStatus BarelyApi_GetSampleRate(BarelyApi api, int32_t* out_sample_rate) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_sample_rate) return BarelyStatus_kInvalidArgument;

  *out_sample_rate = api->sample_rate;
  return BarelyStatus_kOk;
}

BarelyStatus BarelyApi_SetSampleRate(BarelyApi api, int32_t sample_rate) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Temp POC for testing.
  api->sample_rate = sample_rate;
  return BarelyStatus_kOk;
}

BarelyStatus BarelyApi_Update(BarelyApi api, double /*timestamp*/) {
  if (!api) return BarelyStatus_kNotFound;

  return BarelyStatus_kUnimplemented;
}

}  // extern "C"
