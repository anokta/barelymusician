#include "barelymusician/barelymusician.h"

#include <stdbool.h>  // NOLINT(modernize-deprecated-headers)
#include <stdint.h>   // NOLINT(modernize-deprecated-headers)

#include <cstddef>

#include "barelymusician/engine/engine.h"

extern "C" {

/// Engine.
struct BarelyMusician {
  // Default constructor.
  BarelyMusician() = default;

  // Non-copyable and non-movable.
  BarelyMusician(const BarelyMusician& other) = delete;
  BarelyMusician& operator=(const BarelyMusician& other) = delete;
  BarelyMusician(BarelyMusician&& other) noexcept = delete;
  BarelyMusician& operator=(BarelyMusician&& other) noexcept = delete;

  // Internal engine.
  barely::internal::Engine engine;

 private:
  // Ensures that the instance can only be destroyed via explicit destroy call.
  friend BARELY_EXPORT BarelyStatus
  BarelyMusician_Destroy(BarelyMusicianHandle handle);
  ~BarelyMusician() = default;
};

BarelyStatus BarelyInstrument_Create(BarelyMusicianHandle handle,
                                     BarelyInstrumentDefinition definition,
                                     int32_t frame_rate,
                                     BarelyId* out_instrument_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_instrument_id) return BarelyStatus_kInvalidArgument;

  const auto instrument_id_or =
      handle->engine.CreateInstrument(definition, frame_rate);
  if (instrument_id_or.IsOk()) {
    *out_instrument_id = *instrument_id_or;
    return BarelyStatus_kOk;
  }
  return instrument_id_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_Destroy(BarelyMusicianHandle handle,
                                      BarelyId instrument_id) {
  if (!handle) return BarelyStatus_kNotFound;

  return handle->engine.DestroyInstrument(instrument_id);
}

BarelyStatus BarelyInstrument_GetControl(BarelyMusicianHandle handle,
                                         BarelyId instrument_id, int32_t index,
                                         double* out_value) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_value) return BarelyStatus_kInvalidArgument;

  const auto instrument_or = handle->engine.GetInstrument(instrument_id);
  if (instrument_or.IsOk()) {
    const auto control_or = instrument_or->get().GetControl(index);
    if (control_or.IsOk()) {
      *out_value = control_or->get().GetValue();
      return BarelyStatus_kOk;
    }
    return control_or.GetErrorStatus();
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_GetNoteControl(BarelyMusicianHandle handle,
                                             BarelyId instrument_id,
                                             double pitch, int32_t index,
                                             double* out_value) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_value) return BarelyStatus_kInvalidArgument;

  const auto instrument_or = handle->engine.GetInstrument(instrument_id);
  if (instrument_or.IsOk()) {
    const auto note_control_or =
        instrument_or->get().GetNoteControl(pitch, index);
    if (note_control_or.IsOk()) {
      *out_value = note_control_or->get().GetValue();
      return BarelyStatus_kOk;
    }
    return note_control_or.GetErrorStatus();
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_IsNoteOn(BarelyMusicianHandle handle,
                                       BarelyId instrument_id, double pitch,
                                       bool* out_is_note_on) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_is_note_on) return BarelyStatus_kInvalidArgument;

  const auto instrument_or = handle->engine.GetInstrument(instrument_id);
  if (instrument_or.IsOk()) {
    *out_is_note_on = instrument_or->get().IsNoteOn(pitch);
    return BarelyStatus_kOk;
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_Process(BarelyMusicianHandle handle,
                                      BarelyId instrument_id,
                                      double* output_samples,
                                      int32_t output_channel_count,
                                      int32_t output_frame_count,
                                      double timestamp) {
  if (!handle) return BarelyStatus_kNotFound;

  return handle->engine.ProcessInstrument(instrument_id, output_samples,
                                          output_channel_count,
                                          output_frame_count, timestamp);
}

BarelyStatus BarelyInstrument_ResetAllControls(BarelyMusicianHandle handle,
                                               BarelyId instrument_id) {
  if (!handle) return BarelyStatus_kNotFound;

  const auto instrument_or = handle->engine.GetInstrument(instrument_id);
  if (instrument_or.IsOk()) {
    instrument_or->get().ResetAllControls();
    return BarelyStatus_kOk;
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_ResetAllNoteControls(BarelyMusicianHandle handle,
                                                   BarelyId instrument_id,
                                                   double pitch) {
  if (!handle) return BarelyStatus_kNotFound;

  const auto instrument_or = handle->engine.GetInstrument(instrument_id);
  if (instrument_or.IsOk()) {
    return instrument_or->get().ResetAllNoteControls(pitch);
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_ResetControl(BarelyMusicianHandle handle,
                                           BarelyId instrument_id,
                                           int32_t index) {
  if (!handle) return BarelyStatus_kNotFound;

  const auto instrument_or = handle->engine.GetInstrument(instrument_id);
  if (instrument_or.IsOk()) {
    return instrument_or->get().ResetControl(index);
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_ResetNoteControl(BarelyMusicianHandle handle,
                                               BarelyId instrument_id,
                                               double pitch, int32_t index) {
  if (!handle) return BarelyStatus_kNotFound;

  const auto instrument_or = handle->engine.GetInstrument(instrument_id);
  if (instrument_or.IsOk()) {
    return instrument_or->get().ResetNoteControl(pitch, index);
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_SetAllNotesOff(BarelyMusicianHandle handle,
                                             BarelyId instrument_id) {
  if (!handle) return BarelyStatus_kNotFound;

  const auto instrument_or = handle->engine.GetInstrument(instrument_id);
  if (instrument_or.IsOk()) {
    instrument_or->get().SetAllNotesOff();
    return BarelyStatus_kOk;
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_SetControl(BarelyMusicianHandle handle,
                                         BarelyId instrument_id, int32_t index,
                                         double value, double slope_per_beat) {
  if (!handle) return BarelyStatus_kNotFound;

  const auto instrument_or = handle->engine.GetInstrument(instrument_id);
  if (instrument_or.IsOk()) {
    return instrument_or->get().SetControl(index, value, slope_per_beat);
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_SetControlEventCallback(
    BarelyMusicianHandle handle, BarelyId instrument_id,
    BarelyInstrument_ControlEventCallback callback, void* user_data) {
  if (!handle) return BarelyStatus_kNotFound;

  const auto instrument_or = handle->engine.GetInstrument(instrument_id);
  if (instrument_or.IsOk()) {
    if (callback) {
      instrument_or->get().SetControlEventCallback(
          [callback, user_data](int32_t index, double value) {
            callback(index, value, user_data);
          });
    } else {
      instrument_or->get().SetControlEventCallback(nullptr);
    }
    return BarelyStatus_kOk;
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_SetData(BarelyMusicianHandle handle,
                                      BarelyId instrument_id, const void* data,
                                      int32_t size) {
  if (!handle) return BarelyStatus_kNotFound;

  const auto instrument_or = handle->engine.GetInstrument(instrument_id);
  if (instrument_or.IsOk()) {
    instrument_or->get().SetData({static_cast<const std::byte*>(data),
                                  static_cast<const std::byte*>(data) + size});
    return BarelyStatus_kOk;
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_SetNoteControl(BarelyMusicianHandle handle,
                                             BarelyId instrument_id,
                                             double pitch, int32_t index,
                                             double value,
                                             double slope_per_beat) {
  if (!handle) return BarelyStatus_kNotFound;

  const auto instrument_or = handle->engine.GetInstrument(instrument_id);
  if (instrument_or.IsOk()) {
    return instrument_or->get().SetNoteControl(pitch, index, value,
                                               slope_per_beat);
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_SetNoteControlEventCallback(
    BarelyMusicianHandle handle, BarelyId instrument_id,
    BarelyInstrument_NoteControlEventCallback callback, void* user_data) {
  if (!handle) return BarelyStatus_kNotFound;

  const auto instrument_or = handle->engine.GetInstrument(instrument_id);
  if (instrument_or.IsOk()) {
    if (callback) {
      instrument_or->get().SetNoteControlEventCallback(
          [callback, user_data](double pitch, int32_t index, double value) {
            callback(pitch, index, value, user_data);
          });
    } else {
      instrument_or->get().SetNoteControlEventCallback(nullptr);
    }
    return BarelyStatus_kOk;
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_SetNoteOff(BarelyMusicianHandle handle,
                                         BarelyId instrument_id, double pitch) {
  if (!handle) return BarelyStatus_kNotFound;

  const auto instrument_or = handle->engine.GetInstrument(instrument_id);
  if (instrument_or.IsOk()) {
    instrument_or->get().SetNoteOff(pitch);
    return BarelyStatus_kOk;
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_SetNoteOffEventCallback(
    BarelyMusicianHandle handle, BarelyId instrument_id,
    BarelyInstrument_NoteOffEventCallback callback, void* user_data) {
  if (!handle) return BarelyStatus_kNotFound;

  const auto instrument_or = handle->engine.GetInstrument(instrument_id);
  if (instrument_or.IsOk()) {
    if (callback) {
      instrument_or->get().SetNoteOffEventCallback(
          [callback, user_data](double pitch) { callback(pitch, user_data); });
    } else {
      instrument_or->get().SetNoteOffEventCallback(nullptr);
    }
    return BarelyStatus_kOk;
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_SetNoteOn(BarelyMusicianHandle handle,
                                        BarelyId instrument_id, double pitch) {
  if (!handle) return BarelyStatus_kNotFound;

  const auto instrument_or = handle->engine.GetInstrument(instrument_id);
  if (instrument_or.IsOk()) {
    instrument_or->get().SetNoteOn(pitch);
    return BarelyStatus_kOk;
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_SetNoteOnEventCallback(
    BarelyMusicianHandle handle, BarelyId instrument_id,
    BarelyInstrument_NoteOnEventCallback callback, void* user_data) {
  if (!handle) return BarelyStatus_kNotFound;

  const auto instrument_or = handle->engine.GetInstrument(instrument_id);
  if (instrument_or.IsOk()) {
    if (callback) {
      instrument_or->get().SetNoteOnEventCallback(
          [callback, user_data](double pitch) { callback(pitch, user_data); });
    } else {
      instrument_or->get().SetNoteOnEventCallback(nullptr);
    }
    return BarelyStatus_kOk;
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyMusician_Create(BarelyMusicianHandle* out_handle) {
  if (!out_handle) return BarelyStatus_kInvalidArgument;

  *out_handle = new BarelyMusician();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_CreateTask(BarelyMusicianHandle handle,
                                       BarelyTaskDefinition definition,
                                       double timestamp, void* user_data,
                                       BarelyId* out_task_id) {
  if (!handle) return BarelyStatus_kInvalidArgument;
  if (!out_task_id) return BarelyStatus_kInvalidArgument;

  const auto task_id_or =
      handle->engine.CreateTask(definition, timestamp, user_data);
  if (task_id_or.IsOk()) {
    *out_task_id = *task_id_or;
    return BarelyStatus_kOk;
  }
  return task_id_or.GetErrorStatus();
}

BarelyStatus BarelyMusician_Destroy(BarelyMusicianHandle handle) {
  if (!handle) return BarelyStatus_kNotFound;

  delete handle;
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_DestroyTask(BarelyMusicianHandle handle,
                                        BarelyId task_id) {
  if (!handle) return BarelyStatus_kNotFound;

  return handle->engine.DestroyTask(task_id);
}

BarelyStatus BarelyMusician_GetTaskTimestamp(BarelyMusicianHandle handle,
                                             BarelyId task_id,
                                             double* out_timestamp) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_timestamp) return BarelyStatus_kInvalidArgument;

  const auto timestamp_or = handle->engine.GetTaskTimestamp(task_id);
  if (timestamp_or.IsOk()) {
    *out_timestamp = *timestamp_or;
    return BarelyStatus_kOk;
  }
  return timestamp_or.GetErrorStatus();
}

BarelyStatus BarelyMusician_GetTempo(BarelyMusicianHandle handle,
                                     double* out_tempo) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_tempo) return BarelyStatus_kInvalidArgument;

  *out_tempo = handle->engine.GetTempo();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_GetTimestamp(BarelyMusicianHandle handle,
                                         double* out_timestamp) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_timestamp) return BarelyStatus_kInvalidArgument;

  *out_timestamp = handle->engine.GetTimestamp();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_SetTaskTimestamp(BarelyMusicianHandle handle,
                                             BarelyId task_id,
                                             double timestamp) {
  if (!handle) return BarelyStatus_kNotFound;

  return handle->engine.SetTaskTimestamp(task_id, timestamp);
}

BarelyStatus BarelyMusician_SetTempo(BarelyMusicianHandle handle,
                                     double tempo) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->engine.SetTempo(tempo);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_Update(BarelyMusicianHandle handle,
                                   double timestamp) {
  if (!handle) return BarelyStatus_kNotFound;
  // TODO(#109): Should this also be handled internally?
  if (timestamp < 0.0) return BarelyStatus_kInvalidArgument;

  handle->engine.Update(timestamp);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyPerformer_Create(BarelyMusicianHandle handle, int32_t order,
                                    BarelyId* out_performer_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_performer_id) return BarelyStatus_kInvalidArgument;

  const auto performer_id_or = handle->engine.CreatePerformer(order);
  if (performer_id_or.IsOk()) {
    *out_performer_id = *performer_id_or;
    return BarelyStatus_kOk;
  }
  return performer_id_or.GetErrorStatus();
}

BarelyStatus BarelyPerformer_CreateTask(BarelyMusicianHandle handle,
                                        BarelyId performer_id,
                                        BarelyTaskDefinition definition,
                                        double position, BarelyTaskType type,
                                        void* user_data,
                                        BarelyId* out_task_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_task_id) return BarelyStatus_kInvalidArgument;

  const auto task_id_or = handle->engine.CreatePerformerTask(
      performer_id, std::move(definition), position,
      static_cast<barely::TaskType>(type), user_data);
  if (task_id_or.IsOk()) {
    *out_task_id = *task_id_or;
    return BarelyStatus_kOk;
  }
  return task_id_or.GetErrorStatus();
}

BarelyStatus BarelyPerformer_Destroy(BarelyMusicianHandle handle,
                                     BarelyId performer_id) {
  if (!handle) return BarelyStatus_kNotFound;

  return handle->engine.DestroyPerformer(performer_id);
}

BarelyStatus BarelyPerformer_DestroyTask(BarelyMusicianHandle handle,
                                         BarelyId performer_id,
                                         BarelyId task_id) {
  if (!handle) return BarelyStatus_kNotFound;

  const auto performer_or = handle->engine.GetPerformer(performer_id);
  if (performer_or.IsOk()) {
    return performer_or->get().DestroyTask(task_id);
  }
  return performer_or.GetErrorStatus();
}

BarelyStatus BarelyPerformer_GetLoopBeginPosition(
    BarelyMusicianHandle handle, BarelyId performer_id,
    double* out_loop_begin_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_loop_begin_position) return BarelyStatus_kInvalidArgument;

  const auto performer_or = handle->engine.GetPerformer(performer_id);
  if (performer_or.IsOk()) {
    *out_loop_begin_position = performer_or->get().GetLoopBeginPosition();
    return BarelyStatus_kOk;
  }
  return performer_or.GetErrorStatus();
}

BarelyStatus BarelyPerformer_GetLoopLength(BarelyMusicianHandle handle,
                                           BarelyId performer_id,
                                           double* out_loop_length) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_loop_length) return BarelyStatus_kInvalidArgument;

  const auto performer_or = handle->engine.GetPerformer(performer_id);
  if (performer_or.IsOk()) {
    *out_loop_length = performer_or->get().GetLoopLength();
    return BarelyStatus_kOk;
  }
  return performer_or.GetErrorStatus();
}

BarelyStatus BarelyPerformer_GetPosition(BarelyMusicianHandle handle,
                                         BarelyId performer_id,
                                         double* out_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_position) return BarelyStatus_kInvalidArgument;

  const auto performer_or = handle->engine.GetPerformer(performer_id);
  if (performer_or.IsOk()) {
    *out_position = performer_or->get().GetPosition();
    return BarelyStatus_kOk;
  }
  return performer_or.GetErrorStatus();
}

BarelyStatus BarelyPerformer_GetTaskPosition(BarelyMusicianHandle handle,
                                             BarelyId performer_id,
                                             BarelyId task_id,
                                             double* out_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_position) return BarelyStatus_kInvalidArgument;

  const auto performer_or = handle->engine.GetPerformer(performer_id);
  if (performer_or.IsOk()) {
    const auto position_or = performer_or->get().GetTaskPosition(task_id);
    if (position_or.IsOk()) {
      *out_position = *position_or;
      return BarelyStatus_kOk;
    }
    return position_or.GetErrorStatus();
  }
  return performer_or.GetErrorStatus();
}

BarelyStatus BarelyPerformer_IsLooping(BarelyMusicianHandle handle,
                                       BarelyId performer_id,
                                       bool* out_is_looping) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_is_looping) return BarelyStatus_kInvalidArgument;

  const auto performer_or = handle->engine.GetPerformer(performer_id);
  if (performer_or.IsOk()) {
    *out_is_looping = performer_or->get().IsLooping();
    return BarelyStatus_kOk;
  }
  return performer_or.GetErrorStatus();
}

BarelyStatus BarelyPerformer_IsPlaying(BarelyMusicianHandle handle,
                                       BarelyId performer_id,
                                       bool* out_is_playing) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_is_playing) return BarelyStatus_kInvalidArgument;

  const auto performer_or = handle->engine.GetPerformer(performer_id);
  if (performer_or.IsOk()) {
    *out_is_playing = performer_or->get().IsPlaying();
    return BarelyStatus_kOk;
  }
  return performer_or.GetErrorStatus();
}

BarelyStatus BarelyPerformer_SetLoopBeginPosition(BarelyMusicianHandle handle,
                                                  BarelyId performer_id,
                                                  double loop_begin_position) {
  if (!handle) return BarelyStatus_kNotFound;

  const auto performer_or = handle->engine.GetPerformer(performer_id);
  if (performer_or.IsOk()) {
    performer_or->get().SetLoopBeginPosition(loop_begin_position);
    return BarelyStatus_kOk;
  }
  return performer_or.GetErrorStatus();
}

BarelyStatus BarelyPerformer_SetLoopLength(BarelyMusicianHandle handle,
                                           BarelyId performer_id,
                                           double loop_length) {
  if (!handle) return BarelyStatus_kNotFound;

  const auto performer_or = handle->engine.GetPerformer(performer_id);
  if (performer_or.IsOk()) {
    performer_or->get().SetLoopLength(loop_length);
    return BarelyStatus_kOk;
  }
  return performer_or.GetErrorStatus();
}

BarelyStatus BarelyPerformer_SetLooping(BarelyMusicianHandle handle,
                                        BarelyId performer_id,
                                        bool is_looping) {
  if (!handle) return BarelyStatus_kNotFound;

  const auto performer_or = handle->engine.GetPerformer(performer_id);
  if (performer_or.IsOk()) {
    performer_or->get().SetLooping(is_looping);
    return BarelyStatus_kOk;
  }
  return performer_or.GetErrorStatus();
}

BarelyStatus BarelyPerformer_SetPosition(BarelyMusicianHandle handle,
                                         BarelyId performer_id,
                                         double position) {
  if (!handle) return BarelyStatus_kNotFound;

  const auto performer_or = handle->engine.GetPerformer(performer_id);
  if (performer_or.IsOk()) {
    performer_or->get().SetPosition(position);
    return BarelyStatus_kOk;
  }
  return performer_or.GetErrorStatus();
}

BarelyStatus BarelyPerformer_SetTaskPosition(BarelyMusicianHandle handle,
                                             BarelyId performer_id,
                                             BarelyId task_id,
                                             double position) {
  if (!handle) return BarelyStatus_kNotFound;

  const auto performer_or = handle->engine.GetPerformer(performer_id);
  if (performer_or.IsOk()) {
    return performer_or->get().SetTaskPosition(task_id, position);
  }
  return performer_or.GetErrorStatus();
}

BarelyStatus BarelyPerformer_Start(BarelyMusicianHandle handle,
                                   BarelyId performer_id) {
  if (!handle) return BarelyStatus_kNotFound;

  const auto performer_or = handle->engine.GetPerformer(performer_id);
  if (performer_or.IsOk()) {
    performer_or->get().Start();
    return BarelyStatus_kOk;
  }
  return performer_or.GetErrorStatus();
}

BarelyStatus BarelyPerformer_Stop(BarelyMusicianHandle handle,
                                  BarelyId performer_id) {
  if (!handle) return BarelyStatus_kNotFound;

  const auto performer_or = handle->engine.GetPerformer(performer_id);
  if (performer_or.IsOk()) {
    performer_or->get().Stop();
    return BarelyStatus_kOk;
  }
  return performer_or.GetErrorStatus();
}

}  // extern "C"
