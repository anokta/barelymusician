#ifndef BARELYMUSICIAN_API_ENGINE_H_
#define BARELYMUSICIAN_API_ENGINE_H_

#include <barelymusician.h>

#include <cassert>
#include <cmath>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "core/rng.h"
#include "dsp/control.h"
#include "engine/engine_processor.h"
#include "engine/instrument_controller.h"
#include "engine/message_queue.h"
#include "engine/performer_controller.h"

/// Implementation of an engine.
struct BarelyEngine {
 public:
  /// Constructs a new `BarelyEngine`.
  ///
  /// @param sample_rate Sampling rate in hertz.
  /// @param max_frame_count Maximum number of frames.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  BarelyEngine(int sample_rate, int max_frame_count) noexcept;

  [[nodiscard]] bool IsValidInstrument(BarelyRef instrument) const noexcept {
    return instrument_controller_.IsActive(instrument);
  }
  [[nodiscard]] bool IsValidPerformer(BarelyRef performer) const noexcept {
    return performer_controller_.IsActive(performer);
  }
  [[nodiscard]] bool IsValidTask(BarelyRef task) const noexcept {
    return performer_controller_.IsActiveTask(task);
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
  barely::PerformerController& performer_controller() noexcept { return performer_controller_; }
  const barely::PerformerController& performer_controller() const noexcept {
    return performer_controller_;
  }

 private:
  // Array of engine controls.
  barely::EngineControlArray controls_;

  // Engine processor.
  barely::EngineProcessor processor_;

  // Random number generator for the main thread.
  barely::MainRng main_rng_;

  // Message queue.
  barely::MessageQueue message_queue_;

  barely::InstrumentController instrument_controller_;
  barely::PerformerController performer_controller_;

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
};

#endif  // BARELYMUSICIAN_API_ENGINE_H_
