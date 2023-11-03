#ifndef BARELYMUSICIAN_INTERNAL_ENGINE_H_
#define BARELYMUSICIAN_INTERNAL_ENGINE_H_

#include <functional>
#include <memory>
#include <optional>
#include <unordered_map>
#include <utility>

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/id.h"
#include "barelymusician/internal/instrument.h"
#include "barelymusician/internal/mutable_data.h"
#include "barelymusician/internal/performer.h"

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

  /// @param instrument_id Instrument identifier.
  /// @param definition Effect definition.
  /// @param process_order Effect process order.
  /// @return Optional effect identifier.
  std::optional<Id> CreateInstrumentEffect(Id instrument_id,
                                           EffectDefinition definition,
                                           int process_order) noexcept;

  /// Creates a new instrument.
  ///
  /// @param definition Instrument definition.
  /// @param frame_rate Frame rate in hertz.
  /// @return Optional instrument identifier.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  std::optional<Id> CreateInstrument(InstrumentDefinition definition,
                                     int frame_rate) noexcept;

  /// Creates a new performer.
  ///
  /// @return Performer identifier.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  Id CreatePerformer() noexcept;

  /// Creates a new performer task.
  ///
  /// @param performer_id Performer identifier.
  /// @param definition Task definition.
  /// @param is_one_off True if one-off task, false otherwise.
  /// @param position Task position in beats.
  /// @param process_order Task process order.
  /// @param user_data Pointer to user data.
  /// @return Optional task identifier.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  std::optional<Id> CreatePerformerTask(Id performer_id,
                                        TaskDefinition definition,
                                        bool is_one_off, double position,
                                        int process_order,
                                        void* user_data) noexcept;

  /// Destroys instrument.
  ///
  /// @param instrument_id Instrument identifier.
  /// @return True if successful, false otherwise.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  bool DestroyInstrument(Id instrument_id) noexcept;

  /// Destroys performer.
  ///
  /// @param performer_id Performer identifier.
  /// @return True if successful, false otherwise.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  bool DestroyPerformer(Id performer_id) noexcept;

  /// Returns instrument.
  ///
  /// @param instrument_id Instrument identifier.
  /// @return Optional reference to instrument.
  [[nodiscard]] std::optional<std::reference_wrapper<Instrument>> GetInstrument(
      Id instrument_id) noexcept;

  /// Returns performer.
  ///
  /// @param performer_id Performer identifier.
  /// @return Optional reference to performer.
  [[nodiscard]] std::optional<std::reference_wrapper<Performer>> GetPerformer(
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
  /// @return True if successful, false otherwise.
  bool ProcessInstrument(Id instrument_id, double* output_samples,
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

#endif  // BARELYMUSICIAN_INTERNAL_ENGINE_H_
