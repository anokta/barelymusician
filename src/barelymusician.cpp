#include <barelymusician.h>

#include <cassert>
#include <cmath>
#include <cstdint>

#include "api/engine.h"
#include "core/time.h"
#include "engine/performer_state.h"

bool BarelyEngine_Create(int32_t sample_rate, int32_t max_frame_count, BarelyEngine** out_engine) {
  if (sample_rate <= 0) return false;
  if (max_frame_count <= 0) return false;
  if (!out_engine) return false;

  *out_engine = new BarelyEngine(sample_rate, max_frame_count);
  return true;
}

bool BarelyEngine_CreateInstrument(BarelyEngine* engine,
                                   const BarelyInstrumentControlOverride* control_overrides,
                                   int32_t control_override_count, BarelyRef* out_instrument) {
  if (!engine) return false;
  if (!out_instrument) return false;

  *out_instrument =
      engine->instrument_controller().Acquire(control_overrides, control_override_count);
  return out_instrument->index > 0;
}

bool BarelyEngine_CreatePerformer(BarelyEngine* engine, BarelyRef* out_performer) {
  if (!engine) return false;
  if (!out_performer) return false;

  *out_performer = engine->performer_controller().Acquire();
  return out_performer->index > 0;
}

bool BarelyEngine_CreateTask(BarelyEngine* engine, BarelyRef performer, double position,
                             double duration, int32_t priority, BarelyTaskEventCallback callback,
                             void* user_data, BarelyRef* out_task) {
  if (!engine) return false;
  if (!engine->IsValidPerformer(performer)) return false;
  if (duration <= 0.0) return false;
  if (!out_task) return false;

  *out_task = engine->performer_controller().AcquireTask(performer.index, position, duration,
                                                         priority, callback, user_data);
  return out_task->index > 0;
}

bool BarelyEngine_Destroy(BarelyEngine* engine) {
  if (!engine) return false;

  engine->instrument_controller().SetAllNotesOff();
  delete engine;
  return true;
}

bool BarelyEngine_DestroyInstrument(BarelyEngine* engine, BarelyRef instrument) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument)) return false;

  engine->instrument_controller().Release(instrument.index);
  return true;
}

bool BarelyEngine_DestroyPerformer(BarelyEngine* engine, BarelyRef performer) {
  if (!engine) return false;
  if (!engine->IsValidPerformer(performer)) return false;

  engine->performer_controller().Release(performer.index);
  return true;
}

bool BarelyEngine_DestroyTask(BarelyEngine* engine, BarelyRef task) {
  if (!engine) return false;
  if (!engine->IsValidTask(task)) return false;

  engine->performer_controller().ReleaseTask(task.index);
  return true;
}

bool BarelyEngine_GenerateRandomNumber(BarelyEngine* engine, double* out_number) {
  if (!engine) return false;
  if (!out_number) return false;

  *out_number = engine->main_rng().Generate();
  return true;
}

bool BarelyEngine_GetControl(const BarelyEngine* engine, BarelyEngineControlType type,
                             float* out_value) {
  if (!engine) return false;
  if (type >= BarelyEngineControlType_kCount) return false;
  if (!out_value) return false;

  *out_value = engine->GetControl(type);
  return true;
}

bool BarelyEngine_GetSeed(const BarelyEngine* engine, int32_t* out_seed) {
  if (!engine) return false;
  if (!out_seed) return false;

  *out_seed = engine->main_rng().GetSeed();
  return true;
}

bool BarelyEngine_GetTempo(const BarelyEngine* engine, double* out_tempo) {
  if (!engine) return false;
  if (!out_tempo) return false;

  *out_tempo = engine->GetTempo();
  return true;
}

bool BarelyEngine_GetTimestamp(const BarelyEngine* engine, double* out_timestamp) {
  if (!engine) return false;
  if (!out_timestamp) return false;

  *out_timestamp = engine->GetTimestamp();
  return true;
}

bool BarelyEngine_Process(BarelyEngine* engine, float* output_samples, int32_t output_channel_count,
                          int32_t output_frame_count, double timestamp) {
  if (!engine) return false;
  if (!output_samples) return false;
  if (output_channel_count <= 0 || output_frame_count <= 0) return false;

  engine->Process(output_samples, output_channel_count, output_frame_count, timestamp);
  return true;
}

bool BarelyEngine_SetControl(BarelyEngine* engine, BarelyEngineControlType type, float value) {
  if (!engine) return false;
  if (type >= BarelyEngineControlType_kCount) return false;

  engine->SetControl(type, value);
  return true;
}

bool BarelyEngine_SetSeed(BarelyEngine* engine, int32_t seed) {
  if (!engine) return false;

  engine->main_rng().SetSeed(seed);
  engine->ScheduleMessage(barely::EngineSeedMessage{seed});
  return true;
}

bool BarelyEngine_SetTempo(BarelyEngine* engine, double tempo) {
  if (!engine) return false;

  engine->SetTempo(tempo);
  return true;
}

bool BarelyEngine_Update(BarelyEngine* engine, double timestamp) {
  if (!engine) return false;

  engine->Update(timestamp);
  return true;
}

bool BarelyInstrument_GetControl(const BarelyEngine* engine, BarelyRef instrument,
                                 BarelyInstrumentControlType type, float* out_value) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument)) return false;
  if (type >= BarelyInstrumentControlType_kCount) return false;
  if (!out_value) return false;

  *out_value = engine->instrument_controller().GetControl(instrument.index, type);
  return true;
}

bool BarelyInstrument_GetNoteControl(const BarelyEngine* engine, BarelyRef instrument, float pitch,
                                     BarelyNoteControlType type, float* out_value) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument)) return false;
  if (type >= BarelyNoteControlType_kCount) return false;
  if (!out_value) return false;

  if (const float* value =
          engine->instrument_controller().GetNoteControl(instrument.index, pitch, type)) {
    *out_value = *value;
    return true;
  }
  return false;
}

bool BarelyInstrument_IsNoteOn(const BarelyEngine* engine, BarelyRef instrument, float pitch,
                               bool* out_is_note_on) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument)) return false;
  if (!out_is_note_on) return false;

  *out_is_note_on = engine->instrument_controller().IsNoteOn(instrument.index, pitch);
  return true;
}

bool BarelyInstrument_SetAllNotesOff(BarelyEngine* engine, BarelyRef instrument) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument)) return false;

  engine->instrument_controller().SetAllNotesOff(instrument.index);
  return true;
}

bool BarelyInstrument_SetControl(BarelyEngine* engine, BarelyRef instrument,
                                 BarelyInstrumentControlType type, float value) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument)) return false;
  if (type >= BarelyInstrumentControlType_kCount) return false;

  engine->instrument_controller().SetControl(instrument.index, type, value);
  return true;
}

bool BarelyInstrument_SetNoteControl(BarelyEngine* engine, BarelyRef instrument, float pitch,
                                     BarelyNoteControlType type, float value) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument)) return false;
  if (type >= BarelyNoteControlType_kCount) return false;

  engine->instrument_controller().SetNoteControl(instrument.index, pitch, type, value);
  return true;
}

bool BarelyInstrument_SetNoteEventCallback(BarelyEngine* engine, BarelyRef instrument,
                                           BarelyNoteEventCallback callback, void* user_data) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument)) return false;

  engine->instrument_controller().SetNoteEventCallback(instrument.index, callback, user_data);
  return true;
}

bool BarelyInstrument_SetNoteOff(BarelyEngine* engine, BarelyRef instrument, float pitch) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument)) return false;

  engine->instrument_controller().SetNoteOff(instrument.index, pitch);
  return true;
}

bool BarelyInstrument_SetNoteOn(BarelyEngine* engine, BarelyRef instrument, float pitch,
                                const BarelyNoteControlOverride* note_control_overrides,
                                int32_t note_control_override_count) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument)) return false;

  engine->instrument_controller().SetNoteOn(instrument.index, pitch, note_control_overrides,
                                            note_control_override_count);
  return true;
}

bool BarelyInstrument_SetSampleData(BarelyEngine* engine, BarelyRef instrument,
                                    const BarelySlice* slices, int32_t slice_count) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument)) return false;
  if (slice_count < 0 || (!slices && slice_count > 0)) return false;

  engine->ScheduleMessage(barely::SampleDataMessage{
      instrument.index, barely::SampleData({slices, slices + slice_count})});
  return true;
}

bool BarelyPerformer_GetLoopBeginPosition(const BarelyEngine* engine, BarelyRef performer,
                                          double* out_loop_begin_position) {
  if (!engine) return false;
  if (!engine->IsValidPerformer(performer)) return false;
  if (!out_loop_begin_position) return false;

  *out_loop_begin_position =
      engine->performer_controller().Get(performer.index).loop_begin_position;
  return true;
}

bool BarelyPerformer_GetLoopLength(const BarelyEngine* engine, BarelyRef performer,
                                   double* out_loop_length) {
  if (!engine) return false;
  if (!engine->IsValidPerformer(performer)) return false;
  if (!out_loop_length) return false;

  *out_loop_length = engine->performer_controller().Get(performer.index).loop_length;
  return true;
}

bool BarelyPerformer_GetPosition(const BarelyEngine* engine, BarelyRef performer,
                                 double* out_position) {
  if (!engine) return false;
  if (!engine->IsValidPerformer(performer)) return false;
  if (!out_position) return false;

  *out_position = engine->performer_controller().Get(performer.index).position;
  return true;
}

bool BarelyPerformer_IsLooping(const BarelyEngine* engine, BarelyRef performer,
                               bool* out_is_looping) {
  if (!engine) return false;
  if (!engine->IsValidPerformer(performer)) return false;
  if (!out_is_looping) return false;

  *out_is_looping = engine->performer_controller().Get(performer.index).is_looping;
  return true;
}

bool BarelyPerformer_IsPlaying(const BarelyEngine* engine, BarelyRef performer,
                               bool* out_is_playing) {
  if (!engine) return false;
  if (!engine->IsValidPerformer(performer)) return false;
  if (!out_is_playing) return false;

  *out_is_playing = engine->performer_controller().Get(performer.index).is_playing;
  return true;
}

bool BarelyPerformer_SetLoopBeginPosition(BarelyEngine* engine, BarelyRef performer,
                                          double loop_begin_position) {
  if (!engine) return false;
  if (!engine->IsValidPerformer(performer)) return false;

  engine->performer_controller().Get(performer.index).SetLoopBeginPosition(loop_begin_position);
  return true;
}

bool BarelyPerformer_SetLoopLength(BarelyEngine* engine, BarelyRef performer, double loop_length) {
  if (!engine) return false;
  if (!engine->IsValidPerformer(performer)) return false;

  engine->performer_controller().Get(performer.index).SetLoopLength(loop_length);
  return true;
}

bool BarelyPerformer_SetLooping(BarelyEngine* engine, BarelyRef performer, bool is_looping) {
  if (!engine) return false;
  if (!engine->IsValidPerformer(performer)) return false;

  engine->performer_controller().Get(performer.index).SetLooping(is_looping);
  return true;
}

bool BarelyPerformer_SetPosition(BarelyEngine* engine, BarelyRef performer, double position) {
  if (!engine) return false;
  if (!engine->IsValidPerformer(performer)) return false;

  engine->performer_controller().Get(performer.index).SetPosition(position);
  return true;
}

bool BarelyPerformer_Start(BarelyEngine* engine, BarelyRef performer) {
  if (!engine) return false;
  if (!engine->IsValidPerformer(performer)) return false;

  engine->performer_controller().Get(performer.index).Start();
  return true;
}

bool BarelyPerformer_Stop(BarelyEngine* engine, BarelyRef performer) {
  if (!engine) return false;
  if (!engine->IsValidPerformer(performer)) return false;

  engine->performer_controller().Get(performer.index).Stop();
  return true;
}

bool BarelyTask_GetDuration(const BarelyEngine* engine, BarelyRef task, double* out_duration) {
  if (!engine) return false;
  if (!engine->IsValidTask(task)) return false;
  if (!out_duration) return false;

  *out_duration = engine->performer_controller().GetTask(task.index).duration;
  return true;
}

bool BarelyTask_GetPosition(const BarelyEngine* engine, BarelyRef task, double* out_position) {
  if (!engine) return false;
  if (!engine->IsValidTask(task)) return false;
  if (!out_position) return false;

  *out_position = engine->performer_controller().GetTask(task.index).position;
  return true;
}

bool BarelyTask_GetPriority(const BarelyEngine* engine, BarelyRef task, int32_t* out_priority) {
  if (!engine) return false;
  if (!engine->IsValidTask(task)) return false;
  if (!out_priority) return false;

  *out_priority = engine->performer_controller().GetTask(task.index).priority;
  return true;
}

bool BarelyTask_IsActive(const BarelyEngine* engine, BarelyRef task, bool* out_is_active) {
  if (!engine) return false;
  if (!engine->IsValidTask(task)) return false;
  if (!out_is_active) return false;

  *out_is_active = engine->performer_controller().GetTask(task.index).is_active;
  return true;
}

bool BarelyTask_SetDuration(BarelyEngine* engine, BarelyRef task, double duration) {
  if (!engine) return false;
  if (!engine->IsValidTask(task)) return false;
  if (duration <= 0.0) return false;

  engine->performer_controller().SetTaskDuration(task.index, duration);
  return true;
}

bool BarelyTask_SetEventCallback(BarelyEngine* engine, BarelyRef task,
                                 BarelyTaskEventCallback callback, void* user_data) {
  if (!engine) return false;
  if (!engine->IsValidTask(task)) return false;

  engine->performer_controller().SetTaskEventCallback(task.index, callback, user_data);
  return true;
}

bool BarelyTask_SetPosition(BarelyEngine* engine, BarelyRef task, double position) {
  if (!engine) return false;
  if (!engine->IsValidTask(task)) return false;

  engine->performer_controller().SetTaskPosition(task.index, position);
  return true;
}

bool BarelyTask_SetPriority(BarelyEngine* engine, BarelyRef task, int32_t priority) {
  if (!engine) return false;
  if (!engine->IsValidTask(task)) return false;

  engine->performer_controller().SetTaskPriority(task.index, priority);
  return true;
}

bool BarelyQuantization_GetPosition(const BarelyQuantization* quantization, double position,
                                    double* out_position) {
  if (!quantization || !out_position) return false;
  if (quantization->subdivision <= 0) return false;
  if (quantization->amount < 0.0f || quantization->amount > 1.0f) return false;

  *out_position =
      barely::Quantize(position, static_cast<int>(quantization->subdivision), quantization->amount);
  return true;
}

bool BarelyScale_GetPitch(const BarelyScale* scale, int32_t degree, float* out_pitch) {
  if (scale == nullptr) return false;
  if (scale->pitches == nullptr || scale->pitch_count == 0) return false;
  if (scale->mode < 0 || scale->mode >= scale->pitch_count) return false;
  if (out_pitch == nullptr) return false;

  const int scale_degree = degree + scale->mode;
  const int pitch_count = static_cast<int>(scale->pitch_count);
  const int octave = static_cast<int>(
      std::floor(static_cast<float>(scale_degree) / static_cast<float>(pitch_count)));
  const int index = scale_degree - octave * pitch_count;
  assert(index >= 0 && index < pitch_count);
  *out_pitch = scale->root_pitch + static_cast<float>(octave) + scale->pitches[index] -
               scale->pitches[scale->mode];
  return true;
}
