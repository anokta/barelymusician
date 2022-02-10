#include "barelymusician/engine/conductor.h"

#include <utility>
#include <variant>
#include <vector>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/common/status.h"
#include "barelymusician/composition/note_duration.h"
#include "barelymusician/composition/note_intensity.h"
#include "barelymusician/composition/note_pitch.h"
#include "barelymusician/engine/parameter.h"

namespace barelyapi {

Conductor::Conductor(const BarelyConductorDefinition& definition) noexcept
    : adjust_note_duration_fn_(definition.adjust_note_duration_fn),
      adjust_note_intensity_fn_(definition.adjust_note_intensity_fn),
      adjust_note_pitch_fn_(definition.adjust_note_pitch_fn),
      adjust_tempo_fn_(definition.adjust_tempo_fn),
      destroy_fn_(definition.destroy_fn),
      set_data_fn_(definition.set_data_fn),
      set_parameter_fn_(definition.set_parameter_fn) {
  if (definition.create_fn) {
    definition.create_fn(&state_);
  }
  parameters_.reserve(definition.num_parameter_definitions);
  for (int index = 0; index < definition.num_parameter_definitions; ++index) {
    parameters_.emplace_back(definition.parameter_definitions[index]);
  }
}

Conductor::~Conductor() noexcept {
  // Make sure to call `destroy_fn_` only if it's still valid (e.g., not moved).
  if (destroy_fn_) {
    destroy_fn_(&state_);
  }
}

double Conductor::AdjustNoteDuration(NoteDuration note_duration) noexcept {
  if (!adjust_note_duration_fn_) return std::get<double>(note_duration);
  double duration = std::get<double>(note_duration);
  adjust_note_duration_fn_(&state_, &duration);
  return duration;
}

float Conductor::AdjustNoteIntensity(NoteIntensity note_intensity) noexcept {
  if (!adjust_note_intensity_fn_) return std::get<float>(note_intensity);
  float intensity = std::get<float>(note_intensity);
  adjust_note_intensity_fn_(&state_, &intensity);
  return intensity;
}

float Conductor::AdjustNotePitch(NotePitch note_pitch) noexcept {
  if (!adjust_note_pitch_fn_) return std::get<float>(note_pitch);
  BarelyNotePitchType pitch_type = BarelyNotePitchType_kAbsolutePitch;
  float pitch = std::get<float>(note_pitch);
  adjust_note_pitch_fn_(&state_, &pitch_type, &pitch);
  return pitch;
}

double Conductor::AdjustTempo(double tempo) noexcept {
  if (!adjust_tempo_fn_) return tempo;
  adjust_tempo_fn_(&state_, &tempo);
  return tempo;
}

StatusOr<Parameter> Conductor::GetParameter(int index) const noexcept {
  if (index >= 0 && index < static_cast<int>(parameters_.size())) {
    return parameters_[index];
  }
  return Status::kInvalidArgument;
}

Status Conductor::ResetParameter(int index) noexcept {
  if (index >= 0 && index < static_cast<int>(parameters_.size())) {
    if (parameters_[index].ResetValue() && set_parameter_fn_) {
      set_parameter_fn_(&state_, index, parameters_[index].GetValue());
    }
    return Status::kOk;
  }
  return Status::kInvalidArgument;
}

void Conductor::SetData(void* data) noexcept { set_data_fn_(&state_, data); }

Status Conductor::SetParameter(int index, float value) noexcept {
  if (index >= 0 && index < static_cast<int>(parameters_.size())) {
    if (parameters_[index].SetValue(value) && set_parameter_fn_) {
      set_parameter_fn_(&state_, index, parameters_[index].GetValue());
    }
    return Status::kOk;
  }
  return Status::kInvalidArgument;
}

}  // namespace barelyapi
