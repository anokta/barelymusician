#ifndef BARELYMUSICIAN_API_ENGINE_H_
#define BARELYMUSICIAN_API_ENGINE_H_

#include <barelymusician.h>

#include <cassert>
#include <cmath>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "common/rng.h"
#include "dsp/control.h"
#include "dsp/engine_processor.h"
#include "dsp/instrument_params.h"
#include "dsp/message_queue.h"
#include "dsp/voice_pool.h"

/// Implementation of an engine.
struct BarelyEngine {
 public:
  /// Constructs a new `BarelyEngine`.
  ///
  /// @param sample_rate Sampling rate in hertz.
  /// @param max_frame_count Maximum number of frames.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  BarelyEngine(int sample_rate, int max_frame_count) noexcept;

  /// Adds a new instrument.
  ///
  /// @return Instrument index.
  [[nodiscard]] uint32_t AddInstrument() noexcept {
    const uint32_t index = instrument_pool_.Acquire();
    instrument_pool_.Get(index) = {};
    return index;
  }

  /// Removes an instrument.
  ///
  /// @param instrument_index Instrument index.
  void RemoveInstrument(uint32_t instrument_index) noexcept {
    instrument_pool_.Release(instrument_index);
  }

  /// Adds a new performer.
  ///
  /// @return Performer index.
  [[nodiscard]] uint32_t AddPerformer() noexcept {
    const uint32_t index = performer_pool_.Acquire();
    performer_pool_.Get(index) = {};
    return index;
  }

  /// Removes a performer.
  ///
  /// @param performer_index Performer index.
  void RemovePerformer(uint32_t performer_index) noexcept {
    performer_pool_.Release(performer_index);
  }

  void DestroyInstruments() noexcept {
    for (uint32_t i = 0; i < instrument_pool_.GetActiveCount(); ++i) {
      instrument_pool_.GetActive(i).SetAllNotesOff();
    }
  }

  [[nodiscard]] BarelyInstrument& GetInstrument(uint32_t instrument_index) noexcept {
    return instrument_pool_.Get(instrument_index);
  }

  [[nodiscard]] BarelyPerformer& GetPerformer(uint32_t performer_index) noexcept {
    return performer_pool_.Get(performer_index);
  }

  /// Adds a new task.
  ///
  /// @return Task index.
  [[nodiscard]] uint32_t AddTask() noexcept {
    const uint32_t index = task_pool_.Acquire();
    return index;
  }

  /// Removes a task.
  ///
  /// @param task_index Task index.
  void RemoveTask(uint32_t task_index) noexcept { task_pool_.Release(task_index); }

  [[nodiscard]] BarelyTask& GetTask(uint32_t task_index) noexcept {
    return task_pool_.Get(task_index);
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

  barely::MainRng& main_rng() noexcept { return main_rng_; }

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
  barely::InstrumentPool instrument_pool_;

  // Voice pool.
  barely::VoicePool voice_pool_;

  // Performers.
  using PerformerPool = barely::Pool<BarelyPerformer, BARELYMUSICIAN_MAX_PERFORMER_COUNT>;
  PerformerPool performer_pool_;

  // Tasks.
  using TaskPool = barely::Pool<BarelyTask, BARELYMUSICIAN_MAX_TASK_COUNT>;
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
};

#endif  // BARELYMUSICIAN_API_ENGINE_H_
