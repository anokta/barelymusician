#ifndef BARELYMUSICIAN_COMPOSITION_SEQUENCE_H_
#define BARELYMUSICIAN_COMPOSITION_SEQUENCE_H_

#include <functional>
#include <map>
#include <unordered_map>
#include <utility>

#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
#include "barelymusician/composition/note.h"

namespace barelyapi {

/// Musical sequence.
class Sequence {
 public:
  /// Process callback signature.
  ///
  /// @param position Note position.
  /// @param note Note.
  using ProcessCallback =
      std::function<void(double position, const Note& note)>;

  /// Constructs new |Sequence|.
  Sequence();

  /// Adds new note at position.
  ///
  /// @param id Note id.
  /// @param position Note position.
  /// @param note Note.
  /// @return Status.
  Status Add(Id id, double position, Note note);

  /// Returns the begin offset.
  ///
  /// @return Begin offset in beats.
  double GetBeginOffset() const;

  /// Returns the loop begin offset.
  ///
  /// @return Loop begin offset in beats.
  double GetLoopBeginOffset() const;

  /// Returns the loop length.
  ///
  /// @return Loop length in beats.
  double GetLoopLength() const;

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
  /// @param process_callback Process callback.
  void Process(double begin_position, double end_position,
               ProcessCallback process_callback) const;

  /// Removes note.
  ///
  /// @param id Note id.
  /// @return Status.
  Status Remove(Id id);

  /// Removes all notes.
  void RemoveAll();

  /// Removes all notes in range.
  ///
  /// @param begin_position Begin position in beats.
  /// @param end_position End position in beats.
  void RemoveAll(double begin_position, double end_position);

  /// Sets the begin offset.
  ///
  /// @param begin_offset Begin offset in beats.
  void SetBeginOffset(double begin_offset);

  /// Sets the loop begin offset.
  ///
  /// @param loop_begin_offset Loop begin offset in beats.
  void SetLoopBeginOffset(double loop_begin_position);

  /// Sets the loop length for when the sequence is looping.
  ///
  /// @param loop_length Loop length in beats.
  void SetLoopLength(double loop_length);

  /// Sets whether the sequence should be looping or not.
  ///
  /// @param is_looping True if looping.
  void SetLooping(bool is_looping);

 private:
  // Begin offset in beats.
  double begin_offset_;

  // Denotes whether the sequence is looping or not.
  bool is_looping_;

  // Loop begin offset in beats.
  double loop_begin_offset_;

  // Loop length in beats.
  double loop_length_;

  // Sorted notes by their positions.
  std::map<std::pair<double, Id>, Note> notes_;

  // Note positions.
  std::unordered_map<Id, double> positions_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_SEQUENCE_H_
