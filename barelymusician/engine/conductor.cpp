#include "barelymusician/engine/conductor.h"

#include <any>
#include <utility>
#include <variant>
#include <vector>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/common/status.h"
#include "barelymusician/composition/note_duration.h"
#include "barelymusician/composition/note_intensity.h"
#include "barelymusician/composition/note_pitch.h"
#include "barelymusician/engine/conductor_definition.h"
#include "barelymusician/engine/param.h"

namespace barelyapi {

namespace {

// Dummy set custom conductor data function that does nothing.
void NoopSetCustomConductorDataFn(ConductorState* /*state*/,
                                  std::any /*data*/) noexcept {}

// Dummy set conductor parameter function that does nothing.
void NoopSetConductorParamFn(ConductorState* /*state*/, int /*index*/,
                             float /*value*/) noexcept {}

// Dummy transform note duration function that returns raw note duration.
StatusOr<double> NoopTransformNoteDurationFn(
    ConductorState* /*state*/, const NoteDuration& note_duration) noexcept {
  if (std::holds_alternative<double>(note_duration)) {
    return std::get<double>(note_duration);
  }
  return Status::kUnimplemented;
}

// Dummy transform note intensity function that returns raw note intensity.
StatusOr<float> NoopTransformNoteIntensityFn(
    ConductorState* /*state*/, const NoteIntensity& note_intensity) noexcept {
  if (std::holds_alternative<float>(note_intensity)) {
    return std::get<float>(note_intensity);
  }
  return Status::kUnimplemented;
}

// Dummy transform note pitch function that returns raw note pitch.
StatusOr<float> NoopTransformNotePitchFn(ConductorState* /*state*/,
                                         const NotePitch& note_pitch) noexcept {
  if (std::holds_alternative<float>(note_pitch)) {
    return std::get<float>(note_pitch);
  }
  return Status::kUnimplemented;
}

// Dummy transform playback tempo function that returns the original tempo.
double NoopTransformPlaybackTempoFn(ConductorState* /*state*/,
                                    double tempo) noexcept {
  return tempo;
}

}  // namespace

Conductor::Conductor(ConductorDefinition definition) noexcept
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
  params_.reserve(definition.param_definitions.size());
  for (auto& param_definition : definition.param_definitions) {
    params_.emplace_back(std::move(std::move(param_definition)));
  }
}

Conductor::~Conductor() noexcept {
  // Make sure to call |destroy_fn_| only if it's still valid (e.g., not moved).
  if (destroy_fn_) {
    destroy_fn_(&state_);
  }
}

StatusOr<Param> Conductor::GetParam(int index) const noexcept {
  if (index >= 0 && index < static_cast<int>(params_.size())) {
    return params_[index];
  }
  return Status::kInvalidArgument;
}

void Conductor::SetCustomData(std::any data) noexcept {
  set_custom_data_fn_(&state_, std::move(data));
}

Status Conductor::SetParam(int index, float value) noexcept {
  if (index >= 0 && index < static_cast<int>(params_.size())) {
    if (params_[index].SetValue(value)) {
      set_param_fn_(&state_, index, params_[index].GetValue());
    }
    return Status::kOk;
  }
  return Status::kInvalidArgument;
}

Status Conductor::SetParamToDefault(int index) noexcept {
  if (index >= 0 && index < static_cast<int>(params_.size())) {
    if (params_[index].ResetValue()) {
      set_param_fn_(&state_, index, params_[index].GetValue());
    }
    return Status::kOk;
  }
  return Status::kInvalidArgument;
}

StatusOr<double> Conductor::TransformNoteDuration(
    NoteDuration note_duration) noexcept {
  return transform_note_duration_fn_(&state_, std::move(note_duration));
}

StatusOr<float> Conductor::TransformNoteIntensity(
    NoteIntensity note_intensity) noexcept {
  return transform_note_intensity_fn_(&state_, std::move(note_intensity));
}

StatusOr<float> Conductor::TransformNotePitch(NotePitch note_pitch) noexcept {
  return transform_note_pitch_fn_(&state_, std::move(note_pitch));
}

double Conductor::TransformPlaybackTempo(double tempo) noexcept {
  return transform_playback_tempo_fn_(&state_, tempo);
}

}  // namespace barelyapi
