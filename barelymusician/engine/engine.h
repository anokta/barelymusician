#ifndef BARELYMUSICIAN_ENGINE_ENGINE_H_
#define BARELYMUSICIAN_ENGINE_ENGINE_H_

#include <memory>
#include <unordered_map>
#include <utility>

#include "barelymusician/engine/id.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/engine/mutable_data.h"
#include "barelymusician/engine/sequencer.h"

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
  /// @param instrument_id Instrument identifier.
  /// @param definition Instrument definition.
  /// @param frame_rate Frame rate in hz.
  /// @return True if successful, false otherwise.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  bool CreateInstrument(Id instrument_id, Instrument::Definition definition,
                        int frame_rate) noexcept;

  /// Creates new sequencer.
  ///
  /// @param sequencer_id Sequencer identifier.
  /// @param priority Sequencer priority.
  /// @return True if successful, false otherwise.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  bool CreateSequencer(Id sequencer_id, int priority) noexcept;

  /// Destroys instrument.
  ///
  /// @param instrument_id Instrument identifier.
  /// @return True if successful, false otherwise.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  bool DestroyInstrument(Id instrument_id) noexcept;

  /// Destroys sequencer.
  ///
  /// @param sequencer_id Sequencer identifier.
  /// @return True if successful, false otherwise.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  bool DestroySequencer(Id sequencer_id) noexcept;

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

  /// Returns sequencer.
  ///
  /// @param sequencer_id Sequencer identifier.
  /// @return Pointer to sequencer.
  [[nodiscard]] Sequencer* GetSequencer(Id sequencer_id) noexcept;

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
  /// @param output Output buffer.
  /// @param num_output_channels Number of output channels.
  /// @param num_output_frames Number of output frames.
  /// @param timestamp Timestamp in seconds.
  /// @return True if successful, false otherwise.
  bool ProcessInstrument(Id instrument_id, double* output,
                         int num_output_channels, int num_output_frames,
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

  // Map of sequencers by sequencer priority-identifier pairs.
  std::map<std::pair<int, Id>, Sequencer> sequencers_;

  // Map of sequencer priority-reference pairs by sequencer identifiers.
  std::unordered_map<Id, std::pair<int, Sequencer*>> sequencer_refs_;

  // Tempo in beats per minute.
  double tempo_ = 120.0;

  // Timestamp in seconds.
  double timestamp_ = 0.0;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_ENGINE_ENGINE_H_
