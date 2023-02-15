#ifndef BARELYMUSICIAN_ENGINE_ENGINE_H_
#define BARELYMUSICIAN_ENGINE_ENGINE_H_

#include <functional>
#include <memory>
#include <unordered_map>
#include <utility>

#include "barelymusician/engine/id.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/engine/mutable_data.h"
#include "barelymusician/engine/performer.h"
#include "barelymusician/engine/status.h"
#include "barelymusician/engine/task.h"

namespace barely::internal {

/// Internal engine.
class Engine {
 public:
  /// Default constructor.
  Engine() = default;

  /// Destroys `Engine`.
  ~Engine() noexcept;

  // Non-copyable and non-movable.
  Engine(const Engine& other) noexcept = delete;
  Engine& operator=(const Engine& other) noexcept = delete;
  Engine(Engine&& other) noexcept = delete;
  Engine& operator=(Engine&& other) noexcept = delete;

  /// Creates new instrument.
  ///
  /// @param definition Instrument definition.
  /// @param frame_rate Frame rate in hertz.
  /// @return Instrument identifier or error status.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  StatusOr<Id> CreateInstrument(InstrumentDefinition definition,
                                int frame_rate) noexcept;

  /// Creates new performer.
  ///
  /// @return Performer identifier or error status.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  StatusOr<Id> CreatePerformer() noexcept;

  /// Creates new performer task.
  ///
  /// @param performer_id Performer identifier.
  /// @param definition Task definition.
  /// @param is_one_off True if one-off task, false otherwise.
  /// @param position Task position in beats.
  /// @param process_order Task process order.
  /// @param user_data Pointer to user data.
  /// @return Task identifier or error status.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  StatusOr<Id> CreatePerformerTask(Id performer_id, TaskDefinition definition,
                                   bool is_one_off, double position,
                                   int process_order, void* user_data) noexcept;

  /// Destroys instrument.
  ///
  /// @param instrument_id Instrument identifier.
  /// @return Status.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  Status DestroyInstrument(Id instrument_id) noexcept;

  /// Destroys performer.
  ///
  /// @param performer_id Performer identifier.
  /// @return Status.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  Status DestroyPerformer(Id performer_id) noexcept;

  /// Returns instrument.
  ///
  /// @param instrument_id Instrument identifier.
  /// @return Reference to instrument or error status.
  [[nodiscard]] StatusOr<std::reference_wrapper<Instrument>> GetInstrument(
      Id instrument_id) noexcept;

  /// Returns performer.
  ///
  /// @param performer_id Performer identifier.
  /// @return Reference to performer or error status.
  [[nodiscard]] StatusOr<std::reference_wrapper<Performer>> GetPerformer(
      Id performer_id) noexcept;

  /// Returns tempo.
  ///
  /// @return Tempo in beats per minute.
  [[nodiscard]] double GetTempo() const noexcept;

  /// Returns timestamp.
  ///
  /// @return Timestamp in seconds.
  [[nodiscard]] double GetTimestamp() const noexcept;

  /// Processes instrument at timestamp.
  ///
  /// @param instrument_id Instrument identifier.
  /// @param output_samples Interleaved array of output samples.
  /// @param output_channel_count Number of output channels.
  /// @param output_frame_count Number of output frames.
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status ProcessInstrument(Id instrument_id, double* output_samples,
                           int output_channel_count, int output_frame_count,
                           double timestamp) noexcept;

  /// Sets the tempo.
  ///
  /// @param tempo Tempo in beats per minute.
  void SetTempo(double tempo) noexcept;

  /// Updates the engine at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Update(double timestamp) noexcept;

 private:
  // Instrument reference by identifier map.
  using InstrumentReferenceMap = std::unordered_map<Id, Instrument*>;

  // Generates the next identifier to use.
  Id GenerateNextId() noexcept;

  // Updates instrument reference map.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void UpdateInstrumentReferenceMap() noexcept;

  // Monotonic identifier counter.
  Id id_counter_ = 0;

  // Map of instruments by identifiers.
  std::unordered_map<Id, std::unique_ptr<Instrument>> instruments_;

  // Map of instrument references by identifiers.
  MutableData<InstrumentReferenceMap> instrument_refs_;

  // Map of performers by performer order-identifier pairs.
  std::unordered_map<Id, Performer> performers_;

  // Tempo in beats per minute.
  double tempo_ = 120.0;

  // Timestamp in seconds.
  double timestamp_ = 0.0;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_ENGINE_ENGINE_H_
