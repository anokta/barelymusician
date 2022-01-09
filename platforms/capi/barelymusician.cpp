#include "platforms/capi/barelymusician.h"

#include <stdint.h>

#include <optional>
#include <vector>

#include "barelymusician/common/status.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/engine/engine.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/param_definition.h"
#include "examples/instruments/synth_instrument.h"

namespace {

using ::barely::examples::SynthInstrument;
using ::barelyapi::Engine;
using ::barelyapi::GetStatusOrStatus;
using ::barelyapi::GetStatusOrValue;
using ::barelyapi::InstrumentDefinition;
using ::barelyapi::IsOk;
using ::barelyapi::Note;
using ::barelyapi::ParamDefinition;
using ::barelyapi::Status;
using ::barelyapi::StatusOr;

// Returns the corresponding |ParamDefinition| for a given |param_definition|.
ParamDefinition GetParamDefinition(
    const BarelyParamDefinition& param_definition) noexcept {
  return ParamDefinition(param_definition.default_value,
                         param_definition.min_value,
                         param_definition.max_value);
}

// Returns the corresponding |InstrumentDefinition| for a given |definition|.
InstrumentDefinition GetInstrumentDefinition(
    BarelyInstrumentDefinition definition) noexcept {
  std::vector<ParamDefinition> param_definitions;
  param_definitions.reserve(definition.num_param_definitions);
  for (int i = 0; i < definition.num_param_definitions; ++i) {
    const auto& param_definition = definition.param_definitions[i];
    param_definitions.emplace_back(param_definition.default_value,
                                   param_definition.min_value,
                                   param_definition.max_value);
  }
  return InstrumentDefinition{std::move(definition.create_fn),
                              std::move(definition.destroy_fn),
                              std::move(definition.process_fn),
                              std::move(definition.set_data_fn),
                              std::move(definition.set_note_off_fn),
                              std::move(definition.set_note_on_fn),
                              std::move(definition.set_param_fn),
                              std::move(param_definitions)};
}

// Returns the corresponding |BarelyStatus| value for a given |status|.
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

// Returns the corresponding |BarelyStatus| value for a given |status_or|.
template <typename ValueType>
BarelyStatus GetStatus(const StatusOr<ValueType>& status_or) noexcept {
  return GetStatus(GetStatusOrStatus(status_or));
}

}  // namespace

extern "C" {

/// BarelyMusician C api.
struct BarelyMusician {
  /// Engine instance.
  // TODO(#85): Temp hack to create `Engine` without sample rate.
  Engine instance = Engine(0);

 private:
  // Ensure that the instance can only be destroyed via the api call.
  friend BARELY_EXPORT BarelyStatus BarelyApi_Destroy(BarelyApi);
  ~BarelyMusician() = default;
};

BarelyStatus BarelyApi_Create(BarelyApi* out_api) {
  if (!out_api) return BarelyStatus_kInvalidArgument;

  *out_api = new BarelyMusician();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyApi_Destroy(BarelyApi api) {
  if (!api) return BarelyStatus_kNotFound;

  delete api;
  return BarelyStatus_kOk;
}

BarelyStatus BarelyApi_GetSampleRate(BarelyApi api, int32_t* out_sample_rate) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_sample_rate) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyApi_SetSampleRate(BarelyApi api, int32_t sample_rate) {
  if (!api) return BarelyStatus_kNotFound;

  api->instance.SetSampleRate(sample_rate);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyApi_Update(BarelyApi api, double timestamp) {
  if (!api) return BarelyStatus_kNotFound;

  api->instance.Update(timestamp);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyConductor_ConductNote(BarelyApi api,
                                         BarelyNotePitchType pitch_type,
                                         float pitch, bool bypass_adjustment,
                                         float* out_pitch) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_pitch) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  pitch_type;
  pitch;
  bypass_adjustment;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_GetEnergy(BarelyApi api, float* out_energy) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_energy) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_GetParam(BarelyApi api, int32_t index,
                                      float* out_value) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_value) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  index;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_GetParamDefinition(
    BarelyApi api, int32_t index, BarelyParamDefinition* out_param_definition) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_param_definition) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  index;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_GetRootNote(BarelyApi api, float* out_root_pitch) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_root_pitch) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_GetScale(BarelyApi api, float** out_scale_pitches,
                                      int32_t* out_num_scale_pitches) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_scale_pitches) return BarelyStatus_kInvalidArgument;
  if (!out_num_scale_pitches) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_GetStress(BarelyApi api, float* out_stress) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_stress) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_ResetAllParams(BarelyApi api) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_ResetParam(BarelyApi api, int32_t index) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  index;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_SetData(BarelyApi api, void* data) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  data;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_SetDefinition(
    BarelyApi api, BarelyConductorDefinition definition) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  definition;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_SetEnergy(BarelyApi api, float energy) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  energy;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_SetParam(BarelyApi api, int32_t index,
                                      float value) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  index;
  value;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_SetRootNote(BarelyApi api, float root_pitch) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  root_pitch;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_SetScale(BarelyApi api, float* scale_pitches,
                                      int32_t num_scale_pitches) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  scale_pitches;
  num_scale_pitches;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyConductor_SetStress(BarelyApi api, float stress) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  stress;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyInstrument_Clone(BarelyApi api, BarelyId instrument_id,
                                    BarelyId* out_instrument_id) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_instrument_id) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  instrument_id;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyInstrument_Create(BarelyApi api,
                                     BarelyInstrumentDefinition definition,
                                     BarelyId* out_instrument_id) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_instrument_id) return BarelyStatus_kInvalidArgument;

  *out_instrument_id = api->instance.AddInstrument(
      GetInstrumentDefinition(std::move(definition)));
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_Destroy(BarelyApi api, BarelyId instrument_id) {
  if (!api) return BarelyStatus_kNotFound;

  return GetStatus(api->instance.RemoveInstrument(instrument_id));
}

BarelyStatus BarelyInstrument_GetGain(BarelyApi api, BarelyId instrument_id,
                                      float* out_gain) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_gain) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  instrument_id;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyInstrument_GetParam(BarelyApi api, BarelyId instrument_id,
                                       int32_t index, float* out_value) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_value) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  instrument_id;
  index;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyInstrument_GetParamDefinition(
    BarelyApi api, BarelyId instrument_id, int32_t index,
    BarelyParamDefinition* out_param_definition) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_param_definition) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  instrument_id;
  index;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyInstrument_IsMuted(BarelyApi api, BarelyId instrument_id,
                                      bool* out_is_muted) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_is_muted) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  instrument_id;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyInstrument_IsNoteOn(BarelyApi api, BarelyId instrument_id,
                                       float pitch, bool* out_is_note_on) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_is_note_on) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  instrument_id;
  pitch;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyInstrument_PlayNote(BarelyApi api, BarelyId instrument_id,
                                       double position,
                                       BarelyNoteDefinition definition) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  instrument_id;
  position;
  definition;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyInstrument_Process(BarelyApi api, BarelyId instrument_id,
                                      double timestamp, float* output,
                                      int32_t num_output_channels,
                                      int32_t num_output_frames) {
  if (!api) return BarelyStatus_kNotFound;

  api->instance.ProcessInstrument(instrument_id, timestamp, output,
                                  num_output_channels, num_output_frames);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_ResetAllParams(BarelyApi api,
                                             BarelyId instrument_id) {
  if (!api) return BarelyStatus_kNotFound;

  return GetStatus(
      api->instance.SetAllInstrumentParamsToDefault(instrument_id));
}

BarelyStatus BarelyInstrument_ResetParam(BarelyApi api, BarelyId instrument_id,
                                         int32_t index) {
  if (!api) return BarelyStatus_kNotFound;

  return GetStatus(
      api->instance.SetInstrumentParamToDefault(instrument_id, index));
}

BarelyStatus BarelyInstrument_SetData(BarelyApi api, BarelyId instrument_id,
                                      void* data) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  instrument_id;
  data;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyInstrument_SetGain(BarelyApi api, BarelyId instrument_id,
                                      float gain) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  instrument_id;
  gain;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyInstrument_SetMuted(BarelyApi api, BarelyId instrument_id,
                                       bool is_muted) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  instrument_id;
  is_muted;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyInstrument_SetNoteOffCallback(
    BarelyApi api, BarelyId instrument_id,
    BarelyInstrument_NoteOffCallback note_off_callback, void* user_data) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  instrument_id;
  note_off_callback;
  user_data;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyInstrument_SetNoteOnCallback(
    BarelyApi api, BarelyId instrument_id,
    BarelyInstrument_NoteOnCallback note_on_callback, void* user_data) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  instrument_id;
  note_on_callback;
  user_data;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyInstrument_SetParam(BarelyApi api, BarelyId instrument_id,
                                       int32_t index, float value) {
  if (!api) return BarelyStatus_kNotFound;

  return GetStatus(
      api->instance.SetInstrumentParam(instrument_id, index, value));
}

BarelyStatus BarelyInstrument_StartNote(BarelyApi api, BarelyId instrument_id,
                                        float pitch, float intensity) {
  if (!api) return BarelyStatus_kNotFound;

  return GetStatus(
      api->instance.SetInstrumentNoteOn(instrument_id, pitch, intensity));
}

BarelyStatus BarelyInstrument_StopAllNotes(BarelyApi api,
                                           BarelyId instrument_id) {
  if (!api) return BarelyStatus_kNotFound;

  return GetStatus(api->instance.SetAllInstrumentNotesOff(instrument_id));
}

BarelyStatus BarelyInstrument_StopNote(BarelyApi api, BarelyId instrument_id,
                                       float pitch) {
  if (!api) return BarelyStatus_kNotFound;

  return GetStatus(api->instance.SetInstrumentNoteOff(instrument_id, pitch));
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

BarelyStatus BarelySequence_Clone(BarelyApi api, BarelyId sequence_id,
                                  BarelyId* out_sequence_id) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_sequence_id) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  sequence_id;
  return BarelyStatus_kUnimplemented;
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

BarelyStatus BarelySequence_GetAllNotes(BarelyApi api, BarelyId sequence_id,
                                        BarelyId** out_note_ids,
                                        int32_t* out_num_note_ids) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_note_ids) return BarelyStatus_kInvalidArgument;
  if (!out_num_note_ids) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  sequence_id;
  return BarelyStatus_kUnimplemented;
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

BarelyStatus BarelySequence_GetInstrument(BarelyApi api, BarelyId sequence_id,
                                          BarelyId* out_instrument_id) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_instrument_id) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  sequence_id;
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
    BarelyApi api, BarelyId sequence_id, BarelyId note_id,
    BarelyNoteDefinition* out_definition) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_definition) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  sequence_id;
  note_id;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_GetNotePosition(BarelyApi api, BarelyId sequence_id,
                                            BarelyId note_id,
                                            double* out_position) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_position) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  sequence_id;
  note_id;
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
                                                     BarelyId sequence_id,
                                                     double position) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  sequence_id;
  position;
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

  api->instance.RemoveAllPerformerInstruments(sequence_id);
  return GetStatus(
      api->instance.AddPerformerInstrument(sequence_id, instrument_id));
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

BarelyStatus BarelySequence_SetNoteDefinition(BarelyApi api,
                                              BarelyId sequence_id,
                                              BarelyId note_id,
                                              BarelyNoteDefinition definition) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  sequence_id;
  note_id;
  definition;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_SetNotePosition(BarelyApi api, BarelyId sequence_id,
                                            BarelyId note_id, double position) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  sequence_id;
  note_id;
  position;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyTransport_GetPosition(BarelyApi api, double* out_position) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_position) return BarelyStatus_kInvalidArgument;

  *out_position = api->instance.GetPlaybackPosition();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyTransport_GetTempo(BarelyApi api, double* out_tempo) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_tempo) return BarelyStatus_kInvalidArgument;

  *out_tempo = api->instance.GetPlaybackTempo();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyTransport_IsPlaying(BarelyApi api, bool* out_is_playing) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_is_playing) return BarelyStatus_kInvalidArgument;

  *out_is_playing = api->instance.IsPlaying();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyTransport_Pause(BarelyApi api) {
  if (!api) return BarelyStatus_kNotFound;

  api->instance.StopPlayback();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyTransport_Play(BarelyApi api) {
  if (!api) return BarelyStatus_kNotFound;

  api->instance.StartPlayback();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyTransport_SetBeatCallback(
    BarelyApi api, BarelyTransport_BeatCallback beat_callback,
    void* user_data) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  beat_callback;
  user_data;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyTransport_SetPosition(BarelyApi api, double position) {
  if (!api) return BarelyStatus_kNotFound;

  api->instance.SetPlaybackPosition(position);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyTransport_SetTempo(BarelyApi api, double tempo) {
  if (!api) return BarelyStatus_kNotFound;

  api->instance.SetPlaybackTempo(tempo);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyTransport_SetUpdateCallback(
    BarelyApi api, BarelyTransport_UpdateCallback update_callback,
    void* user_data) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  update_callback;
  user_data;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyTransport_Stop(BarelyApi api) {
  if (!api) return BarelyStatus_kNotFound;

  api->instance.StopPlayback();
  api->instance.SetPlaybackPosition(0.0);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyExamples_CreateSynthInstrument(BarelyApi api,
                                                  BarelyId* out_instrument_id) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_instrument_id) return BarelyStatus_kInvalidArgument;

  *out_instrument_id =
      api->instance.AddInstrument(SynthInstrument::GetDefinition());
  return BarelyStatus_kOk;
}

BarelyStatus BarelyAddPerformerNote(BarelyApi api, BarelyId performer_id,
                                    double note_position, double note_duration,
                                    float note_pitch, float note_intensity,
                                    BarelyId* out_note_id) {
  return BarelySequence_AddNote(
      api, performer_id, note_position,
      BarelyNoteDefinition{note_duration, BarelyNotePitchType_kAbsolutePitch,
                           note_pitch, note_intensity},
      out_note_id);
}

BarelyStatus BarelySetInstrumentNoteOffCallback(
    BarelyApi api,
    BarelyInstrumentNoteOffCallback instrument_note_off_callback) {
  if (api) {
    if (instrument_note_off_callback) {
      api->instance.SetInstrumentNoteOffCallback(
          [instrument_note_off_callback](auto instrument_id, float note_pitch) {
            instrument_note_off_callback(instrument_id, note_pitch);
          });
    } else {
      api->instance.SetInstrumentNoteOffCallback(nullptr);
    }
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySetInstrumentNoteOnCallback(
    BarelyApi api, BarelyInstrumentNoteOnCallback instrument_note_on_callback) {
  if (api) {
    if (instrument_note_on_callback) {
      api->instance.SetInstrumentNoteOnCallback(
          [instrument_note_on_callback](auto instrument_id, float note_pitch,
                                        float note_intensity) {
            instrument_note_on_callback(instrument_id, note_pitch,
                                        note_intensity);
          });
    } else {
      api->instance.SetInstrumentNoteOffCallback(nullptr);
    }
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySetPlaybackBeatCallback(
    BarelyApi api, BarelyPlaybackBeatCallback playback_beat_callback) {
  if (api) {
    if (playback_beat_callback) {
      api->instance.SetPlaybackBeatCallback(
          [playback_beat_callback](double position) {
            playback_beat_callback(position);
          });
    } else {
      api->instance.SetPlaybackBeatCallback(nullptr);
    }
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySetPlaybackUpdateCallback(
    BarelyApi api, BarelyPlaybackUpdateCallback playback_update_callback) {
  if (api) {
    if (playback_update_callback) {
      api->instance.SetPlaybackUpdateCallback(
          [playback_update_callback](double begin_position,
                                     double end_position) {
            playback_update_callback(begin_position, end_position);
          });
    } else {
      api->instance.SetPlaybackUpdateCallback(nullptr);
    }
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

}  // extern "C"
