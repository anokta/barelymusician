#ifndef BARELYMUSICIAN_SEQUENCE_H_
#define BARELYMUSICIAN_SEQUENCE_H_

#include <functional>
#include <limits>
#include <map>
#include <unordered_map>
#include <utility>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/event.h"

namespace barelyapi {

/// Musical note sequence.
class Sequence {
 public:
  /// Note position-id pair type.
  using NotePositionIdPair = std::pair<double, BarelyId>;

  /// Note with position type.
  using NoteWithPosition = std::pair<double, BarelyNoteDefinition>;

  /// Note with position-id pair type.
  using NoteWithPositionIdPair =
      std::pair<NotePositionIdPair, BarelyNoteDefinition>;

  // TODO: Refactor this.
  using EventCallback = std::function<void(double, Event)>;

  /// Adds new note at position.
  ///
  /// @param id Note identifier.
  /// @param position Note position.
  /// @param note Note.
  /// @return True if success.
  bool AddNote(BarelyId id, double position,
               BarelyNoteDefinition note) noexcept;

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
  /// @return Instrument identifier.
  [[nodiscard]] BarelyId GetInstrument() const noexcept;

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
  /// @return True if empty.
  [[nodiscard]] bool IsEmpty() const noexcept;

  /// Returns whether sequence is looping or not.
  ///
  /// @return True if looping.
  [[nodiscard]] bool IsLooping() const noexcept;

  /// Processes sequence at given position range.
  ///
  /// @param begin_position Begin position.
  /// @param end_position End position.
  /// @param process_callback Process callback.
  // TODO: Pass conductor.
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
  bool RemoveNote(BarelyId id) noexcept;

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

  void SetEventCallback(EventCallback event_callback) noexcept;

  /// Sets instrument.
  ///
  /// @param instrument_id Instrument identifier.
  void SetInstrument(BarelyId instrument_id) noexcept;

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
  bool SetNoteDefinition(BarelyId id, BarelyNoteDefinition definition) noexcept;

  /// Sets note position.
  ///
  /// @param id Note identifier.
  /// @return True if success.
  bool SetNotePosition(BarelyId id, double position) noexcept;

  // TODO: Temp.
  void Stop() { active_notes_.clear(); }

  // TODO: Temp.
  std::vector<double> GetActiveNotes() {
    std::vector<double> pitches;
    pitches.reserve(active_notes_.size());
    for (const auto& [position, note] : active_notes_) {
      pitches.push_back(note.pitch);
    }
    return pitches;
  }

 private:
  // Internal process helper function.
  void ProcessInternal(double begin_position, double end_position,
                       double position_offset) noexcept;

  // Active note that is being performed.
  struct ActiveNote {
    // TODO: Add note id to check remove events etc.

    // Note end position.
    double end_position;

    // Note pitch.
    double pitch;
  };

  // List of active notes.
  std::multimap<double, ActiveNote> active_notes_;

  // Begin offset in beats.
  double begin_offset_ = 0.0;

  // Begin position in beats.
  double begin_position_ = 0.0;

  // End position in beats.
  double end_position_ = std::numeric_limits<double>::max();

  // TODO: Refactor this.
  EventCallback event_callback_;

  // Instrument identifier.
  BarelyId instrument_id_ = BarelyId_kInvalid;

  // Denotes whether sequence is looping or not.
  bool loop_ = false;

  // Loop begin offset in beats.
  double loop_begin_offset_ = 0.0;

  // Loop length in beats.
  double loop_length_ = 1.0;

  // Sorted note by position map.
  std::map<NotePositionIdPair, BarelyNoteDefinition> notes_;

  // Note positions.
  std::unordered_map<BarelyId, double> positions_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_SEQUENCE_H_
