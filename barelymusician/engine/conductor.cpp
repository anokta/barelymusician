#include "barelymusician/engine/conductor.h"

#include <any>
#include <utility>
#include <variant>

#include "barelymusician/common/status.h"
#include "barelymusician/composition/note_duration.h"
#include "barelymusician/composition/note_intensity.h"
#include "barelymusician/composition/note_pitch.h"
#include "barelymusician/engine/conductor_definition.h"

namespace barely {

namespace {

// Dummy set custom conductor data function that does nothing.
void NoopSetCustomConductorDataFn(ConductorState* /*state*/,
                                  std::any /*data*/) {}

// Dummy set conductor parameter function that does nothing.
void NoopSetConductorParamFn(ConductorState* /*state*/, int /*id*/,
                             float /*value*/) {}

// Dummy transform note duration function that returns raw note duration.
StatusOr<double> NoopTransformNoteDurationFn(
    ConductorState* /*state*/, const NoteDuration& note_duration) {
  if (std::holds_alternative<double>(note_duration)) {
    return std::get<double>(note_duration);
  }
  return Status::kUnimplemented;
}

// Dummy transform note intensity function that returns raw note intensity.
StatusOr<float> NoopTransformNoteIntensityFn(
    ConductorState* /*state*/, const NoteIntensity& note_intensity) {
  if (std::holds_alternative<float>(note_intensity)) {
    return std::get<float>(note_intensity);
  }
  return Status::kUnimplemented;
}

// Dummy transform note pitch function that returns raw note pitch.
StatusOr<float> NoopTransformNotePitchFn(ConductorState* /*state*/,
                                         const NotePitch& note_pitch) {
  if (std::holds_alternative<float>(note_pitch)) {
    return std::get<float>(note_pitch);
  }
  return Status::kUnimplemented;
}

// Dummy transform playback tempo function that returns the original tempo.
double NoopTransformPlaybackTempoFn(ConductorState* /*state*/, double tempo) {
  return tempo;
}

}  // namespace

Conductor::Conductor(ConductorDefinition definition)
    : destroy_fn_(std::move(definition.destroy_fn)),
      set_custom_data_fn_(definition.set_custom_data_fn
                              ? std::move(definition.set_custom_data_fn)
                              : &NoopSetCustomConductorDataFn),
      set_param_fn_(definition.set_param_fn ? std::move(definition.set_param_fn)
                                            : &NoopSetConductorParamFn),
      transform_note_duration_fn_(
          definition.transform_note_duration_fn
              ? std::move(definition.transform_note_duration_fn)
              : &NoopTransformNoteDurationFn),
      transform_note_intensity_fn_(
          definition.transform_note_intensity_fn
              ? std::move(definition.transform_note_intensity_fn)
              : &NoopTransformNoteIntensityFn),
      transform_note_pitch_fn_(
          definition.transform_note_pitch_fn
              ? std::move(definition.transform_note_pitch_fn)
              : &NoopTransformNotePitchFn),
      transform_playback_tempo_fn_(
          definition.transform_playback_tempo_fn
              ? std::move(definition.transform_playback_tempo_fn)
              : &NoopTransformPlaybackTempoFn) {
  if (definition.create_fn) {
    definition.create_fn(&state_);
  }
}

Conductor::~Conductor() {
  // Make sure to call |destroy_fn_| only if it's still valid (e.g., not moved).
  if (destroy_fn_) {
    destroy_fn_(&state_);
  }
}

void Conductor::SetCustomData(std::any data) {
  set_custom_data_fn_(&state_, std::move(data));
}

void Conductor::SetParam(int id, float value) {
  set_param_fn_(&state_, id, value);
}

StatusOr<double> Conductor::TransformNoteDuration(NoteDuration note_duration) {
  return transform_note_duration_fn_(&state_, std::move(note_duration));
}

StatusOr<float> Conductor::TransformNoteIntensity(
    NoteIntensity note_intensity) {
  return transform_note_intensity_fn_(&state_, std::move(note_intensity));
}

StatusOr<float> Conductor::TransformNotePitch(NotePitch note_pitch) {
  return transform_note_pitch_fn_(&state_, std::move(note_pitch));
}

double Conductor::TransformPlaybackTempo(double tempo) {
  return transform_playback_tempo_fn_(&state_, tempo);
}

}  // namespace barely
