#include "barelymusician/barelymusician.h"

#include <stdbool.h>  // NOLINT(modernize-deprecated-headers)
#include <stdint.h>   // NOLINT(modernize-deprecated-headers)

#include <cstddef>

#include "barelymusician/engine/engine.h"

extern "C" {

/// Musician.
struct BarelyMusician {
  // Default constructor.
  BarelyMusician() = default;

  // Non-copyable and non-movable.
  BarelyMusician(const BarelyMusician& other) = delete;
  BarelyMusician& operator=(const BarelyMusician& other) = delete;
  BarelyMusician(BarelyMusician&& other) noexcept = delete;
  BarelyMusician& operator=(BarelyMusician&& other) noexcept = delete;

  // Engine.
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
  return BarelyStatus_kAlreadyExists;
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

BarelyStatus BarelyInstrument_GetParameter(BarelyMusicianHandle handle,
                                           BarelyId instrument_id,
                                           int32_t index, double* out_value) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (index < 0) return BarelyStatus_kInvalidArgument;
  if (!out_value) return BarelyStatus_kInvalidArgument;

  if (const auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    if (const auto* parameter = instrument->GetParameter(index)) {
      *out_value = parameter->GetValue();
      return BarelyStatus_kOk;
    }
    return BarelyStatus_kInvalidArgument;
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
                                      BarelyId instrument_id, double* output,
                                      int32_t num_output_channels,
                                      int32_t num_output_frames,
                                      double timestamp) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if ((!output && num_output_channels > 0 && num_output_frames) ||
      num_output_channels < 0 || num_output_frames < 0 || timestamp < 0.0) {
    return BarelyStatus_kInvalidArgument;
  }

  if (handle->engine.ProcessInstrument(instrument_id, output,
                                       num_output_channels, num_output_frames,
                                       timestamp)) {
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_ResetAllParameters(BarelyMusicianHandle handle,
                                                 BarelyId instrument_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    instrument->ResetAllParameters(handle->engine.GetTimestamp());
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_ResetParameter(BarelyMusicianHandle handle,
                                             BarelyId instrument_id,
                                             int32_t index) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (index < 0) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    if (instrument->ResetParameter(index, handle->engine.GetTimestamp())) {
      return BarelyStatus_kOk;
    }
    return BarelyStatus_kInvalidArgument;
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
                         static_cast<const std::byte*>(data) + size},
                        handle->engine.GetTimestamp());
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_SetNoteOffCallback(
    BarelyMusicianHandle handle, BarelyId instrument_id,
    BarelyInstrument_NoteOffCallback callback, void* user_data) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    if (callback) {
      instrument->SetNoteOffCallback(
          [callback, user_data](double pitch, double timestamp) {
            callback(pitch, timestamp, user_data);
          });
    } else {
      instrument->SetNoteOffCallback(nullptr);
    }
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_SetNoteOnCallback(
    BarelyMusicianHandle handle, BarelyId instrument_id,
    BarelyInstrument_NoteOnCallback callback, void* user_data) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    if (callback) {
      instrument->SetNoteOnCallback([callback, user_data](double pitch,
                                                          double intensity,
                                                          double timestamp) {
        callback(pitch, intensity, timestamp, user_data);
      });
    } else {
      instrument->SetNoteOnCallback(nullptr);
    }
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_SetParameter(BarelyMusicianHandle handle,
                                           BarelyId instrument_id,
                                           int32_t index, double value,
                                           double slope) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (index < 0) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    if (instrument->SetParameter(index, value, slope,
                                 handle->engine.GetTimestamp())) {
      return BarelyStatus_kOk;
    }
    return BarelyStatus_kInvalidArgument;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_StartNote(BarelyMusicianHandle handle,
                                        BarelyId instrument_id, double pitch,
                                        double intensity) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    instrument->StartNote(pitch, intensity, handle->engine.GetTimestamp());
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_StopAllNotes(BarelyMusicianHandle handle,
                                           BarelyId instrument_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    instrument->StopAllNotes(handle->engine.GetTimestamp());
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_StopNote(BarelyMusicianHandle handle,
                                       BarelyId instrument_id, double pitch) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    instrument->StopNote(pitch, handle->engine.GetTimestamp());
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

BarelyStatus BarelyMusician_GetBeats(BarelyMusicianHandle handle,
                                     double seconds, double* out_beats) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_beats) return BarelyStatus_kInvalidArgument;

  *out_beats = handle->engine.GetBeats(seconds);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_GetSeconds(BarelyMusicianHandle handle,
                                       double beats, double* out_seconds) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_seconds) return BarelyStatus_kInvalidArgument;

  *out_seconds = handle->engine.GetSeconds(beats);
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

BarelyStatus BarelySequencerEvent_Create(
    BarelyMusicianHandle handle, BarelyId sequencer_id, double position,
    [[maybe_unused]] BarelySequencerEvent_Callback callback,
    BarelyId* out_event_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequencer_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (position < 0.0) return BarelyStatus_kInvalidArgument;
  if (!out_event_id) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequencerEvent_Destroy(BarelyMusicianHandle handle,
                                          BarelyId sequencer_id,
                                          BarelyId event_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequencer_id == BarelyId_kInvalid || event_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequencerEvent_GetCallback(
    BarelyMusicianHandle handle, BarelyId sequencer_id, BarelyId event_id,
    BarelySequencerEvent_Callback* out_callback) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequencer_id == BarelyId_kInvalid || event_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }
  if (!out_callback) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequencerEvent_GetPosition(BarelyMusicianHandle handle,
                                              BarelyId sequencer_id,
                                              BarelyId event_id,
                                              double* out_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequencer_id == BarelyId_kInvalid || event_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }
  if (!out_position) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequencerEvent_SetCallback(
    BarelyMusicianHandle handle, BarelyId sequencer_id, BarelyId event_id,
    [[maybe_unused]] BarelySequencerEvent_Callback callback) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequencer_id == BarelyId_kInvalid || event_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequencerEvent_SetPosition(BarelyMusicianHandle handle,
                                              BarelyId sequencer_id,
                                              BarelyId event_id,
                                              double position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequencer_id == BarelyId_kInvalid || event_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }
  if (position < 0.0) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequencer_Create(BarelyMusicianHandle handle,
                                    [[maybe_unused]] int32_t priority,
                                    [[maybe_unused]] void* user_data,
                                    BarelyId* out_sequencer_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_sequencer_id) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequencer_Destroy(BarelyMusicianHandle handle,
                                     BarelyId sequencer_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequencer_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequencer_GetLoopBeginPosition(
    BarelyMusicianHandle handle, BarelyId sequencer_id,
    double* out_loop_begin_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequencer_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_loop_begin_position) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequencer_GetLoopLength(BarelyMusicianHandle handle,
                                           BarelyId sequencer_id,
                                           double* out_loop_length) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequencer_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_loop_length) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequencer_GetPosition(BarelyMusicianHandle handle,
                                         BarelyId sequencer_id,
                                         double* out_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequencer_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_position) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequencer_IsLooping(BarelyMusicianHandle handle,
                                       BarelyId sequencer_id,
                                       bool* out_is_looping) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequencer_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_is_looping) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequencer_IsPlaying(BarelyMusicianHandle handle,
                                       BarelyId sequencer_id,
                                       bool* out_is_playing) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequencer_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_is_playing) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequencer_SetLoopBeginPosition(
    BarelyMusicianHandle handle, BarelyId sequencer_id,
    [[maybe_unused]] double loop_begin_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequencer_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequencer_SetLoopLength(
    BarelyMusicianHandle handle, BarelyId sequencer_id,
    [[maybe_unused]] double loop_length) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequencer_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequencer_SetLooping(BarelyMusicianHandle handle,
                                        BarelyId sequencer_id,
                                        [[maybe_unused]] bool is_looping) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequencer_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequencer_SetPosition(BarelyMusicianHandle handle,
                                         BarelyId sequencer_id,
                                         [[maybe_unused]] double position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequencer_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequencer_Start(BarelyMusicianHandle handle,
                                   BarelyId sequencer_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequencer_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequencer_Stop(BarelyMusicianHandle handle,
                                  BarelyId sequencer_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequencer_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequencer_TriggerOneOffEvent(
    BarelyMusicianHandle handle, BarelyId sequencer_id, double delay,
    [[maybe_unused]] BarelySequencerEvent_Callback callback) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequencer_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (delay <= 0.0) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

}  // extern "C"
