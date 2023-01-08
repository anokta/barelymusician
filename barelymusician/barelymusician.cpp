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

  // Monotonic identifier counter.
  BarelyId id_counter = 0;

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
  if (frame_rate < 0) return BarelyStatus_kInvalidArgument;
  if (!out_instrument_id) return BarelyStatus_kInvalidArgument;

  *out_instrument_id = ++handle->id_counter;
  if (handle->engine.CreateInstrument(*out_instrument_id, definition,
                                      frame_rate)) {
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kInternal;
}

BarelyStatus BarelyInstrument_Destroy(BarelyMusicianHandle handle,
                                      BarelyId instrument_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (handle->engine.DestroyInstrument(instrument_id)) {
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_GetControl(BarelyMusicianHandle handle,
                                         BarelyId instrument_id, int32_t index,
                                         double* out_value) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (index < 0) return BarelyStatus_kInvalidArgument;
  if (!out_value) return BarelyStatus_kInvalidArgument;

  if (const auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    const auto control_or = instrument->GetControl(index);
    if (control_or.IsOk()) {
      *out_value = control_or.GetValue().get().Get();
      return BarelyStatus_kOk;
    }
    return control_or.GetErrorStatus();
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_GetNoteControl(BarelyMusicianHandle handle,
                                             BarelyId instrument_id,
                                             double pitch, int32_t index,
                                             double* out_value) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (index < 0) return BarelyStatus_kInvalidArgument;
  if (!out_value) return BarelyStatus_kInvalidArgument;

  if (const auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    const auto note_control_or = instrument->GetNoteControl(pitch, index);
    if (note_control_or.IsOk()) {
      *out_value = note_control_or.GetValue().get().Get();
      return BarelyStatus_kOk;
    }
    return note_control_or.GetErrorStatus();
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_IsNoteOn(BarelyMusicianHandle handle,
                                       BarelyId instrument_id, double pitch,
                                       bool* out_is_note_on) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_is_note_on) return BarelyStatus_kInvalidArgument;

  if (const auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    *out_is_note_on = instrument->IsNoteOn(pitch);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_Process(BarelyMusicianHandle handle,
                                      BarelyId instrument_id,
                                      double* output_samples,
                                      int32_t output_channel_count,
                                      int32_t output_frame_count,
                                      double timestamp) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if ((!output_samples && output_channel_count > 0 && output_frame_count) ||
      output_channel_count < 0 || output_frame_count < 0 || timestamp < 0.0) {
    return BarelyStatus_kInvalidArgument;
  }

  if (handle->engine.ProcessInstrument(instrument_id, output_samples,
                                       output_channel_count, output_frame_count,
                                       timestamp)) {
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_ResetAllControls(BarelyMusicianHandle handle,
                                               BarelyId instrument_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    instrument->ResetAllControls();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_ResetAllNoteControls(
    BarelyMusicianHandle handle, BarelyId instrument_id,
    [[maybe_unused]] double pitch) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    return instrument->ResetAllNoteControls(pitch);
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_ResetControl(BarelyMusicianHandle handle,
                                           BarelyId instrument_id,
                                           int32_t index) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (index < 0) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    return instrument->ResetControl(index);
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_ResetNoteControl(BarelyMusicianHandle handle,
                                               BarelyId instrument_id,
                                               double pitch, int32_t index) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (index < 0) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    return instrument->ResetNoteControl(pitch, index);
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_SetAllNotesOff(BarelyMusicianHandle handle,
                                             BarelyId instrument_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    instrument->SetAllNotesOff();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_SetControl(BarelyMusicianHandle handle,
                                         BarelyId instrument_id, int32_t index,
                                         double value, double slope_per_beat) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (index < 0) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    // TODO(#109): Convert `slope_per_beat` to slope per second.
    return instrument->SetControl(index, value, slope_per_beat);
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_SetControlEventCallback(
    BarelyMusicianHandle handle, BarelyId instrument_id,
    BarelyInstrument_ControlEventCallback callback, void* user_data) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    if (callback) {
      instrument->SetControlEventCallback(
          [callback, user_data](int32_t index, double value) {
            callback(index, value, user_data);
          });
    } else {
      instrument->SetControlEventCallback(nullptr);
    }
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_SetData(BarelyMusicianHandle handle,
                                      BarelyId instrument_id, const void* data,
                                      int32_t size) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (size < 0) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    instrument->SetData({static_cast<const std::byte*>(data),
                         static_cast<const std::byte*>(data) + size});
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_SetNoteControl(BarelyMusicianHandle handle,
                                             BarelyId instrument_id,
                                             double pitch, int32_t index,
                                             double value,
                                             double slope_per_beat) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (index < 0) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    // TODO(#109): Convert `slope_per_beat` to slope per second.
    return instrument->SetNoteControl(pitch, index, value, slope_per_beat);
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_SetNoteControlEventCallback(
    BarelyMusicianHandle handle, BarelyId instrument_id,
    [[maybe_unused]] BarelyInstrument_NoteControlEventCallback callback,
    [[maybe_unused]] void* user_data) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    if (callback) {
      instrument->SetNoteControlEventCallback(
          [callback, user_data](double pitch, int32_t index, double value) {
            callback(pitch, index, value, user_data);
          });
    } else {
      instrument->SetNoteControlEventCallback(nullptr);
    }
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_SetNoteOff(BarelyMusicianHandle handle,
                                         BarelyId instrument_id, double pitch) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    instrument->SetNoteOff(pitch);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_SetNoteOffEventCallback(
    BarelyMusicianHandle handle, BarelyId instrument_id,
    BarelyInstrument_NoteOffEventCallback callback, void* user_data) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    if (callback) {
      instrument->SetNoteOffEventCallback(
          [callback, user_data](double pitch) { callback(pitch, user_data); });
    } else {
      instrument->SetNoteOffEventCallback(nullptr);
    }
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_SetNoteOn(BarelyMusicianHandle handle,
                                        BarelyId instrument_id, double pitch) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    instrument->SetNoteOn(pitch);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_SetNoteOnEventCallback(
    BarelyMusicianHandle handle, BarelyId instrument_id,
    BarelyInstrument_NoteOnEventCallback callback, void* user_data) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    if (callback) {
      instrument->SetNoteOnEventCallback(
          [callback, user_data](double pitch) { callback(pitch, user_data); });
    } else {
      instrument->SetNoteOnEventCallback(nullptr);
    }
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyMusician_Create(BarelyMusicianHandle* out_handle) {
  if (!out_handle) return BarelyStatus_kInvalidArgument;

  *out_handle = new BarelyMusician();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_Destroy(BarelyMusicianHandle handle) {
  if (!handle) return BarelyStatus_kNotFound;

  delete handle;
  return BarelyStatus_kOk;
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

BarelyStatus BarelyMusician_SetTempo(BarelyMusicianHandle handle,
                                     double tempo) {
  if (!handle) return BarelyStatus_kNotFound;
  if (tempo < 0.0) return BarelyStatus_kInvalidArgument;

  handle->engine.SetTempo(tempo);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_Update(BarelyMusicianHandle handle,
                                   double timestamp) {
  if (!handle) return BarelyStatus_kNotFound;
  if (timestamp < 0.0) return BarelyStatus_kInvalidArgument;

  handle->engine.Update(timestamp);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyPerformer_Create(BarelyMusicianHandle handle, int32_t order,
                                    BarelyId* out_performer_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_performer_id) return BarelyStatus_kInvalidArgument;

  *out_performer_id = ++handle->id_counter;
  if (handle->engine.CreatePerformer(*out_performer_id, order)) {
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kInternal;
}

BarelyStatus BarelyPerformer_CreateOneOffTask(
    BarelyMusicianHandle handle, BarelyId performer_id,
    [[maybe_unused]] BarelyTaskDefinition definition, double position,
    [[maybe_unused]] void* user_data, BarelyId* out_task_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (performer_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (position < 0.0) return BarelyStatus_kInvalidArgument;
  if (!out_task_id) return BarelyStatus_kInvalidArgument;

  // TODO(#109): Reenable after API cleanup.
  // if (auto* performer = handle->engine.GetPerformer(performer_id)) {
  //   *out_task_id = ++handle->id_counter;
  //   if (performer->CreateTask(*out_task_id, definition, position,
  //                              is_one_off)) {
  //     return BarelyStatus_kOk;
  //   }
  //   return BarelyStatus_kInternal;
  // }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyPerformer_CreateTask(
    BarelyMusicianHandle handle, BarelyId performer_id,
    [[maybe_unused]] BarelyTaskDefinition definition, double position,
    [[maybe_unused]] void* user_data, BarelyId* out_task_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (performer_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (position < 0.0) return BarelyStatus_kInvalidArgument;
  if (!out_task_id) return BarelyStatus_kInvalidArgument;

  // TODO(#109): Reenable after API cleanup.
  // if (auto* performer = handle->engine.GetPerformer(performer_id)) {
  //   *out_task_id = ++handle->id_counter;
  //   if (performer->CreateTask(*out_task_id, definition, position,
  //                              is_one_off)) {
  //     return BarelyStatus_kOk;
  //   }
  //   return BarelyStatus_kInternal;
  // }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyPerformer_Destroy(BarelyMusicianHandle handle,
                                     BarelyId performer_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (performer_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (handle->engine.DestroyPerformer(performer_id)) {
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyPerformer_DestroyTask(BarelyMusicianHandle handle,
                                         BarelyId performer_id,
                                         BarelyId task_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (performer_id == BarelyId_kInvalid || task_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }

  // TODO(#109): Reenable after API cleanup.
  // if (auto* performer = handle->engine.GetPerformer(performer_id)) {
  //   if (performer->DestroyTask(task_id)) {
  //     return BarelyStatus_kOk;
  //   }
  // }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyPerformer_GetLoopBeginPosition(
    BarelyMusicianHandle handle, BarelyId performer_id,
    double* out_loop_begin_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (performer_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_loop_begin_position) return BarelyStatus_kInvalidArgument;

  // if (const auto* performer = handle->engine.GetPerformer(performer_id)) {
  //   *out_loop_begin_position = performer->GetLoopBeginPosition();
  //   return BarelyStatus_kOk;
  // }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyPerformer_GetLoopLength(BarelyMusicianHandle handle,
                                           BarelyId performer_id,
                                           double* out_loop_length) {
  if (!handle) return BarelyStatus_kNotFound;
  if (performer_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_loop_length) return BarelyStatus_kInvalidArgument;

  // if (const auto* performer = handle->engine.GetPerformer(performer_id)) {
  //   *out_loop_length = performer->GetLoopLength();
  //   return BarelyStatus_kOk;
  // }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyPerformer_GetPosition(BarelyMusicianHandle handle,
                                         BarelyId performer_id,
                                         double* out_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (performer_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_position) return BarelyStatus_kInvalidArgument;

  // if (const auto* performer = handle->engine.GetPerformer(performer_id)) {
  //   *out_position = performer->GetPosition();
  //   return BarelyStatus_kOk;
  // }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyPerformer_GetTaskPosition(BarelyMusicianHandle handle,
                                             BarelyId performer_id,
                                             BarelyId task_id,
                                             double* out_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (performer_id == BarelyId_kInvalid || task_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }
  if (!out_position) return BarelyStatus_kInvalidArgument;

  // TODO(#109): Reenable after API cleanup.
  // if (auto* performer = handle->engine.GetPerformer(performer_id)) {
  //   if (const auto* position = performer->GetTaskPosition(task_id)) {
  //     *out_position = *position;
  //     return BarelyStatus_kOk;
  //   }
  // }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyPerformer_IsLooping(BarelyMusicianHandle handle,
                                       BarelyId performer_id,
                                       bool* out_is_looping) {
  if (!handle) return BarelyStatus_kNotFound;
  if (performer_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_is_looping) return BarelyStatus_kInvalidArgument;

  // if (const auto* performer = handle->engine.GetPerformer(performer_id)) {
  //   *out_is_looping = performer->IsLooping();
  //   return BarelyStatus_kOk;
  // }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyPerformer_IsPlaying(BarelyMusicianHandle handle,
                                       BarelyId performer_id,
                                       bool* out_is_playing) {
  if (!handle) return BarelyStatus_kNotFound;
  if (performer_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_is_playing) return BarelyStatus_kInvalidArgument;

  // if (const auto* performer = handle->engine.GetPerformer(performer_id)) {
  //   *out_is_playing = performer->IsPlaying();
  //   return BarelyStatus_kOk;
  // }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyPerformer_SetLoopBeginPosition(
    BarelyMusicianHandle handle, BarelyId performer_id,
    [[maybe_unused]] double loop_begin_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (performer_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  // if (auto* performer = handle->engine.GetPerformer(performer_id)) {
  //   performer->SetLoopBeginPosition(loop_begin_position);
  //   return BarelyStatus_kOk;
  // }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyPerformer_SetLoopLength(BarelyMusicianHandle handle,
                                           BarelyId performer_id,
                                           double loop_length) {
  if (!handle) return BarelyStatus_kNotFound;
  if (performer_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (loop_length < 0.0) return BarelyStatus_kInvalidArgument;

  // if (auto* performer = handle->engine.GetPerformer(performer_id)) {
  //   performer->SetLoopLength(loop_length);
  //   return BarelyStatus_kOk;
  // }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyPerformer_SetLooping(BarelyMusicianHandle handle,
                                        BarelyId performer_id,
                                        [[maybe_unused]] bool is_looping) {
  if (!handle) return BarelyStatus_kNotFound;
  if (performer_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  // if (auto* performer = handle->engine.GetPerformer(performer_id)) {
  //   performer->SetLooping(is_looping);
  //   return BarelyStatus_kOk;
  // }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyPerformer_SetPosition(BarelyMusicianHandle handle,
                                         BarelyId performer_id,
                                         [[maybe_unused]] double position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (performer_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  // if (auto* performer = handle->engine.GetPerformer(performer_id)) {
  //   performer->SetPosition(position);
  //   return BarelyStatus_kOk;
  // }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyPerformer_SetTaskPosition(BarelyMusicianHandle handle,
                                             BarelyId performer_id,
                                             BarelyId task_id,
                                             [[maybe_unused]] double position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (performer_id == BarelyId_kInvalid || task_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }

  // TODO(#109): Reenable after API cleanup.
  // if (auto* performer = handle->engine.GetPerformer(performer_id)) {
  //   if (performer->SetTaskPosition(task_id, position)) {
  //     return BarelyStatus_kOk;
  //   }
  // }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyPerformer_Start(BarelyMusicianHandle handle,
                                   BarelyId performer_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (performer_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  // if (auto* performer = handle->engine.GetPerformer(performer_id)) {
  //   performer->Start();
  //   return BarelyStatus_kOk;
  // }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyPerformer_Stop(BarelyMusicianHandle handle,
                                  BarelyId performer_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (performer_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  // if (auto* performer = handle->engine.GetPerformer(performer_id)) {
  //   performer->Stop();
  //   return BarelyStatus_kOk;
  // }
  return BarelyStatus_kNotFound;
}

}  // extern "C"
