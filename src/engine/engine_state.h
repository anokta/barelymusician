#ifndef BARELYMUSICIAN_ENGINE_ENGINE_STATE_H_
#define BARELYMUSICIAN_ENGINE_ENGINE_STATE_H_

#include <barelymusician.h>

#include <array>
#include <atomic>
#include <bit>
#include <cstdint>

#include "core/arena.h"
#include "core/control.h"
#include "core/pool.h"
#include "core/rng.h"
#include "core/time.h"
#include "dsp/compressor.h"
#include "dsp/delay_filter.h"
#include "dsp/reverb.h"
#include "dsp/sidechain.h"
#include "engine/effect_params.h"
#include "engine/instrument_params.h"
#include "engine/instrument_state.h"
#include "engine/message.h"
#include "engine/message_queue.h"
#include "engine/note_state.h"
#include "engine/performer_state.h"
#include "engine/slice_pool.h"
#include "engine/task_state.h"
#include "engine/voice_state.h"

namespace barely {

static_assert((kInvalidIndex + 1) == 0);

struct EngineState {
  EngineState(Arena& arena, const BarelyEngineConfig& config) noexcept
      : delay_filter(arena, std::bit_ceil(static_cast<uint32_t>(std::ceil(
                                static_cast<float>(config.sample_rate) *
                                controls[BarelyEngineControlType_kDelayTime].max_value)))),
        reverb(arena, static_cast<float>(config.sample_rate)),

        instrument_pool(arena, config.max_instrument_count),
        note_pool(arena, config.max_note_count),
        performer_pool(arena, config.max_performer_count),
        task_pool(arena, config.max_task_count),
        voice_pool(arena, config.max_voice_count),
        slice_pool(arena, config.max_slice_count),
        message_queue(arena),

        instrument_generations(arena.AllocArray<uint32_t>(config.max_instrument_count)),
        performer_generations(arena.AllocArray<uint32_t>(config.max_performer_count)),
        task_generations(arena.AllocArray<uint32_t>(config.max_task_count)),

        instrument_params(arena.AllocArray<InstrumentParams>(config.max_instrument_count)),
        note_to_voice(arena.AllocArray<uint32_t>(config.max_note_count)),
        queued_sample_data_counts(
            arena.AllocArray<std::atomic<int32_t>>(config.max_instrument_count)),
        temp_samples(arena.AllocArray<float>(kStereoChannelCount * config.max_frame_count)),

        sample_rate(static_cast<float>(config.sample_rate)),
        smoothing_coeff(GetCoefficient(sample_rate, /*50ms*/ 0.05f)),

        id_index_bit_count(std::bit_width(std::bit_ceil(static_cast<uint32_t>(std::max(
            {config.max_instrument_count, config.max_performer_count, config.max_task_count}))))),
        max_id_index((1u << id_index_bit_count) - 1u),
        max_id_generation((1u << (32u - id_index_bit_count)) - 1u),

        max_frame_count(static_cast<uint32_t>(config.max_frame_count)) {
    assert(id_index_bit_count < 32);
    assert(sample_rate > 0.0f);
  }

  // Array of engine controls.
  std::array<Control, BarelyEngineControlType_kCount> controls = {
      BARELY_ENGINE_CONTROL_TYPES(EngineControlType, BARELY_DEFINE_CONTROL)};

  MainRng main_rng;
  AudioRng audio_rng;

  EffectParams current_params = {};
  EffectParams target_params = {};

  Compressor comp = {};
  Sidechain sidechain = {};

  DelayFilter delay_filter;
  Reverb reverb;

  Pool<InstrumentState> instrument_pool;
  Pool<NoteState> note_pool;
  Pool<PerformerState> performer_pool;
  Pool<TaskState> task_pool;
  Pool<VoiceState> voice_pool;

  SlicePool slice_pool;

  MessageQueue message_queue;

  uint32_t* instrument_generations = nullptr;
  uint32_t* performer_generations = nullptr;
  uint32_t* task_generations = nullptr;

  InstrumentParams* instrument_params = nullptr;
  uint32_t* note_to_voice = nullptr;

  std::atomic<int32_t>* queued_sample_data_counts = nullptr;  // queued messages per instrument

  float* temp_samples = nullptr;

  double tempo = 120.0;      // beats per minute
  double timestamp = 0.0;    // seconds
  float sample_rate = 0.0f;  // hertz

  float smoothing_coeff = 0.0f;

  uint32_t id_index_bit_count = 0;
  uint32_t max_id_index = 0;
  uint32_t max_id_generation = 0;

  uint32_t max_frame_count = 0;

  std::atomic_bool process_fence;

  void Approach() noexcept {
    current_params.comp_params.Approach(target_params.comp_params, smoothing_coeff);
    current_params.sidechain_params.Approach(target_params.sidechain_params, smoothing_coeff);
    current_params.delay_params.Approach(target_params.delay_params, smoothing_coeff);
    current_params.reverb_params.Approach(target_params.reverb_params, smoothing_coeff);
    ApproachValue(current_params.gain, target_params.gain, smoothing_coeff);
  }

  void ScheduleMessage(Message message) noexcept {
    message_queue.Add(SecondsToFrames(sample_rate, timestamp), message);
  }

  [[nodiscard]] uint32_t SelectSlice(uint32_t instrument_index, uint32_t first_slice_index,
                                     float pitch) noexcept {
    if (instrument_index == kInvalidIndex ||
        queued_sample_data_counts[instrument_index].load(std::memory_order_acquire) > 0) {
      return kInvalidIndex;
    }
    return slice_pool.Select(first_slice_index, pitch, audio_rng);
  }

  [[nodiscard]] uint32_t BuildId(uint32_t index, uint32_t generation) const noexcept {
    return (generation << id_index_bit_count) | (index + 1);
  }

  [[nodiscard]] uint32_t GetIdGeneration(uint32_t id) const noexcept {
    return id >> id_index_bit_count;
  }

  [[nodiscard]] uint32_t GetIdIndex(uint32_t id) const noexcept { return (id & max_id_index) - 1; }

  [[nodiscard]] uint32_t GetNextIdGeneration(uint32_t generation) const noexcept {
    return (generation + 1) & max_id_generation;
  }

  [[nodiscard]] uint32_t GetMaxIdIndex() const noexcept { return max_id_index; }

  [[nodiscard]] const SliceState* GetSlice(uint32_t instrument_index,
                                           uint32_t slice_index) const noexcept {
    if (instrument_index == kInvalidIndex ||
        queued_sample_data_counts[instrument_index].load(std::memory_order_acquire) > 0) {
      return nullptr;
    }
    return slice_pool.Get(slice_index);
  }

  [[nodiscard]] InstrumentState& GetInstrument(uint32_t instrument_index) noexcept {
    return instrument_pool.Get(instrument_index);
  }
  [[nodiscard]] const InstrumentState& GetInstrument(uint32_t instrument_index) const noexcept {
    return instrument_pool.Get(instrument_index);
  }

  [[nodiscard]] PerformerState& GetPerformer(uint32_t performer_index) noexcept {
    return performer_pool.Get(performer_index);
  }
  [[nodiscard]] const PerformerState& GetPerformer(uint32_t performer_index) const noexcept {
    return performer_pool.Get(performer_index);
  }

  [[nodiscard]] TaskState& GetTask(uint32_t task_index) noexcept {
    return task_pool.Get(task_index);
  }
  [[nodiscard]] const TaskState& GetTask(uint32_t task_index) const noexcept {
    return task_pool.Get(task_index);
  }

  [[nodiscard]] VoiceState& GetVoice(uint32_t voice_index) noexcept {
    return voice_pool.Get(voice_index);
  }
  [[nodiscard]] const VoiceState& GetVoice(uint32_t voice_index) const noexcept {
    return voice_pool.Get(voice_index);
  }
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_ENGINE_STATE_H_
