#ifndef BARELYMUSICIAN_ENGINE_ENGINE_H_
#define BARELYMUSICIAN_ENGINE_ENGINE_H_

#include <memory>
#include <unordered_map>
#include <utility>

#include "barelymusician/engine/id.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/engine/mutable_data.h"
// #include "barelymusician/engine/performer.h"

namespace barely::internal {

// TODO(#109): Remove after API cleanup.
class Performer;

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
  /// @param instrument_id Instrument identifier.
  /// @param definition Instrument definition.
  /// @param frame_rate Frame rate in hz.
  /// @return True if successful, false otherwise.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  bool CreateInstrument(Id instrument_id, InstrumentDefinition definition,
                        int frame_rate) noexcept;

  /// Creates new performer.
  ///
  /// @param performer_id Performer identifier.
  /// @param priority Performer priority.
  /// @return True if successful, false otherwise.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  bool CreatePerformer(Id performer_id, int priority) noexcept;

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

  /// Returns beats from seconds.
  ///
  /// @param seconds Number of seconds.
  /// @return Number of beats.
  [[nodiscard]] double GetBeats(double seconds) const noexcept;

  /// Returns instrument.
  ///
  /// @param instrument_id Instrument identifier.
  /// @return Pointer to instrument.
  [[nodiscard]] Instrument* GetInstrument(Id instrument_id) noexcept;

  /// Returns performer.
  ///
  /// @param performer_id Performer identifier.
  /// @return Pointer to performer.
  [[nodiscard]] Performer* GetPerformer(Id performer_id) noexcept;

  /// Returns seconds from beats.
  ///
  /// @param beats Number of beats.
  /// @return Number of seconds.
  [[nodiscard]] double GetSeconds(double beats) const noexcept;

  /// Returns tempo.
  ///
  /// @return Tempo in beats per minute.
  [[nodiscard]] double GetTempo() const noexcept;

  /// Returns timestamp.
  ///
  /// @return Timestamp in seconds.
  double GetTimestamp() const noexcept;

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

  /// Sets tempo.
  ///
  /// @param tempo Tempo in beats per minute.
  void SetTempo(double tempo) noexcept;

  /// Updates engine at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Update(double timestamp) noexcept;

 private:
  // Instrument reference by identifier map.
  using InstrumentReferenceMap = std::unordered_map<Id, Instrument*>;

  // Updates instrument reference map.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void UpdateInstrumentReferenceMap() noexcept;

  // Map of instruments by identifiers.
  std::unordered_map<Id, std::unique_ptr<Instrument>> instruments_;

  // Map of instrument references by identifiers.
  MutableData<InstrumentReferenceMap> instrument_refs_;

  // TODO(#109): Reenable after API cleanup.
  // // Map of performers by performer priority-identifier pairs.
  // std::map<std::pair<int, Id>, Performer> performers_;
  // // Map of performer priority-reference pairs by performer identifiers.
  // std::unordered_map<Id, std::pair<int, Performer*>> performer_refs_;

  // Tempo in beats per minute.
  double tempo_ = 120.0;

  // Timestamp in seconds.
  double timestamp_ = 0.0;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_ENGINE_ENGINE_H_
