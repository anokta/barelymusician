#ifndef BARELYMUSICIAN_API_ENGINE_H_
#define BARELYMUSICIAN_API_ENGINE_H_

#include <barelymusician.h>

#include <cassert>
#include <cmath>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "api/performer.h"
#include "core/rng.h"
#include "dsp/control.h"
#include "dsp/engine_processor.h"
#include "dsp/voice_pool.h"
#include "engine/instrument_controller.h"
#include "engine/instrument_params.h"
#include "engine/message.h"
#include "engine/message_queue.h"
#include "engine/task_state.h"

/// Implementation of an engine.
struct BarelyEngine {
 public:
  /// Constructs a new `BarelyEngine`.
  ///
  /// @param sample_rate Sampling rate in hertz.
  /// @param max_frame_count Maximum number of frames.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  BarelyEngine(int sample_rate, int max_frame_count) noexcept;

  /// Adds a new performer.
  ///
  /// @return Performer index.
  [[nodiscard]] BarelyRef AddPerformer() noexcept {
    const uint32_t index = performer_pool_.Acquire();
    performer_pool_.Get(index) = {};
    return {index, performer_pool_.GetGeneration(index)};
  }

  /// Removes a performer.
  ///
  /// @param performer_index Performer reference.
  void RemovePerformer(BarelyRef performer) noexcept { performer_pool_.Release(performer.index); }

  [[nodiscard]] BarelyPerformer& GetPerformer(uint32_t performer_index) noexcept {
    return performer_pool_.Get(performer_index);
  }
  [[nodiscard]] BarelyPerformer& GetPerformer(BarelyRef performer) noexcept {
    return performer_pool_.Get(performer.index);
  }
  [[nodiscard]] const BarelyPerformer& GetPerformer(BarelyRef performer) const noexcept {
    return performer_pool_.Get(performer.index);
  }

  [[nodiscard]] TaskState& GetTask(BarelyRef task) noexcept { return task_pool_.Get(task.index); }
  [[nodiscard]] const TaskState& GetTask(BarelyRef task) const noexcept {
    return task_pool_.Get(task.index);
  }

  [[nodiscard]] bool IsValidInstrument(BarelyRef instrument) const noexcept {
    return instrument_controller_.IsActive(instrument);
  }
  [[nodiscard]] bool IsValidPerformer(BarelyRef performer) const noexcept {
    return performer_pool_.IsActive(performer.index, performer.generation);
  }
  [[nodiscard]] bool IsValidTask(BarelyRef task) const noexcept {
    return task_pool_.IsActive(task.index, task.generation);
  }

  /// Adds a new task.
  ///
  /// @return Task index.
  [[nodiscard]] BarelyRef AddTask() noexcept {
    const uint32_t index = task_pool_.Acquire();
    return {index, task_pool_.GetGeneration(index)};
  }

  /// Removes a task.
  ///
  /// @param task Task reference.
  void RemoveTask(BarelyRef task) noexcept {
    performer_pool_.Get(GetTask(task).performer_index).RemoveTask(&GetTask(task));
    task_pool_.Release(task.index);
  }

  /// Returns a control value.
  ///
  /// @param type Engine control type.
  /// @return Engine control value.
  [[nodiscard]] float GetControl(BarelyEngineControlType type) const noexcept;

  /// Returns the sampling rate.
  ///
  /// @return Sampling rate in hertz.
  [[nodiscard]] double GetSampleRate() const noexcept { return sample_rate_; }

  /// Returns the tempo.
  ///
  /// @return Tempo in beats per minute.
  [[nodiscard]] double GetTempo() const noexcept { return tempo_; }

  /// Returns the timestamp.
  ///
  /// @return Timestamp in seconds.
  [[nodiscard]] double GetTimestamp() const noexcept { return timestamp_; }

  /// Processes output samples at timestamp.
  ///
  /// @param output_samples Array of interleaved output samples.
  /// @param output_channel_count Number of output channels.
  /// @param output_frame_count Number of output frames.
  /// @param timestamp Timestamp in seconds.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Process(float* output_samples, int output_channel_count, int output_frame_count,
               double timestamp) noexcept;

  /// Schedules a new message in the queue.
  ///
  /// @param message Message
  void ScheduleMessage(barely::Message message) noexcept;

  /// Sets a control value.
  ///
  /// @param type Engine control type.
  /// @param value Engine control value.
  void SetControl(BarelyEngineControlType type, float value) noexcept;

  /// Sets the tempo.
  ///
  /// @param tempo Tempo in beats per minute.
  void SetTempo(double tempo) noexcept;

  /// Updates the engine at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Update(double timestamp) noexcept;

  const barely::MainRng& main_rng() const noexcept { return main_rng_; }
  barely::MainRng& main_rng() noexcept { return main_rng_; }
  barely::InstrumentController& instrument_controller() noexcept { return instrument_controller_; }
  const barely::InstrumentController& instrument_controller() const noexcept {
    return instrument_controller_;
  }

 private:
  // Array of engine controls.
  barely::EngineControlArray controls_;

  // Engine processor.
  barely::EngineProcessor engine_processor_;

  // Random number generator for the audio thread.
  barely::AudioRng audio_rng_;

  // Random number generator for the main thread.
  barely::MainRng main_rng_;

  // Message queue.
  barely::MessageQueue message_queue_;

  // Instrument pool.
  barely::InstrumentParamsArray params_array_;

  // Voice pool.
  barely::VoicePool voice_pool_;

  // Performers.
  using PerformerPool = barely::Pool<BarelyPerformer, BARELYMUSICIAN_MAX_PERFORMER_COUNT>;
  PerformerPool performer_pool_;

  // Tasks.
  using TaskPool = barely::Pool<TaskState, BARELYMUSICIAN_MAX_TASK_COUNT>;
  TaskPool task_pool_;

  // Output samples.
  std::vector<float> output_samples_;

  // Tempo in beats per minute.
  double tempo_ = 120.0;

  // Timestamp in seconds.
  double timestamp_ = 0.0;

  // Update frame.
  int64_t update_frame_ = 0;

  // Sampling rate in hertz.
  int sample_rate_ = 0;

  // Sampling interval in seconds.
  float sample_interval_ = 0.0f;

  barely::InstrumentController instrument_controller_;
};

#endif  // BARELYMUSICIAN_API_ENGINE_H_
