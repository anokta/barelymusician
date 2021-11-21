#ifndef BARELYMUSICIAN_COMPOSITION_CONDUCTOR_DEFINITION_H_
#define BARELYMUSICIAN_COMPOSITION_CONDUCTOR_DEFINITION_H_

#include <any>
#include <functional>

#include "barelymusician/common/status.h"
#include "barelymusician/composition/note_duration.h"
#include "barelymusician/composition/note_intensity.h"
#include "barelymusician/composition/note_pitch.h"

namespace barelyapi {

/// Conductor state type.
using ConductorState = std::any;

/// Conductor create function signature.
///
/// @param state Pointer to conductor state.
using CreateConductorFn = std::function<void(ConductorState* state)>;

/// Conductor destroy function signature.
///
/// @param state Pointer to conductor state.
using DestroyConductorFn = std::function<void(ConductorState* state)>;

/// Conductor set custom data function signature.
///
/// @param state Pointer to conductor state.
/// @param data Custom data.
using SetCustomConductorDataFn =
    std::function<void(ConductorState* state, std::any data)>;

/// Conductor set parameter function signature.
///
/// @param state Pointer to conductor state.
/// @param id Parameter id.
/// @param value Parameter value.
using SetConductorParamFn =
    std::function<void(ConductorState* state, int id, float value)>;

/// Conductor transform note duration function signature.
///
/// @param state Pointer to conductor state.
/// @param note_duration Note duration.
/// @return Raw note duration, or error status.
using TransformNoteDurationFn = std::function<StatusOr<double>(
    ConductorState* state, const NoteDuration& note_duration)>;

/// Conductor transform note intensity function signature.
///
/// @param state Pointer to conductor state.
/// @param note_intensity Note intensity.
/// @return Raw note intensity, or error status.
using TransformNoteIntensityFn = std::function<StatusOr<float>(
    ConductorState* state, const NoteIntensity& note_intensity)>;

/// Conductor transform note intensity function signature.
///
/// @param state Pointer to conductor state.
/// @param note_pitch Note pitch.
/// @return Raw note pitch, or error status.
using TransformNotePitchFn = std::function<StatusOr<float>(
    ConductorState* state, const NotePitch& note_pitch)>;

/// Conductor definition.
struct ConductorDefinition {
  /// Create function.
  CreateConductorFn create_fn;

  /// Destroy function.
  DestroyConductorFn destroy_fn;

  /// Set custom data function.
  SetCustomConductorDataFn set_custom_data_fn;

  /// Set parameter function.
  SetConductorParamFn set_param_fn;

  /// Transform note duration function.
  TransformNoteDurationFn transform_note_duration_fn;

  /// Transform note intensity function.
  TransformNoteIntensityFn transform_note_intensity_fn;

  /// Transform note pitch function.
  TransformNotePitchFn transform_note_pitch_fn;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_CONDUCTOR_DEFINITION_H_
