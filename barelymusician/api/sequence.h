#ifndef BARELYMUSICIAN_API_SEQUENCE_H_
#define BARELYMUSICIAN_API_SEQUENCE_H_

// NOLINTBEGIN
#include <stdbool.h>
#include <stdint.h>

#include "barelymusician/api/status.h"
#include "barelymusician/api/visibility.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Note duration definition.
typedef struct BarelyNoteDurationDefinition {
  /// Value.
  double duration;
} BarelyNoteDurationDefinition;

/// Note intensity definition.
typedef struct BarelyNoteIntensityDefinition {
  /// Value.
  double intensity;
} BarelyNoteIntensityDefinition;

/// Note pitch type enum alias.
typedef int32_t BarelyNotePitchType;

/// Note pitch type enum values.
enum BarelyNotePitchType_Values {
  /// Absolute pitch.
  BarelyNotePitchType_kAbsolutePitch = 0,
  /// Relative pitch with respect to root note.
  BarelyNotePitchType_kRelativePitch = 1,
  /// Scale index with respect to root note and scale.
  BarelyNotePitchType_kScaleIndex = 2,
};

/// Note pitch definition.
typedef struct BarelyNotePitchDefinition {
  /// Type.
  BarelyNotePitchType type;

  /// Value.
  union {
    /// Absolute pitch.
    double absolute_pitch;
    /// Relative pitch.
    double relative_pitch;
    /// Scale index.
    int32_t scale_index;
  };
} BarelyNotePitchDefinition;

/// Note definition.
typedef struct BarelyNoteDefinition {
  /// Duration definition.
  BarelyNoteDurationDefinition duration_definition;

  /// Intensity definition.
  BarelyNoteIntensityDefinition intensity_definition;

  /// Pitch definition.
  BarelyNotePitchDefinition pitch_definition;
} BarelyNoteDefinition;

/// Parameter automation definition.
typedef struct BarelyParameterAutomationDefinition {
  /// Index.
  int32_t index;

  /// Value.
  double value;
};

/// Note alias.
typedef struct BarelyNote* BarelyNoteHandle;

/// Parameter automation alias.
typedef struct BarelyParameterAutomation* BarelyParameterAutomationHandle;

/// Sequence alias.
typedef struct BarelySequence* BarelySequenceHandle;

/// Sequence note callback signature.
///
/// @param definition Note definition.
/// @param position Note position.
/// @param user_data User data.
typedef void (*BarelySequence_NoteCallback)(BarelyNoteDefinition definition,
                                            double position, void* user_data);

/// Sequence parameter automation callback signature.
///
/// @param definition Parameter automation definition.
/// @param position Parameter automation position.
/// @param user_data User data.
typedef void (*BarelySequence_ParameterAutomationCallback)(
    BarelyParameterAutomationDefinition definition, double position,
    void* user_data);

/// Adds sequence note at position.
///
/// @param handle Sequence handle.
/// @param definition Note definition.
/// @param position Note position.
/// @param out_note_handle Output note handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_AddNote(
    BarelySequenceHandle handle, BarelyNoteDefinition definition,
    double position, BarelyNoteHandle* out_note_handle);

/// Adds sequence parameter automation at position.
///
/// @param handle Sequence handle.
/// @param definition Parameter automation definition.
/// @param position Parameter automation position.
/// @param out_note_handle Output parameter automation handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_AddParameterAutomation(
    BarelySequenceHandle handle, BarelyParameterAutomationDefinition definition,
    double position,
    BarelyParameterAutomationHandle* out_parameter_automation_handle);

/// Creates new sequence.
///
/// @param out_handle Output sequence handle.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelySequence_Create(BarelySequenceHandle* out_handle);

/// Destroys sequence.
///
/// @param handle Sequence handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_Destroy(BarelySequenceHandle handle);

/// Gets sequence begin offset.
///
/// @param handle Sequence handle.
/// @param out_begin_offset Output begin offset.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetBeginOffset(
    BarelySequenceHandle handle, double* out_begin_offset);

/// Gets sequence loop begin offset.
///
/// @param handle Sequence handle.
/// @param out_loop_begin_offset Output loop begin offset.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetLoopBeginOffset(
    BarelySequenceHandle handle, double* out_loop_begin_offset);

/// Gets sequence loop length.
///
/// @param handle Sequence handle.
/// @param out_loop_length Output loop length.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetLoopLength(
    BarelySequenceHandle handle, double* out_loop_length);

/// Gets sequence note definition.
///
/// @param handle Sequence handle.
/// @param note_handle Note handle.
/// @param out_definition Output note definition.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetNoteDefinition(
    BarelySequenceHandle handle, BarelyNoteHandle note_handle,
    BarelyNoteDefinition* out_definition);

/// Gets sequence note position.
///
/// @param handle Sequence handle.
/// @param note_handle Note handle.
/// @param out_position Output note position.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetNotePosition(
    BarelySequenceHandle handle, BarelyNoteHandle note_handle,
    double* out_position);

/// Gets sequence parameter automation definition.
///
/// @param handle Sequence handle.
/// @param parameter_automation_handle Parameter automation handle.
/// @param out_definition Output parameter automation definition.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetParameterAutomationDefinition(
    BarelySequenceHandle handle,
    BarelyParameterAutomationHandle parameter_automation_handle,
    BarelyParameterAutomationDefinition* out_definition);

/// Gets sequence parameter automation position.
///
/// @param handle Sequence handle.
/// @param parameter_automation_handle Parameter automation handle.
/// @param out_position Output parameter automation position.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_GetParameterAutomationPosition(
    BarelySequenceHandle handle,
    BarelyParameterAutomationHandle parameter_automation_handle,
    double* out_position);

/// Gets whether sequence is empty or not.
///
/// @param handle Sequence handle.
/// @param out_is_empty Output true if empty, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_IsEmpty(BarelySequenceHandle handle,
                                                  bool* out_is_empty);

/// Gets whether sequence is looping or not.
///
/// @param handle Sequence handle.
/// @param out_is_looping Output true if looping, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_IsLooping(BarelySequenceHandle handle,
                                                    bool* out_is_looping);

/// Processes sequence at range.
///
/// @param handle Sequence handle.
/// @param begin_position Begin position.
/// @param end_position End position.
/// @param note_callback Note callback.
/// @param parameter_automation_callback Parameter automation callback.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_Process(
    BarelySequenceHandle handle, double begin_position, double end_position,
    BarelySequence_NoteCallback note_callback,
    BarelySequence_ParameterAutomationCallback parameter_automation_callback);

/// Removes all sequence notes.
///
/// @param handle Sequence handle.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelySequence_RemoveAllNotes(BarelySequenceHandle handle);

/// Removes sequence note at position.
///
/// @param handle Sequence handle.
/// @param position Position.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_RemoveAllNotesAtPosition(
    BarelySequenceHandle handle, double position);

/// Removes all sequence notes at range.
///
/// @param handle Sequence handle.
/// @param begin_position Begin position.
/// @param end_position End position.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_RemoveAllNotesAtRange(
    BarelySequenceHandle handle, double begin_position, double end_position);

/// Removes all sequence parameter automations.
///
/// @param handle Sequence handle.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelySequence_RemoveAllParameterAutomations(BarelySequenceHandle handle);

/// Removes all sequence parameter automations at position.
///
/// @param handle Sequence handle.
/// @param position Position.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelySequence_RemoveAllParameterAutomationsAtPosition(
    BarelySequenceHandle handle, double position);

/// Removes all sequence parameter automations at range.
///
/// @param handle Sequence handle.
/// @param begin_position Begin position.
/// @param end_position End position.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_RemoveAllParameterAutomationsAtRange(
    BarelySequenceHandle handle, double begin_position, double end_position);

/// Removes sequence note.
///
/// @param handle Sequence handle.
/// @param note_handle Note handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_RemoveNote(
    BarelySequenceHandle handle, BarelyNoteHandle note_handle);

/// Removes sequence parameter automation.
///
/// @param handle Sequence handle.
/// @param parameter_automation_handle Parameter automation handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_RemoveParameterAutomation(
    BarelySequenceHandle handle,
    BarelyParameterAutomationHandle parameter_automation_handle);

/// Sets sequence begin offset.
///
/// @param handle Sequence handle.
/// @param begin_offset Begin offset.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelySequence_SetBeginOffset(BarelySequenceHandle handle, double begin_offset);

/// Sets sequence loop begin offset.
///
/// @param handle Sequence handle.
/// @param loop_begin_offset Loop begin offset.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetLoopBeginOffset(
    BarelySequenceHandle handle, double loop_begin_offset);

/// Sets sequence loop length.
///
/// @param handle Sequence handle.
/// @param loop_length Loop length.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelySequence_SetLoopLength(BarelySequenceHandle handle, double loop_length);

/// Sets whether sequence should be looping or not.
///
/// @param handle Sequence handle.
/// @param is_looping True if looping, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelySequence_SetLooping(BarelySequenceHandle handle, bool is_looping);

/// Sets sequence note definition.
///
/// @param handle Sequence handle.
/// @param note_handle Note handle.
/// @param definition Note definition.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetNoteDefinition(
    BarelySequenceHandle handle, BarelyNoteHandle note_handle,
    BarelyNoteDefinition definition);

/// Sets sequence note position.
///
/// @param handle Sequence handle.
/// @param note_handle Note handle.
/// @param position Note position.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetNotePosition(
    BarelySequenceHandle handle, BarelyNoteHandle note_handle, double position);

/// Sets sequence parameter automation definition.
///
/// @param handle Sequence handle.
/// @param parameter_automation_handle Parameter automation handle.
/// @param definition Parameter automation definition.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetParameterAutomationDefinition(
    BarelySequenceHandle handle,
    BarelyParameterAutomationHandle parameter_automation_handle,
    BarelyParameterAutomationDefinition definition);

/// Sets sequence parameter automation position.
///
/// @param handle Sequence handle.
/// @param parameter_automation_handle Parameter automation handle.
/// @param position Parameter automation position.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequence_SetParameterAutomationPosition(
    BarelySequenceHandle handle,
    BarelyParameterAutomationHandle parameter_automation_handle,
    double position);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
// NOLINTEND

#endif  // BARELYMUSICIAN_API_SEQUENCE_H_
