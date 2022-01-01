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

BarelyStatus BarelyApi_SetBeatCallback(BarelyApi api,
                                       BarelyApi_BeatCallback beat_callback) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  beat_callback;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyApi_SetPositionCallback(
    BarelyApi api, BarelyApi_PositionCallback position_callback) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  position_callback;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyApi_GetSampleRate(BarelyApi api, int32_t* out_sample_rate) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_sample_rate) return BarelyStatus_kInvalidArgument;

  *out_sample_rate = api->sample_rate;
  return BarelyStatus_kOk;
}

BarelyStatus BarelyApi_SetSampleRate(BarelyApi api, int32_t sample_rate) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement - this is a temp POC for testing.
  api->sample_rate = sample_rate;
  return BarelyStatus_kOk;
}

BarelyStatus BarelyApi_Update(BarelyApi api, double timestamp) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  timestamp;
  return BarelyStatus_kUnimplemented;
}

}  // extern "C"
