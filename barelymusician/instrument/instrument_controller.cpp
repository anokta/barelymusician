#include "barelymusician/instrument/instrument_controller.h"

#include <algorithm>
#include <iterator>
#include <variant>

#include "barelymusician/instrument/instrument_utils.h"

namespace barelyapi {

namespace {

// Returns sanitized parameter |value| with respect to the given |definition|.
float Sanitize(const InstrumentParamDefinition& definition, float value) {
  if (definition.max_value.has_value()) {
    value = std::min(value, *definition.max_value);
  }
  if (definition.min_value.has_value()) {
    value = std::max(value, *definition.min_value);
  }
  return value;
}

}  // namespace

InstrumentController::InstrumentController(
    const InstrumentParamDefinitions& definitions) {
  params_.reserve(definitions.size());
  for (const auto& definition : definitions) {
    params_.emplace(
        definition.id,
        std::pair{definition, Sanitize(definition, definition.default_value)});
  }
}

std::vector<float> InstrumentController::GetAllNotes() const {
  return std::vector<float>{notes_.begin(), notes_.end()};
}

std::vector<Param> InstrumentController::GetAllParams() const {
  std::vector<Param> params;
  params.reserve(params_.size());
  std::transform(params_.begin(), params_.end(), std::back_inserter(params),
                 [](const auto& param) {
                   return Param{param.first, param.second.second};
                 });
  return params;
}

const float* InstrumentController::GetParam(int id) const {
  if (const auto* param = FindOrNull(params_, id)) {
    return &param->second;
  }
  return nullptr;
}

bool InstrumentController::IsNoteOn(float pitch) const {
  return notes_.find(pitch) != notes_.end();
}

void InstrumentController::ResetAllParams() {
  for (auto& [id, param] : params_) {
    param.second = Sanitize(param.first, param.first.default_value);
  }
}

bool InstrumentController::ResetParam(int id) {
  if (auto* param = FindOrNull(params_, id)) {
    param->second = Sanitize(param->first, param->first.default_value);
    return true;
  }
  return false;
}

void InstrumentController::SetAllNotesOff() { notes_.clear(); }

bool InstrumentController::SetNoteOff(float pitch) {
  return notes_.erase(pitch) > 0;
}

bool InstrumentController::SetNoteOn(float pitch) {
  return notes_.emplace(pitch).second;
}

bool InstrumentController::SetParam(int id, float value) {
  if (auto* param = FindOrNull(params_, id)) {
    param->second = Sanitize(param->first, value);
    return true;
  }
  return false;
}

}  // namespace barelyapi
