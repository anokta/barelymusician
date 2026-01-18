#include <barelymusician.h>

#include <cassert>
#include <cmath>
#include <cstdint>

#include "core/time.h"
#include "engine/engine_controller.h"
#include "engine/engine_processor.h"
#include "engine/engine_state.h"
#include "engine/message.h"

namespace {

static_assert(BARELY_ID_INDEX_BIT_COUNT > 0 && BARELY_ID_INDEX_BIT_COUNT < 32);
static_assert(BARELY_MAX_FRAME_COUNT > 0);

constexpr uint32_t kMaxIdIndex = ((1 << BARELY_ID_INDEX_BIT_COUNT) - 1);
constexpr uint32_t kMaxIdGeneration = ((1 << (32 - BARELY_ID_INDEX_BIT_COUNT)) - 1);

static_assert(BARELY_MAX_INSTRUMENT_COUNT > 0 && BARELY_MAX_INSTRUMENT_COUNT <= kMaxIdIndex);
static_assert(BARELY_MAX_PERFORMER_COUNT > 0 && BARELY_MAX_PERFORMER_COUNT <= kMaxIdIndex);
static_assert(BARELY_MAX_TASK_COUNT > 0 && BARELY_MAX_TASK_COUNT <= kMaxIdIndex);

[[nodiscard]] uint32_t BuildId(uint32_t index, uint32_t generation) noexcept {
  return (generation << BARELY_ID_INDEX_BIT_COUNT) | (index + 1);
}

[[nodiscard]] uint32_t GetGeneration(uint32_t id) noexcept {
  return id >> BARELY_ID_INDEX_BIT_COUNT;
}

[[nodiscard]] uint32_t GetIndex(uint32_t id) noexcept { return (id & kMaxIdIndex) - 1; }

[[nodiscard]] uint32_t GetNextGeneration(uint32_t generation) noexcept {
  return (generation + 1) & kMaxIdGeneration;
}

}  // namespace

struct BarelyEngine {
  barely::EngineState state;
  barely::EngineController controller;
  barely::EngineProcessor processor;

  explicit BarelyEngine(int sample_rate) noexcept : controller(state), processor(state) {
    state.sample_rate = static_cast<float>(sample_rate);
    state.controls = barely::BuildEngineControlArray(state.sample_rate);
  }

  [[nodiscard]] bool IsValidInstrument(uint32_t instrument_id) const noexcept {
    const uint32_t instrument_index = GetIndex(instrument_id);
    return state.instrument_pool.IsActive(instrument_index) &&
           GetGeneration(instrument_id) == state.instrument_generations[instrument_index];
  }

  [[nodiscard]] bool IsValidPerformer(uint32_t performer_id) const noexcept {
    const uint32_t performer_index = GetIndex(performer_id);
    return state.performer_pool.IsActive(performer_index) &&
           GetGeneration(performer_id) == state.performer_generations[performer_index];
  }

  [[nodiscard]] bool IsValidTask(uint32_t task_id) const noexcept {
    const uint32_t task_index = GetIndex(task_id);
    return state.task_pool.IsActive(task_index) &&
           GetGeneration(task_id) == state.task_generations[task_index];
  }
};

bool BarelyEngine_Create(int32_t sample_rate, BarelyEngine** out_engine) {
  if (sample_rate <= 0) return false;
  if (!out_engine) return false;

  *out_engine = new BarelyEngine(sample_rate);
  return true;
}

bool BarelyEngine_CreateInstrument(BarelyEngine* engine,
                                   const BarelyInstrumentControlOverride* control_overrides,
                                   int32_t control_override_count, uint32_t* out_instrument_id) {
  if (!engine) return false;
  if (!out_instrument_id) return false;

  const uint32_t instrument_index =
      engine->controller.instrument_controller().Acquire(control_overrides, control_override_count);
  if (instrument_index != UINT32_MAX) {
    *out_instrument_id =
        BuildId(instrument_index, engine->state.instrument_generations[instrument_index]);
    return true;
  }
  return false;
}

bool BarelyEngine_CreatePerformer(BarelyEngine* engine, uint32_t* out_performer_id) {
  if (!engine) return false;
  if (!out_performer_id) return false;

  const uint32_t performer_index = engine->controller.performer_controller().Acquire();
  if (performer_index != UINT32_MAX) {
    *out_performer_id =
        BuildId(performer_index, engine->state.performer_generations[performer_index]);
    return true;
  }
  return false;
}

bool BarelyEngine_CreateTask(BarelyEngine* engine, uint32_t performer_id, double position,
                             double duration, int32_t priority, BarelyTaskEventCallback callback,
                             void* user_data, uint32_t* out_task_id) {
  if (!engine) return false;
  if (!engine->IsValidPerformer(performer_id)) return false;
  if (duration <= 0.0) return false;
  if (!out_task_id) return false;

  const uint32_t task_index = engine->controller.performer_controller().AcquireTask(
      GetIndex(performer_id), position, duration, priority, callback, user_data);
  if (task_index != UINT32_MAX) {
    *out_task_id = BuildId(task_index, engine->state.task_generations[task_index]);
    return true;
  }
  return false;
}

bool BarelyEngine_Destroy(BarelyEngine* engine) {
  if (!engine) return false;

  engine->controller.instrument_controller().SetAllNotesOff();
  delete engine;
  return true;
}

bool BarelyEngine_DestroyInstrument(BarelyEngine* engine, uint32_t instrument_id) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument_id)) return false;

  const uint32_t instrument_index = GetIndex(instrument_id);
  engine->controller.instrument_controller().Release(instrument_index);
  engine->state.instrument_generations[instrument_index] =
      GetNextGeneration(engine->state.instrument_generations[instrument_index]);
  return true;
}

bool BarelyEngine_DestroyPerformer(BarelyEngine* engine, uint32_t performer_id) {
  if (!engine) return false;
  if (!engine->IsValidPerformer(performer_id)) return false;

  const uint32_t performer_index = GetIndex(performer_id);
  engine->controller.performer_controller().Release(performer_index);
  engine->state.performer_generations[performer_index] =
      GetNextGeneration(engine->state.performer_generations[performer_index]);
  return true;
}

bool BarelyEngine_DestroyTask(BarelyEngine* engine, uint32_t task_id) {
  if (!engine) return false;
  if (!engine->IsValidTask(task_id)) return false;

  const uint32_t task_index = GetIndex(task_id);
  engine->controller.performer_controller().ReleaseTask(task_index);
  engine->state.task_generations[task_index] =
      GetNextGeneration(engine->state.task_generations[task_index]);
  return true;
}

bool BarelyEngine_GenerateRandomNumber(BarelyEngine* engine, double* out_number) {
  if (!engine) return false;
  if (!out_number) return false;

  *out_number = engine->state.main_rng.Generate();
  return true;
}

bool BarelyEngine_GetControl(const BarelyEngine* engine, BarelyEngineControlType type,
                             float* out_value) {
  if (!engine) return false;
  if (type >= BarelyEngineControlType_kCount) return false;
  if (!out_value) return false;

  *out_value = engine->state.controls[type].value;
  return true;
}

bool BarelyEngine_GetSeed(const BarelyEngine* engine, int32_t* out_seed) {
  if (!engine) return false;
  if (!out_seed) return false;

  *out_seed = engine->state.main_rng.GetSeed();
  return true;
}

bool BarelyEngine_GetTempo(const BarelyEngine* engine, double* out_tempo) {
  if (!engine) return false;
  if (!out_tempo) return false;

  *out_tempo = engine->state.tempo;
  return true;
}

bool BarelyEngine_GetTimestamp(const BarelyEngine* engine, double* out_timestamp) {
  if (!engine) return false;
  if (!out_timestamp) return false;

  *out_timestamp = engine->state.timestamp;
  return true;
}

bool BarelyEngine_Process(BarelyEngine* engine, float* output_samples, int32_t output_channel_count,
                          int32_t output_frame_count, double timestamp) {
  if (!engine) return false;
  if (!output_samples) return false;
  if (output_channel_count <= 0 || output_frame_count <= 0) return false;

  if (output_frame_count > BARELY_MAX_FRAME_COUNT) {
    const uint32_t extra_process_count = output_frame_count / BARELY_MAX_FRAME_COUNT - 1;
    for (uint32_t i = 0; i < extra_process_count; ++i) {
      engine->processor.Process(output_samples, output_channel_count, BARELY_MAX_FRAME_COUNT,
                                timestamp);
      timestamp += barely::FramesToSeconds(engine->state.sample_rate, BARELY_MAX_FRAME_COUNT);
    }
    output_frame_count -= extra_process_count * BARELY_MAX_FRAME_COUNT;
  }
  engine->processor.Process(output_samples, output_channel_count, output_frame_count, timestamp);
  return true;
}

bool BarelyEngine_SetControl(BarelyEngine* engine, BarelyEngineControlType type, float value) {
  if (!engine) return false;
  if (type >= BarelyEngineControlType_kCount) return false;

  engine->controller.SetControl(type, value);
  return true;
}

bool BarelyEngine_SetSeed(BarelyEngine* engine, int32_t seed) {
  if (!engine) return false;

  engine->state.main_rng.SetSeed(seed);
  engine->state.ScheduleMessage(barely::EngineSeedMessage{seed});
  return true;
}

bool BarelyEngine_SetTempo(BarelyEngine* engine, double tempo) {
  if (!engine) return false;
  if (tempo < 0.0) return false;

  engine->state.tempo = tempo;
  return true;
}

bool BarelyEngine_Update(BarelyEngine* engine, double timestamp) {
  if (!engine) return false;

  engine->controller.Update(timestamp);
  return true;
}

bool BarelyInstrument_GetControl(const BarelyEngine* engine, uint32_t instrument_id,
                                 BarelyInstrumentControlType type, float* out_value) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument_id)) return false;
  if (type >= BarelyInstrumentControlType_kCount) return false;
  if (!out_value) return false;

  *out_value = engine->controller.instrument_controller().GetControl(GetIndex(instrument_id), type);
  return true;
}

bool BarelyInstrument_GetNoteControl(const BarelyEngine* engine, uint32_t instrument_id,
                                     float pitch, BarelyNoteControlType type, float* out_value) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument_id)) return false;
  if (type >= BarelyNoteControlType_kCount) return false;
  if (!out_value) return false;

  if (const float* value = engine->controller.instrument_controller().GetNoteControl(
          GetIndex(instrument_id), pitch, type)) {
    *out_value = *value;
    return true;
  }
  return false;
}

bool BarelyInstrument_IsNoteOn(const BarelyEngine* engine, uint32_t instrument_id, float pitch,
                               bool* out_is_note_on) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument_id)) return false;
  if (!out_is_note_on) return false;

  *out_is_note_on =
      engine->controller.instrument_controller().IsNoteOn(GetIndex(instrument_id), pitch);
  return true;
}

bool BarelyInstrument_SetAllNotesOff(BarelyEngine* engine, uint32_t instrument_id) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument_id)) return false;

  engine->controller.instrument_controller().SetAllNotesOff(GetIndex(instrument_id));
  return true;
}

bool BarelyInstrument_SetControl(BarelyEngine* engine, uint32_t instrument_id,
                                 BarelyInstrumentControlType type, float value) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument_id)) return false;
  if (type >= BarelyInstrumentControlType_kCount) return false;

  engine->controller.instrument_controller().SetControl(GetIndex(instrument_id), type, value);
  return true;
}

bool BarelyInstrument_SetNoteControl(BarelyEngine* engine, uint32_t instrument_id, float pitch,
                                     BarelyNoteControlType type, float value) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument_id)) return false;
  if (type >= BarelyNoteControlType_kCount) return false;

  engine->controller.instrument_controller().SetNoteControl(GetIndex(instrument_id), pitch, type,
                                                            value);
  return true;
}

bool BarelyInstrument_SetNoteEventCallback(BarelyEngine* engine, uint32_t instrument_id,
                                           BarelyNoteEventCallback callback, void* user_data) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument_id)) return false;

  engine->controller.instrument_controller().SetNoteEventCallback(GetIndex(instrument_id), callback,
                                                                  user_data);
  return true;
}

bool BarelyInstrument_SetNoteOff(BarelyEngine* engine, uint32_t instrument_id, float pitch) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument_id)) return false;

  engine->controller.instrument_controller().SetNoteOff(GetIndex(instrument_id), pitch);
  return true;
}

bool BarelyInstrument_SetNoteOn(BarelyEngine* engine, uint32_t instrument_id, float pitch,
                                const BarelyNoteControlOverride* note_control_overrides,
                                int32_t note_control_override_count) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument_id)) return false;

  engine->controller.instrument_controller().SetNoteOn(
      GetIndex(instrument_id), pitch, note_control_overrides, note_control_override_count);
  return true;
}

bool BarelyInstrument_SetSampleData(BarelyEngine* engine, uint32_t instrument_id,
                                    const BarelySlice* slices, int32_t slice_count) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument_id)) return false;
  if (slice_count < 0 || (!slices && slice_count > 0)) return false;

  engine->controller.instrument_controller().SetSampleData(GetIndex(instrument_id), slices,
                                                           slice_count);
  return true;
}

bool BarelyPerformer_GetLoopBeginPosition(const BarelyEngine* engine, uint32_t performer_id,
                                          double* out_loop_begin_position) {
  if (!engine) return false;
  if (!engine->IsValidPerformer(performer_id)) return false;
  if (!out_loop_begin_position) return false;

  *out_loop_begin_position = engine->state.GetPerformer(GetIndex(performer_id)).loop_begin_position;
  return true;
}

bool BarelyPerformer_GetLoopLength(const BarelyEngine* engine, uint32_t performer_id,
                                   double* out_loop_length) {
  if (!engine) return false;
  if (!engine->IsValidPerformer(performer_id)) return false;
  if (!out_loop_length) return false;

  *out_loop_length = engine->state.GetPerformer(GetIndex(performer_id)).loop_length;
  return true;
}

bool BarelyPerformer_GetPosition(const BarelyEngine* engine, uint32_t performer_id,
                                 double* out_position) {
  if (!engine) return false;
  if (!engine->IsValidPerformer(performer_id)) return false;
  if (!out_position) return false;

  *out_position = engine->state.GetPerformer(GetIndex(performer_id)).position;
  return true;
}

bool BarelyPerformer_IsLooping(const BarelyEngine* engine, uint32_t performer_id,
                               bool* out_is_looping) {
  if (!engine) return false;
  if (!engine->IsValidPerformer(performer_id)) return false;
  if (!out_is_looping) return false;

  *out_is_looping = engine->state.GetPerformer(GetIndex(performer_id)).is_looping;
  return true;
}

bool BarelyPerformer_IsPlaying(const BarelyEngine* engine, uint32_t performer_id,
                               bool* out_is_playing) {
  if (!engine) return false;
  if (!engine->IsValidPerformer(performer_id)) return false;
  if (!out_is_playing) return false;

  *out_is_playing = engine->state.GetPerformer(GetIndex(performer_id)).is_playing;
  return true;
}

bool BarelyPerformer_SetLoopBeginPosition(BarelyEngine* engine, uint32_t performer_id,
                                          double loop_begin_position) {
  if (!engine) return false;
  if (!engine->IsValidPerformer(performer_id)) return false;

  engine->controller.performer_controller().SetLoopBeginPosition(GetIndex(performer_id),
                                                                 loop_begin_position);
  return true;
}

bool BarelyPerformer_SetLoopLength(BarelyEngine* engine, uint32_t performer_id,
                                   double loop_length) {
  if (!engine) return false;
  if (!engine->IsValidPerformer(performer_id)) return false;

  engine->controller.performer_controller().SetLoopLength(GetIndex(performer_id), loop_length);
  return true;
}

bool BarelyPerformer_SetLooping(BarelyEngine* engine, uint32_t performer_id, bool is_looping) {
  if (!engine) return false;
  if (!engine->IsValidPerformer(performer_id)) return false;

  engine->controller.performer_controller().SetLooping(GetIndex(performer_id), is_looping);
  return true;
}

bool BarelyPerformer_SetPosition(BarelyEngine* engine, uint32_t performer_id, double position) {
  if (!engine) return false;
  if (!engine->IsValidPerformer(performer_id)) return false;

  engine->controller.performer_controller().SetPosition(GetIndex(performer_id), position);
  return true;
}

bool BarelyPerformer_Start(BarelyEngine* engine, uint32_t performer_id) {
  if (!engine) return false;
  if (!engine->IsValidPerformer(performer_id)) return false;

  engine->controller.performer_controller().Start(GetIndex(performer_id));
  return true;
}

bool BarelyPerformer_Stop(BarelyEngine* engine, uint32_t performer_id) {
  if (!engine) return false;
  if (!engine->IsValidPerformer(performer_id)) return false;

  engine->controller.performer_controller().Stop(GetIndex(performer_id));
  return true;
}

bool BarelyTask_GetDuration(const BarelyEngine* engine, uint32_t task_id, double* out_duration) {
  if (!engine) return false;
  if (!engine->IsValidTask(task_id)) return false;
  if (!out_duration) return false;

  *out_duration = engine->state.GetTask(GetIndex(task_id)).duration;
  return true;
}

bool BarelyTask_GetPosition(const BarelyEngine* engine, uint32_t task_id, double* out_position) {
  if (!engine) return false;
  if (!engine->IsValidTask(task_id)) return false;
  if (!out_position) return false;

  *out_position = engine->state.GetTask(GetIndex(task_id)).position;
  return true;
}

bool BarelyTask_GetPriority(const BarelyEngine* engine, uint32_t task_id, int32_t* out_priority) {
  if (!engine) return false;
  if (!engine->IsValidTask(task_id)) return false;
  if (!out_priority) return false;

  *out_priority = engine->state.GetTask(GetIndex(task_id)).priority;
  return true;
}

bool BarelyTask_IsActive(const BarelyEngine* engine, uint32_t task_id, bool* out_is_active) {
  if (!engine) return false;
  if (!engine->IsValidTask(task_id)) return false;
  if (!out_is_active) return false;

  *out_is_active = engine->state.GetTask(GetIndex(task_id)).is_active;
  return true;
}

bool BarelyTask_SetDuration(BarelyEngine* engine, uint32_t task_id, double duration) {
  if (!engine) return false;
  if (!engine->IsValidTask(task_id)) return false;
  if (duration <= 0.0) return false;

  engine->controller.performer_controller().SetTaskDuration(GetIndex(task_id), duration);
  return true;
}

bool BarelyTask_SetEventCallback(BarelyEngine* engine, uint32_t task_id,
                                 BarelyTaskEventCallback callback, void* user_data) {
  if (!engine) return false;
  if (!engine->IsValidTask(task_id)) return false;

  engine->controller.performer_controller().SetTaskEventCallback(GetIndex(task_id), callback,
                                                                 user_data);
  return true;
}

bool BarelyTask_SetPosition(BarelyEngine* engine, uint32_t task_id, double position) {
  if (!engine) return false;
  if (!engine->IsValidTask(task_id)) return false;

  engine->controller.performer_controller().SetTaskPosition(GetIndex(task_id), position);
  return true;
}

bool BarelyTask_SetPriority(BarelyEngine* engine, uint32_t task_id, int32_t priority) {
  if (!engine) return false;
  if (!engine->IsValidTask(task_id)) return false;

  engine->controller.performer_controller().SetTaskPriority(GetIndex(task_id), priority);
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
