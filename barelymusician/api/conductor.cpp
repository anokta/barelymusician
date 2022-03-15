#include "barelymusician/api/conductor.h"

#include <stdbool.h>  // NOLINT(modernize-deprecated-headers)

#include "barelymusician/api/status.h"
#include "barelymusician/conductor/transport.h"

extern "C" {

namespace {

// Converts minutes from seconds.
constexpr double kMinutesFromSeconds = 1.0 / 60.0;

// Converts seconds from minutes.
constexpr double kSecondsFromMinutes = 60.0;

}  // namespace

/// Conductor.
struct BarelyConductor : public barelyapi::Transport {
  // Default constructor.
  BarelyConductor() = default;

  // Non-copyable and non-movable.
  BarelyConductor(const BarelyConductor& other) = delete;
  BarelyConductor& operator=(const BarelyConductor& other) = delete;
  BarelyConductor(BarelyConductor&& other) noexcept = delete;
  BarelyConductor& operator=(BarelyConductor&& other) noexcept = delete;

 private:
  // Ensures that the instance can only be destroyed via explicit destroy call.
  friend BARELY_EXPORT BarelyStatus
  BarelyConductor_Destroy(BarelyConductorHandle handle);
  ~BarelyConductor() = default;
};

BarelyStatus BarelyConductor_Create(BarelyConductorHandle* out_handle) {
  if (!out_handle) return BarelyStatus_kInvalidArgument;

  *out_handle = new BarelyConductor();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyConductor_Destroy(BarelyConductorHandle handle) {
  if (!handle) return BarelyStatus_kNotFound;

  delete handle;
  return BarelyStatus_kOk;
}

BarelyStatus BarelyConductor_GetNote(BarelyConductorHandle handle,
                                     BarelyNoteDefinition_Pitch pitch,
                                     double* out_pitch) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_pitch) return BarelyStatus_kInvalidArgument;

  pitch;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_GetPosition(BarelyConductorHandle handle,
                                         double* out_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_position) return BarelyStatus_kInvalidArgument;

  *out_position = handle->GetPosition();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyConductor_GetRootNote(BarelyConductorHandle handle,
                                         double* out_root_pitch) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_root_pitch) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_GetScale(BarelyConductorHandle handle,
                                      double** out_scale_pitches,
                                      int32_t* out_num_scale_pitches) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_scale_pitches) return BarelyStatus_kInvalidArgument;
  if (!out_num_scale_pitches) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_GetTempo(BarelyConductorHandle handle,
                                      double* out_tempo) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_tempo) return BarelyStatus_kInvalidArgument;

  *out_tempo = handle->GetTempo() * kSecondsFromMinutes;
  return BarelyStatus_kOk;
}

BarelyStatus BarelyConductor_GetTimestamp(BarelyConductorHandle handle,
                                          double* out_timestamp) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_timestamp) return BarelyStatus_kInvalidArgument;

  *out_timestamp = handle->GetTimestamp();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyConductor_GetTimestampAtPosition(
    BarelyConductorHandle handle, double position, double* out_timestamp) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_timestamp) return BarelyStatus_kInvalidArgument;

  *out_timestamp = handle->GetTimestamp(position);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyConductor_IsPlaying(BarelyConductorHandle handle,
                                       bool* out_is_playing) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_is_playing) return BarelyStatus_kInvalidArgument;

  *out_is_playing = handle->IsPlaying();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyConductor_SetAdjustNoteCallback(
    BarelyConductorHandle handle, BarelyConductor_AdjustNoteCallback callback,
    void* user_data) {
  if (!handle) return BarelyStatus_kNotFound;

  callback;
  user_data;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_SetAdjustParameterAutomationCallback(
    BarelyConductorHandle handle,
    BarelyConductor_AdjustParameterAutomationCallback callback,
    void* user_data) {
  if (!handle) return BarelyStatus_kNotFound;

  callback;
  user_data;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_SetAdjustTempoCallback(
    BarelyConductorHandle handle, BarelyConductor_AdjustTempoCallback callback,
    void* user_data) {
  if (!handle) return BarelyStatus_kNotFound;

  callback;
  user_data;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_SetBeatCallback(
    BarelyConductorHandle handle, BarelyConductor_BeatCallback callback,
    void* user_data) {
  if (!handle) return BarelyStatus_kNotFound;

  if (callback) {
    handle->SetBeatCallback(
        [callback, user_data](double position, double timestamp) {
          callback(position, timestamp, user_data);
        });
  } else {
    handle->SetBeatCallback(nullptr);
  }
  return BarelyStatus_kOk;
}

BarelyStatus BarelyConductor_SetPosition(BarelyConductorHandle handle,
                                         double position) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->SetPosition(position);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyConductor_SetRootNote(BarelyConductorHandle handle,
                                         double root_pitch) {
  if (!handle) return BarelyStatus_kNotFound;

  root_pitch;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_SetScale(BarelyConductorHandle handle,
                                      double* scale_pitches,
                                      int32_t num_scale_pitches) {
  if (!handle) return BarelyStatus_kNotFound;

  scale_pitches;
  num_scale_pitches;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_SetTempo(BarelyConductorHandle handle,
                                      double tempo) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->SetTempo(tempo * kMinutesFromSeconds);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyConductor_SetTimestamp(BarelyConductorHandle handle,
                                          double timestamp) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->SetTimestamp(timestamp);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyConductor_SetUpdateCallback(
    BarelyConductorHandle handle, BarelyConductor_UpdateCallback callback,
    void* user_data) {
  if (!handle) return BarelyStatus_kNotFound;

  if (callback) {
    handle->SetUpdateCallback(
        [callback, user_data](double begin_position, double end_position,
                              double begin_timestamp, double end_timestamp) {
          callback(begin_position, end_position, begin_timestamp, end_timestamp,
                   user_data);
        });
  } else {
    handle->SetUpdateCallback(nullptr);
  }
  return BarelyStatus_kOk;
}

BarelyStatus BarelyConductor_Start(BarelyConductorHandle handle) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->Start();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyConductor_Stop(BarelyConductorHandle handle) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->Stop();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyConductor_Update(BarelyConductorHandle handle,
                                    double timestamp) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->Update(timestamp);
  return BarelyStatus_kOk;
}

}  // extern "C"
