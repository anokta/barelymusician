#include <barelymusician.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>

#include "core/arena.h"
#include "core/constants.h"
#include "core/control.h"
#include "core/time.h"
#include "engine/engine_controller.h"
#include "engine/engine_processor.h"
#include "engine/engine_state.h"
#include "engine/message.h"

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

  [[nodiscard]] bool IsValidTrigger(uint32_t trigger_id) const noexcept {
    const uint32_t trigger_index = state.GetIdIndex(trigger_id);
    return state.trigger_pool.IsActive(trigger_index) &&
           state.GetIdGeneration(trigger_index) == state.trigger_generations[trigger_index];
  }
};

bool BarelyEngine_Create(const BarelyEngineConfig* config, BarelyEngine** out_engine) {
  if (!out_engine) return false;
  if (!config) return false;
  if (config->sample_rate <= 0 || config->max_frame_count <= 0 ||
      config->max_instrument_count <= 0 || config->max_trigger_count <= 0 ||
      config->max_note_count <= 0 || config->max_slice_count <= 0 || config->max_voice_count <= 0) {
    return false;
  }

  const size_t size = barely::GetAllocSize<BarelyEngine>(*config);
  std::byte* data = new std::byte[size];
  barely::Arena arena(data, size);
  *out_engine = new (arena.Alloc<BarelyEngine>()) BarelyEngine(arena, *config);
  return true;
}

bool BarelyEngine_CreateInstrument(BarelyEngine* engine, uint32_t* out_instrument_id) {
  if (!engine) return false;
  if (!out_instrument_id) return false;

  const uint32_t instrument_index = engine->controller.instrument_controller().Acquire();
  if (instrument_index != barely::kInvalidIndex) {
    *out_instrument_id = engine->state.BuildId(
        instrument_index, engine->state.instrument_generations[instrument_index]);
    return true;
  }
  return false;
}

bool BarelyEngine_CreateTrigger(BarelyEngine* engine, uint32_t* out_trigger_id) {
  if (!engine) return false;
  if (!out_trigger_id) return false;

  const uint32_t trigger_index = engine->controller.trigger_controller().Acquire();
  if (trigger_index != barely::kInvalidIndex) {
    *out_trigger_id =
        engine->state.BuildId(trigger_index, engine->state.trigger_generations[trigger_index]);
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

  const uint32_t instrument_index = engine->state.GetIdIndex(instrument_id);
  engine->controller.instrument_controller().Release(instrument_index);
  engine->state.instrument_generations[instrument_index] =
      engine->state.GetNextIdGeneration(engine->state.instrument_generations[instrument_index]);
  return true;
}

bool BarelyEngine_DestroyTrigger(BarelyEngine* engine, uint32_t trigger_id) {
  if (!engine) return false;
  if (!engine->IsValidTrigger(trigger_id)) return false;

  const uint32_t trigger_index = engine->state.GetIdIndex(trigger_id);
  engine->controller.trigger_controller().Release(trigger_index);
  engine->state.trigger_generations[trigger_index] =
      engine->state.GetNextIdGeneration(engine->state.trigger_generations[trigger_index]);
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

bool BarelyEngine_GetMaxIdIndex(const BarelyEngine* engine, uint32_t* out_max_id_index) {
  if (!engine) return false;
  if (!out_max_id_index) return false;

  *out_max_id_index = engine->state.GetMaxIdIndex();
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

  engine->processor.Process(output_samples, output_channel_count, output_frame_count, timestamp);
  for (int32_t i = 0; i < output_channel_count * output_frame_count; ++i) {
    output_samples[i] = std::tanh(output_samples[i] * 0.5f);  // soft-clip with -6dB headroom
  }

  return true;
}

bool BarelyEngine_SetControl(BarelyEngine* engine, BarelyEngineControlType type, float value) {
  if (!engine) return false;
  if (type >= BarelyEngineControlType_kCount) return false;

  engine->controller.SetControl(type, value);
  return true;
}

bool BarelyEngine_ResetSeed(BarelyEngine* engine, int32_t seed) {
  if (!engine) return false;

  engine->state.main_rng.ResetSeed(seed);
  engine->state.ScheduleMessage(barely::EngineSeedMessage{seed});
  return true;
}

bool BarelyEngine_SetTempo(BarelyEngine* engine, double tempo) {
  if (!engine) return false;

  engine->state.tempo = std::max(tempo, 0.0);
  return true;
}

bool BarelyEngine_Update(BarelyEngine* engine, double timestamp) {
  if (!engine) return false;

  engine->controller.Update(timestamp);
  return true;
}

bool BarelyInstrument_CancelAllScheduled(BarelyEngine* engine, uint32_t instrument_id) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument_id)) return false;

  // TODO(schedule): Implement
  return false;
}

bool BarelyInstrument_GetControl(const BarelyEngine* engine, uint32_t instrument_id,
                                 BarelyInstrumentControlType type, float* out_value) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument_id)) return false;
  if (type >= BarelyInstrumentControlType_kCount) return false;
  if (!out_value) return false;

  *out_value = engine->controller.instrument_controller().GetControl(
      engine->state.GetIdIndex(instrument_id), type);
  return true;
}

bool BarelyInstrument_GetNoteControl(const BarelyEngine* engine, uint32_t instrument_id,
                                     float pitch, BarelyNoteControlType type, float* out_value) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument_id)) return false;
  if (type >= BarelyNoteControlType_kCount) return false;
  if (!out_value) return false;

  if (const float* value = engine->controller.instrument_controller().GetNoteControl(
          engine->state.GetIdIndex(instrument_id), pitch, type)) {
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

  *out_is_note_on = engine->controller.instrument_controller().IsNoteOn(
      engine->state.GetIdIndex(instrument_id), pitch);
  return true;
}

bool BarelyInstrument_ScheduleControl(BarelyEngine* engine, uint32_t instrument_id,
                                      BarelyInstrumentControlType type, float value, double offset,
                                      double duration) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument_id)) return false;
  if (type >= BarelyInstrumentControlType_kCount) return false;
  if (offset < 0.0) return false;
  if (duration < 0.0) return false;

  // TODO(schedule): Implement
  value;
  return false;
}

bool BarelyInstrument_ScheduleNote(BarelyEngine* engine, uint32_t instrument_id, float pitch,
                                   double offset, double duration) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument_id)) return false;
  if (offset < 0.0) return false;
  if (duration <= 0.0) return false;

  // TODO(schedule): Implement
  pitch;
  return false;
}

bool BarelyInstrument_ScheduleNoteControl(BarelyEngine* engine, uint32_t instrument_id, float pitch,
                                          BarelyNoteControlType type, float value, double offset,
                                          double duration) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument_id)) return false;
  if (type >= BarelyNoteControlType_kCount) return false;
  if (offset < 0.0) return false;
  if (duration < 0.0) return false;

  // TODO(schedule): Implement
  pitch;
  value;
  return false;
}

bool BarelyInstrument_SetAllNotesOff(BarelyEngine* engine, uint32_t instrument_id) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument_id)) return false;

  engine->controller.instrument_controller().SetAllNotesOff(
      engine->state.GetIdIndex(instrument_id));
  return true;
}

bool BarelyInstrument_SetControl(BarelyEngine* engine, uint32_t instrument_id,
                                 BarelyInstrumentControlType type, float value) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument_id)) return false;
  if (type >= BarelyInstrumentControlType_kCount) return false;

  engine->controller.instrument_controller().SetControl(engine->state.GetIdIndex(instrument_id),
                                                        type, value);
  return true;
}

bool BarelyInstrument_SetNoteControl(BarelyEngine* engine, uint32_t instrument_id, float pitch,
                                     BarelyNoteControlType type, float value) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument_id)) return false;
  if (type >= BarelyNoteControlType_kCount) return false;

  engine->controller.instrument_controller().SetNoteControl(engine->state.GetIdIndex(instrument_id),
                                                            pitch, type, value);
  return true;
}

bool BarelyInstrument_SetNoteEventCallback(BarelyEngine* engine, uint32_t instrument_id,
                                           BarelyNoteEventCallback callback, void* user_data) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument_id)) return false;

  engine->controller.instrument_controller().SetNoteEventCallback(
      engine->state.GetIdIndex(instrument_id), callback, user_data);
  return true;
}

bool BarelyInstrument_SetNoteOff(BarelyEngine* engine, uint32_t instrument_id, float pitch) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument_id)) return false;

  engine->controller.instrument_controller().SetNoteOff(engine->state.GetIdIndex(instrument_id),
                                                        pitch);
  return true;
}

bool BarelyInstrument_SetNoteOn(BarelyEngine* engine, uint32_t instrument_id, float pitch) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument_id)) return false;

  engine->controller.instrument_controller().SetNoteOn(engine->state.GetIdIndex(instrument_id),
                                                       pitch);
  return true;
}

bool BarelyInstrument_SetSampleData(BarelyEngine* engine, uint32_t instrument_id,
                                    const BarelySlice* slices, int32_t slice_count) {
  if (!engine) return false;
  if (!engine->IsValidInstrument(instrument_id)) return false;
  if (slice_count < 0 || (!slices && slice_count > 0)) return false;

  engine->controller.instrument_controller().SetSampleData(engine->state.GetIdIndex(instrument_id),
                                                           slices, slice_count);
  return true;
}

bool BarelyTrigger_IsPlaying(const BarelyEngine* engine, uint32_t trigger_id,
                             bool* out_is_playing) {
  if (!engine) return false;
  if (!engine->IsValidTrigger(trigger_id)) return false;
  if (!out_is_playing) return false;

  *out_is_playing = engine->state.GetTrigger((engine->state.GetIdIndex(trigger_id))).is_playing;
  return true;
}

bool BarelyTrigger_SetCallback(BarelyEngine* engine, uint32_t trigger_id,
                               BarelyTriggerEventCallback callback, void* user_data) {
  if (!engine) return false;
  if (!engine->IsValidTrigger(trigger_id)) return false;

  engine->controller.trigger_controller().SetCallback(engine->state.GetIdIndex(trigger_id),
                                                      callback, user_data);
  return true;
}

bool BarelyTrigger_Start(BarelyEngine* engine, uint32_t trigger_id, double offset,
                         double duration) {
  if (!engine) return false;
  if (!engine->IsValidTrigger(trigger_id)) return false;

  engine->controller.trigger_controller().Start(engine->state.GetIdIndex(trigger_id), offset,
                                                duration);
  return true;
}

bool BarelyTrigger_Stop(BarelyEngine* engine, uint32_t trigger_id) {
  if (!engine) return false;
  if (!engine->IsValidTrigger(trigger_id)) return false;

  engine->controller.trigger_controller().Stop(engine->state.GetIdIndex(trigger_id));
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
