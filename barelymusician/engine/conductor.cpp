#include "barelymusician/engine/conductor.h"

#include <utility>
#include <variant>
#include <vector>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/common/status.h"
#include "barelymusician/composition/note_duration.h"
#include "barelymusician/composition/note_intensity.h"
#include "barelymusician/composition/note_pitch.h"
#include "barelymusician/engine/conductor_definition.h"
#include "barelymusician/engine/parameter.h"

namespace barelyapi {

Conductor::Conductor(const ConductorDefinition& definition) noexcept
    : destroy_fn_(definition.destroy_fn),
      set_data_fn_(definition.set_data_fn),
      set_param_fn_(definition.set_param_fn),
      transform_note_duration_fn_(definition.transform_note_duration_fn),
      transform_note_intensity_fn_(definition.transform_note_intensity_fn),
      transform_note_pitch_fn_(definition.transform_note_pitch_fn),
      transform_playback_tempo_fn_(definition.transform_playback_tempo_fn) {
  if (definition.create_fn) {
    definition.create_fn(&state_);
  }
  params_.reserve(definition.param_definitions.size());
  for (const auto& param_definition : definition.param_definitions) {
    params_.emplace_back(param_definition);
  }
}

Conductor::~Conductor() noexcept {
  // Make sure to call `destroy_fn_` only if it's still valid (e.g., not moved).
  if (destroy_fn_) {
    destroy_fn_(&state_);
  }
}

StatusOr<Parameter> Conductor::GetParam(int index) const noexcept {
  if (index >= 0 && index < static_cast<int>(params_.size())) {
    return params_[index];
  }
  return Status::kInvalidArgument;
}

void Conductor::SetData(void* data) noexcept { set_data_fn_(&state_, data); }

Status Conductor::SetParam(int index, float value) noexcept {
  if (index >= 0 && index < static_cast<int>(params_.size())) {
    if (params_[index].SetValue(value) && set_param_fn_) {
      set_param_fn_(&state_, index, params_[index].GetValue());
    }
    return Status::kOk;
  }
  return Status::kInvalidArgument;
}

Status Conductor::SetParamToDefault(int index) noexcept {
  if (index >= 0 && index < static_cast<int>(params_.size())) {
    if (params_[index].ResetValue() && set_param_fn_) {
      set_param_fn_(&state_, index, params_[index].GetValue());
    }
    return Status::kOk;
  }
  return Status::kInvalidArgument;
}

StatusOr<double> Conductor::TransformNoteDuration(
    NoteDuration note_duration) noexcept {
  if (!transform_note_duration_fn_) return std::get<double>(note_duration);
  return transform_note_duration_fn_(&state_, note_duration);
}

StatusOr<float> Conductor::TransformNoteIntensity(
    NoteIntensity note_intensity) noexcept {
  if (!transform_note_intensity_fn_) return std::get<float>(note_intensity);
  return transform_note_intensity_fn_(&state_, note_intensity);
}

StatusOr<float> Conductor::TransformNotePitch(NotePitch note_pitch) noexcept {
  if (!transform_note_pitch_fn_) return std::get<float>(note_pitch);
  return transform_note_pitch_fn_(&state_, note_pitch);
}

double Conductor::TransformPlaybackTempo(double tempo) noexcept {
  if (!transform_playback_tempo_fn_) return tempo;
  return transform_playback_tempo_fn_(&state_, tempo);
}

}  // namespace barelyapi
