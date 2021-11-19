#ifndef BARELYMUSICIAN_COMPOSITION_NOTE_SEQUENCE_H_
#define BARELYMUSICIAN_COMPOSITION_NOTE_SEQUENCE_H_

#include <functional>
#include <map>
#include <optional>
#include <unordered_map>
#include <utility>

#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
#include "barelymusician/composition/note.h"

namespace barelyapi {

/// Musical note sequence.
class NoteSequence {
 public:
  /// Note callback signature.
  ///
  /// @param position Note position.
  /// @param note Note.
  using NoteCallback = std::function<void(double position, const Note& note)>;

  /// Constructs new |NoteSequence|.
  NoteSequence();

  /// Adds new note at position.
  ///
  /// @param id Note id.
  /// @param position Note position.
  /// @param note Note.
  /// @return Status.
  Status Add(Id id, double position, Note note);

  /// Returns the loop length.
  ///
  /// @return Loop length in beats.
  double GetLoopLength() const;
  
  /// Returns the loop start position.
  ///
  /// @return Loop start position in beats.
  double GetLoopStartPosition() const;

  /// Returns the start offset.
  ///
  /// @return Start offset in beats.
  double GetStartOffset() const;

  /// Returns the start position.
  ///
  /// @return Optional start position.
  std::optional<double> GetStartPosition() const;

  /// Returns the end position.
  ///
  /// @return Optional end position.
  std::optional<double> GetEndPosition() const;

  /// Returns whether the sequence is empty or not.
  ///
  /// @return True if empty.
  bool IsEmpty() const;

  /// Returns whether the sequence is looping or not.
  ///
  /// @return True if looping.
  bool IsLooping() const;

  /// Processes the sequence at range.
  ///
  /// @param begin_position Begin position.
  /// @param end_position End position.
  /// @param note_callback Note callback.
  void Process(double begin_position, double end_position,
               const NoteCallback& note_callback) const;

  /// Removes note.
  ///
  /// @param id Note id.
  /// @return Status.
  Status Remove(Id id);

  /// Sets the loop length for when the sequence is looping.
  ///
  /// @param loop_length Loop length in beats.
  void SetLoopLength(double loop_length);

  /// Sets the loop start position.
  ///
  /// @param loop_start_position Loop start position in beats.
  void SetLoopStartPosition(double loop_start_position);

  /// Sets whether the sequence should be looping or not.
  ///
  /// @param is_looping True if looping.
  void SetLooping(bool is_looping);

  /// Sets the start offset.
  ///
  /// @param start_offset Start offset in beats.
  void SetStartOffset(double start_offset);

  /// Sets start position.
  ///
  /// @param start_position Optional start position.
  void SetStartPosition(std::optional<double> start_position);

  /// Sets end position.
  ///
  /// @param end_position Optional end position.
  void SetEndPosition(std::optional<double> end_position);

 private:
  // Denotes whether the sequence is looping or not.
  bool is_looping_;

  // Loop length in beats.
  double loop_length_;

  // Loop start position in beats.
  double loop_start_position_;

  // Start position offset in beats.
  double start_offset_;

  // Optional start position.
  std::optional<double> start_position_;

  // Optional end position.
  std::optional<double> end_position_;

  // Note callback.
  NoteCallback note_callback_;

  // Sorted notes by their positions.
  std::map<std::pair<double, Id>, Note> notes_;

  // Note positions.
  std::unordered_map<Id, double> positions_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_NOTE_SEQUENCE_H_
