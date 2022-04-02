#ifndef BARELYMUSICIAN_ENGINE_ENGINE_H_
#define BARELYMUSICIAN_ENGINE_ENGINE_H_

#include <memory>
#include <unordered_map>
#include <utility>

#include "barelymusician/engine/conductor.h"
#include "barelymusician/engine/id.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/engine/mutable_data.h"
#include "barelymusician/engine/note.h"
#include "barelymusician/engine/sequence.h"
#include "barelymusician/engine/transport.h"

namespace barelyapi {

/// Musician engine.
class Engine {
 public:
  /// Default constructor.
  Engine() = default;

  /// Destroys `Engine`.
  ~Engine();

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

  /// Creates new sequence.
  ///
  /// @param sequence_id Sequence identifier.
  /// @return True if successful, false otherwise.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  bool CreateSequence(Id sequence_id) noexcept;

  /// Destroys instrument.
  ///
  /// @param instrument_id Instrument identifier.
  /// @return True if successful, false otherwise.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  bool DestroyInstrument(Id instrument_id) noexcept;

  /// Destroys sequence.
  ///
  /// @param sequence_id Sequence identifier.
  /// @return True if successful, false otherwise.
  bool DestroySequence(Id sequence_id) noexcept;

  /// Returns conductor.
  ///
  /// @return Conductor.
  Conductor& GetConductor() noexcept;

  /// Returns instrument.
  ///
  /// @param instrument_id Instrument identifier.
  /// @return Pointer to instrument.
  Instrument* GetInstrument(Id instrument_id) noexcept;

  /// Returns playback tempo.
  ///
  /// @return Tempo in bpm.
  double GetPlaybackTempo() const noexcept;

  /// Returns sequence.
  ///
  /// @param sequence_id Sequence identifier.
  /// @return Pointer to sequence.
  Sequence* GetSequence(Id sequence_id) noexcept;

  /// Returns sequence.
  ///
  /// @param sequence_id Sequence identifier.
  /// @return Pointer to sequence.
  // TODO: Refactor this?
  Id GetSequenceInstrumentId(Id sequence_id) const noexcept;

  /// Returns timestamp.
  ///
  /// @return Timestamp in seconds.
  double GetTimestamp() const noexcept;

  /// Returns transport.
  ///
  /// @return Transport.
  Transport& GetTransport() noexcept;

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

  /// Sets playback tempo.
  ///
  /// @param tempo Tempo in bpm.
  void SetPlaybackTempo(double tempo) noexcept;

  /// Returns sequence.
  ///
  /// @param sequence_id Sequence identifier.
  /// @param instrument_id Instrument identifier.
  /// @return True if successful, false otherwise.
  // TODO: Refactor this?
  bool SetSequenceInstrumentId(Id sequence_id, Id instrument_id) noexcept;

  /// Starts playback.
  void StartPlayback() noexcept;

  /// Stops playback.
  void StopPlayback() noexcept;

  /// Updates internal state.
  ///
  /// @param timestamp Timestamp in seconds.
  void Update(double timestamp) noexcept;

 private:
  // Instrument reference by identifier map.
  using InstrumentReferenceMap = std::unordered_map<Id, Instrument*>;

  // Updates instrument reference map.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void UpdateInstrumentReferenceMap() noexcept;

  // Conductor.
  Conductor conductor_;

  // Map of instruments by identifiers.
  std::unordered_map<Id, std::unique_ptr<Instrument>> instruments_;

  // Map of instrument references by identifiers.
  MutableData<InstrumentReferenceMap> instrument_refs_;

  // Map of sequences by identifiers.
  // TODO: is there a better way than pairing this id?
  std::unordered_map<Id, std::pair<Sequence, Id>> sequences_;

  // Tempo in bpm.
  double tempo_;

  // Transport.
  Transport transport_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_ENGINE_H_
