#include "barelymusician/engine/instrument_controller.h"

#include <variant>

namespace barelyapi {

namespace {

float Sanitize(const InstrumentParamDefinition&, float value) { return value; }

}  // namespace

InstrumentController::InstrumentController(
    InstrumentParamDefinitions param_definitions) {
  for (const auto& definition : param_definitions) {
    const float value = Sanitize(definition, definition.default_value);
    params_.emplace(definition.id, std::pair{definition, value});
  }
}

std::vector<float> InstrumentController::GetAllActiveNotes() const {
  return std::vector<float>{active_notes_.begin(), active_notes_.end()};
}

std::vector<std::pair<int, float>> InstrumentController::GetAllParams() const {
  std::vector<std::pair<int, float>> params;
  params.reserve(params_.size());
  for (const auto& [id, param] : params_) {
    params.emplace_back(id, param.second);
  };
  return params;
}

const float* InstrumentController::GetParam(int id) const {
  if (const auto it = params_.find(id); it != params_.end()) {
    return &it->second.second;
  }
  return nullptr;
}

bool InstrumentController::IsNoteOn(float pitch) const {
  return active_notes_.find(pitch) != active_notes_.end();
}

void InstrumentController::ResetAllParams() {
  for (auto& [id, param] : params_) {
    const float value = Sanitize(param.first, param.first.default_value);
    param.second = value;
  }
}

void InstrumentController::SetAllNotesOff() { active_notes_.clear(); }

bool InstrumentController::SetData(int64 timestamp, InstrumentData data) {
  bool success = false;
  std::visit(
      InstrumentDataVisitor{
          [&](const CustomData&) {},
          [&](const NoteOff& note_off) {
            if (active_notes_.erase(note_off.pitch) > 0) {
              if (note_off_callback_) {
                note_off_callback_(timestamp, note_off.pitch);
              }
              success = true;
            }
          },
          [&](const NoteOn& note_on) {
            if (active_notes_.emplace(note_on.pitch).second) {
              if (note_on_callback_) {
                note_on_callback_(timestamp, note_on.pitch, note_on.intensity);
              }
              success = true;
            }
          },
          [&](const Param& param) {
            if (auto it = params_.find(param.id); it != params_.end()) {
              it->second.second = Sanitize(it->second.first, param.value);
              success = true;
            }
          }},
      data);
  return success;
}

void InstrumentController::SetNoteOffCallback(
    NoteOffCallback note_off_callback) {
  note_off_callback_ = std::move(note_off_callback);
}
void InstrumentController::SetNoteOnCallback(NoteOnCallback note_on_callback) {
  note_on_callback_ = std::move(note_on_callback);
}

}  // namespace barelyapi
