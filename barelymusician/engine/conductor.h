#ifndef BARELYMUSICIAN_ENGINE_CONDUCTOR_H_
#define BARELYMUSICIAN_ENGINE_CONDUCTOR_H_

#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/status.h"
#include "barelymusician/composition/note_duration.h"
#include "barelymusician/composition/note_intensity.h"
#include "barelymusician/composition/note_pitch.h"
#include "barelymusician/engine/parameter.h"

namespace barelyapi {

/// Class that wraps a conductor.
class Conductor {
 public:
  /// Constructs new `Conductor`.
  ///
  /// @param definition Conductor definition.
  explicit Conductor(const BarelyConductorDefinition& definition = {}) noexcept;

  /// Destroys `Conductor`.
  ~Conductor() noexcept;

  /// Copyable and movable.
  Conductor(const Conductor& other) = default;
  Conductor& operator=(const Conductor& other) = default;
  Conductor(Conductor&& other) noexcept = default;
  Conductor& operator=(Conductor&& other) noexcept = default;

  /// Adjusts note duration.
  ///
  /// @param note_duration Note duration.
  /// @return Raw note duration.
  double AdjustNoteDuration(NoteDuration note_duration) noexcept;

  /// Adjusts note intensity.
  ///
  /// @param note_intensity Note intensity.
  /// @return Raw note intensity.
  float AdjustNoteIntensity(NoteIntensity note_intensity) noexcept;

  /// Adjusts note pitch.
  ///
  /// @param note_pitch Note pitch.
  /// @return Raw note pitch.
  float AdjustNotePitch(NotePitch note_pitch) noexcept;

  /// Adjusts tempo.
  ///
  /// @param tempo Original tempo in bpm.
  /// @return Adjusted tempo in bpm.
  double AdjustTempo(double tempo) noexcept;

  /// Returns parameter.
  ///
  /// @param index Parameter index.
  /// @return Parameter or error status.
  [[nodiscard]] StatusOr<Parameter> GetParameter(int index) const noexcept;

  /// Resets parameter to default value.
  ///
  /// @param index Parameter index.
  /// @return Status.
  Status ResetParameter(int index) noexcept;

  /// Sets data.
  ///
  /// @param data Data.
  void SetData(void* data) noexcept;

  /// Sets parameter value.
  ///
  /// @param index Parameter index.
  /// @param value Parameter value.
  /// @return Status.
  Status SetParameter(int index, float value) noexcept;

 private:
  // Adjust note duration function.
  BarelyConductorDefinition_AdjustNoteDurationFn adjust_note_duration_fn_;

  // Adjust note intensity function.
  BarelyConductorDefinition_AdjustNoteIntensityFn adjust_note_intensity_fn_;

  // Adjust note pitch function.
  BarelyConductorDefinition_AdjustNotePitchFn adjust_note_pitch_fn_;

  // Adjust tempo function.
  BarelyConductorDefinition_AdjustTempoFn adjust_tempo_fn_;

  // Conductor destroy function.
  BarelyConductorDefinition_DestroyFn destroy_fn_;

  // Conductor set data function.
  BarelyConductorDefinition_SetDataFn set_data_fn_;

  // Conductor set parameter function.
  BarelyConductorDefinition_SetParameterFn set_parameter_fn_;

  // Conductor state.
  void* state_;

  // Conductor parameters.
  std::vector<Parameter> parameters_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_CONDUCTOR_H_
