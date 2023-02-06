#ifndef BARELYMUSICIAN_ENGINE_ENGINE_H_
#define BARELYMUSICIAN_ENGINE_ENGINE_H_

#include <functional>
#include <memory>
#include <unordered_map>
#include <utility>

#include "barelymusician/engine/id.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/engine/mutable_data.h"
#include "barelymusician/engine/number.h"
#include "barelymusician/engine/performer.h"
#include "barelymusician/engine/status.h"
#include "barelymusician/engine/task.h"

namespace barely::internal {

/// Musician engine.
class Engine {
 public:
  /// Default constructor.
  Engine() = default;

  /// Destroys `Engine`.
  ~Engine() noexcept;

  // Non-copyable and non-movable.
  Engine(const Engine& other) = delete;
  Engine& operator=(const Engine& other) = delete;
  Engine(Engine&& other) noexcept = delete;
  Engine& operator=(Engine&& other) noexcept = delete;

  /// Creates new instrument.
  ///
  /// @param definition Instrument definition.
  /// @param frame_rate Frame rate in hertz.
  /// @return Instrument identifier or error status.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  StatusOr<Id> CreateInstrument(InstrumentDefinition definition,
                                Integer frame_rate) noexcept;

  /// Creates new performer.
  ///
  /// @param order Performer task execution order.
  /// @return Performer identifier or error status.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  StatusOr<Id> CreatePerformer(Integer order) noexcept;

  /// Creates new performer task.
  ///
  /// @param performer_id Performer identifier.
  /// @param definition Task definition.
  /// @param position Task position in beats.
  /// @param type Task type.
  /// @param user_data Pointer to user data.
  /// @return Task identifier or error status.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  StatusOr<Id> CreatePerformerTask(Id performer_id, TaskDefinition definition,
                                   Real position, TaskType type,
                                   void* user_data) noexcept;

  /// Creates new task.
  ///
  /// @param definition Task definition.
  /// @param timestamp Task timestamp in seconds.
  /// @param user_data Pointer to user data.
  /// @return Task identifier or error status.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  StatusOr<Id> CreateTask(TaskDefinition definition, Real timestamp,
                          void* user_data) noexcept;

  /// Destroys task.
  ///
  /// @param task_id Task identifier.
  /// @return Status.
  Status DestroyTask(Id task_id) noexcept;

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

  /// Returns task timestamp.
  ///
  /// @param task_id Task identifier.
  /// @return Timestamp in seconds or error status.
  [[nodiscard]] StatusOr<Real> GetTaskTimestamp(Id task_id) const noexcept;

  /// Returns tempo.
  ///
  /// @return Tempo in beats per minute.
  [[nodiscard]] Real GetTempo() const noexcept;

  /// Returns timestamp.
  ///
  /// @return Timestamp in seconds.
  [[nodiscard]] Real GetTimestamp() const noexcept;

  /// Processes instrument at timestamp.
  ///
  /// @param instrument_id Instrument identifier.
  /// @param output_samples Interleaved array of output samples.
  /// @param output_channel_count Number of output channels.
  /// @param output_frame_count Number of output frames.
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status ProcessInstrument(Id instrument_id, Real* output_samples,
                           Integer output_channel_count,
                           Integer output_frame_count, Real timestamp) noexcept;

  /// Sets task timestamp.
  /// @param task_id Task identifier.
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status SetTaskTimestamp(Id task_id, Real timestamp) noexcept;

  /// Sets tempo.
  ///
  /// @param tempo Tempo in beats per minute.
  void SetTempo(Real tempo) noexcept;

  /// Updates engine at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Update(Real timestamp) noexcept;

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
  std::map<std::pair<Integer, Id>, Performer> performers_;

  // Map of performer order-reference pairs by performer identifiers.
  std::unordered_map<Id, std::pair<Integer, std::reference_wrapper<Performer>>>
      performer_refs_;

  // Tempo in beats per minute.
  Real tempo_ = 120.0;

  // Timestamp in seconds.
  Real timestamp_ = 0.0;

  // Tasks.
  std::unordered_map<Id, Real> task_timestamps_;
  TaskMap tasks_;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_ENGINE_ENGINE_H_
