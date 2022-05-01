#ifndef BARELYMUSICIAN_ENGINE_SEQUENCE_H_
#define BARELYMUSICIAN_ENGINE_SEQUENCE_H_

#include <limits>
#include <map>
#include <unordered_map>
#include <utility>

#include "barelymusician/engine/clock.h"
#include "barelymusician/engine/id.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/engine/note.h"

namespace barelyapi {

/// Class that wraps sequence.
class Sequence {
 public:
  /// Constructs new `Sequence`.
  ///
  /// @param clock Clock.
  explicit Sequence(const Clock& clock) noexcept;

  /// Creates new note at position.
  ///
  /// @param note_id Note identifier.
  /// @param position Note position in beats.
  /// @param note Note.
  /// @return True if success.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  bool CreateNote(Id note_id, double position, Note note) noexcept;

  // TODO(#98): Add `ParameterAutomation` functionality.

  /// Destroys note.
  ///
  /// @param note_id Note identifier.
  /// @return True if success.
  bool DestroyNote(Id note_id) noexcept;

  /// Returns begin offset.
  ///
  /// @return Begin offset in beats.
  [[nodiscard]] double GetBeginOffset() const noexcept;

  /// Returns begin position.
  ///
  /// @return Begin position in beats.
  [[nodiscard]] double GetBeginPosition() const noexcept;

  /// Returns end position.
  ///
  /// @return End position in beats.
  [[nodiscard]] double GetEndPosition() const noexcept;

  /// Returns instrument.
  ///
  /// @return Pointer to instrument.
  [[nodiscard]] Instrument* GetInstrument() const noexcept;

  /// Returns loop begin offset.
  ///
  /// @return Loop begin offset in beats.
  [[nodiscard]] double GetLoopBeginOffset() const noexcept;

  /// Returns loop length.
  ///
  /// @return Loop length in beats.
  [[nodiscard]] double GetLoopLength() const noexcept;

  /// Returns note.
  ///
  /// @param note_id Note identifier.
  /// @return Pointer to mutable note.
  [[nodiscard]] Note* GetNote(Id note_id) noexcept;

  /// Returns note.
  ///
  /// @param note_id Note identifier.
  /// @return Pointer to note.
  [[nodiscard]] const Note* GetNote(Id note_id) const noexcept;

  /// Returns note position.
  ///
  /// @param note_id Note identifier.
  /// @return Pointer to note position in beats.
  [[nodiscard]] const double* GetNotePosition(Id note_id) const noexcept;

  /// Returns whether sequence is looping or not.
  ///
  /// @return True if looping, false otherwise.
  [[nodiscard]] bool IsLooping() const noexcept;

  /// Processes sequence at range.
  ///
  /// @param begin_position Begin position in beats.
  /// @param end_position End position in beats.
  void Process(double begin_position, double end_position,
               const std::function<double(double)>& get_timestamp_fn) noexcept;

  /// Sets begin offset.
  ///
  /// @param begin_offset Begin offset in beats.
  void SetBeginOffset(double begin_offset) noexcept;

  /// Sets begin position.
  ///
  /// @param begin_position Begin position in beats.
  void SetBeginPosition(double begin_position) noexcept;

  /// Sets end position.
  ///
  /// @param end_position End position in beats.
  void SetEndPosition(double end_position) noexcept;

  /// Sets instrument.
  ///
  /// @param instrument Pointer to instrument.
  void SetInstrument(Instrument* instrument) noexcept;

  /// Sets loop begin offset.
  ///
  /// @param loop_begin_offset Loop begin offset in beats.
  void SetLoopBeginOffset(double loop_begin_offset) noexcept;

  /// Sets loop length.
  ///
  /// @param loop_length Loop length in beats.
  void SetLoopLength(double loop_length) noexcept;

  /// Sets whether sequence should be looping or not.
  ///
  /// @param is_looping True if looping.
  void SetLooping(bool is_looping) noexcept;

  /// Sets note position.
  ///
  /// @param note_id Note identifier.
  /// @param position Note position in beats.
  /// @return True if success.
  bool SetNotePosition(Id note_id, double position) noexcept;

  /// Stops sequence.
  void Stop() noexcept;

 private:
  // Active note that is being processed.
  struct ActiveNote {
    // End position.
    double end_position;

    // Pitch.
    double pitch;
  };

  // Internal process helper function.
  void ProcessInternal(
      double begin_position, double end_position, double position_offset,
      double process_end_position,
      const std::function<double(double)>& get_timestamp_fn) noexcept;

  // Clock.
  const Clock& clock_;

  // Map of active notes by note positions.
  std::multimap<double, ActiveNote> active_notes_;

  // Begin offset in beats.
  double begin_offset_ = 0.0;

  // Begin position in beats.
  double begin_position_ = 0.0;

  // End position in beats.
  double end_position_ = std::numeric_limits<double>::max();

  // Instrument.
  Instrument* instrument_ = nullptr;

  // Denotes whether sequence is looping or not.
  bool is_looping_ = false;

  // Denotes whether sequence is skipping adjustments or not.
  bool is_skipping_adjustments_ = false;

  // Loop begin offset in beats.
  double loop_begin_offset_ = 0.0;

  // Loop length in beats.
  double loop_length_ = 1.0;

  // Sorted map of note definitions by note position-identifier pair.
  std::map<std::pair<double, Id>, Note> notes_;

  // Map of note positions by note identifiers.
  std::unordered_map<Id, double> positions_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_SEQUENCE_H_
