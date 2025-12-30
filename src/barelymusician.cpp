#include <barelymusician.h>

#include <cassert>
#include <cmath>
#include <cstdint>

#include "api/engine.h"
#include "api/instrument.h"
#include "api/performer.h"
#include "api/task.h"
#include "common/constants.h"
#include "common/time.h"
#include "dsp/message.h"

bool BarelyEngine_Create(int32_t sample_rate, int32_t max_frame_count,
                         BarelyEngineHandle* out_engine) {
  if (sample_rate <= 0) return false;
  if (max_frame_count <= 0) return false;
  if (!out_engine) return false;

  *out_engine = new BarelyEngine(sample_rate, max_frame_count);
  return true;
}

bool BarelyEngine_CreateInstrument(BarelyEngineHandle engine,
                                   const BarelyInstrumentControlOverride* control_overrides,
                                   int32_t control_override_count,
                                   BarelyInstrumentRef* out_instrument) {
  if (!engine) return false;
  if (!out_instrument) return false;

  *out_instrument = engine->AddInstrument();
  auto& instrument = engine->GetInstrument(*out_instrument);
  instrument.Init(*engine, {control_overrides, control_overrides + control_override_count});

  instrument.instrument_index = *out_instrument;
  for (int i = 0; i < BarelyInstrumentControlType_kCount; ++i) {
    engine->ScheduleMessage(barely::InstrumentControlMessage{
        instrument.instrument_index, static_cast<barely::InstrumentControlType>(i),
        instrument.controls_[i].value});
  }
  return true;
}

bool BarelyEngine_CreatePerformer(BarelyEngineHandle engine, BarelyPerformerRef* out_performer) {
  if (!engine) return false;
  if (!out_performer) return false;

  *out_performer = engine->AddPerformer();
  engine->GetPerformer((*out_performer)).engine = engine;
  return *out_performer != 0;
}

bool BarelyEngine_CreateTask(BarelyEngineHandle engine, BarelyPerformerRef performer,
                             double position, double duration, int32_t priority,
                             BarelyTaskEventCallback callback, void* user_data,
                             BarelyTaskRef* out_task) {
  if (!engine) return false;
  if (performer == 0) return false;
  if (duration <= 0.0) return false;
  if (!out_task) return false;

  *out_task = engine->AddTask();
  if (*out_task == 0) {
    return false;
  }

  // TODO(#126): Clean this up once all pools are established.
  auto& task = engine->GetTask(*out_task);
  task = {{callback, user_data}, position, duration, static_cast<int>(priority), performer};
  engine->GetPerformer(performer).AddTask(&task);

  return true;
}

bool BarelyEngine_Destroy(BarelyEngineHandle engine) {
  if (!engine) return false;

  engine->DestroyInstruments();
  delete engine;
  return true;
}

bool BarelyEngine_DestroyInstrument(BarelyEngineHandle engine, BarelyInstrumentRef instrument) {
  if (!engine) return false;
  if (instrument == 0) return false;

  engine->GetInstrument(instrument).SetAllNotesOff();
  engine->RemovePerformer(engine->GetInstrument(instrument).arp_index);
  engine->RemoveTask(engine->GetInstrument(instrument).arp_task_index);
  engine->RemoveInstrument(instrument);
  return true;
}

bool BarelyEngine_DestroyPerformer(BarelyEngineHandle engine, BarelyPerformerRef performer) {
  if (!engine) return false;
  if (performer == 0) return false;

  engine->RemovePerformer(performer);
  return true;
}

bool BarelyEngine_DestroyTask(BarelyEngineHandle engine, BarelyTaskRef task) {
  if (!engine) return false;
  if (task == 0) return false;

  engine->GetPerformer(engine->GetTask(task).performer_index).RemoveTask(&engine->GetTask(task));
  engine->RemoveTask(task);
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

bool BarelyInstrument_GetControl(BarelyEngineHandle engine, BarelyInstrumentRef instrument,
                                 BarelyInstrumentControlType type, float* out_value) {
  if (!engine) return false;
  if (instrument == 0) return false;
  if (type >= BarelyInstrumentControlType_kCount) return false;
  if (!out_value) return false;

  *out_value = engine->GetInstrument(instrument).GetControl(type);
  return true;
}

bool BarelyInstrument_GetNoteControl(BarelyEngineHandle engine, BarelyInstrumentRef instrument,
                                     float pitch, BarelyNoteControlType type, float* out_value) {
  if (!engine) return false;
  if (instrument == 0) return false;
  if (type >= BarelyNoteControlType_kCount) return false;
  if (!out_value) return false;

  if (const auto* value = engine->GetInstrument(instrument).GetNoteControl(pitch, type);
      value != nullptr) {
    *out_value = *value;
    return true;
  }
  return false;
}

bool BarelyInstrument_IsNoteOn(BarelyEngineHandle engine, BarelyInstrumentRef instrument,
                               float pitch, bool* out_is_note_on) {
  if (!engine) return false;
  if (instrument == 0) return false;
  if (!out_is_note_on) return false;

  *out_is_note_on = engine->GetInstrument(instrument).IsNoteOn(pitch);
  return true;
}

bool BarelyInstrument_SetAllNotesOff(BarelyEngineHandle engine, BarelyInstrumentRef instrument) {
  if (!engine) return false;

  engine->GetInstrument(instrument).SetAllNotesOff();
  return true;
}

bool BarelyInstrument_SetControl(BarelyEngineHandle engine, BarelyInstrumentRef instrument,
                                 BarelyInstrumentControlType type, float value) {
  if (!engine) return false;
  if (instrument == 0) return false;
  if (type >= BarelyInstrumentControlType_kCount) return false;

  engine->GetInstrument(instrument).SetControl(type, value);
  return true;
}

bool BarelyInstrument_SetNoteControl(BarelyEngineHandle engine, BarelyInstrumentRef instrument,
                                     float pitch, BarelyNoteControlType type, float value) {
  if (!engine) return false;
  if (instrument == 0) return false;
  if (type >= BarelyNoteControlType_kCount) return false;

  engine->GetInstrument(instrument).SetNoteControl(pitch, type, value);
  return true;
}

bool BarelyInstrument_SetNoteEventCallback(BarelyEngineHandle engine,
                                           BarelyInstrumentRef instrument,
                                           BarelyNoteEventCallback callback, void* user_data) {
  if (!engine) return false;
  if (instrument == 0) return false;

  engine->GetInstrument(instrument).SetNoteEventCallback({callback, user_data});
  return true;
}

bool BarelyInstrument_SetNoteOff(BarelyEngineHandle engine, BarelyInstrumentRef instrument,
                                 float pitch) {
  if (!engine) return false;
  if (instrument == 0) return false;

  engine->GetInstrument(instrument).SetNoteOff(pitch);
  return true;
}

bool BarelyInstrument_SetNoteOn(BarelyEngineHandle engine, BarelyInstrumentRef instrument,
                                float pitch,
                                const BarelyNoteControlOverride* note_control_overrides,
                                int32_t note_control_override_count) {
  if (!engine) return false;
  if (instrument == 0) return false;

  engine->GetInstrument(instrument)
      .SetNoteOn(pitch,
                 {note_control_overrides, note_control_overrides + note_control_override_count});
  return true;
}

bool BarelyInstrument_SetSampleData(BarelyEngineHandle engine, BarelyInstrumentRef instrument,
                                    const BarelySlice* slices, int32_t slice_count) {
  if (!engine) return false;
  if (instrument == 0) return false;
  if (slice_count < 0 || (!slices && slice_count > 0)) return false;

  engine->GetInstrument(instrument).SetSampleData({slices, slices + slice_count});
  return true;
}

bool BarelyPerformer_GetLoopBeginPosition(BarelyEngineHandle engine, BarelyPerformerRef performer,
                                          double* out_loop_begin_position) {
  if (!engine) return false;
  if (performer == 0) return false;
  if (!out_loop_begin_position) return false;

  *out_loop_begin_position = engine->GetPerformer(performer).loop_begin_position;
  return true;
}

bool BarelyPerformer_GetLoopLength(BarelyEngineHandle engine, BarelyPerformerRef performer,
                                   double* out_loop_length) {
  if (!engine) return false;
  if (performer == 0) return false;
  if (!out_loop_length) return false;

  *out_loop_length = engine->GetPerformer(performer).loop_length;
  return true;
}

bool BarelyPerformer_GetPosition(BarelyEngineHandle engine, BarelyPerformerRef performer,
                                 double* out_position) {
  if (!engine) return false;
  if (performer == 0) return false;
  if (!out_position) return false;

  *out_position = engine->GetPerformer(performer).position;
  return true;
}

bool BarelyPerformer_IsLooping(BarelyEngineHandle engine, BarelyPerformerRef performer,
                               bool* out_is_looping) {
  if (!engine) return false;
  if (performer == 0) return false;
  if (!out_is_looping) return false;

  *out_is_looping = engine->GetPerformer(performer).is_looping;
  return true;
}

bool BarelyPerformer_IsPlaying(BarelyEngineHandle engine, BarelyPerformerRef performer,
                               bool* out_is_playing) {
  if (!engine) return false;
  if (performer == 0) return false;
  if (!out_is_playing) return false;

  *out_is_playing = engine->GetPerformer(performer).is_playing;
  return true;
}

bool BarelyPerformer_SetLoopBeginPosition(BarelyEngineHandle engine, BarelyPerformerRef performer,
                                          double loop_begin_position) {
  if (!engine) return false;
  if (performer == 0) return false;

  engine->GetPerformer(performer).SetLoopBeginPosition(loop_begin_position);
  return true;
}

bool BarelyPerformer_SetLoopLength(BarelyEngineHandle engine, BarelyPerformerRef performer,
                                   double loop_length) {
  if (!engine) return false;
  if (performer == 0) return false;

  engine->GetPerformer(performer).SetLoopLength(loop_length);
  return true;
}

bool BarelyPerformer_SetLooping(BarelyEngineHandle engine, BarelyPerformerRef performer,
                                bool is_looping) {
  if (!engine) return false;
  if (performer == 0) return false;

  engine->GetPerformer(performer).SetLooping(is_looping);
  return true;
}

bool BarelyPerformer_SetPosition(BarelyEngineHandle engine, BarelyPerformerRef performer,
                                 double position) {
  if (!engine) return false;
  if (performer == 0) return false;

  engine->GetPerformer(performer).SetPosition(position);
  return true;
}

bool BarelyPerformer_Start(BarelyEngineHandle engine, BarelyPerformerRef performer) {
  if (!engine) return false;
  if (performer == 0) return false;

  engine->GetPerformer(performer).Start();
  return true;
}

bool BarelyPerformer_Stop(BarelyEngineHandle engine, BarelyPerformerRef performer) {
  if (!engine) return false;
  if (performer == 0) return false;

  engine->GetPerformer(performer).Stop();
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

bool BarelyTask_GetDuration(BarelyEngineHandle engine, BarelyTaskRef task, double* out_duration) {
  if (!engine) return false;
  if (task == 0) return false;
  if (!out_duration) return false;

  *out_duration = engine->GetTask(task).duration;
  return true;
}

bool BarelyTask_GetPosition(BarelyEngineHandle engine, BarelyTaskRef task, double* out_position) {
  if (!engine) return false;
  if (task == 0) return false;
  if (!out_position) return false;

  *out_position = engine->GetTask(task).position;
  return true;
}

bool BarelyTask_GetPriority(BarelyEngineHandle engine, BarelyTaskRef task, int32_t* out_priority) {
  if (!engine) return false;
  if (task == 0) return false;
  if (!out_priority) return false;

  *out_priority = static_cast<int32_t>(engine->GetTask(task).priority);
  return true;
}

bool BarelyTask_IsActive(BarelyEngineHandle engine, BarelyTaskRef task, bool* out_is_active) {
  if (!engine) return false;
  if (task == 0) return false;
  if (!out_is_active) return false;

  *out_is_active = engine->GetTask(task).is_active;
  return true;
}

bool BarelyTask_SetDuration(BarelyEngineHandle engine, BarelyTaskRef task, double duration) {
  if (!engine) return false;
  if (task == 0) return false;
  if (duration <= 0.0) return false;

  engine->GetPerformer(engine->GetTask(task).performer_index)
      .SetTaskDuration(&engine->GetTask(task), duration);
  return true;
}

bool BarelyTask_SetEventCallback(BarelyEngineHandle engine, BarelyTaskRef task,
                                 BarelyTaskEventCallback callback, void* user_data) {
  if (!engine) return false;
  if (task == 0) return false;

  engine->GetTask(task).SetEventCallback({callback, user_data});
  return true;
}

bool BarelyTask_SetPosition(BarelyEngineHandle engine, BarelyTaskRef task, double position) {
  if (!engine) return false;
  if (task == 0) return false;

  engine->GetPerformer(engine->GetTask(task).performer_index)
      .SetTaskPosition(&engine->GetTask(task), position);
  return true;
}

bool BarelyTask_SetPriority(BarelyEngineHandle engine, BarelyTaskRef task, int32_t priority) {
  if (!engine) return false;
  if (task == 0) return false;

  engine->GetPerformer(engine->GetTask(task).performer_index)
      .SetTaskPriority(&engine->GetTask(task), static_cast<int>(priority));
  return true;
}
