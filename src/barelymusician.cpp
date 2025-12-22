#include <barelymusician.h>

#include <cassert>
#include <cmath>
#include <cstdint>

#include "api/engine.h"
#include "api/instrument.h"
#include "api/performer.h"
#include "api/task.h"
#include "common/time.h"

bool BarelyEngine_Create(int32_t sample_rate, int32_t max_frame_count,
                         BarelyEngineHandle* out_engine) {
  if (sample_rate <= 0) return false;
  if (max_frame_count <= 0) return false;
  if (!out_engine) return false;

  *out_engine = new BarelyEngine(sample_rate, max_frame_count);
  return true;
}

bool BarelyEngine_Destroy(BarelyEngineHandle engine) {
  if (!engine) return false;

  delete engine;
  return true;
}

bool BarelyEngine_GenerateRandomNumber(BarelyEngineHandle engine, double* out_number) {
  if (!engine) return false;
  if (!out_number) return false;

  *out_number = engine->main_rng().Generate();
  return true;
}

bool BarelyEngine_GetControl(BarelyEngineHandle engine, BarelyEngineControlType type,
                             float* out_value) {
  if (!engine) return false;
  if (type >= BarelyEngineControlType_kCount) return false;
  if (!out_value) return false;

  *out_value = engine->GetControl(type);
  return true;
}

bool BarelyEngine_GetSeed(BarelyEngineHandle engine, int32_t* out_seed) {
  if (!engine) return false;
  if (!out_seed) return false;

  *out_seed = engine->main_rng().GetSeed();
  return true;
}

bool BarelyEngine_GetTempo(BarelyEngineHandle engine, double* out_tempo) {
  if (!engine) return false;
  if (!out_tempo) return false;

  *out_tempo = engine->GetTempo();
  return true;
}

bool BarelyEngine_GetTimestamp(BarelyEngineHandle engine, double* out_timestamp) {
  if (!engine) return false;
  if (!out_timestamp) return false;

  *out_timestamp = engine->GetTimestamp();
  return true;
}

bool BarelyEngine_Process(BarelyEngineHandle engine, float* output_samples,
                          int32_t output_channel_count, int32_t output_frame_count,
                          double timestamp) {
  if (!engine) return false;
  if (!output_samples) return false;
  if (output_channel_count <= 0 || output_frame_count <= 0) return false;

  engine->Process(output_samples, output_channel_count, output_frame_count, timestamp);
  return true;
}

bool BarelyEngine_SetControl(BarelyEngineHandle engine, BarelyEngineControlType type, float value) {
  if (!engine) return false;
  if (type >= BarelyEngineControlType_kCount) return false;

  engine->SetControl(type, value);
  return true;
}

bool BarelyEngine_SetSeed(BarelyEngineHandle engine, int32_t seed) {
  if (!engine) return false;

  // TODO(#146): This should ideally set the seed of `audio_rng_` as well.
  engine->main_rng().SetSeed(seed);
  return true;
}

bool BarelyEngine_SetTempo(BarelyEngineHandle engine, double tempo) {
  if (!engine) return false;

  engine->SetTempo(tempo);
  return true;
}

bool BarelyEngine_Update(BarelyEngineHandle engine, double timestamp) {
  if (!engine) return false;

  engine->Update(timestamp);
  return true;
}

bool BarelyInstrument_Create(BarelyEngineHandle engine,
                             const BarelyInstrumentControlOverride* control_overrides,
                             int32_t control_override_count,
                             BarelyInstrumentHandle* out_instrument) {
  if (!engine) return false;
  if (!out_instrument) return false;

  *out_instrument = new BarelyInstrument(
      *engine, {control_overrides, control_overrides + control_override_count});
  return true;
}

bool BarelyInstrument_Destroy(BarelyInstrumentHandle instrument) {
  if (!instrument) return false;

  delete instrument;
  return true;
}

bool BarelyInstrument_GetControl(BarelyInstrumentHandle instrument,
                                 BarelyInstrumentControlType type, float* out_value) {
  if (!instrument) return false;
  if (type >= BarelyInstrumentControlType_kCount) return false;
  if (!out_value) return false;

  *out_value = instrument->GetControl(type);
  return true;
}

bool BarelyInstrument_GetNoteControl(BarelyInstrumentHandle instrument, float pitch,
                                     BarelyNoteControlType type, float* out_value) {
  if (!instrument) return false;
  if (type >= BarelyNoteControlType_kCount) return false;
  if (!out_value) return false;

  if (const auto* value = instrument->GetNoteControl(pitch, type); value != nullptr) {
    *out_value = *value;
    return true;
  }
  return false;
}

bool BarelyInstrument_IsNoteOn(BarelyInstrumentHandle instrument, float pitch,
                               bool* out_is_note_on) {
  if (!instrument) return false;
  if (!out_is_note_on) return false;

  *out_is_note_on = instrument->IsNoteOn(pitch);
  return true;
}

bool BarelyInstrument_SetAllNotesOff(BarelyInstrumentHandle instrument) {
  if (!instrument) return false;

  instrument->SetAllNotesOff();
  return true;
}

bool BarelyInstrument_SetControl(BarelyInstrumentHandle instrument,
                                 BarelyInstrumentControlType type, float value) {
  if (!instrument) return false;
  if (type >= BarelyInstrumentControlType_kCount) return false;

  instrument->SetControl(type, value);
  return true;
}

bool BarelyInstrument_SetNoteControl(BarelyInstrumentHandle instrument, float pitch,
                                     BarelyNoteControlType type, float value) {
  if (!instrument) return false;
  if (type >= BarelyNoteControlType_kCount) return false;

  instrument->SetNoteControl(pitch, type, value);
  return true;
}

bool BarelyInstrument_SetNoteEventCallback(BarelyInstrumentHandle instrument,
                                           BarelyNoteEventCallback callback, void* user_data) {
  if (!instrument) return false;

  instrument->SetNoteEventCallback({callback, user_data});
  return true;
}

bool BarelyInstrument_SetNoteOff(BarelyInstrumentHandle instrument, float pitch) {
  if (!instrument) return false;

  instrument->SetNoteOff(pitch);
  return true;
}

bool BarelyInstrument_SetNoteOn(BarelyInstrumentHandle instrument, float pitch,
                                const BarelyNoteControlOverride* note_control_overrides,
                                int32_t note_control_override_count) {
  if (!instrument) return false;

  instrument->SetNoteOn(
      pitch, {note_control_overrides, note_control_overrides + note_control_override_count});
  return true;
}

bool BarelyInstrument_SetSampleData(BarelyInstrumentHandle instrument, const BarelySlice* slices,
                                    int32_t slice_count) {
  if (!instrument) return false;
  if (slice_count < 0 || (!slices && slice_count > 0)) return false;

  instrument->SetSampleData({slices, slices + slice_count});
  return true;
}

bool BarelyPerformer_Create(BarelyEngineHandle engine, BarelyPerformerHandle* out_performer) {
  if (!engine) return false;
  if (!out_performer) return false;

  *out_performer = new BarelyPerformer(*engine);
  return true;
}

bool BarelyPerformer_Destroy(BarelyPerformerHandle performer) {
  if (!performer) return false;

  delete performer;
  return true;
}

bool BarelyPerformer_GetLoopBeginPosition(BarelyPerformerHandle performer,
                                          double* out_loop_begin_position) {
  if (!performer) return false;
  if (!out_loop_begin_position) return false;

  *out_loop_begin_position = performer->GetLoopBeginPosition();
  return true;
}

bool BarelyPerformer_GetLoopLength(BarelyPerformerHandle performer, double* out_loop_length) {
  if (!performer) return false;
  if (!out_loop_length) return false;

  *out_loop_length = performer->GetLoopLength();
  return true;
}

bool BarelyPerformer_GetPosition(BarelyPerformerHandle performer, double* out_position) {
  if (!performer) return false;
  if (!out_position) return false;

  *out_position = performer->GetPosition();
  return true;
}

bool BarelyPerformer_IsLooping(BarelyPerformerHandle performer, bool* out_is_looping) {
  if (!performer) return false;
  if (!out_is_looping) return false;

  *out_is_looping = performer->IsLooping();
  return true;
}

bool BarelyPerformer_IsPlaying(BarelyPerformerHandle performer, bool* out_is_playing) {
  if (!performer) return false;
  if (!out_is_playing) return false;

  *out_is_playing = performer->IsPlaying();
  return true;
}

bool BarelyPerformer_SetLoopBeginPosition(BarelyPerformerHandle performer,
                                          double loop_begin_position) {
  if (!performer) return false;

  performer->SetLoopBeginPosition(loop_begin_position);
  return true;
}

bool BarelyPerformer_SetLoopLength(BarelyPerformerHandle performer, double loop_length) {
  if (!performer) return false;

  performer->SetLoopLength(loop_length);
  return true;
}

bool BarelyPerformer_SetLooping(BarelyPerformerHandle performer, bool is_looping) {
  if (!performer) return false;

  performer->SetLooping(is_looping);
  return true;
}

bool BarelyPerformer_SetPosition(BarelyPerformerHandle performer, double position) {
  if (!performer) return false;

  performer->SetPosition(position);
  return true;
}

bool BarelyPerformer_Start(BarelyPerformerHandle performer) {
  if (!performer) return false;

  performer->Start();
  return true;
}

bool BarelyPerformer_Stop(BarelyPerformerHandle performer) {
  if (!performer) return false;

  performer->Stop();
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

bool BarelyTask_Create(BarelyPerformerHandle performer, double position, double duration,
                       int32_t priority, BarelyTaskEventCallback callback, void* user_data,
                       BarelyTaskHandle* out_task) {
  if (!performer) return false;
  if (duration <= 0.0) return false;
  if (!out_task) return false;

  *out_task = new BarelyTask({callback, user_data}, performer, position, duration,
                             static_cast<int>(priority));
  performer->AddTask(*out_task);

  return *out_task;
}

bool BarelyTask_Destroy(BarelyTaskHandle task) {
  if (!task) return false;

  task->performer->RemoveTask(task);
  delete task;
  return true;
}

bool BarelyTask_GetDuration(BarelyTaskHandle task, double* out_duration) {
  if (!task) return false;
  if (!out_duration) return false;

  *out_duration = task->duration;
  return true;
}

bool BarelyTask_GetPosition(BarelyTaskHandle task, double* out_position) {
  if (!task) return false;
  if (!out_position) return false;

  *out_position = task->position;
  return true;
}

bool BarelyTask_GetPriority(BarelyTaskHandle task, int32_t* out_priority) {
  if (!task) return false;
  if (!out_priority) return false;

  *out_priority = static_cast<int32_t>(task->priority);
  return true;
}

bool BarelyTask_IsActive(BarelyTaskHandle task, bool* out_is_active) {
  if (!task) return false;
  if (!out_is_active) return false;

  *out_is_active = task->is_active;
  return true;
}

bool BarelyTask_SetDuration(BarelyTaskHandle task, double duration) {
  if (!task) return false;
  if (duration <= 0.0) return false;

  task->SetDuration(duration);
  return true;
}

bool BarelyTask_SetEventCallback(BarelyTaskHandle task, BarelyTaskEventCallback callback,
                                 void* user_data) {
  if (!task) return false;

  task->SetEventCallback({callback, user_data});
  return true;
}

bool BarelyTask_SetPosition(BarelyTaskHandle task, double position) {
  if (!task) return false;

  task->SetPosition(position);
  return true;
}

bool BarelyTask_SetPriority(BarelyTaskHandle task, int32_t priority) {
  if (!task) return false;

  task->SetPriority(static_cast<int>(priority));
  return true;
}
