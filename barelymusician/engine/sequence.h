#ifndef BARELYMUSICIAN_ENGINE_SEQUENCE_H_
#define BARELYMUSICIAN_ENGINE_SEQUENCE_H_

#include <functional>
#include <limits>
#include <map>
#include <unordered_map>
#include <utility>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/id.h"
#include "barelymusician/engine/instrument_event.h"

namespace barelyapi {

/// Musical note sequence.
class Sequence {
 public:
  /// Note position-id pair type.
  using NotePositionIdPair = std::pair<double, Id>;

  /// Note with position type.
  using NoteWithPosition = std::pair<double, BarelyNoteDefinition>;

  /// Note with position-id pair type.
  using NoteWithPositionIdPair =
      std::pair<NotePositionIdPair, BarelyNoteDefinition>;

  // TODO: Refactor this.
  using EventCallback = std::function<void(double, InstrumentEvent)>;

  /// Adds new note at position.
  ///
  /// @param id Note identifier.
  /// @param position Note position.
  /// @param note Note.
  /// @return True if success.
  bool AddNote(Id id, double position, BarelyNoteDefinition note) noexcept;

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
  [[nodiscard]] Id GetInstrument() const noexcept;

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

  void SetEventCallback(EventCallback event_callback) noexcept;

  /// Sets instrument.
  ///
  /// @param instrument_id Instrument identifier.
  void SetInstrument(Id instrument_id) noexcept;

  /// Sets whether sequence should be looping or not.
  ///
  /// @param loop True if looping.
  void SetLoop(bool loop) noexcept;

  /// Sets loop begin offset.
  ///
  /// @param loop_begin_offset Loop begin offset in beats.
  void SetLoopBeginOffset(double loop_begin_offset) noexcept;

  /// Sets loop length.
  ///
  /// @param loop_length Loop length in beats.
  void SetLoopLength(double loop_length) noexcept;

  /// Sets note.
  ///
  /// @param id Note identifier.
  /// @param definition Note definition.
  /// @return True if success.
  bool SetNoteDefinition(Id id, BarelyNoteDefinition definition) noexcept;

  /// Sets note position.
  ///
  /// @param id Note identifier.
  /// @return True if success.
  bool SetNotePosition(Id id, double position) noexcept;

  // TODO: Temp.
  void Stop() { active_notes_.clear(); }

  // TODO: Temp.
  std::vector<float> GetActiveNotes() {
    std::vector<float> pitches;
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
    float pitch;
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
  Id instrument_id_ = kInvalidId;

  // Denotes whether sequence is looping or not.
  bool loop_ = false;

  // Loop begin offset in beats.
  double loop_begin_offset_ = 0.0;

  // Loop length in beats.
  double loop_length_ = 1.0;

  // Sorted note by position map.
  std::map<NotePositionIdPair, BarelyNoteDefinition> notes_;

  // Note positions.
  std::unordered_map<Id, double> positions_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_SEQUENCE_H_
