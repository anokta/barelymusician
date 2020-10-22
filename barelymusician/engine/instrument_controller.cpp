#include "barelymusician/engine/instrument_controller.h"

#include <algorithm>
#include <iterator>

namespace barelyapi {

InstrumentController::InstrumentController(
    const std::vector<std::pair<int, float>>& params) {
  for (const auto& [id, default_value] : params) {
    params_.emplace(id, InstrumentParam{default_value, default_value});
  }
}

std::vector<float> InstrumentController::GetAllNotes() const {
  return std::vector<float>(notes_.cbegin(), notes_.cend());
}

std::vector<std::pair<int, float>> InstrumentController::GetAllParams() const {
  std::vector<std::pair<int, float>> params(params_.size());
  std::transform(params_.begin(), params_.end(), std::back_inserter(params),
                 [](const auto& param_it) -> std::pair<int, float> {
                   return {param_it.first, param_it.second.value};
                 });
  return params;
}

std::optional<float> InstrumentController::GetParam(int id) const {
  if (const auto it = params_.find(id); it != params_.cend()) {
    return it->second.value;
  }
  return std::nullopt;
}

bool InstrumentController::IsNoteOn(float index) const {
  return notes_.find(index) != notes_.cend();
}

bool InstrumentController::NoteOff(float index) {
  return notes_.erase(index) > 0;
}

bool InstrumentController::NoteOn(float index) {
  return notes_.emplace(index).second;
}

void InstrumentController::ResetAllParams() {
  for (auto& [id, param] : params_) {
    param.value = param.default_value;
  }
}

std::optional<bool> InstrumentController::SetParam(int id, float value) {
  if (auto it = params_.find(id); it != params_.end()) {
    if (it->second.value != value) {
      it->second.value = value;
      return true;
    }
    return false;
  }
  return std::nullopt;
}

}  // namespace barelyapi
