#ifndef BARELYMUSICIAN_API_ENGINE_H_
#define BARELYMUSICIAN_API_ENGINE_H_

#include <barelymusician.h>

#include <cassert>
#include <cmath>
#include <cstdint>

#include "core/rng.h"
#include "dsp/control.h"
#include "engine/engine_processor.h"
#include "engine/engine_state.h"
#include "engine/instrument_controller.h"
#include "engine/message_queue.h"
#include "engine/performer_controller.h"
#include "engine/performer_state.h"
#include "engine/task_state.h"

/// Implementation of an engine.
struct BarelyEngine {
 public:
  /// Constructs a new `BarelyEngine`.
  ///
  /// @param sample_rate Sampling rate in hertz.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  explicit BarelyEngine(int sample_rate) noexcept;

  [[nodiscard]] barely::PerformerState& GetPerformer(uint32_t performer_index) noexcept {
    return state_.performer_pool.Get(performer_index);
  }

  [[nodiscard]] const barely::PerformerState& GetPerformer(
      uint32_t performer_index) const noexcept {
    return state_.performer_pool.Get(performer_index);
  }

  [[nodiscard]] const barely::TaskState& GetTask(uint32_t task_index) const noexcept {
    return state_.task_pool.Get(task_index);
  }

  [[nodiscard]] bool IsValidInstrument(BarelyRef instrument) const noexcept {
    return state_.instrument_pool.IsActive(instrument.index, instrument.generation);
  }
  [[nodiscard]] bool IsValidPerformer(BarelyRef performer) const noexcept {
    return state_.performer_pool.IsActive(performer.index, performer.generation);
  }
  [[nodiscard]] bool IsValidTask(BarelyRef task) const noexcept {
    return state_.task_pool.IsActive(task.index, task.generation);
  }

  /// Returns a control value.
  ///
  /// @param type Engine control type.
  /// @return Engine control value.
  [[nodiscard]] float GetControl(BarelyEngineControlType type) const noexcept;

  /// Returns the sampling rate.
  ///
  /// @return Sampling rate in hertz.
  [[nodiscard]] double GetSampleRate() const noexcept { return state_.sample_rate; }

  /// Returns the tempo.
  ///
  /// @return Tempo in beats per minute.
  [[nodiscard]] double GetTempo() const noexcept { return state_.tempo; }

  /// Returns the timestamp.
  ///
  /// @return Timestamp in seconds.
  [[nodiscard]] double GetTimestamp() const noexcept { return state_.timestamp; }

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
  void ScheduleMessage(barely::Message message) noexcept {
    state_.ScheduleMessage(std::move(message));
  }

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

  const barely::MainRng& main_rng() const noexcept { return state_.main_rng; }
  barely::MainRng& main_rng() noexcept { return state_.main_rng; }
  barely::InstrumentController& instrument_controller() noexcept { return instrument_controller_; }
  const barely::InstrumentController& instrument_controller() const noexcept {
    return instrument_controller_;
  }
  barely::PerformerController& performer_controller() noexcept { return performer_controller_; }
  const barely::PerformerController& performer_controller() const noexcept {
    return performer_controller_;
  }

 private:
  barely::EngineState state_;
  barely::InstrumentController instrument_controller_;
  barely::PerformerController performer_controller_;
  barely::EngineProcessor processor_;
};

#endif  // BARELYMUSICIAN_API_ENGINE_H_
