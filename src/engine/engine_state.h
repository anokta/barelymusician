#ifndef BARELYMUSICIAN_ENGINE_ENGINE_STATE_H_
#define BARELYMUSICIAN_ENGINE_ENGINE_STATE_H_

#include <barelymusician.h>

#include <array>
#include <cstdint>

#include "core/control.h"
#include "core/pool.h"
#include "core/rng.h"
#include "core/time.h"
#include "dsp/compressor.h"
#include "dsp/delay_filter.h"
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

// Engine control array.
using EngineControlArray = std::array<Control, BarelyEngineControlType_kCount>;

// Returns an engine control array.
inline EngineControlArray BuildEngineControlArray(float sample_rate) noexcept {
  return {
      Control(0.0f, 0.0f, 1.0f),                // kCompressorMix
      Control(0.0f, 0.0f, 10.0f),               // kCompressorAttack
      Control(0.0f, 0.0f, 10.0f),               // kCompressorRelease
      Control(1.0f, 0.0f, 1.0f),                // kCompressorThreshold
      Control(1.0f, 1.0f, 64.0f),               // kCompressorRatio
      Control(1.0f, 0.0f, 1.0f),                // kDelayMix
      Control(0.0f, 0.0f, 10.0f),               // kDelayTime
      Control(0.0f, 0.0f, 1.0f),                // kDelayFeedback
      Control(sample_rate, 0.0f, sample_rate),  // kDelayLowPassFrequency
      Control(0.0f, 0.0f, sample_rate),         // kDelayHighPassFrequency
      Control(1.0f, 0.0f, 1.0f),                // kSidechainMix
      Control(0.0f, 0.0f, 10.0f),               // kSidechainAttack
      Control(0.0f, 0.0f, 10.0f),               // kSidechainRelease
      Control(1.0f, 0.0f, 1.0f),                // kSidechainThreshold
      Control(1.0f, 1.0f, 64.0f),               // kSidechainRatio
  };
}

struct EngineState {
  // Performer pool.
  Pool<PerformerState, BARELYMUSICIAN_MAX_PERFORMER_COUNT> performer_pool = {};

  // Array of performer generations.
  std::array<uint32_t, BARELYMUSICIAN_MAX_PERFORMER_COUNT> performer_generations = {};

  // Task pool.
  Pool<TaskState, BARELYMUSICIAN_MAX_TASK_COUNT> task_pool = {};

  // Array of task generations.
  std::array<uint32_t, BARELYMUSICIAN_MAX_TASK_COUNT> task_generations = {};

  // Instrument pool.
  Pool<InstrumentState, BARELYMUSICIAN_MAX_INSTRUMENT_COUNT> instrument_pool = {};

  // Array of instrument generations.
  std::array<uint32_t, BARELYMUSICIAN_MAX_INSTRUMENT_COUNT> instrument_generations = {};

  // Note pool.
  Pool<NoteState, BARELYMUSICIAN_MAX_NOTE_COUNT> note_pool = {};

  // Slice pool.
  SlicePool slice_pool = {};

  // Array of engine controls.
  EngineControlArray controls = {};

  // Random number generator for the main thread.
  MainRng main_rng = {};

  // Message queue.
  MessageQueue message_queue = {};

  // Array of instrument parameters.
  std::array<InstrumentParams, BARELYMUSICIAN_MAX_INSTRUMENT_COUNT> instrument_params = {};

  // Maps note indices to voice indices.
  std::array<uint32_t, BARELYMUSICIAN_MAX_NOTE_COUNT> note_to_voice = {};

  // Voice pool.
  Pool<VoiceState, BARELYMUSICIAN_MAX_VOICE_COUNT> voice_pool = {};

  // Random number generator for the audio thread.
  AudioRng audio_rng = {};

  // Current parameters.
  EffectParams current_params = {};

  // Target parameters.
  EffectParams target_params = {};

  // Compressor.
  Compressor compressor = {};

  // Delay filter.
  DelayFilter delay_filter = {};

  // Sidechain.
  Sidechain sidechain = {};

  // Tempo in beats per minute.
  double tempo = 120.0;

  // Timestamp in seconds.
  double timestamp = 0.0;

  // Sampling rate in hertz.
  float sample_rate = 0.0f;

  // Approaches parameters.
  void Approach() noexcept {
    current_params.compressor_params.Approach(target_params.compressor_params);
    current_params.delay_params.Approach(target_params.delay_params);
    ApproachValue(current_params.sidechain_mix, target_params.sidechain_mix);
    ApproachValue(current_params.sidechain_threshold_db, target_params.sidechain_threshold_db);
    ApproachValue(current_params.sidechain_ratio, target_params.sidechain_ratio);
  }

  // Schedules a new message in the queue.
  void ScheduleMessage(Message message) noexcept {
    message_queue.Add(SecondsToFrames(sample_rate, timestamp), std::move(message));
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
