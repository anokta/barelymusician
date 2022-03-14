#include "barelymusician/api/transport.h"

#include <stdbool.h>  // NOLINT(modernize-deprecated-headers)

#include "barelymusician/api/status.h"
#include "barelymusician/transport/transport.h"

extern "C" {

namespace {

// Converts minutes from seconds.
constexpr double kMinutesFromSeconds = 1.0 / 60.0;

// Converts seconds from minutes.
constexpr double kSecondsFromMinutes = 60.0;

}  // namespace

/// Transport.
struct BarelyTransport : public barelyapi::Transport {
  // Default constructor.
  BarelyTransport() = default;

  // Non-copyable and non-movable.
  BarelyTransport(const BarelyTransport& other) = delete;
  BarelyTransport& operator=(const BarelyTransport& other) = delete;
  BarelyTransport(BarelyTransport&& other) noexcept = delete;
  BarelyTransport& operator=(BarelyTransport&& other) noexcept = delete;

 private:
  // Ensures that the instance can only be destroyed via explicit destroy call.
  friend BARELY_EXPORT BarelyStatus
  BarelyTransport_Destroy(BarelyTransportHandle handle);
  ~BarelyTransport() = default;
};

BarelyStatus BarelyTransport_Create(BarelyTransportHandle* out_handle) {
  if (!out_handle) return BarelyStatus_kInvalidArgument;

  *out_handle = new BarelyTransport();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyTransport_Destroy(BarelyTransportHandle handle) {
  if (!handle) return BarelyStatus_kNotFound;

  delete handle;
  return BarelyStatus_kOk;
}

BarelyStatus BarelyTransport_GetPosition(BarelyTransportHandle handle,
                                         double* out_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_position) return BarelyStatus_kInvalidArgument;

  *out_position = handle->GetPosition();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyTransport_GetTempo(BarelyTransportHandle handle,
                                      double* out_tempo) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_tempo) return BarelyStatus_kInvalidArgument;

  *out_tempo = handle->GetTempo() * kSecondsFromMinutes;
  return BarelyStatus_kOk;
}

BarelyStatus BarelyTransport_GetTimestamp(BarelyTransportHandle handle,
                                          double* out_timestamp) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_timestamp) return BarelyStatus_kInvalidArgument;

  *out_timestamp = handle->GetTimestamp();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyTransport_GetTimestampAtPosition(
    BarelyTransportHandle handle, double position, double* out_timestamp) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_timestamp) return BarelyStatus_kInvalidArgument;

  *out_timestamp = handle->GetTimestamp(position);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyTransport_IsPlaying(BarelyTransportHandle handle,
                                       bool* out_is_playing) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_is_playing) return BarelyStatus_kInvalidArgument;

  *out_is_playing = handle->IsPlaying();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyTransport_SetBeatCallback(
    BarelyTransportHandle handle, BarelyTransport_BeatCallback beat_callback,
    void* user_data) {
  if (!handle) return BarelyStatus_kNotFound;

  if (beat_callback) {
    handle->SetBeatCallback(
        [beat_callback, user_data](double position, double timestamp) {
          beat_callback(position, timestamp, user_data);
        });
  } else {
    handle->SetBeatCallback(nullptr);
  }
  return BarelyStatus_kOk;
}

BarelyStatus BarelyTransport_SetPosition(BarelyTransportHandle handle,
                                         double position) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->SetPosition(position);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyTransport_SetTempo(BarelyTransportHandle handle,
                                      double tempo) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->SetTempo(tempo * kMinutesFromSeconds);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyTransport_SetTimestamp(BarelyTransportHandle handle,
                                          double timestamp) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->SetTimestamp(timestamp);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyTransport_Start(BarelyTransportHandle handle) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->Start();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyTransport_Stop(BarelyTransportHandle handle) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->Stop();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyTransport_Update(BarelyTransportHandle handle,
                                    double timestamp) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->Update(timestamp);
  return BarelyStatus_kOk;
}

}  // extern "C"
