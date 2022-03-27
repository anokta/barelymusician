#ifndef BARELYMUSICIAN_ENGINE_SEQUENCE_H_
#define BARELYMUSICIAN_ENGINE_SEQUENCE_H_

#include <functional>
#include <limits>
#include <map>
#include <unordered_map>
#include <utility>
#include <vector>

#include "barelymusician/engine/conductor.h"
#include "barelymusician/engine/id.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/engine/note.h"
#include "barelymusician/engine/transport.h"

namespace barelyapi {

/// Class that wraps sequence.
class Sequence {
 public:
  /// Note position-id pair type.
  using NotePositionIdPair = std::pair<double, Id>;

  /// Note with position type.
  using NoteWithPosition = std::pair<double, Note::Definition>;

  /// Note with position-id pair type.
  using NoteWithPositionIdPair =
      std::pair<NotePositionIdPair, Note::Definition>;

  Sequence(Conductor& conductor, const Transport& transport) noexcept;

  /// Adds new note at position.
  ///
  /// @param id Note identifier.
  /// @param position Note position.
  /// @param note Note.
  /// @return True if success.
  bool AddNote(Id id, Note::Definition definition, double position) noexcept;

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

  /// Returns whether sequence is empty or not.
  ///
  /// @return True if empty, false otherwise.
  [[nodiscard]] bool IsEmpty() const noexcept;

  /// Returns whether sequence is looping or not.
  ///
  /// @return True if looping, false otherwise.
  [[nodiscard]] bool IsLooping() const noexcept;

  /// Returns whether sequence is skipping adjustments or not.
  ///
  /// @return True if skipping, false otherwise.
  [[nodiscard]] bool IsSkippingAdjustments() const noexcept;

  /// Processes sequence at given position range.
  ///
  /// @param begin_position Begin position.
  /// @param end_position End position.
  void Process(double begin_position, double end_position) noexcept;

  /// Removes all notes.
  void RemoveAllNotes() noexcept;

  /// Removes all notes in range.
  ///
  /// @param begin_position Begin position in beats.
  /// @param end_position End position in beats.
  void RemoveAllNotes(double begin_position, double end_position) noexcept;

  /// Removes note.
  ///
  /// @param id Note identifier.
  /// @return True if success.
  bool RemoveNote(Id id) noexcept;

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

  /// Sets note.
  ///
  /// @param id Note identifier.
  /// @param definition Note definition.
  /// @return True if success.
  bool SetNoteDefinition(Id id, Note::Definition definition) noexcept;

  /// Sets note position.
  ///
  /// @param id Note identifier.
  /// @return True if success.
  bool SetNotePosition(Id id, double position) noexcept;

  /// Sets whether sequence should be skipping adjustments or not.
  ///
  /// @param is_skipping_adjustments True if skipping.
  void SetSkippingAdjustments(bool is_skipping_adjustments) noexcept;

  /// Stops playback.
  void Stop() noexcept;

 private:
  // Internal process helper function.
  void ProcessInternal(double begin_position, double end_position,
                       double position_offset) noexcept;

  // Active note that is being performed.
  struct ActiveNote {
    // Note end position.
    double end_position;

    // Note pitch.
    double pitch;
  };

  Conductor& conductor_;

  const Transport& transport_;

  Instrument* instrument_;

  // List of active notes.
  std::multimap<double, ActiveNote> active_notes_;

  // Begin offset in beats.
  double begin_offset_ = 0.0;

  // Begin position in beats.
  double begin_position_ = 0.0;

  // End position in beats.
  double end_position_ = std::numeric_limits<double>::max();

  // Denotes whether sequence is looping or not.
  bool is_looping_ = false;

  // Denotes whether sequence is skipping adjustments or not.
  bool is_skipping_adjustments_ = false;

  // Loop begin offset in beats.
  double loop_begin_offset_ = 0.0;

  // Loop length in beats.
  double loop_length_ = 1.0;

  // Sorted note by position map.
  std::map<NotePositionIdPair, Note::Definition> notes_;

  // Note positions.
  std::unordered_map<Id, double> positions_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_SEQUENCE_H_
