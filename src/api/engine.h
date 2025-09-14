#ifndef BARELYMUSICIAN_API_ENGINE_H_
#define BARELYMUSICIAN_API_ENGINE_H_

#include <barelymusician.h>

#include <cmath>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "common/mutable.h"
#include "common/rng.h"
#include "dsp/control.h"
#include "dsp/engine_processor.h"
#include "dsp/instrument_processor.h"
#include "dsp/message_queue.h"

/// Implementation of an engine.
struct BarelyEngine {
 public:
  /// Constructs a new `BarelyEngine`.
  ///
  /// @param sample_rate Sampling rate in hertz.
  /// @param channel_count Number of channels.
  /// @param max_frame_count Maximum number of frames.
  /// @param reference_frequency Reference frequency in hertz.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  BarelyEngine(int sample_rate, int channel_count, int max_frame_count,
               float reference_frequency) noexcept;

  /// Destroys `BarelyEngine`.
  ~BarelyEngine() noexcept;

  /// Adds a new instrument.
  ///
  /// @param instrument Pointer to instrument.
  /// @param control_overrides Span of control overrides.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void AddInstrument(BarelyInstrument* instrument,
                     std::span<const BarelyControlOverride> control_overrides) noexcept;

  /// Adds a new performer.
  ///
  /// @return Pointer to performer.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void AddPerformer(BarelyPerformer* performer) noexcept;

  /// Returns an effect control value.
  ///
  /// @param type Effect control type.
  /// @return Effect control value.
  [[nodiscard]] float GetEffectControl(BarelyEffectControlType type) const noexcept;

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
  /// @param output_frame_count Number of output frames.
  /// @param timestamp Timestamp in seconds.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Process(float* output_samples, int output_frame_count, double timestamp) noexcept;

  /// Removes an instrument.
  ///
  /// @param instrument Pointer to instrument.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void RemoveInstrument(BarelyInstrument* instrument) noexcept;

  /// Removes a performer.
  ///
  /// @param performer Pointer to performer.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void RemovePerformer(BarelyPerformer* performer) noexcept;

  /// Schedules a new message in the queue.
  ///
  /// @param message Message
  void ScheduleMessage(barely::Message message) noexcept;

  /// Sets an effect control value.
  ///
  /// @param type Effect control type.
  /// @param value Effect control value.
  void SetEffectControl(BarelyEffectControlType type, float value) noexcept;

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
  // Sampling rate in hertz.
  int sample_rate_ = 0;

  // Number of channels.
  int channel_count_ = 0;

  // Reference frequency at zero pitch.
  float reference_frequency_ = 0.0f;

  // Array of effect controls.
  barely::EffectControlArray effect_controls_;

  // Random number generator for the audio thread.
  barely::AudioRng audio_rng_;

  // Random number generator for the main thread.
  barely::MainRng main_rng_;

  // Message queue.
  barely::MessageQueue message_queue_;

  // Map of instrument pointers to processors.
  std::unordered_map<BarelyInstrument*, barely::InstrumentProcessor> instruments_;
  barely::Mutable<std::unordered_map<BarelyInstrument*, barely::InstrumentProcessor*>>
      mutable_instruments_;

  // Set of pointers to performers.
  std::unordered_set<BarelyPerformer*> performers_;

  // Tempo in beats per minute.
  double tempo_ = 120.0;

  // Timestamp in seconds.
  double timestamp_ = 0.0;

  // Update frame.
  int64_t update_frame_ = 0;

  // Engine processor.
  // TODO(#145): Make this a variant with respect to channel count.
  static constexpr int kChannelCount = 2;
  barely::EngineProcessor<kChannelCount> engine_processor_;
};

#endif  // BARELYMUSICIAN_API_ENGINE_H_
