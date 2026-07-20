#include <barelymusician.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <memory>

#include "core/arena.h"
#include "core/constants.h"
#include "core/scale.h"
#include "core/time.h"
#include "engine/cmd.h"
#include "engine/engine_controller.h"
#include "engine/engine_processor.h"
#include "engine/engine_state.h"

struct BarelyEngine {
  barely::EngineState state;
  barely::EngineController controller;
  barely::EngineProcessor processor;

  BarelyEngine(barely::Arena& arena, const BarelyEngineConfig& config) noexcept
      : state(arena, config), controller(state), processor(state) {}

  [[nodiscard]] bool IsValidInstrument(uint32_t instrument_id) const noexcept {
    const uint32_t instrument_index = state.GetIdIndex(instrument_id);
    return state.instrument_pool.IsActive(instrument_index) &&
           state.GetIdGeneration(instrument_id) == state.instrument_generations[instrument_index];
  }

  [[nodiscard]] bool IsValidPerformer(uint32_t performer_id) const noexcept {
    const uint32_t performer_index = state.GetIdIndex(performer_id);
    return state.performer_pool.IsActive(performer_index) &&
           state.GetIdGeneration(performer_id) == state.performer_generations[performer_index];
  }

  [[nodiscard]] bool IsValidTask(uint32_t task_id) const noexcept {
    const uint32_t task_index = state.GetIdIndex(task_id);
    return state.task_pool.IsActive(task_index) &&
           state.GetIdGeneration(task_id) == state.task_generations[task_index];
  }
};

int32_t BarelyEngineConfig_GetRequiredAllocationSize(const BarelyEngineConfig* config) {
  return (config != nullptr) ? static_cast<int32_t>(barely::GetAllocSize<BarelyEngine>(*config))
                             : 0;
}

double BarelyQuantization_GetPosition(const BarelyQuantization* quantization, double position) {
  return (quantization != nullptr)
             ? barely::Quantize(position, std::max(quantization->subdivision, 1),
                                std::clamp(quantization->amount, 0.0f, 1.0f))
             : position;
}

float BarelyScale_GetPitch(const BarelyScale* scale, int32_t degree) {
  return (scale != nullptr) ? barely::GetPitch(*scale, degree) : 0.0f;
}

BarelyEngine* BarelyEngine_Create(const BarelyEngineConfig* config, void* allocation,
                                  int32_t allocation_size) {
  if (!config || config->sample_rate <= 0 || config->max_instrument_count <= 0 ||
      config->max_performer_count <= 0 || config->max_task_count <= 0 ||
      config->max_command_count <= 0 || config->max_frame_count <= 0 ||
      config->max_slice_count <= 0 || config->max_voice_count <= 0) {
    return nullptr;
  }

  const size_t size = barely::GetAllocSize<BarelyEngine>(*config);
  if (allocation == nullptr || static_cast<size_t>(allocation_size) < size) return nullptr;

  barely::Arena arena(allocation, size);
  return ::new (arena.Alloc<BarelyEngine>()) BarelyEngine(arena, *config);
}

uint32_t BarelyEngine_CreateInstrument(BarelyEngine* engine) {
  if (engine != nullptr) {
    const uint32_t instrument_index = engine->controller.instrument_controller().Acquire();
    if (instrument_index != barely::kInvalidIndex) {
      return engine->state.BuildId(instrument_index,
                                   engine->state.instrument_generations[instrument_index]);
    }
  }
  return 0;
}

uint32_t BarelyEngine_CreatePerformer(BarelyEngine* engine) {
  if (engine != nullptr) {
    const uint32_t performer_index = engine->controller.performer_controller().Acquire();
    if (performer_index != barely::kInvalidIndex) {
      return engine->state.BuildId(performer_index,
                                   engine->state.performer_generations[performer_index]);
    }
  }
  return 0;
}

void BarelyEngine_Destroy(BarelyEngine* engine) {
  if (engine != nullptr) {
    std::destroy_at(engine);
  }
}

double BarelyEngine_GenerateRandomNumber(BarelyEngine* engine) {
  return (engine != nullptr) ? engine->state.main_rng.Generate() : 0.0;
}

double BarelyEngine_GetTimestamp(const BarelyEngine* engine) {
  return (engine != nullptr) ? engine->state.timestamp : 0.0;
}

void BarelyEngine_Process(BarelyEngine* engine, float* output_samples, int32_t output_channel_count,
                          int32_t output_frame_count, double timestamp) {
  if (!engine || !output_samples || output_channel_count <= 0 || output_frame_count <= 0) return;

  engine->processor.Process(output_samples, output_channel_count, output_frame_count, timestamp);
  for (int32_t i = 0; i < output_channel_count * output_frame_count; ++i) {
    output_samples[i] = std::tanh(output_samples[i] * 0.5f);  // soft-clip with -6dB headroom
  }
}

void BarelyEngine_SetControl(BarelyEngine* engine, BarelyEngineControlType type, float value) {
  if (engine != nullptr && type < BarelyEngineControlType_kCount) {
    engine->controller.SetControl(type, value);
  }
}

void BarelyEngine_ResetSeed(BarelyEngine* engine, int32_t seed) {
  if (engine != nullptr) {
    engine->state.main_rng.ResetSeed(seed);
    engine->state.ScheduleCmd(barely::EngineSeedCmd{seed});
  }
}

void BarelyEngine_SetTempo(BarelyEngine* engine, double tempo) {
  if (engine != nullptr) {
    engine->state.tempo = std::max(tempo, 0.0);
  }
}

void BarelyEngine_Update(BarelyEngine* engine, double timestamp) {
  if (engine != nullptr) {
    engine->controller.Update(timestamp);
  }
}

void BarelyInstrument_Destroy(BarelyEngine* engine, uint32_t instrument_id) {
  if (engine != nullptr && engine->IsValidInstrument(instrument_id)) {
    const uint32_t instrument_index = engine->state.GetIdIndex(instrument_id);
    engine->controller.instrument_controller().Release(instrument_index);
    engine->state.instrument_generations[instrument_index] =
        engine->state.GetNextIdGeneration(engine->state.instrument_generations[instrument_index]);
  }
}

void BarelyInstrument_SetControl(BarelyEngine* engine, uint32_t instrument_id,
                                 BarelyInstrumentControlType type, float value) {
  if (engine != nullptr && engine->IsValidInstrument(instrument_id) &&
      type < BarelyInstrumentControlType_kCount) {
    engine->controller.instrument_controller().SetControl(engine->state.GetIdIndex(instrument_id),
                                                          type, value);
  }
}

void BarelyInstrument_SetNoteControl(BarelyEngine* engine, uint32_t instrument_id, float pitch,
                                     BarelyNoteControlType type, float value) {
  if (engine != nullptr && engine->IsValidInstrument(instrument_id) &&
      type < BarelyNoteControlType_kCount) {
    engine->controller.instrument_controller().SetNoteControl(
        engine->state.GetIdIndex(instrument_id), pitch, type, value);
  }
}

void BarelyInstrument_SetNoteOff(BarelyEngine* engine, uint32_t instrument_id, float pitch) {
  if (engine != nullptr && engine->IsValidInstrument(instrument_id)) {
    engine->controller.instrument_controller().SetNoteOff(engine->state.GetIdIndex(instrument_id),
                                                          pitch);
  }
}

void BarelyInstrument_SetNoteOn(BarelyEngine* engine, uint32_t instrument_id, float pitch) {
  if (engine != nullptr && engine->IsValidInstrument(instrument_id)) {
    engine->controller.instrument_controller().SetNoteOn(engine->state.GetIdIndex(instrument_id),
                                                         pitch);
  }
}

void BarelyInstrument_SetSampleData(BarelyEngine* engine, uint32_t instrument_id,
                                    const BarelySlice* slices, int32_t slice_count) {
  if (engine != nullptr && engine->IsValidInstrument(instrument_id) && slice_count >= 0 &&
      (slices != nullptr || slice_count == 0)) {
    engine->controller.instrument_controller().SetSampleData(
        engine->state.GetIdIndex(instrument_id), slices, slice_count);
  }
}

uint32_t BarelyPerformer_CreateTask(BarelyEngine* engine, uint32_t performer_id, double position,
                                    double duration, int32_t priority, BarelyTaskCallback callback,
                                    void* user_data) {
  if (engine != nullptr && engine->IsValidPerformer(performer_id)) {
    const uint32_t task_index = engine->controller.performer_controller().AcquireTask(
        engine->state.GetIdIndex(performer_id), position, std::max(duration, 0.0), priority,
        callback, user_data);
    if (task_index != barely::kInvalidIndex) {
      return engine->state.BuildId(task_index, engine->state.task_generations[task_index]);
    }
  }
  return 0;
}

void BarelyPerformer_Destroy(BarelyEngine* engine, uint32_t performer_id) {
  if (engine != nullptr && engine->IsValidPerformer(performer_id)) {
    const uint32_t performer_index = engine->state.GetIdIndex(performer_id);
    engine->controller.performer_controller().Release(performer_index);
    engine->state.performer_generations[performer_index] =
        engine->state.GetNextIdGeneration(engine->state.performer_generations[performer_index]);
  }
}

double BarelyPerformer_GetPosition(const BarelyEngine* engine, uint32_t performer_id) {
  return (engine != nullptr && engine->IsValidPerformer(performer_id))
             ? engine->state.GetPerformer(engine->state.GetIdIndex(performer_id)).position
             : 0.0;
}

void BarelyPerformer_SetLoopBeginPosition(BarelyEngine* engine, uint32_t performer_id,
                                          double loop_begin_position) {
  if (engine != nullptr && engine->IsValidPerformer(performer_id)) {
    engine->controller.performer_controller().SetLoopBeginPosition(
        engine->state.GetIdIndex(performer_id), loop_begin_position);
  }
}

void BarelyPerformer_SetLoopLength(BarelyEngine* engine, uint32_t performer_id,
                                   double loop_length) {
  if (engine != nullptr && engine->IsValidPerformer(performer_id)) {
    engine->controller.performer_controller().SetLoopLength(engine->state.GetIdIndex(performer_id),
                                                            loop_length);
  }
}

void BarelyPerformer_SetLooping(BarelyEngine* engine, uint32_t performer_id, bool is_looping) {
  if (engine != nullptr && engine->IsValidPerformer(performer_id)) {
    engine->controller.performer_controller().SetLooping(engine->state.GetIdIndex(performer_id),
                                                         is_looping);
  }
}

void BarelyPerformer_SetPosition(BarelyEngine* engine, uint32_t performer_id, double position) {
  if (engine != nullptr && engine->IsValidPerformer(performer_id)) {
    engine->controller.performer_controller().SetPosition(engine->state.GetIdIndex(performer_id),
                                                          position);
  }
}

void BarelyPerformer_Start(BarelyEngine* engine, uint32_t performer_id) {
  if (engine != nullptr && engine->IsValidPerformer(performer_id)) {
    engine->controller.performer_controller().Start(engine->state.GetIdIndex(performer_id));
  }
}

void BarelyPerformer_Stop(BarelyEngine* engine, uint32_t performer_id) {
  if (engine != nullptr && engine->IsValidPerformer(performer_id)) {
    engine->controller.performer_controller().Stop(engine->state.GetIdIndex(performer_id));
  }
}

void BarelyTask_Destroy(BarelyEngine* engine, uint32_t task_id) {
  if (engine != nullptr && engine->IsValidTask(task_id)) {
    const uint32_t task_index = engine->state.GetIdIndex(task_id);
    engine->controller.performer_controller().ReleaseTask(task_index);
    engine->state.task_generations[task_index] =
        engine->state.GetNextIdGeneration(engine->state.task_generations[task_index]);
  }
}

bool BarelyTask_IsActive(const BarelyEngine* engine, uint32_t task_id) {
  if (engine != nullptr && engine->IsValidTask(task_id)) {
    return engine->state.GetTask(engine->state.GetIdIndex(task_id)).is_active;
  }
  return false;
}

void BarelyTask_SetCallback(BarelyEngine* engine, uint32_t task_id, BarelyTaskCallback callback,
                            void* user_data) {
  if (engine != nullptr && engine->IsValidTask(task_id)) {
    engine->controller.performer_controller().SetTaskCallback(engine->state.GetIdIndex(task_id),
                                                              callback, user_data);
  }
}

void BarelyTask_SetDuration(BarelyEngine* engine, uint32_t task_id, double duration) {
  if (engine != nullptr && engine->IsValidTask(task_id)) {
    engine->controller.performer_controller().SetTaskDuration(engine->state.GetIdIndex(task_id),
                                                              std::max(duration, 0.0));
  }
}

void BarelyTask_SetPosition(BarelyEngine* engine, uint32_t task_id, double position) {
  if (engine != nullptr && engine->IsValidTask(task_id)) {
    engine->controller.performer_controller().SetTaskPosition(engine->state.GetIdIndex(task_id),
                                                              position);
  }
}

void BarelyTask_SetPriority(BarelyEngine* engine, uint32_t task_id, int32_t priority) {
  if (engine != nullptr && engine->IsValidTask(task_id)) {
    engine->controller.performer_controller().SetTaskPriority(engine->state.GetIdIndex(task_id),
                                                              priority);
  }
}
