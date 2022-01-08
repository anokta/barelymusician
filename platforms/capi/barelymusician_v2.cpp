#include "platforms/capi/barelymusician_v2.h"

#include <stdint.h>

extern "C" {

BarelyStatus BarelyConductor_GetParam(BarelyApi api, int32_t index,
                                      float* out_value) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_value) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  index;
  return BarelyStatus_kUnimplemented;
}

/// BarelyMusician C api.
struct BarelyMusician {
  // TODO(#85): This is a temp POC for testing.
  int sample_rate;
  double tempo;
  double begin_offset;

  BarelyTransport_BeatCallback beat_callback;
  void* user_data;

 private:
  // Ensure that the instance can only be destroyed via the api call.
  friend BARELY_EXPORT BarelyStatus BarelyApi_Destroy(BarelyApi);
  ~BarelyMusician() = default;
};

BarelyStatus BarelyApi_Create(BarelyApi* out_api) {
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

  // TODO(#85): Implement - this is a temp POC for testing.
  *out_sample_rate = api->sample_rate;
  return BarelyStatus_kOk;
}

BarelyStatus BarelyApi_SetSampleRate(BarelyApi api, int32_t sample_rate) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement - this is a temp POC for testing.
  api->sample_rate = sample_rate;
  return BarelyStatus_kOk;
}

BarelyStatus BarelyApi_Update(BarelyApi api, double timestamp) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  timestamp;
  return BarelyStatus_kUnimplemented;
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
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_instrument_id) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement - this is a temp POC for testing.
  *out_instrument_id = instrument_id + 1;
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_Create(BarelyApi api,
                                     BarelyInstrumentDefinition definition,
                                     BarelyId* out_instrument_id) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_instrument_id) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement - this is a temp POC for testing.
  *out_instrument_id =
      definition.num_param_definitions > 0
          ? static_cast<BarelyId>(definition.param_definitions[0].default_value)
          : 0;
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_Destroy(BarelyApi api, BarelyId instrument_id) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  instrument_id;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyInstrument_GetGain(BarelyApi api, BarelyId instrument_id,
                                      float* out_gain) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_gain) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement - this is a temp POC for testing.
  *out_gain = static_cast<float>(instrument_id);
  return BarelyStatus_kOk;
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

  // TODO(#85): Implement.
  instrument_id;
  timestamp;
  output;
  num_output_channels;
  num_output_frames;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyInstrument_ResetAllParams(BarelyApi api,
                                             BarelyId instrument_id) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  instrument_id;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyInstrument_ResetParam(BarelyApi api, BarelyId instrument_id,
                                         int32_t index) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  instrument_id;
  index;
  return BarelyStatus_kUnimplemented;
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

  // TODO(#85): Implement.
  instrument_id;
  index;
  value;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyInstrument_StartNote(BarelyApi api, BarelyId instrument_id,
                                        float pitch, float intensity) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  instrument_id;
  pitch;
  intensity;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyInstrument_StopAllNotes(BarelyApi api,
                                           BarelyId instrument_id) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  instrument_id;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyInstrument_StopNote(BarelyApi api, BarelyId instrument_id,
                                       float pitch) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  instrument_id;
  pitch;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_AddNote(BarelyApi api, BarelyId sequence_id,
                                    double position,
                                    BarelyNoteDefinition definition,
                                    BarelyId* out_note_id) {
  if (!api) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_note_id) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  position;
  definition;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_Clone(BarelyApi api, BarelyId sequence_id,
                                  BarelyId* out_sequence_id) {
  if (!api) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_sequence_id) return BarelyStatus_kInvalidArgument;
  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_Create(BarelyApi api, BarelyId* out_sequence_id) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_sequence_id) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement - this is a temp POC for testing.
  *out_sequence_id = 0;
  return BarelyStatus_kOk;
}

BarelyStatus BarelySequence_Destroy(BarelyApi api, BarelyId sequence_id) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  sequence_id;
  return BarelyStatus_kUnimplemented;
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

  // TODO(#85): Implement - this is a temp POC for testing.
  sequence_id;
  *out_begin_offset = api->begin_offset;
  return BarelyStatus_kOk;
}

BarelyStatus BarelySequence_GetBeginPosition(BarelyApi api,
                                             BarelyId sequence_id,
                                             double* out_begin_position) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_begin_position) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  sequence_id;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_GetEndPosition(BarelyApi api, BarelyId sequence_id,
                                           double* out_end_position) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_end_position) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  sequence_id;
  return BarelyStatus_kUnimplemented;
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

  // TODO(#85): Implement.
  sequence_id;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_GetLoopLength(BarelyApi api, BarelyId sequence_id,
                                          double* out_loop_length) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_loop_length) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  sequence_id;
  return BarelyStatus_kUnimplemented;
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

  // TODO(#85): Implement.
  sequence_id;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_IsLooping(BarelyApi api, BarelyId sequence_id,
                                      bool* out_is_looping) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_is_looping) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  sequence_id;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_RemoveAllNotes(BarelyApi api,
                                           BarelyId sequence_id) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  sequence_id;
  return BarelyStatus_kUnimplemented;
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

  // TODO(#85): Implement.
  sequence_id;
  begin_position;
  end_position;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_RemoveNote(BarelyApi api, BarelyId sequence_id,
                                       BarelyId note_id) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  sequence_id;
  note_id;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_SetBeginOffset(BarelyApi api, BarelyId sequence_id,
                                           double begin_offset) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement - this is a temp POC for testing.
  sequence_id;
  api->begin_offset = begin_offset;
  return BarelyStatus_kOk;
}

BarelyStatus BarelySequence_SetBeginPosition(BarelyApi api,
                                             BarelyId sequence_id,
                                             double begin_position) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  sequence_id;
  begin_position;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_SetEndPosition(BarelyApi api, BarelyId sequence_id,
                                           double end_position) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  sequence_id;
  end_position;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_SetInstrument(BarelyApi api, BarelyId sequence_id,
                                          BarelyId instrument_id) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  sequence_id;
  instrument_id;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_SetLoopBeginOffset(BarelyApi api,
                                               BarelyId sequence_id,
                                               double loop_begin_offset) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  sequence_id;
  loop_begin_offset;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_SetLoopLength(BarelyApi api, BarelyId sequence_id,
                                          double loop_length) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  sequence_id;
  loop_length;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_SetLooping(BarelyApi api, BarelyId sequence_id,
                                       bool is_looping) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  sequence_id;
  is_looping;
  return BarelyStatus_kUnimplemented;
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

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyTransport_GetTempo(BarelyApi api, double* out_tempo) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_tempo) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement - this is a temp POC for testing.
  *out_tempo = api->tempo;
  return BarelyStatus_kOk;
}

BarelyStatus BarelyTransport_IsPlaying(BarelyApi api, bool* out_is_playing) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_is_playing) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyTransport_Pause(BarelyApi api) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyTransport_Play(BarelyApi api) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement - this is a temp POC for testing.
  if (api->beat_callback) {
    api->beat_callback(api->tempo, static_cast<double>(api->sample_rate),
                       api->user_data);
  }
  return BarelyStatus_kOk;
}

BarelyStatus BarelyTransport_SetBeatCallback(
    BarelyApi api, BarelyTransport_BeatCallback beat_callback,
    void* user_data) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement - this is a temp POC for testing.
  api->beat_callback = beat_callback;
  api->user_data = user_data;
  return BarelyStatus_kOk;
}

BarelyStatus BarelyTransport_SetPosition(BarelyApi api, double position) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  position;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyTransport_SetTempo(BarelyApi api, double tempo) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement - this is a temp POC for testing.
  api->tempo = tempo;
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

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

}  // extern "C"
