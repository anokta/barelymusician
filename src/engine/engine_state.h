#ifndef BARELYMUSICIAN_ENGINE_ENGINE_STATE_H_
#define BARELYMUSICIAN_ENGINE_ENGINE_STATE_H_

#include <barelymusician.h>

#include <array>
#include <atomic>
#include <cstdint>

#include "core/arena.h"
#include "core/control.h"
#include "core/pool.h"
#include "core/pool2.h"
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

struct EngineState {
  void Init(Arena& arena) noexcept {
    temp_samples = arena.AllocArray<float>(kStereoChannelCount * BARELY_MAX_FRAME_COUNT);
    message_queue.Init(arena);
    voice_pool.Init(arena, BARELY_MAX_VOICE_COUNT);
  }

  // Temp output samples.
  float* temp_samples = nullptr;

  // Performer pool.
  Pool<PerformerState, BARELY_MAX_PERFORMER_COUNT> performer_pool = {};
  std::array<uint32_t, BARELY_MAX_PERFORMER_COUNT> performer_generations = {};

  // Task pool.
  Pool<TaskState, BARELY_MAX_TASK_COUNT> task_pool = {};
  std::array<uint32_t, BARELY_MAX_TASK_COUNT> task_generations = {};

  // Instrument pool.
  Pool<InstrumentState, BARELY_MAX_INSTRUMENT_COUNT> instrument_pool = {};
  std::array<uint32_t, BARELY_MAX_INSTRUMENT_COUNT> instrument_generations = {};

  // Note pool.
  Pool<NoteState, BARELY_MAX_NOTE_COUNT> note_pool = {};

  // Slice pool.
  SlicePool slice_pool = {};

  // Array of engine controls.
  std::array<Control, BarelyEngineControlType_kCount> controls = {
      BARELY_ENGINE_CONTROL_TYPES(EngineControlType, BARELY_DEFINE_CONTROL)};

  // Random number generator for the main thread.
  MainRng main_rng = {};

  // Message queue.
  MessageQueue message_queue = {};

  // Process fence.
  std::atomic_flag process_fence = {};

  // Array of instrument parameters.
  std::array<InstrumentParams, BARELY_MAX_INSTRUMENT_COUNT> instrument_params = {};

  // Number of queued sample data messages per instrument.
  std::array<std::atomic<int32_t>, BARELY_MAX_INSTRUMENT_COUNT> queued_sample_data_counts = {};

  // Maps note indices to voice indices.
  std::array<uint32_t, BARELY_MAX_NOTE_COUNT> note_to_voice = {};

  // Voice pool.
  Pool2<VoiceState> voice_pool = {};

  // Random number generator for the audio thread.
  AudioRng audio_rng = {};

  // Current parameters.
  EffectParams current_params = {};

  // Target parameters.
  EffectParams target_params = {};

  // Compressor.
  Compressor comp = {};

  // Delay filter.
  DelayFilter delay_filter = {};

  // Reverb.
  Reverb reverb = {};

  // Sidechain.
  Sidechain sidechain = {};

  // Tempo in beats per minute.
  double tempo = 120.0;

  // Timestamp in seconds.
  double timestamp = 0.0;

  // Sampling rate in hertz.
  float sample_rate = 0.0f;

  // Smoothing coefficient.
  float smoothing_coeff = 0.0f;

  void Approach() noexcept {
    current_params.comp_params.Approach(target_params.comp_params, smoothing_coeff);
    current_params.sidechain_params.Approach(target_params.sidechain_params, smoothing_coeff);
    current_params.delay_params.Approach(target_params.delay_params, smoothing_coeff);
    current_params.reverb_params.Approach(target_params.reverb_params, smoothing_coeff);
    ApproachValue(current_params.gain, target_params.gain, smoothing_coeff);
  }

  void ScheduleMessage(Message message) noexcept {
    message_queue.Add(SecondsToFrames(sample_rate, timestamp), std::move(message));
  }

  [[nodiscard]] uint32_t SelectSlice(uint32_t instrument_index, uint32_t first_slice_index,
                                     float pitch) noexcept {
    if (instrument_index == kInvalidIndex ||
        queued_sample_data_counts[instrument_index].load(std::memory_order_acquire) > 0) {
      return kInvalidIndex;
    }
    return slice_pool.Select(first_slice_index, pitch, audio_rng);
  }

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
