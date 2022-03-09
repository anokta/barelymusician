#include "barelymusician/api/instrument.h"

#include <stdbool.h>  // NOLINT(modernize-deprecated-headers)
#include <stdint.h>   // NOLINT(modernize-deprecated-headers)

#include "barelymusician/instrument/instrument.h"

extern "C" {

/// Instrument.
struct BarelyInstrument {
  // Constructs new `BarelyInstrument`.
  BarelyInstrument(BarelyInstrumentDefinition definition,
                   int32_t sample_rate) noexcept
      : instrument(definition, sample_rate) {}

  // Non-copyable and non-movable.
  BarelyInstrument(const BarelyInstrument& other) = delete;
  BarelyInstrument& operator=(const BarelyInstrument& other) = delete;
  BarelyInstrument(BarelyInstrument&& other) noexcept = delete;
  BarelyInstrument& operator=(BarelyInstrument&& other) noexcept = delete;

  // Instrument instance.
  barelyapi::Instrument instrument;

 private:
  // Ensures that the instance can only be destroyed via explicit destroy call.
  friend BARELY_EXPORT BarelyStatus
  BarelyInstrument_Destroy(BarelyInstrumentHandle handle);
  ~BarelyInstrument() = default;
};

BarelyStatus BarelyInstrument_Create(BarelyInstrumentDefinition definition,
                                     int32_t sample_rate,
                                     BarelyInstrumentHandle* out_handle) {
  if (!out_handle) return BarelyStatus_kInvalidArgument;

  *out_handle = new BarelyInstrument(definition, sample_rate);
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

  *out_value = handle->instrument.GetParameter(index)->GetValue();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_GetParamDefinition(
    BarelyInstrumentHandle handle, int32_t index,
    BarelyParameterDefinition* out_definition) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_definition) return BarelyStatus_kInvalidArgument;

  *out_definition = handle->instrument.GetParameter(index)->GetDefinition();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_IsNoteOn(BarelyInstrumentHandle handle,
                                       float pitch, bool* out_is_note_on) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_is_note_on) return BarelyStatus_kInvalidArgument;

  *out_is_note_on = handle->instrument.IsNoteOn(pitch);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_Process(BarelyInstrumentHandle handle,
                                      double timestamp, float* output,
                                      int32_t num_output_channels,
                                      int32_t num_output_frames) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->instrument.Process(output, num_output_channels, num_output_frames,
                             timestamp);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_ResetAllParameters(BarelyInstrumentHandle handle,
                                                 double timestamp) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->instrument.ResetAllParameters(timestamp);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_ResetParameter(BarelyInstrumentHandle handle,
                                             double timestamp, int32_t index) {
  if (!handle) return BarelyStatus_kNotFound;

  if (handle->instrument.ResetParameter(index, timestamp)) {
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kInvalidArgument;
}

BarelyStatus BarelyInstrument_SetData(BarelyInstrumentHandle handle,
                                      double timestamp,
                                      BarelyDataDefinition definition) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->instrument.SetData(definition, timestamp);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_SetNoteOffCallback(
    BarelyInstrumentHandle handle,
    BarelyInstrument_NoteOffCallback note_off_callback, void* user_data) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->instrument.SetNoteOffCallback(note_off_callback, user_data);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_SetNoteOnCallback(
    BarelyInstrumentHandle handle,
    BarelyInstrument_NoteOnCallback note_on_callback, void* user_data) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->instrument.SetNoteOnCallback(note_on_callback, user_data);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_SetParameter(BarelyInstrumentHandle handle,
                                           double timestamp, int32_t index,
                                           double value) {
  if (!handle) return BarelyStatus_kNotFound;

  if (handle->instrument.SetParameter(index, value, timestamp)) {
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kInvalidArgument;
}

BarelyStatus BarelyInstrument_StartNote(BarelyInstrumentHandle handle,
                                        double timestamp, float pitch,
                                        float intensity) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->instrument.StartNote(pitch, intensity, timestamp);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_StopAllNotes(BarelyInstrumentHandle handle,
                                           double timestamp) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->instrument.StopAllNotes(timestamp);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_StopNote(BarelyInstrumentHandle handle,
                                       double timestamp, float pitch) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->instrument.StopNote(pitch, timestamp);
  return BarelyStatus_kOk;
}

}  // extern "C"
