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

BarelyStatus BarelyInstrument_GetParamDefinition(
    BarelyMusicianHandle handle, BarelyId instrument_id, int32_t index,
    BarelyParameterDefinition* out_definition) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (index < 0) return BarelyStatus_kInvalidArgument;
  if (!out_definition) return BarelyStatus_kInvalidArgument;

  if (const auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    if (const auto* parameter = instrument->GetParameter(index)) {
      *out_definition = parameter->GetDefinition();
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
      num_output_channels < 0 || num_output_frames < 0) {
    return BarelyStatus_kInvalidArgument;
  }
  if (timestamp < 0.0) return BarelyStatus_kInvalidArgument;

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
    instrument->ResetAllParameters(handle->engine.GetClock().GetTimestamp());
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
    if (instrument->ResetParameter(index,
                                   handle->engine.GetClock().GetTimestamp())) {
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
                        handle->engine.GetClock().GetTimestamp());
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
                                           int32_t index, double value) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (index < 0) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    if (instrument->SetParameter(index, value, 0.0,
                                 handle->engine.GetClock().GetTimestamp())) {
      return BarelyStatus_kOk;
    }
    return BarelyStatus_kInvalidArgument;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_StartNote(BarelyMusicianHandle handle,
                                        BarelyId instrument_id, double pitch,
                                        double intensity) {
  return BarelyInstrument_StartNoteAt(handle, instrument_id, pitch, intensity,
                                      handle->engine.GetClock().GetTimestamp());
}

BarelyStatus BarelyInstrument_StartNoteAt(BarelyMusicianHandle handle,
                                          BarelyId instrument_id, double pitch,
                                          double intensity, double timestamp) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (timestamp < 0.0) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    instrument->StartNote(pitch, intensity, timestamp);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_StopAllNotes(BarelyMusicianHandle handle,
                                           BarelyId instrument_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    instrument->StopAllNotes(handle->engine.GetClock().GetTimestamp());
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_StopNote(BarelyMusicianHandle handle,
                                       BarelyId instrument_id, double pitch) {
  return BarelyInstrument_StopNoteAt(handle, instrument_id, pitch,
                                     handle->engine.GetClock().GetTimestamp());
}

BarelyStatus BarelyInstrument_StopNoteAt(BarelyMusicianHandle handle,
                                         BarelyId instrument_id, double pitch,
                                         double timestamp) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (timestamp < 0.0) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    instrument->StopNote(pitch, timestamp);
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

BarelyStatus BarelyMusician_GetPosition(BarelyMusicianHandle handle,
                                        double* out_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_position) return BarelyStatus_kInvalidArgument;

  *out_position = handle->engine.GetTransport().GetPosition();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_GetTempo(BarelyMusicianHandle handle,
                                     double* out_tempo) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_tempo) return BarelyStatus_kInvalidArgument;

  *out_tempo = handle->engine.GetClock().GetTempo();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_GetTimestamp(BarelyMusicianHandle handle,
                                         double* out_timestamp) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_timestamp) return BarelyStatus_kInvalidArgument;

  *out_timestamp = handle->engine.GetClock().GetTimestamp();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_IsPlaying(BarelyMusicianHandle handle,
                                      bool* out_is_playing) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_is_playing) return BarelyStatus_kInvalidArgument;

  *out_is_playing = handle->engine.GetTransport().IsPlaying();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_SetBeatCallback(
    BarelyMusicianHandle handle, BarelyMusician_BeatCallback callback,
    void* user_data) {
  if (!handle) return BarelyStatus_kNotFound;

  if (callback) {
    handle->engine.SetBeatCallback(
        [callback, user_data](double position, double timestamp) {
          callback(position, timestamp, user_data);
        });
  } else {
    handle->engine.SetBeatCallback(nullptr);
  }
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_SetPosition(BarelyMusicianHandle handle,
                                        double position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (position < 0.0) return BarelyStatus_kInvalidArgument;

  handle->engine.GetTransport().SetPosition(position);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_SetTempo(BarelyMusicianHandle handle,
                                     double tempo) {
  if (!handle) return BarelyStatus_kNotFound;
  if (tempo < 0.0) return BarelyStatus_kInvalidArgument;

  handle->engine.GetClock().SetTempo(tempo);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_SetTimestamp(BarelyMusicianHandle handle,
                                         double timestamp) {
  if (!handle) return BarelyStatus_kNotFound;
  if (timestamp < 0.0) return BarelyStatus_kInvalidArgument;

  handle->engine.GetClock().SetTimestamp(timestamp);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_Start(BarelyMusicianHandle handle) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->engine.Start();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_Stop(BarelyMusicianHandle handle) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->engine.Stop();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_Update(BarelyMusicianHandle handle,
                                   double timestamp) {
  if (!handle) return BarelyStatus_kNotFound;
  if (timestamp < 0.0) return BarelyStatus_kInvalidArgument;

  handle->engine.Update(timestamp);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyNote_Create(BarelyMusicianHandle handle,
                               BarelyId sequence_id, double position,
                               double duration, double pitch, double intensity,
                               BarelyId* out_note_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (position < 0.0 || duration < 0.0) return BarelyStatus_kInvalidArgument;
  if (!out_note_id) return BarelyStatus_kInvalidArgument;

  if (auto* sequence = handle->engine.GetSequence(sequence_id)) {
    *out_note_id = ++handle->id_counter;
    if (sequence->CreateNote(*out_note_id, position,
                             {duration, pitch, intensity})) {
      return BarelyStatus_kOk;
    }
    return BarelyStatus_kAlreadyExists;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyNote_Destroy(BarelyMusicianHandle handle,
                                BarelyId sequence_id, BarelyId note_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid || note_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }

  if (auto* sequence = handle->engine.GetSequence(sequence_id)) {
    if (sequence->DestroyNote(note_id)) {
      return BarelyStatus_kOk;
    }
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyNote_GetDuration(BarelyMusicianHandle handle,
                                    BarelyId sequence_id, BarelyId note_id,
                                    double* out_duration) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid || note_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }
  if (!out_duration) return BarelyStatus_kInvalidArgument;

  if (const auto* sequence = handle->engine.GetSequence(sequence_id)) {
    if (const auto* note = sequence->GetNote(note_id)) {
      *out_duration = note->duration;
      return BarelyStatus_kOk;
    }
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyNote_GetIntensity(BarelyMusicianHandle handle,
                                     BarelyId sequence_id, BarelyId note_id,
                                     double* out_intensity) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid || note_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }
  if (!out_intensity) return BarelyStatus_kInvalidArgument;

  if (const auto* sequence = handle->engine.GetSequence(sequence_id)) {
    if (const auto* note = sequence->GetNote(note_id)) {
      *out_intensity = note->intensity;
      return BarelyStatus_kOk;
    }
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyNote_GetPitch(BarelyMusicianHandle handle,
                                 BarelyId sequence_id, BarelyId note_id,
                                 double* out_pitch) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid || note_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }
  if (!out_pitch) return BarelyStatus_kInvalidArgument;

  if (const auto* sequence = handle->engine.GetSequence(sequence_id)) {
    if (const auto* note = sequence->GetNote(note_id)) {
      *out_pitch = note->pitch;
      return BarelyStatus_kOk;
    }
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyNote_GetPosition(BarelyMusicianHandle handle,
                                    BarelyId sequence_id, BarelyId note_id,
                                    double* out_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid || note_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }
  if (!out_position) return BarelyStatus_kInvalidArgument;

  if (const auto* sequence = handle->engine.GetSequence(sequence_id)) {
    if (const auto* position = sequence->GetNotePosition(note_id)) {
      *out_position = *position;
      return BarelyStatus_kOk;
    }
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyNote_SetDuration(BarelyMusicianHandle handle,
                                    BarelyId sequence_id, BarelyId note_id,
                                    double duration) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid || note_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }
  if (duration < 0.0) return BarelyStatus_kInvalidArgument;

  if (auto* sequence = handle->engine.GetSequence(sequence_id)) {
    if (auto* note = sequence->GetNote(note_id)) {
      note->duration = duration;
      return BarelyStatus_kOk;
    }
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyNote_SetIntensity(BarelyMusicianHandle handle,
                                     BarelyId sequence_id, BarelyId note_id,
                                     double intensity) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid || note_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }

  if (auto* sequence = handle->engine.GetSequence(sequence_id)) {
    if (auto* note = sequence->GetNote(note_id)) {
      note->intensity = intensity;
      return BarelyStatus_kOk;
    }
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyNote_SetPitch(BarelyMusicianHandle handle,
                                 BarelyId sequence_id, BarelyId note_id,
                                 double pitch) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid || note_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }

  if (auto* sequence = handle->engine.GetSequence(sequence_id)) {
    if (auto* note = sequence->GetNote(note_id)) {
      note->pitch = pitch;
      return BarelyStatus_kOk;
    }
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyNote_SetPosition(BarelyMusicianHandle handle,
                                    BarelyId sequence_id, BarelyId note_id,
                                    double position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid || note_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }
  if (position < 0.0) return BarelyStatus_kInvalidArgument;

  if (auto* sequence = handle->engine.GetSequence(sequence_id)) {
    if (sequence->SetNotePosition(note_id, position)) {
      return BarelyStatus_kOk;
    }
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyParameterAutomation_Create(
    BarelyMusicianHandle handle, BarelyId sequence_id, double position,
    int32_t index, [[maybe_unused]] double value,
    BarelyId* out_parameter_automation_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (position < 0.0 || index < 0) return BarelyStatus_kInvalidArgument;
  if (!out_parameter_automation_id) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyParameterAutomation_Destroy(
    BarelyMusicianHandle handle, BarelyId sequence_id,
    BarelyId parameter_automation_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid ||
      parameter_automation_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyParameterAutomation_GetIndex(
    BarelyMusicianHandle handle, BarelyId sequence_id,
    BarelyId parameter_automation_id, int32_t* out_index) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid ||
      parameter_automation_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }
  if (!out_index) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}
BarelyStatus BarelyParameterAutomation_GetPosition(
    BarelyMusicianHandle handle, BarelyId sequence_id,
    BarelyId parameter_automation_id, double* out_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid ||
      parameter_automation_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }
  if (!out_position) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyParameterAutomation_GetValue(
    BarelyMusicianHandle handle, BarelyId sequence_id,
    BarelyId parameter_automation_id, double* out_value) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid ||
      parameter_automation_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }
  if (!out_value) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyParameterAutomation_SetIndex(
    BarelyMusicianHandle handle, BarelyId sequence_id,
    BarelyId parameter_automation_id, int32_t index) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid ||
      parameter_automation_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }
  if (index < 0) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyParameterAutomation_SetPosition(
    BarelyMusicianHandle handle, BarelyId sequence_id,
    BarelyId parameter_automation_id, double position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid ||
      parameter_automation_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }
  if (position < 0.0) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyParameterAutomation_SetValue(
    BarelyMusicianHandle handle, BarelyId sequence_id,
    BarelyId parameter_automation_id, [[maybe_unused]] double value) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid ||
      parameter_automation_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_Create(BarelyMusicianHandle handle,
                                   BarelyId* out_sequence_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_sequence_id) return BarelyStatus_kInvalidArgument;

  *out_sequence_id = ++handle->id_counter;
  if (handle->engine.CreateSequence(*out_sequence_id)) {
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kAlreadyExists;
}

BarelyStatus BarelySequence_Destroy(BarelyMusicianHandle handle,
                                    BarelyId sequence_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (handle->engine.DestroySequence(sequence_id)) {
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_GetBeginOffset(BarelyMusicianHandle handle,
                                           BarelyId sequence_id,
                                           double* out_begin_offset) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_begin_offset) return BarelyStatus_kInvalidArgument;

  if (const auto* sequence = handle->engine.GetSequence(sequence_id)) {
    *out_begin_offset = sequence->GetBeginOffset();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_GetBeginPosition(BarelyMusicianHandle handle,
                                             BarelyId sequence_id,
                                             double* out_begin_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_begin_position) return BarelyStatus_kInvalidArgument;

  if (const auto* sequence = handle->engine.GetSequence(sequence_id)) {
    *out_begin_position = sequence->GetBeginPosition();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_GetEndPosition(BarelyMusicianHandle handle,
                                           BarelyId sequence_id,
                                           double* out_end_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_end_position) return BarelyStatus_kInvalidArgument;

  if (const auto* sequence = handle->engine.GetSequence(sequence_id)) {
    *out_end_position = sequence->GetEndPosition();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_GetInstrument(BarelyMusicianHandle handle,
                                          BarelyId sequence_id,
                                          BarelyId* out_instrument_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_instrument_id) return BarelyStatus_kInvalidArgument;

  if (const auto* sequence_instrument_id =
          handle->engine.GetSequenceInstrumentId(sequence_id)) {
    *out_instrument_id = *sequence_instrument_id;
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_GetLoopBeginOffset(BarelyMusicianHandle handle,
                                               BarelyId sequence_id,
                                               double* out_loop_begin_offset) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_loop_begin_offset) return BarelyStatus_kInvalidArgument;

  if (const auto* sequence = handle->engine.GetSequence(sequence_id)) {
    *out_loop_begin_offset = sequence->GetLoopBeginOffset();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_GetLoopLength(BarelyMusicianHandle handle,
                                          BarelyId sequence_id,
                                          double* out_loop_length) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_loop_length) return BarelyStatus_kInvalidArgument;

  if (const auto* sequence = handle->engine.GetSequence(sequence_id)) {
    *out_loop_length = sequence->GetLoopLength();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_IsLooping(BarelyMusicianHandle handle,
                                      BarelyId sequence_id,
                                      bool* out_is_looping) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_is_looping) return BarelyStatus_kInvalidArgument;

  if (const auto* sequence = handle->engine.GetSequence(sequence_id)) {
    *out_is_looping = sequence->IsLooping();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_SetBeginOffset(BarelyMusicianHandle handle,
                                           BarelyId sequence_id,
                                           double begin_offset) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* sequence = handle->engine.GetSequence(sequence_id)) {
    sequence->SetBeginOffset(begin_offset);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_SetBeginPosition(BarelyMusicianHandle handle,
                                             BarelyId sequence_id,
                                             double begin_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (begin_position < 0.0) return BarelyStatus_kInvalidArgument;

  if (auto* sequence = handle->engine.GetSequence(sequence_id)) {
    sequence->SetBeginPosition(begin_position);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_SetEndPosition(BarelyMusicianHandle handle,
                                           BarelyId sequence_id,
                                           double end_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (end_position < 0.0) return BarelyStatus_kInvalidArgument;

  if (auto* sequence = handle->engine.GetSequence(sequence_id)) {
    sequence->SetEndPosition(end_position);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_SetInstrument(BarelyMusicianHandle handle,
                                          BarelyId sequence_id,
                                          BarelyId instrument_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (handle->engine.SetSequenceInstrumentId(sequence_id, instrument_id)) {
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_SetLoopBeginOffset(BarelyMusicianHandle handle,
                                               BarelyId sequence_id,
                                               double loop_begin_offset) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* sequence = handle->engine.GetSequence(sequence_id)) {
    sequence->SetLoopBeginOffset(loop_begin_offset);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_SetLoopLength(BarelyMusicianHandle handle,
                                          BarelyId sequence_id,
                                          double loop_length) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (loop_length < 0.0) return BarelyStatus_kInvalidArgument;

  if (auto* sequence = handle->engine.GetSequence(sequence_id)) {
    sequence->SetLoopLength(loop_length);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_SetLooping(BarelyMusicianHandle handle,
                                       BarelyId sequence_id, bool is_looping) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* sequence = handle->engine.GetSequence(sequence_id)) {
    sequence->SetLooping(is_looping);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

}  // extern "C"
