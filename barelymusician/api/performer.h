#ifndef BARELYMUSICIAN_API_PERFORMER_H_
#define BARELYMUSICIAN_API_PERFORMER_H_

// NOLINTBEGIN
#include <stdbool.h>
#include <stdint.h>

#include "barelymusician/api/conductor.h"
#include "barelymusician/api/instrument.h"
#include "barelymusician/api/status.h"
#include "barelymusician/api/visibility.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Note alias.
typedef struct BarelyNote* BarelyNoteHandle;

/// Parameter automation alias.
typedef struct BarelyParameterAutomation* BarelyParameterAutomationHandle;

/// Performer alias.
typedef struct BarelyPerformer* BarelyPerformerHandle;

/// Adds performer note at position.
///
/// @param handle Performer handle.
/// @param definition Note definition.
/// @param position Note position in beats.
/// @param out_note_handle Output note handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_AddNote(
    BarelyPerformerHandle handle, BarelyNoteDefinition definition,
    double position, BarelyNoteHandle* out_note_handle);

/// Adds performer parameter automation at position.
///
/// @param handle Performer handle.
/// @param definition Parameter automation definition.
/// @param position Parameter automation position in beats.
/// @param out_note_handle Output parameter automation handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_AddParameterAutomation(
    BarelyPerformerHandle handle,
    BarelyParameterAutomationDefinition definition, double position,
    BarelyParameterAutomationHandle* out_parameter_automation_handle);

/// Creates new performer.
///
/// @param out_handle Output performer handle.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyPerformer_Create(BarelyPerformerHandle* out_handle);

/// Destroys performer.
///
/// @param handle Performer handle.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyPerformer_Destroy(BarelyPerformerHandle handle);

/// Gets performer begin offset in beats.
///
/// @param handle Performer handle.
/// @param out_begin_offset Output begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_GetBeginOffset(
    BarelyPerformerHandle handle, double* out_begin_offset);

/// Gets performer begin position in beats.
///
/// @param handle Performer handle.
/// @param out_begin_position Output begin position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_GetBeginPosition(
    BarelyPerformerHandle handle, double* out_begin_position);

/// Gets performer end position in beats.
///
/// @param handle Performer handle.
/// @param out_end_position Output end position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_GetEndPosition(
    BarelyPerformerHandle handle, double* out_end_position);

/// Gets performer instrument.
///
/// @param handle Performer handle.
/// @param out_instrument Output instrument handle.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyPerformer_GetInstrument(BarelyPerformerHandle handle,
                              BarelyInstrumentHandle* out_instrument_handle);

/// Gets performer loop begin offset.
///
/// @param handle Performer handle.
/// @param out_loop_begin_offset Output loop begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_GetLoopBeginOffset(
    BarelyPerformerHandle handle, double* out_loop_begin_offset);

/// Gets performer loop length.
///
/// @param handle Performer handle.
/// @param out_loop_length Output loop length.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_GetLoopLength(
    BarelyPerformerHandle handle, double* out_loop_length);

/// Gets performer note definition.
///
/// @param handle Performer handle.
/// @param note_handle Note handle.
/// @param out_definition Output note definition.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_GetNoteDefinition(
    BarelyPerformerHandle handle, BarelyNoteHandle note_handle,
    BarelyNoteDefinition* out_definition);

/// Gets performer note position.
///
/// @param handle Performer handle.
/// @param note_handle Note handle.
/// @param out_position Output note position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_GetNotePosition(
    BarelyPerformerHandle handle, BarelyNoteHandle note_handle,
    double* out_position);

/// Gets performer parameter automation definition.
///
/// @param handle Performer handle.
/// @param parameter_automation_handle Parameter automation handle.
/// @param out_definition Output parameter automation definition.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_GetParameterAutomationDefinition(
    BarelyPerformerHandle handle,
    BarelyParameterAutomationHandle parameter_automation_handle,
    BarelyParameterAutomationDefinition* out_definition);

/// Gets performer parameter automation position.
///
/// @param handle Performer handle.
/// @param parameter_automation_handle Parameter automation handle.
/// @param out_position Output parameter automation position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_GetParameterAutomationPosition(
    BarelyPerformerHandle handle,
    BarelyParameterAutomationHandle parameter_automation_handle,
    double* out_position);

/// Gets whether performer is empty or not.
///
/// @param handle Performer handle.
/// @param out_is_empty Output true if empty, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_IsEmpty(BarelyPerformerHandle handle,
                                                   bool* out_is_empty);

/// Gets whether performer is looping or not.
///
/// @param handle Performer handle.
/// @param out_is_looping Output true if looping, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyPerformer_IsLooping(BarelyPerformerHandle handle, bool* out_is_looping);

/// Performs performer at range.
///
/// @param handle Performer handle.
/// @param conductor_handle Conductor handle.
/// @param begin_position Begin position in beats.
/// @param end_position End position in beats.
BARELY_EXPORT BarelyStatus BarelyPerformer_Perform(
    BarelyPerformerHandle handle, BarelyConductorHandle conductor_handle,
    double begin_position, double end_position);

/// Removes all performer notes.
///
/// @param handle Performer handle.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyPerformer_RemoveAllNotes(BarelyPerformerHandle handle);

/// Removes performer note at position.
///
/// @param handle Performer handle.
/// @param position Position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_RemoveAllNotesAtPosition(
    BarelyPerformerHandle handle, double position);

/// Removes all performer notes at range.
///
/// @param handle Performer handle.
/// @param begin_position Begin position in beats.
/// @param end_position End position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_RemoveAllNotesAtRange(
    BarelyPerformerHandle handle, double begin_position, double end_position);

/// Removes all performer parameter automations.
///
/// @param handle Performer handle.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyPerformer_RemoveAllParameterAutomations(BarelyPerformerHandle handle);

/// Removes all performer parameter automations at position.
///
/// @param handle Performer handle.
/// @param position Position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyPerformer_RemoveAllParameterAutomationsAtPosition(
    BarelyPerformerHandle handle, double position);

/// Removes all performer parameter automations at range.
///
/// @param handle Performer handle.
/// @param begin_position Begin position in beats.
/// @param end_position End position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_RemoveAllParameterAutomationsAtRange(
    BarelyPerformerHandle handle, double begin_position, double end_position);

/// Removes performer note.
///
/// @param handle Performer handle.
/// @param note_handle Note handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_RemoveNote(
    BarelyPerformerHandle handle, BarelyNoteHandle note_handle);

/// Removes performer parameter automation.
///
/// @param handle Performer handle.
/// @param parameter_automation_handle Parameter automation handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_RemoveParameterAutomation(
    BarelyPerformerHandle handle,
    BarelyParameterAutomationHandle parameter_automation_handle);

/// Sets performer begin offset.
///
/// @param handle Performer handle.
/// @param begin_offset Begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_SetBeginOffset(
    BarelyPerformerHandle handle, double begin_offset);

/// Sets performer begin position.
///
/// @param handle Performer handle.
/// @param begin_position Begin position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_SetBeginPosition(
    BarelyPerformerHandle handle, double begin_position);

/// Sets performer end position.
///
/// @param handle Performer handle.
/// @param end_position End position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_SetEndPosition(
    BarelyPerformerHandle handle, double end_position);

/// Sets performer instrument.
///
/// @param handle Performer handle.
/// @param instrument_handle Instrument handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_SetInstrument(
    BarelyPerformerHandle handle, BarelyInstrumentHandle instrument_handle);

/// Sets performer loop begin offset.
///
/// @param handle Performer handle.
/// @param loop_begin_offset Loop begin offset in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_SetLoopBeginOffset(
    BarelyPerformerHandle handle, double loop_begin_offset);

/// Sets performer loop length.
///
/// @param handle Performer handle.
/// @param loop_length Loop length in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyPerformer_SetLoopLength(BarelyPerformerHandle handle, double loop_length);

/// Sets whether performer should be looping or not.
///
/// @param handle Performer handle.
/// @param is_looping True if looping, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyPerformer_SetLooping(BarelyPerformerHandle handle, bool is_looping);

/// Sets performer note definition.
///
/// @param handle Performer handle.
/// @param note_handle Note handle.
/// @param definition Note definition.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_SetNoteDefinition(
    BarelyPerformerHandle handle, BarelyNoteHandle note_handle,
    BarelyNoteDefinition definition);

/// Sets performer note position.
///
/// @param handle Performer handle.
/// @param note_handle Note handle.
/// @param position Note position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyPerformer_SetNotePosition(BarelyPerformerHandle handle,
                                BarelyNoteHandle note_handle, double position);

/// Sets performer parameter automation definition.
///
/// @param handle Performer handle.
/// @param parameter_automation_handle Parameter automation handle.
/// @param definition Parameter automation definition.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_SetParameterAutomationDefinition(
    BarelyPerformerHandle handle,
    BarelyParameterAutomationHandle parameter_automation_handle,
    BarelyParameterAutomationDefinition definition);

/// Sets performer parameter automation position.
///
/// @param handle Performer handle.
/// @param parameter_automation_handle Parameter automation handle.
/// @param position Parameter automation position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_SetParameterAutomationPosition(
    BarelyPerformerHandle handle,
    BarelyParameterAutomationHandle parameter_automation_handle,
    double position);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
// NOLINTEND

#ifdef __cplusplus

namespace barely {

using NoteDefinition = BarelyNoteDefinition;

class Performer {
 public:
  /// Constructs new `Performer`.
  Performer() {
    const auto status = BarelyPerformer_Create(&handle_);
    assert(IsOk(static_cast<Status>(status)));
  }

  /// Destroys `Performer`.
  ~Performer() {
    if (handle_) {
      const auto status = BarelyPerformer_Destroy(handle_);
      assert(IsOk(static_cast<Status>(status)));
      handle_ = nullptr;
    }
  }

  /// Non-copyable.
  Performer(const Performer& other) = delete;
  Performer& operator=(const Performer& other) = delete;

  /// Constructs new `Performer` via move.
  ///
  /// @param other Other performer.
  Performer(Performer&& other) noexcept
      : handle_(std::exchange(other.handle_, nullptr)) {}

  /// Assigns `Performer` via move.
  ///
  /// @param other Other performer.
  Performer& operator=(Performer&& other) noexcept {
    if (this != &other) {
      if (handle_) {
        BarelyPerformer_Destroy(handle_);
      }
      handle_ = std::exchange(other.handle_, nullptr);
    }
    return *this;
  }

  /// Adds note at position.
  ///
  /// @param position Note position.
  /// @param definition Note definition.
  /// @return Note reference.
  BarelyNoteHandle AddNote(double position, NoteDefinition definition) {
    BarelyNoteHandle note_handle = nullptr;
    const auto status =
        BarelyPerformer_AddNote(handle_, definition, position, &note_handle);
    assert(IsOk(static_cast<Status>(status)));
    return note_handle;
  }

  /// Returns begin offset.
  ///
  /// @return Begin offset in beats.
  [[nodiscard]] double GetBeginOffset() const {
    double begin_offset = 0.0;
    const auto status = BarelyPerformer_GetBeginOffset(handle_, &begin_offset);
    assert(IsOk(static_cast<Status>(status)));
    return begin_offset;
  }

  /// Returns begin position.
  ///
  /// @return Begin position in beats.
  [[nodiscard]] double GetBeginPosition() const {
    double begin_position = 0.0;
    const auto status =
        BarelyPerformer_GetBeginPosition(handle_, &begin_position);
    assert(IsOk(static_cast<Status>(status)));
    return begin_position;
  }

  /// Returns end position.
  ///
  /// @return End position in beats.
  [[nodiscard]] double GetEndPosition() const {
    double end_position = 0.0;
    const auto status = BarelyPerformer_GetEndPosition(handle_, &end_position);
    assert(IsOk(static_cast<Status>(status)));
    return end_position;
  }

  // TODO: add back.
  /// Returns instrument.
  ///
  /// @return Pointer to instrument, or nullptr.
  // [[nodiscard]] const Instrument* GetInstrument() const { return instrument_;
  // }

  /// Returns loop begin offset.
  ///
  /// @return Loop begin offset in beats.
  [[nodiscard]] double GetLoopBeginOffset() const {
    double loop_begin_offset = 0.0;
    const auto status =
        BarelyPerformer_GetLoopBeginOffset(handle_, &loop_begin_offset);
    assert(IsOk(static_cast<Status>(status)));
    return loop_begin_offset;
  }

  /// Returns loop length.
  ///
  /// @return Loop length in beats.
  [[nodiscard]] double GetLoopLength() const {
    double loop_length = 0.0;
    const auto status = BarelyPerformer_GetLoopLength(handle_, &loop_length);
    assert(IsOk(static_cast<Status>(status)));
    return loop_length;
  }

  /// Returns whether performer is empty or not.
  ///
  /// @return True if empty, false otherwise.
  [[nodiscard]] bool IsEmpty() const {
    bool is_empty = false;
    const auto status = BarelyPerformer_IsEmpty(handle_, &is_empty);
    assert(IsOk(static_cast<Status>(status)));
    return is_empty;
  }

  /// Returns whether performer should be looping or not.
  ///
  /// @return True if looping, false otherwise.
  [[nodiscard]] bool IsLooping() const {
    bool is_looping = false;
    const auto status = BarelyPerformer_IsLooping(handle_, &is_looping);
    assert(IsOk(static_cast<Status>(status)));
    return is_looping;
  }

  // TODO: temp workaround for testing.
  Status Perform(const Conductor& conductor, double begin_position,
                 double end_position) {
    return static_cast<Status>(BarelyPerformer_Perform(
        handle_, conductor.handle_, begin_position, end_position));
  }

  /// Removes all notes.
  ///
  /// @return Status.
  Status RemoveAllNotes() {
    return static_cast<Status>(BarelyPerformer_RemoveAllNotes(handle_));
  }

  /// Removes all notes at position.
  ///
  /// @param position Position in beats.
  /// @return Status.
  Status RemoveAllNotes(double position) {
    return static_cast<Status>(
        BarelyPerformer_RemoveAllNotesAtPosition(handle_, position));
  }

  /// Removes all notes at range.
  ///
  /// @param begin_position Begin position in beats.
  /// @param end_position End position in beats.
  /// @return Status.
  Status RemoveAllNotes(double begin_position, double end_position) {
    return static_cast<Status>(BarelyPerformer_RemoveAllNotesAtRange(
        handle_, begin_position, end_position));
  }

  /// Removes note.
  ///
  /// @param note Note reference.
  /// @return Status.
  Status RemoveNote(BarelyNoteHandle note) {
    return static_cast<Status>(BarelyPerformer_RemoveNote(handle_, note));
  }

  /// Sets begin offset.
  ///
  /// @param begin_offset Begin offset in beats.
  /// @return Status.
  Status SetBeginOffset(double begin_offset) {
    return static_cast<Status>(
        BarelyPerformer_SetBeginOffset(handle_, begin_offset));
  }

  /// Sets begin position.
  ///
  /// @param begin_position Begin position in beats.
  /// @return Status.
  Status SetBeginPosition(double begin_position) {
    return static_cast<Status>(
        BarelyPerformer_SetBeginPosition(handle_, begin_position));
  }

  /// Sets end position.
  ///
  /// @param end_position End position in beats.
  /// @return Status.
  Status SetEndPosition(double end_position) {
    return static_cast<Status>(
        BarelyPerformer_SetEndPosition(handle_, end_position));
  }

  /// Sets instrument.
  ///
  /// @param instrument Pointer to instrument, or nullptr.
  /// @return Status.
  Status SetInstrument(const Instrument* instrument) {
    return static_cast<Status>(BarelyPerformer_SetInstrument(
        handle_, instrument ? instrument->handle_ : nullptr));
  }

  /// Sets loop begin offset.
  ///
  /// @param loop_begin_offset Loop begin offset in beats.
  /// @return Status.
  Status SetLoopBeginOffset(double loop_begin_offset) {
    return static_cast<Status>(
        BarelyPerformer_SetLoopBeginOffset(handle_, loop_begin_offset));
  }

  /// Sets loop length.
  ///
  /// @param loop_length Loop length in beats.
  /// @return Status.
  Status SetLoopLength(double loop_length) {
    return static_cast<Status>(
        BarelyPerformer_SetLoopLength(handle_, loop_length));
  }

  /// Sets whether performer should be looping or not.
  ///
  /// @param is_looping True if looping, false otherwise.
  /// @return Status.
  Status SetLooping(bool is_looping) {
    return static_cast<Status>(BarelyPerformer_SetLooping(handle_, is_looping));
  }

 private:
  // Internal api handle.
  BarelyPerformerHandle handle_;
};

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_API_PERFORMER_H_
