#include "barelymusician/api/instrument.h"

#include <stdbool.h>  // NOLINT(modernize-deprecated-headers)
#include <stdint.h>   // NOLINT(modernize-deprecated-headers)

#include "barelymusician/api/status.h"
#include "barelymusician/instrument/instrument.h"

extern "C" {

/// Instrument.
struct BarelyInstrument : public barelyapi::Instrument {
  // Constructs new `BarelyInstrument` with given `definition` and `frame_rate`.
  BarelyInstrument(BarelyInstrumentDefinition definition,
                   int32_t frame_rate) noexcept
      : barelyapi::Instrument(definition, frame_rate) {}

  // Non-copyable and non-movable.
  BarelyInstrument(const BarelyInstrument& other) = delete;
  BarelyInstrument& operator=(const BarelyInstrument& other) = delete;
  BarelyInstrument(BarelyInstrument&& other) noexcept = delete;
  BarelyInstrument& operator=(BarelyInstrument&& other) noexcept = delete;

 private:
  // Ensures that the instance can only be destroyed via explicit destroy call.
  friend BARELY_EXPORT BarelyStatus
  BarelyInstrument_Destroy(BarelyInstrumentHandle handle);
  ~BarelyInstrument() = default;
};

BarelyStatus BarelyInstrument_Create(BarelyInstrumentDefinition definition,
                                     int32_t frame_rate,
                                     BarelyInstrumentHandle* out_handle) {
  if (!out_handle) return BarelyStatus_kInvalidArgument;

  *out_handle = new BarelyInstrument(definition, frame_rate);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_Destroy(BarelyInstrumentHandle handle) {
  if (!handle) return BarelyStatus_kNotFound;

  delete handle;
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_GetParameter(BarelyInstrumentHandle handle,
                                           int32_t index, double* out_value) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_value) return BarelyStatus_kInvalidArgument;

  *out_value = handle->GetParameter(index)->GetValue();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_GetParamDefinition(
    BarelyInstrumentHandle handle, int32_t index,
    BarelyParameterDefinition* out_definition) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_definition) return BarelyStatus_kInvalidArgument;

  *out_definition = handle->GetParameter(index)->GetDefinition();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_IsNoteOn(BarelyInstrumentHandle handle,
                                       double pitch, bool* out_is_note_on) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_is_note_on) return BarelyStatus_kInvalidArgument;

  *out_is_note_on = handle->IsNoteOn(pitch);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_Process(BarelyInstrumentHandle handle,
                                      double* output,
                                      int32_t num_output_channels,
                                      int32_t num_output_frames,
                                      double timestamp) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->Process(output, num_output_channels, num_output_frames, timestamp);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_ResetAllParameters(BarelyInstrumentHandle handle,
                                                 double timestamp) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->ResetAllParameters(timestamp);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_ResetParameter(BarelyInstrumentHandle handle,
                                             int32_t index, double timestamp) {
  if (!handle) return BarelyStatus_kNotFound;

  if (handle->ResetParameter(index, timestamp)) {
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kInvalidArgument;
}

BarelyStatus BarelyInstrument_SetData(BarelyInstrumentHandle handle,
                                      BarelyDataDefinition definition,
                                      double timestamp) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->SetData(definition, timestamp);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_SetNoteOffCallback(
    BarelyInstrumentHandle handle,
    BarelyInstrument_NoteOffCallback note_off_callback, void* user_data) {
  if (!handle) return BarelyStatus_kNotFound;

  if (note_off_callback) {
    handle->SetNoteOffCallback(
        [note_off_callback, user_data](double pitch, double timestamp) {
          note_off_callback(pitch, timestamp, user_data);
        });
  } else {
    handle->SetNoteOffCallback(nullptr);
  }
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_SetNoteOnCallback(
    BarelyInstrumentHandle handle,
    BarelyInstrument_NoteOnCallback note_on_callback, void* user_data) {
  if (!handle) return BarelyStatus_kNotFound;

  if (note_on_callback) {
    handle->SetNoteOnCallback([note_on_callback, user_data](double pitch,
                                                            double intensity,
                                                            double timestamp) {
      note_on_callback(pitch, intensity, timestamp, user_data);
    });
  } else {
    handle->SetNoteOnCallback(nullptr);
  }
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_SetParameter(BarelyInstrumentHandle handle,
                                           int32_t index, double value,
                                           double slope, double timestamp) {
  if (!handle) return BarelyStatus_kNotFound;

  if (handle->SetParameter(index, value, slope, timestamp)) {
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kInvalidArgument;
}

BarelyStatus BarelyInstrument_StartNote(BarelyInstrumentHandle handle,
                                        double pitch, double intensity,
                                        double timestamp) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->StartNote(pitch, intensity, timestamp);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_StopAllNotes(BarelyInstrumentHandle handle,
                                           double timestamp) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->StopAllNotes(timestamp);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_StopNote(BarelyInstrumentHandle handle,
                                       double pitch, double timestamp) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->StopNote(pitch, timestamp);
  return BarelyStatus_kOk;
}

}  // extern "C"
