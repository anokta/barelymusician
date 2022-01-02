#include "platforms/capi/barelymusician_v2.h"

#include <stdint.h>

extern "C" {

/// BarelyMusician C API.
struct BarelyMusician {
  // TODO(#85): This is a temp POC for testing.
  int sample_rate;
  BarelyId id_count;
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

BarelyStatus BarelyInstrument_Create(BarelyApi api,
                                     BarelyInstrumentDefinition definition,
                                     BarelyId* out_instrument_id) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_instrument_id) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement - this is a temp POC for testing.
  definition;
  *out_instrument_id = ++api->id_count;
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_Destroy(BarelyApi api, BarelyId instrument_id) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  instrument_id;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyInstrument_IsNoteOn(BarelyApi api, BarelyId instrument_id,
                                       float pitch, bool* out_is_note_on) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_is_note_on) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  instrument_id;
  pitch;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyInstrument_SetNoteOff(BarelyApi api, BarelyId instrument_id,
                                         float pitch) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  instrument_id;
  pitch;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyInstrument_SetNoteOn(BarelyApi api, BarelyId instrument_id,
                                        float pitch, float intensity) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  instrument_id;
  pitch;
  intensity;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyTransport_SetBeatCallback(
    BarelyApi api, BarelyTransport_BeatCallback beat_callback) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  beat_callback;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyTransport_SetUpdateCallback(
    BarelyApi api, BarelyTransport_UpdateCallback update_callback) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  update_callback;
  return BarelyStatus_kUnimplemented;
}

}  // extern "C"
