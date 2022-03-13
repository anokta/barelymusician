#ifndef BARELYMUSICIAN_API_CONDUCTOR_H_
#define BARELYMUSICIAN_API_CONDUCTOR_H_

// NOLINTBEGIN
#include <stdbool.h>
#include <stdint.h>

#include "barelymusician/api/sequence.h"
#include "barelymusician/api/status.h"
#include "barelymusician/api/visibility.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Conductor alias.
typedef struct BarelyConductor* BarelyConductorHandle;

/// Note duration adjustment callback signature.
///
/// @param definition Mutable note duration definition.
/// @param user_data User data.
typedef void (*BarelyConductor_NoteDurationAdjustmentCallback)(
    BarelyNoteDurationDefinition* definition, void* user_data);

/// Note intensity adjustment callback signature.
///
/// @param definition Mutable note intensity definition.
/// @param user_data User data.
typedef void (*BarelyConductor_NoteIntensityAdjustmentCallback)(
    BarelyNoteIntensityDefinition* definition, void* user_data);

/// Note pitch adjustment callback signature.
///
/// @param definition Mutable note pitch definition.
/// @param user_data User data.
typedef void (*BarelyConductor_NotePitchAdjustmentCallback)(
    BarelyNotePitchDefinition* definition, void* user_data);

/// Creates new conductor.
///
/// @param out_handle Output conductor handle.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyConductor_Create(BarelyConductorHandle* out_handle);

/// Destroys conductor.
///
/// @param handle Conductor handle.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyConductor_Destroy(BarelyConductorHandle handle);

/// Gets note duration.
///
/// @param handle Conductor handle.
/// @param definition Note duration definition.
/// @param bypass_adjustment True if duration adjustment should be bypassed.
/// @param out_duration Output note duration.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_GetNoteDuration(
    BarelyConductorHandle handle, BarelyNoteDurationDefinition definition,
    bool bypass_adjustment, double* out_duration);

/// Gets note intensity.
///
/// @param handle Conductor handle.
/// @param intensity Note pitch intensity.
/// @param bypass_adjustment True if intensity adjustment should be bypassed.
/// @param out_intensity Output note intensity.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_GetNoteIntensity(
    BarelyConductorHandle handle, BarelyNoteIntensityDefinition definition,
    bool bypass_adjustment, double* out_intensity);

/// Gets note pitch.
///
/// @param handle Conductor handle.
/// @param definition Note pitch definition.
/// @param bypass_adjustment True if pitch adjustment should be bypassed.
/// @param out_pitch Output note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_GetNotePitch(
    BarelyConductorHandle handle, BarelyNotePitchDefinition definition,
    bool bypass_adjustment, double* out_pitch);

/// Gets root note.
///
/// @param handle Conductor handle.
/// @param out_root_pitch Output root note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_GetRootNote(
    BarelyConductorHandle handle, double* out_root_pitch);

/// Gets scale.
///
/// @param handle Conductor handle.
/// @param out_scale_pitches Output list of scale note pitches.
/// @param out_num_scale_pitches Output number of scale note pitches.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_GetScale(
    BarelyConductorHandle handle, double** out_scale_pitches,
    int32_t* out_num_scale_pitches);

/// Sets note duration adjustment callback.
///
/// @param handle Conductor handle.
/// @param adjustment_callback Note duration adjustment callback.
/// @param user_data User data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_SetNoteDurationAdjustmentCallback(
    BarelyConductorHandle handle,
    BarelyConductor_NoteDurationAdjustmentCallback adjustment_callback,
    void* user_data);

/// Sets note intensity adjustment callback.
///
/// @param handle Conductor handle.
/// @param adjustment_callback Note intensity adjustment callback.
/// @param user_data User data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_SetNoteIntensityAdjustmentCallback(
    BarelyConductorHandle handle,
    BarelyConductor_NoteIntensityAdjustmentCallback adjustment_callback,
    void* user_data);

/// Sets note pitch adjustment callback.
///
/// @param handle Conductor handle.
/// @param adjustment_callback Note pitch adjustment callback.
/// @param user_data User data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_SetNotePitchAdjustmentCallback(
    BarelyConductorHandle handle,
    BarelyConductor_NotePitchAdjustmentCallback adjustment_callback,
    void* user_data);

/// Sets root note.
///
/// @param handle Conductor handle.
/// @param root_pitch Root note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyConductor_SetRootNote(BarelyConductorHandle handle, double root_pitch);

/// Sets scale.
///
/// @param handle Conductor handle.
/// @param scale_pitches List of scale note pitches.
/// @param num_scale_pitches Number of scale note pitches.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyConductor_SetScale(BarelyConductorHandle handle, double* scale_pitches,
                         int32_t num_scale_pitches);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
// NOLINTEND

#endif  // BARELYMUSICIAN_API_CONDUCTOR_H_
