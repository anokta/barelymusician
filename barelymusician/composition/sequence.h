#ifndef BARELYMUSICIAN_COMPOSITION_SEQUENCE_H_
#define BARELYMUSICIAN_COMPOSITION_SEQUENCE_H_

#include <functional>
#include <map>
#include <unordered_map>
#include <utility>
#include <vector>

#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
#include "barelymusician/composition/note.h"

namespace barelyapi {

/// Musical note sequence.
class Sequence {
 public:
  /// Note position-id pair type.
  using NotePositionIdPair = std::pair<double, Id>;

  /// Note with position type.
  using NoteWithPosition = std::pair<double, Note>;

  /// Note with position-id pair type.
  using NoteWithPositionIdPair = std::pair<NotePositionIdPair, Note>;

  /// Process callback signature.
  ///
  /// @param position Note position.
  /// @param note Note.
  using ProcessCallback =
      std::function<void(double position, const Note& note)>;

  /// Constructs new `Sequence`.
  Sequence() noexcept;

  /// Adds new note at position.
  ///
  /// @param id Note id.
  /// @param position Note position.
  /// @param note Note.
  /// @return Status.
  Status AddNote(Id id, double position, Note note) noexcept;

  /// Returns all notes in the sequence.
  ///
  /// @return List of notes with their position-id pairs.
  std::vector<NoteWithPositionIdPair> GetAllNotes() const noexcept;

  /// Returns the begin offset.
  ///
  /// @return Begin offset in beats.
  double GetBeginOffset() const noexcept;

  /// Returns the begin position.
  ///
  /// @return Begin position in beats.
  double GetBeginPosition() const noexcept;

  /// Returns the end position.
  ///
  /// @return End position in beats.
  double GetEndPosition() const noexcept;

  /// Returns the loop begin offset.
  ///
  /// @return Loop begin offset in beats.
  double GetLoopBeginOffset() const noexcept;

  /// Returns the loop length.
  ///
  /// @return Loop length in beats.
  double GetLoopLength() const noexcept;

  /// Returns note.
  ///
  /// @param id Note id.
  /// @return Note with position, or error status.
  StatusOr<NoteWithPosition> GetNote(Id id) const noexcept;

  /// Returns whether the sequence is empty or not.
  ///
  /// @return True if empty.
  bool IsEmpty() const noexcept;

  /// Returns whether the sequence is looping or not.
  ///
  /// @return True if looping.
  bool IsLooping() const noexcept;

  /// Processes the sequence at given position range.
  ///
  /// @param begin_position Begin position.
  /// @param end_position End position.
  /// @param process_callback Process callback.
  void Process(double begin_position, double end_position,
               const ProcessCallback& process_callback) const noexcept;

  /// Removes all notes.
  void RemoveAllNotes() noexcept;

  /// Removes all notes in range.
  ///
  /// @param begin_position Begin position in beats.
  /// @param end_position End position in beats.
  void RemoveAllNotes(double begin_position, double end_position) noexcept;

  /// Removes note.
  ///
  /// @param id Note id.
  /// @return Status.
  Status RemoveNote(Id id) noexcept;

  /// Sets the begin offset.
  ///
  /// @param begin_offset Begin offset in beats.
  void SetBeginOffset(double begin_offset) noexcept;

  /// Sets the begin position.
  ///
  /// @param begin_position Begin position in beats.
  void SetBeginPosition(double begin_position) noexcept;

  /// Sets the end position.
  ///
  /// @param end_position End position in beats.
  void SetEndPosition(double end_position) noexcept;

  /// Sets whether the sequence should be looping or not.
  ///
  /// @param loop True if looping.
  void SetLoop(bool loop) noexcept;

  /// Sets the loop begin offset.
  ///
  /// @param loop_begin_offset Loop begin offset in beats.
  void SetLoopBeginOffset(double loop_begin_position) noexcept;

  /// Sets the loop length.
  ///
  /// @param loop_length Loop length in beats.
  void SetLoopLength(double loop_length) noexcept;

  /// Sets note.
  ///
  /// @param id Note id.
  /// @param position Note position.
  /// @param note Note.
  /// @return Status.
  Status SetNote(Id id, double position, Note note) noexcept;

  /// Sets note duration.
  ///
  /// @param id Note id.
  /// @return Status.
  Status SetNoteDuration(Id id, NoteDuration note_duration) noexcept;

  /// Sets note intensity.
  ///
  /// @param id Note id.
  /// @return Status.
  Status SetNoteIntensity(Id id, NoteIntensity note_intensity) noexcept;

  /// Sets note pitch.
  ///
  /// @param id Note id.
  /// @return Status.
  Status SetNotePitch(Id id, NotePitch note_pitch) noexcept;

  /// Sets note position.
  ///
  /// @param id Note id.
  /// @return Status.
  Status SetNotePosition(Id id, double position) noexcept;

 private:
  // Internal process helper function.
  void ProcessInternal(double begin_position, double end_position,
                       double position_offset,
                       const ProcessCallback& process_callback) const noexcept;

  // Begin offset in beats.
  double begin_offset_;

  // Begin position in beats.
  double begin_position_;

  // End position in beats.
  double end_position_;

  // Denotes whether the sequence is looping or not.
  bool loop_;

  // Loop begin offset in beats.
  double loop_begin_offset_;

  // Loop length in beats.
  double loop_length_;

  // Sorted notes by their positions.
  std::map<NotePositionIdPair, Note> notes_;

  // Note positions.
  std::unordered_map<Id, double> positions_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_SEQUENCE_H_
