#ifndef BARELYMUSICIAN_ENGINE_CONDUCTOR_H_
#define BARELYMUSICIAN_ENGINE_CONDUCTOR_H_

#include <any>

#include "barelymusician/common/status.h"
#include "barelymusician/composition/note_duration.h"
#include "barelymusician/composition/note_intensity.h"
#include "barelymusician/composition/note_pitch.h"
#include "barelymusician/engine/conductor_definition.h"

namespace barelyapi {

/// Class that wraps a conductor.
class Conductor {
 public:
  /// Constructs new |Conductor|.
  ///
  /// @param definition Conductor definition.
  explicit Conductor(ConductorDefinition definition);

  /// Destroys |Conductor|.
  ~Conductor();

  /// Copyable and movable.
  Conductor(const Conductor& other) = default;
  Conductor& operator=(const Conductor& other) = default;
  Conductor(Conductor&& other) = default;
  Conductor& operator=(Conductor&& other) = default;

  /// Sets custom data.
  ///
  /// @param data Custom data.
  void SetCustomData(std::any data);

  /// Sets parameter.
  ///
  /// @param id Parameter id.
  /// @param value Parameter value.
  void SetParam(int id, float value);

  /// Transforms note duration.
  ///
  /// @param note_duration Note duration.
  /// @return Raw note duration, or error status.
  StatusOr<double> TransformNoteDuration(NoteDuration note_duration);

  /// Transforms note intensity.
  ///
  /// @param note_intensity Note intensity.
  /// @return Raw note intensity, or error status.
  StatusOr<float> TransformNoteIntensity(NoteIntensity note_intensity);

  /// Transforms note pitch.
  ///
  /// @param note_pitch Note pitch.
  /// @return Raw note pitch, or error status.
  StatusOr<float> TransformNotePitch(NotePitch note_pitch);

 private:
  // Conductor destroy function.
  DestroyConductorFn destroy_fn_;

  // Conductor set custom data function.
  SetCustomConductorDataFn set_custom_data_fn_;

  // Conductor set parameter function.
  SetConductorParamFn set_param_fn_;

  /// Transform note duration function.
  TransformNoteDurationFn transform_note_duration_fn_;

  /// Transform note intensity function.
  TransformNoteIntensityFn transform_note_intensity_fn_;

  /// Transform note pitch function.
  TransformNotePitchFn transform_note_pitch_fn_;

  // Conductor state.
  ConductorState state_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_CONDUCTOR_H_
