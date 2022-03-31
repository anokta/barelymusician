#include "barelymusician/barelymusician.h"

#include <stdbool.h>  // NOLINT(modernize-deprecated-headers)
#include <stdint.h>   // NOLINT(modernize-deprecated-headers)

#include <algorithm>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "barelymusician/engine/engine.h"
#include "barelymusician/instruments/percussion_instrument.h"
#include "barelymusician/instruments/synth_instrument.h"

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

  // Musician engine.
  barelyapi::Engine engine;

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

  *out_instrument_id = handle->engine.CreateInstrument(definition, frame_rate);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_CreateOfType(BarelyMusicianHandle handle,
                                           BarelyInstrumentType type,
                                           int32_t frame_rate,
                                           BarelyId* out_instrument_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_instrument_id) return BarelyStatus_kInvalidArgument;

  switch (type) {
    case BarelyInstrumentType_kSynth:
      *out_instrument_id = handle->engine.CreateInstrument(
          barelyapi::SynthInstrument::GetDefinition(), frame_rate);
      break;
    case BarelyInstrumentType_kPercussion:
      *out_instrument_id = handle->engine.CreateInstrument(
          barelyapi::PercussionInstrument::GetDefinition(), frame_rate);
      break;
    default:
      return BarelyStatus_kInvalidArgument;
  }
  return BarelyStatus_kOk;
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

  if (auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    if (instrument->ResetParameter(index, handle->engine.GetTimestamp())) {
      return BarelyStatus_kOk;
    }
    return BarelyStatus_kInvalidArgument;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_SetData(BarelyMusicianHandle handle,
                                      BarelyId instrument_id,
                                      BarelyDataDefinition definition) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    instrument->SetData(definition, handle->engine.GetTimestamp());
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

  if (auto* instrument = handle->engine.GetInstrument(instrument_id)) {
    if (instrument->SetParameter(index, value, 0.0,
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

BarelyStatus BarelyMusician_GetNote(BarelyMusicianHandle handle,
                                    BarelyNotePitchDefinition definition,
                                    double* out_pitch) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_pitch) return BarelyStatus_kInvalidArgument;

  *out_pitch = handle->engine.GetConductor().GetNote(definition);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_GetPosition(BarelyMusicianHandle handle,
                                        double* out_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_position) return BarelyStatus_kInvalidArgument;

  *out_position = handle->engine.GetTransport().GetPosition();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_GetRootNote(BarelyMusicianHandle handle,
                                        double* out_root_pitch) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_root_pitch) return BarelyStatus_kInvalidArgument;

  *out_root_pitch = handle->engine.GetConductor().GetRootNote();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_GetScale(BarelyMusicianHandle handle,
                                     const double** out_scale_pitches,
                                     int32_t* out_num_scale_pitches) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_scale_pitches) return BarelyStatus_kInvalidArgument;
  if (!out_num_scale_pitches) return BarelyStatus_kInvalidArgument;

  const auto& scale_pitches = handle->engine.GetConductor().GetScale();
  *out_scale_pitches = scale_pitches.data();
  *out_num_scale_pitches = static_cast<int>(scale_pitches.size());
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_GetTempo(BarelyMusicianHandle handle,
                                     double* out_tempo) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_tempo) return BarelyStatus_kInvalidArgument;

  *out_tempo = handle->engine.GetPlaybackTempo();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_GetTimestamp(BarelyMusicianHandle handle,
                                         double* out_timestamp) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_timestamp) return BarelyStatus_kInvalidArgument;

  *out_timestamp = handle->engine.GetTimestamp();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_GetTimestampAtPosition(BarelyMusicianHandle handle,
                                                   double position,
                                                   double* out_timestamp) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_timestamp) return BarelyStatus_kInvalidArgument;

  *out_timestamp = handle->engine.GetTransport().GetTimestamp(position);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_IsPlaying(BarelyMusicianHandle handle,
                                      bool* out_is_playing) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_is_playing) return BarelyStatus_kInvalidArgument;

  *out_is_playing = handle->engine.GetTransport().IsPlaying();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_SetAdjustNoteDefinitionCallback(
    BarelyMusicianHandle handle,
    BarelyMusician_AdjustNoteDefinitionCallback callback, void* user_data) {
  if (!handle) return BarelyStatus_kNotFound;

  if (callback) {
    handle->engine.GetConductor().SetAdjustNoteDefinitionCallback(
        [callback, user_data](barelyapi::Note::Definition* definition) {
          callback(definition, user_data);
        });
  } else {
    handle->engine.GetConductor().SetAdjustNoteDefinitionCallback(nullptr);
  }
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_SetAdjustParameterAutomationDefinitionCallback(
    BarelyMusicianHandle handle,
    BarelyMusician_AdjustParameterAutomationDefinitionCallback callback,
    void* user_data) {
  if (!handle) return BarelyStatus_kNotFound;

  callback;
  user_data;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyMusician_SetBeatCallback(
    BarelyMusicianHandle handle, BarelyMusician_BeatCallback callback,
    void* user_data) {
  if (!handle) return BarelyStatus_kNotFound;

  if (callback) {
    handle->engine.GetTransport().SetBeatCallback(
        [callback, user_data](double position, double timestamp) {
          callback(position, timestamp, user_data);
        });
  } else {
    handle->engine.GetTransport().SetBeatCallback(nullptr);
  }
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_SetPosition(BarelyMusicianHandle handle,
                                        double position) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->engine.GetTransport().SetPosition(position);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_SetRootNote(BarelyMusicianHandle handle,
                                        double root_pitch) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->engine.GetConductor().SetRootNote(root_pitch);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_SetScale(BarelyMusicianHandle handle,
                                     const double* scale_pitches,
                                     int32_t num_scale_pitches) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->engine.GetConductor().SetScale(
      std::vector<double>(scale_pitches, scale_pitches + num_scale_pitches));
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_SetTempo(BarelyMusicianHandle handle,
                                     double tempo) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->engine.SetPlaybackTempo(tempo);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_SetTimestamp(BarelyMusicianHandle handle,
                                         double timestamp) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->engine.GetTransport().SetTimestamp(timestamp);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_Start(BarelyMusicianHandle handle) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->engine.StartPlayback();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_Stop(BarelyMusicianHandle handle) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->engine.StopPlayback();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_Update(BarelyMusicianHandle handle,
                                   double timestamp) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->engine.Update(timestamp);
  return BarelyStatus_kOk;
}

BarelyStatus BarelySequence_AddNote(BarelyMusicianHandle handle,
                                    BarelyId sequence_id,
                                    BarelyNoteDefinition definition,
                                    double position, BarelyId* out_note_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_note_id) return BarelyStatus_kInvalidArgument;

  if (auto* sequence = handle->engine.GetSequence(sequence_id)) {
    *out_note_id =
        handle->engine.AddSequenceNote(sequence, definition, position);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_AddParameterAutomation(
    BarelyMusicianHandle handle, BarelyId sequence_id,
    BarelyParameterAutomationDefinition definition, double position,
    BarelyId* out_note_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_note_id) return BarelyStatus_kInvalidArgument;

  definition, position;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_Create(BarelyMusicianHandle handle,
                                   BarelyId* out_sequence_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_sequence_id) return BarelyStatus_kInvalidArgument;

  *out_sequence_id = handle->engine.CreateSequence();
  return BarelyStatus_kOk;
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

  // TODO: This should ideally be able to return false when `kNotFound`.
  *out_instrument_id = handle->engine.GetSequenceInstrumentId(sequence_id);
  return BarelyStatus_kOk;
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

BarelyStatus BarelySequence_GetNoteDefinition(
    BarelyMusicianHandle handle, BarelyId sequence_id, BarelyId note_id,
    BarelyNoteDefinition* out_definition) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid || note_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }
  if (!out_definition) return BarelyStatus_kInvalidArgument;

  if (const auto* sequence = handle->engine.GetSequence(sequence_id)) {
    if (const auto* definition = sequence->GetNoteDefinition(note_id)) {
      *out_definition = *definition;
      return BarelyStatus_kOk;
    }
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_GetNotePosition(BarelyMusicianHandle handle,
                                            BarelyId sequence_id,
                                            BarelyId note_id,
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

BarelyStatus BarelySequence_GetParameterAutomationDefinition(
    BarelyMusicianHandle handle, BarelyId sequence_id,
    BarelyId parameter_automation_id,
    BarelyParameterAutomationDefinition* out_definition) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid ||
      parameter_automation_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }
  if (!out_definition) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_GetParameterAutomationPosition(
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

BarelyStatus BarelySequence_IsEmpty(BarelyMusicianHandle handle,
                                    BarelyId sequence_id, bool* out_is_empty) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_is_empty) return BarelyStatus_kInvalidArgument;

  if (const auto* sequence = handle->engine.GetSequence(sequence_id)) {
    *out_is_empty = sequence->IsEmpty();
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

BarelyStatus BarelySequence_IsSkippingAdjustments(
    BarelyMusicianHandle handle, BarelyId sequence_id,
    bool* out_is_skipping_adjustments) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_is_skipping_adjustments) return BarelyStatus_kInvalidArgument;

  if (const auto* sequence = handle->engine.GetSequence(sequence_id)) {
    *out_is_skipping_adjustments = sequence->IsSkippingAdjustments();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_RemoveAllNotes(BarelyMusicianHandle handle,
                                           BarelyId sequence_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* sequence = handle->engine.GetSequence(sequence_id)) {
    sequence->RemoveAllNotes();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_RemoveAllNotesAtPosition(
    BarelyMusicianHandle handle, BarelyId sequence_id, double position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* sequence = handle->engine.GetSequence(sequence_id)) {
    sequence->RemoveAllNotes(position);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_RemoveAllNotesAtRange(BarelyMusicianHandle handle,
                                                  BarelyId sequence_id,
                                                  double begin_position,
                                                  double end_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* sequence = handle->engine.GetSequence(sequence_id)) {
    sequence->RemoveAllNotes(begin_position, end_position);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_RemoveAllParameterAutomations(
    BarelyMusicianHandle handle, BarelyId sequence_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_RemoveAllParameterAutomationsAtPosition(
    BarelyMusicianHandle handle, BarelyId sequence_id, double /*position*/) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_RemoveAllParameterAutomationsAtRange(
    BarelyMusicianHandle handle, BarelyId sequence_id, double begin_position,
    double end_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  begin_position, end_position;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_RemoveNote(BarelyMusicianHandle handle,
                                       BarelyId sequence_id, BarelyId note_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid || note_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }

  if (auto* sequence = handle->engine.GetSequence(sequence_id)) {
    if (sequence->RemoveNote(note_id)) {
      return BarelyStatus_kOk;
    }
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_RemoveParameterAutomation(
    BarelyMusicianHandle handle, BarelyId sequence_id,
    BarelyId parameter_automation_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid ||
      parameter_automation_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }

  return BarelyStatus_kUnimplemented;
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

BarelyStatus BarelySequence_SetNoteDefinition(BarelyMusicianHandle handle,
                                              BarelyId sequence_id,
                                              BarelyId note_id,
                                              BarelyNoteDefinition definition) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid || note_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }

  if (auto* sequence = handle->engine.GetSequence(sequence_id)) {
    if (sequence->SetNoteDefinition(note_id, definition)) {
      return BarelyStatus_kOk;
    }
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_SetNotePosition(BarelyMusicianHandle handle,
                                            BarelyId sequence_id,
                                            BarelyId note_id, double position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid || note_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }

  if (auto* sequence = handle->engine.GetSequence(sequence_id)) {
    if (sequence->SetNotePosition(note_id, position)) {
      return BarelyStatus_kOk;
    }
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_SetParameterAutomationDefinition(
    BarelyMusicianHandle handle, BarelyId sequence_id,
    BarelyId parameter_automation_id,
    BarelyParameterAutomationDefinition /*definition*/) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid ||
      parameter_automation_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_SetParameterAutomationPosition(
    BarelyMusicianHandle handle, BarelyId sequence_id,
    BarelyId parameter_automation_id, double /*position*/) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid ||
      parameter_automation_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_SetSkippingAdjustments(
    BarelyMusicianHandle handle, BarelyId sequence_id,
    bool is_skipping_adjustments) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* sequence = handle->engine.GetSequence(sequence_id)) {
    sequence->SetSkippingAdjustments(is_skipping_adjustments);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

}  // extern "C"
