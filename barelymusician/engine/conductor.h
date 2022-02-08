#ifndef BARELYMUSICIAN_ENGINE_CONDUCTOR_H_
#define BARELYMUSICIAN_ENGINE_CONDUCTOR_H_

#include <vector>

#include "barelymusician/common/status.h"
#include "barelymusician/composition/note_duration.h"
#include "barelymusician/composition/note_intensity.h"
#include "barelymusician/composition/note_pitch.h"
#include "barelymusician/engine/conductor_definition.h"
#include "barelymusician/engine/parameter.h"

namespace barelyapi {

/// Class that wraps a conductor.
class Conductor {
 public:
  /// Constructs new `Conductor`.
  ///
  /// @param definition Conductor definition.
  explicit Conductor(const ConductorDefinition& definition = {}) noexcept;

  /// Destroys `Conductor`.
  ~Conductor() noexcept;

  /// Copyable and movable.
  Conductor(const Conductor& other) = default;
  Conductor& operator=(const Conductor& other) = default;
  Conductor(Conductor&& other) noexcept = default;
  Conductor& operator=(Conductor&& other) noexcept = default;

  /// Returns parameter.
  ///
  /// @param index Parameter index.
  /// @return Parameter or error status.
  [[nodiscard]] StatusOr<Parameter> GetParam(int index) const noexcept;

  /// Sets data.
  ///
  /// @param data Data.
  void SetData(void* data) noexcept;

  /// Sets parameter.
  ///
  /// @param index Parameter index.
  /// @param value Parameter value.
  /// @return Status.
  Status SetParam(int index, float value) noexcept;

  /// Sets parameter to default.
  ///
  /// @param index Parameter index.
  /// @return Status.
  Status SetParamToDefault(int index) noexcept;

  /// Transforms note duration.
  ///
  /// @param note_duration Note duration.
  /// @return Raw note duration, or error status.
  StatusOr<double> TransformNoteDuration(NoteDuration note_duration) noexcept;

  /// Transforms note intensity.
  ///
  /// @param note_intensity Note intensity.
  /// @return Raw note intensity, or error status.
  StatusOr<float> TransformNoteIntensity(NoteIntensity note_intensity) noexcept;

  /// Transforms note pitch.
  ///
  /// @param note_pitch Note pitch.
  /// @return Raw note pitch, or error status.
  StatusOr<float> TransformNotePitch(NotePitch note_pitch) noexcept;

  /// Transforms playback tempo.
  ///
  /// @param tempo Original tempo in bpm.
  /// @return Transformed tempo in bpm.
  double TransformPlaybackTempo(double tempo) noexcept;

 private:
  // Conductor destroy function.
  DestroyConductorFn destroy_fn_;

  // Conductor set data function.
  SetConductorDataFn set_data_fn_;

  // Conductor set parameter function.
  SetConductorParamFn set_param_fn_;

  // Transform note duration function.
  TransformNoteDurationFn transform_note_duration_fn_;

  // Transform note intensity function.
  TransformNoteIntensityFn transform_note_intensity_fn_;

  // Transform note pitch function.
  TransformNotePitchFn transform_note_pitch_fn_;

  // Transform playback tempo function.
  TransformPlaybackTempoFn transform_playback_tempo_fn_;

  // Conductor state.
  void* state_;

  // Conductor parameters.
  std::vector<Parameter> params_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_CONDUCTOR_H_
