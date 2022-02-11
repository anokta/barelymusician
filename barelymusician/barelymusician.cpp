#include "barelymusician/barelymusician.h"

#include <stdbool.h>  // NOLINT(modernize-deprecated-headers)
#include <stdint.h>   // NOLINT(modernize-deprecated-headers)

#include <algorithm>
#include <vector>

#include "barelymusician/common/status.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/engine/engine.h"
#include "examples/instruments/synth_instrument.h"

namespace {

using ::barely::examples::SynthInstrument;
using ::barelyapi::Engine;
using ::barelyapi::GetStatusOrStatus;
using ::barelyapi::GetStatusOrValue;
using ::barelyapi::IsOk;
using ::barelyapi::Note;
using ::barelyapi::Status;
using ::barelyapi::StatusOr;

// Returns the corresponding `BarelyStatus` value for a given `status`.
BarelyStatus GetStatus(Status status) noexcept {
  switch (status) {
    case Status::kOk:
      return BarelyStatus_kOk;
    case Status::kInvalidArgument:
      return BarelyStatus_kInvalidArgument;
    case Status::kNotFound:
      return BarelyStatus_kNotFound;
    case Status::kAlreadyExists:
      return BarelyStatus_kAlreadyExists;
    case Status::kFailedPrecondition:
      return BarelyStatus_kFailedPrecondition;
    case Status::kUnimplemented:
      return BarelyStatus_kUnimplemented;
    case Status::kInternal:
      return BarelyStatus_kInternal;
    case Status::kUnknown:
    default:
      return BarelyStatus_kUnknown;
  }
}

// Returns the corresponding `BarelyStatus` value for a given `status_or`.
template <typename ValueType>
BarelyStatus GetStatus(const StatusOr<ValueType>& status_or) noexcept {
  return GetStatus(GetStatusOrStatus(status_or));
}

}  // namespace

extern "C" {

/// BarelyMusician C api.
struct BarelyMusician {
  /// Engine instance.
  Engine instance;

 private:
  // Ensure that the instance can only be destroyed via the api call.
  friend BARELY_EXPORT BarelyStatus BarelyMusician_Destroy(BarelyApi api);
  ~BarelyMusician() = default;
};

BarelyStatus BarelyConductor_ConductNote(BarelyApi api,
                                         BarelyNotePitchType /*pitch_type*/,
                                         float /*pitch*/,
                                         bool /*bypass_adjustment*/,
                                         float* out_pitch) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_pitch) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_GetEnergy(BarelyApi api, float* out_energy) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_energy) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_GetParameter(BarelyApi api, int32_t /*index*/,
                                          float* out_value) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_value) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_GetParameterDefinition(
    BarelyApi api, int32_t /*index*/,
    BarelyParameterDefinition* out_parameter_definition) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_parameter_definition) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_GetStress(BarelyApi api, float* out_stress) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_stress) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_ResetAllParameters(BarelyApi api) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_ResetParameter(BarelyApi api, int32_t /*index*/) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_SetData(BarelyApi api, void* /*data*/) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.x
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_SetDefinition(
    BarelyApi api, BarelyConductorDefinition /*definition*/) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_SetEnergy(BarelyApi api, float /*energy*/) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_SetParameter(BarelyApi api, int32_t /*index*/,
                                          float /*value*/) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_SetStress(BarelyApi api, float /*stress*/) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyInstrument_Create(BarelyApi api,
                                     BarelyInstrumentDefinition definition,
                                     int32_t sample_rate,
                                     BarelyId* out_instrument_id) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_instrument_id) return BarelyStatus_kInvalidArgument;

  *out_instrument_id =
      api->instance.CreateInstrument(std::move(definition), sample_rate);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_Destroy(BarelyApi api, BarelyId instrument_id) {
  if (!api) return BarelyStatus_kNotFound;

  return GetStatus(api->instance.DestroyInstrument(instrument_id));
}

BarelyStatus BarelyInstrument_GetGain(BarelyApi api, BarelyId instrument_id,
                                      float* out_gain) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_gain) return BarelyStatus_kInvalidArgument;

  const auto gain_or = api->instance.GetInstrumentGain(instrument_id);
  if (IsOk(gain_or)) {
    *out_gain = GetStatusOrValue(gain_or);
    return BarelyStatus_kOk;
  }
  return GetStatus(gain_or);
}

BarelyStatus BarelyInstrument_GetParameter(BarelyApi api,
                                           BarelyId instrument_id,
                                           int32_t index, float* out_value) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_value) return BarelyStatus_kInvalidArgument;

  const auto parameter_or =
      api->instance.GetInstrumentParameter(instrument_id, index);
  if (IsOk(parameter_or)) {
    *out_value = GetStatusOrValue(parameter_or).GetValue();
    return BarelyStatus_kOk;
  }
  return GetStatus(parameter_or);
}

BarelyStatus BarelyInstrument_GetParamDefinition(
    BarelyApi api, BarelyId instrument_id, int32_t index,
    BarelyParameterDefinition* out_parameter_definition) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_parameter_definition) return BarelyStatus_kInvalidArgument;

  const auto parameter_or =
      api->instance.GetInstrumentParameter(instrument_id, index);
  if (IsOk(parameter_or)) {
    const auto& definition = GetStatusOrValue(parameter_or).GetDefinition();
    out_parameter_definition->default_value = definition.default_value;
    out_parameter_definition->min_value = definition.min_value;
    out_parameter_definition->max_value = definition.max_value;
    return BarelyStatus_kOk;
  }
  return GetStatus(parameter_or);
}

BarelyStatus BarelyInstrument_IsMuted(BarelyApi api, BarelyId instrument_id,
                                      bool* out_is_muted) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_is_muted) return BarelyStatus_kInvalidArgument;

  const auto is_muted_or = api->instance.IsInstrumentMuted(instrument_id);
  if (IsOk(is_muted_or)) {
    *out_is_muted = GetStatusOrValue(is_muted_or);
    return BarelyStatus_kOk;
  }
  return GetStatus(is_muted_or);
}

BarelyStatus BarelyInstrument_IsNoteOn(BarelyApi api, BarelyId instrument_id,
                                       float pitch, bool* out_is_note_on) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_is_note_on) return BarelyStatus_kInvalidArgument;

  const auto is_note_on_or =
      api->instance.IsInstrumentNoteOn(instrument_id, pitch);
  if (IsOk(is_note_on_or)) {
    *out_is_note_on = GetStatusOrValue(is_note_on_or);
    return BarelyStatus_kOk;
  }
  return GetStatus(is_note_on_or);
}

BarelyStatus BarelyInstrument_Process(BarelyApi api, BarelyId instrument_id,
                                      double timestamp, float* output,
                                      int32_t num_output_channels,
                                      int32_t num_output_frames) {
  if (!api) return BarelyStatus_kNotFound;

  return GetStatus(api->instance.ProcessInstrument(instrument_id, timestamp,
                                                   output, num_output_channels,
                                                   num_output_frames));
}

BarelyStatus BarelyInstrument_ResetAllParameters(BarelyApi api,
                                                 BarelyId instrument_id) {
  if (!api) return BarelyStatus_kNotFound;

  return GetStatus(api->instance.ResetAllInstrumentParameters(instrument_id));
}

BarelyStatus BarelyInstrument_ResetParameter(BarelyApi api,
                                             BarelyId instrument_id,
                                             int32_t index) {
  if (!api) return BarelyStatus_kNotFound;

  return GetStatus(
      api->instance.ResetInstrumentParameter(instrument_id, index));
}

BarelyStatus BarelyInstrument_SetData(BarelyApi api, BarelyId instrument_id,
                                      void* data) {
  if (!api) return BarelyStatus_kNotFound;

  return GetStatus(api->instance.SetInstrumentData(instrument_id, data));
}

BarelyStatus BarelyInstrument_SetGain(BarelyApi api, BarelyId instrument_id,
                                      float gain) {
  if (!api) return BarelyStatus_kNotFound;

  return GetStatus(api->instance.SetInstrumentGain(instrument_id, gain));
}

BarelyStatus BarelyInstrument_SetMuted(BarelyApi api, BarelyId instrument_id,
                                       bool is_muted) {
  if (!api) return BarelyStatus_kNotFound;

  return GetStatus(api->instance.SetInstrumentMuted(instrument_id, is_muted));
}

BarelyStatus BarelyInstrument_SetNoteOffCallback(
    BarelyApi api, BarelyId instrument_id,
    BarelyInstrument_NoteOffCallback note_off_callback, void* user_data) {
  if (!api) return BarelyStatus_kNotFound;

  api->instance.SetInstrumentNoteOffCallback(instrument_id, note_off_callback,
                                             user_data);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_SetNoteOnCallback(
    BarelyApi api, BarelyId instrument_id,
    BarelyInstrument_NoteOnCallback note_on_callback, void* user_data) {
  if (!api) return BarelyStatus_kNotFound;

  api->instance.SetInstrumentNoteOnCallback(instrument_id, note_on_callback,
                                            user_data);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_SetParameter(BarelyApi api,
                                           BarelyId instrument_id,
                                           int32_t index, float value) {
  if (!api) return BarelyStatus_kNotFound;

  return GetStatus(
      api->instance.SetInstrumentParameter(instrument_id, index, value));
}

BarelyStatus BarelyInstrument_StartNote(BarelyApi api, BarelyId instrument_id,
                                        float pitch, float intensity) {
  if (!api) return BarelyStatus_kNotFound;

  return GetStatus(
      api->instance.StartInstrumentNote(instrument_id, pitch, intensity));
}

BarelyStatus BarelyInstrument_StopAllNotes(BarelyApi api,
                                           BarelyId instrument_id) {
  if (!api) return BarelyStatus_kNotFound;

  return GetStatus(api->instance.StopAllInstrumentNotes(instrument_id));
}

BarelyStatus BarelyInstrument_StopNote(BarelyApi api, BarelyId instrument_id,
                                       float pitch) {
  if (!api) return BarelyStatus_kNotFound;

  return GetStatus(api->instance.StopInstrumentNote(instrument_id, pitch));
}

BarelyStatus BarelyMusician_Create(BarelyApi* out_api) {
  if (!out_api) return BarelyStatus_kInvalidArgument;

  *out_api = new BarelyMusician();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_Destroy(BarelyApi api) {
  if (!api) return BarelyStatus_kNotFound;

  delete api;
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_GetPosition(BarelyApi api, double* out_position) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_position) return BarelyStatus_kInvalidArgument;

  *out_position = api->instance.GetPlaybackPosition();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_GetRootNote(BarelyApi api, float* out_root_pitch) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_root_pitch) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyMusician_GetScale(BarelyApi api, float** out_scale_pitches,
                                     int32_t* out_num_scale_pitches) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_scale_pitches) return BarelyStatus_kInvalidArgument;
  if (!out_num_scale_pitches) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyMusician_GetTempo(BarelyApi api, double* out_tempo) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_tempo) return BarelyStatus_kInvalidArgument;

  *out_tempo = api->instance.GetPlaybackTempo();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_IsPlaying(BarelyApi api, bool* out_is_playing) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_is_playing) return BarelyStatus_kInvalidArgument;

  *out_is_playing = api->instance.IsPlaying();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_SetBeatCallback(
    BarelyApi api, BarelyMusician_BeatCallback beat_callback, void* user_data) {
  if (!api) return BarelyStatus_kNotFound;

  if (beat_callback) {
    api->instance.SetPlaybackBeatCallback(
        [beat_callback, user_data](double position, double timestamp) {
          beat_callback(position, timestamp, user_data);
        });
  } else {
    api->instance.SetPlaybackBeatCallback(nullptr);
  }
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyMusician_SetPosition(BarelyApi api, double position) {
  if (!api) return BarelyStatus_kNotFound;

  api->instance.SetPlaybackPosition(position);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_SetRootNote(BarelyApi api, float /*root_pitch*/) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyMusician_SetScale(BarelyApi api, float* /*scale_pitches*/,
                                     int32_t /*num_scale_pitches*/) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyMusician_SetTempo(BarelyApi api, double tempo) {
  if (!api) return BarelyStatus_kNotFound;

  api->instance.SetPlaybackTempo(tempo);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_Start(BarelyApi api) {
  if (!api) return BarelyStatus_kNotFound;

  api->instance.StartPlayback();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_Stop(BarelyApi api) {
  if (!api) return BarelyStatus_kNotFound;

  api->instance.StopPlayback();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_Update(BarelyApi api, double timestamp) {
  if (!api) return BarelyStatus_kNotFound;

  api->instance.Update(timestamp);
  return BarelyStatus_kOk;
}

BarelyStatus BarelySequence_AddNote(BarelyApi api, BarelyId sequence_id,
                                    double position,
                                    BarelyNoteDefinition definition,
                                    BarelyId* out_note_id) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_note_id) return BarelyStatus_kInvalidArgument;

  const auto note_id_or =
      api->instance.AddPerformerNote(sequence_id, position,
                                     Note{.pitch = definition.pitch,
                                          .intensity = definition.intensity,
                                          .duration = definition.duration});
  if (IsOk(note_id_or)) {
    *out_note_id = GetStatusOrValue(note_id_or);
    return BarelyStatus_kOk;
  }
  return GetStatus(note_id_or);
}

BarelyStatus BarelySequence_Create(BarelyApi api, BarelyId* out_sequence_id) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_sequence_id) return BarelyStatus_kInvalidArgument;

  *out_sequence_id = api->instance.AddPerformer();
  return BarelyStatus_kOk;
}

BarelyStatus BarelySequence_Destroy(BarelyApi api, BarelyId sequence_id) {
  if (!api) return BarelyStatus_kNotFound;

  return GetStatus(api->instance.RemovePerformer(sequence_id));
}

BarelyStatus BarelySequence_GetBeginOffset(BarelyApi api, BarelyId sequence_id,
                                           double* out_begin_offset) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_begin_offset) return BarelyStatus_kInvalidArgument;

  const auto begin_offset_or =
      api->instance.GetPerformerBeginOffset(sequence_id);
  if (IsOk(begin_offset_or)) {
    *out_begin_offset = GetStatusOrValue(begin_offset_or);
    return BarelyStatus_kOk;
  }
  return GetStatus(begin_offset_or);
}

BarelyStatus BarelySequence_GetBeginPosition(BarelyApi api,
                                             BarelyId sequence_id,
                                             double* out_begin_position) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_begin_position) return BarelyStatus_kInvalidArgument;

  const auto begin_position_or =
      api->instance.GetPerformerBeginPosition(sequence_id);
  if (IsOk(begin_position_or)) {
    *out_begin_position = GetStatusOrValue(begin_position_or);
    return BarelyStatus_kOk;
  }
  return GetStatus(begin_position_or);
}

BarelyStatus BarelySequence_GetEndPosition(BarelyApi api, BarelyId sequence_id,
                                           double* out_end_position) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_end_position) return BarelyStatus_kInvalidArgument;

  const auto end_position_or =
      api->instance.GetPerformerEndPosition(sequence_id);
  if (IsOk(end_position_or)) {
    *out_end_position = GetStatusOrValue(end_position_or);
    return BarelyStatus_kOk;
  }
  return GetStatus(end_position_or);
}

BarelyStatus BarelySequence_GetInstrument(BarelyApi api,
                                          BarelyId /*sequence_id*/,
                                          BarelyId* out_instrument_id) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_instrument_id) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_GetLoopBeginOffset(BarelyApi api,
                                               BarelyId sequence_id,
                                               double* out_loop_begin_offset) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_loop_begin_offset) return BarelyStatus_kInvalidArgument;

  const auto loop_begin_offset_or =
      api->instance.GetPerformerLoopBeginOffset(sequence_id);
  if (IsOk(loop_begin_offset_or)) {
    *out_loop_begin_offset = GetStatusOrValue(loop_begin_offset_or);
    return BarelyStatus_kOk;
  }
  return GetStatus(loop_begin_offset_or);
}

BarelyStatus BarelySequence_GetLoopLength(BarelyApi api, BarelyId sequence_id,
                                          double* out_loop_length) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_loop_length) return BarelyStatus_kInvalidArgument;

  const auto loop_length_or = api->instance.GetPerformerLoopLength(sequence_id);
  if (IsOk(loop_length_or)) {
    *out_loop_length = GetStatusOrValue(loop_length_or);
    return BarelyStatus_kOk;
  }
  return GetStatus(loop_length_or);
}

BarelyStatus BarelySequence_GetNoteDefinition(
    BarelyApi api, BarelyId /*sequence_id*/, BarelyId /*note_id*/,
    BarelyNoteDefinition* out_definition) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_definition) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_GetNotePosition(BarelyApi api,
                                            BarelyId /*sequence_id*/,
                                            BarelyId /*note_id*/,
                                            double* out_position) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_position) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_IsEmpty(BarelyApi api, BarelyId sequence_id,
                                    bool* out_is_empty) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_is_empty) return BarelyStatus_kInvalidArgument;

  const auto is_empty_or = api->instance.IsPerformerEmpty(sequence_id);
  if (IsOk(is_empty_or)) {
    *out_is_empty = GetStatusOrValue(is_empty_or);
    return BarelyStatus_kOk;
  }
  return GetStatus(is_empty_or);
}

BarelyStatus BarelySequence_IsLooping(BarelyApi api, BarelyId sequence_id,
                                      bool* out_is_looping) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_is_looping) return BarelyStatus_kInvalidArgument;

  const auto is_looping_or = api->instance.IsPerformerLooping(sequence_id);
  if (IsOk(is_looping_or)) {
    *out_is_looping = GetStatusOrValue(is_looping_or);
    return BarelyStatus_kOk;
  }
  return GetStatus(is_looping_or);
}

BarelyStatus BarelySequence_RemoveAllNotes(BarelyApi api,
                                           BarelyId sequence_id) {
  if (!api) return BarelyStatus_kNotFound;

  return GetStatus(api->instance.RemoveAllPerformerNotes(sequence_id));
}

BarelyStatus BarelySequence_RemoveAllNotesAtPosition(BarelyApi api,
                                                     BarelyId /*sequence_id*/,
                                                     double /*position*/) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_RemoveAllNotesAtRange(BarelyApi api,
                                                  BarelyId sequence_id,
                                                  double begin_position,
                                                  double end_position) {
  if (!api) return BarelyStatus_kNotFound;

  return GetStatus(api->instance.RemoveAllPerformerNotes(
      sequence_id, begin_position, end_position));
}

BarelyStatus BarelySequence_RemoveNote(BarelyApi api, BarelyId sequence_id,
                                       BarelyId note_id) {
  if (!api) return BarelyStatus_kNotFound;

  return GetStatus(api->instance.RemovePerformerNote(sequence_id, note_id));
}

BarelyStatus BarelySequence_SetBeginOffset(BarelyApi api, BarelyId sequence_id,
                                           double begin_offset) {
  if (!api) return BarelyStatus_kNotFound;

  return GetStatus(
      api->instance.SetPerformerBeginOffset(sequence_id, begin_offset));
}

BarelyStatus BarelySequence_SetBeginPosition(BarelyApi api,
                                             BarelyId sequence_id,
                                             double begin_position) {
  if (!api) return BarelyStatus_kNotFound;

  return GetStatus(
      api->instance.SetPerformerBeginPosition(sequence_id, begin_position));
}

BarelyStatus BarelySequence_SetEndPosition(BarelyApi api, BarelyId sequence_id,
                                           double end_position) {
  if (!api) return BarelyStatus_kNotFound;

  return GetStatus(
      api->instance.SetPerformerEndPosition(sequence_id, end_position));
}

BarelyStatus BarelySequence_SetInstrument(BarelyApi api, BarelyId sequence_id,
                                          BarelyId instrument_id) {
  if (!api) return BarelyStatus_kNotFound;

  return GetStatus(
      api->instance.SetPerformerInstrument(sequence_id, instrument_id));
}

BarelyStatus BarelySequence_SetLoopBeginOffset(BarelyApi api,
                                               BarelyId sequence_id,
                                               double loop_begin_offset) {
  if (!api) return BarelyStatus_kNotFound;

  return GetStatus(api->instance.SetPerformerLoopBeginOffset(
      sequence_id, loop_begin_offset));
}

BarelyStatus BarelySequence_SetLoopLength(BarelyApi api, BarelyId sequence_id,
                                          double loop_length) {
  if (!api) return BarelyStatus_kNotFound;

  return GetStatus(
      api->instance.SetPerformerLoopLength(sequence_id, loop_length));
}

BarelyStatus BarelySequence_SetLooping(BarelyApi api, BarelyId sequence_id,
                                       bool is_looping) {
  if (!api) return BarelyStatus_kNotFound;

  return GetStatus(api->instance.SetPerformerLoop(sequence_id, is_looping));
}

BarelyStatus BarelySequence_SetNoteDefinition(
    BarelyApi api, BarelyId /*sequence_id*/, BarelyId /*note_id*/,
    BarelyNoteDefinition /*definition*/) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_SetNotePosition(BarelyApi api,
                                            BarelyId /*sequence_id*/,
                                            BarelyId /*note_id*/,
                                            double /*position*/) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyExamples_CreateSynthInstrument(BarelyApi api,
                                                  int32_t sample_rate,
                                                  BarelyId* out_instrument_id) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_instrument_id) return BarelyStatus_kInvalidArgument;

  *out_instrument_id = api->instance.CreateInstrument(
      SynthInstrument::GetDefinition(), sample_rate);
  return BarelyStatus_kOk;
}

}  // extern "C"
