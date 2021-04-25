#include "barelymusician/engine/instrument_controller.h"

#include <algorithm>
#include <iterator>
#include <variant>
#include <vector>

#include "barelymusician/common/common_utils.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_event.h"
#include "barelymusician/engine/instrument_param.h"

namespace barelyapi {

namespace {

// Returns sanitized |value| with respect to the given |param_definition|.
float Sanitize(const InstrumentParamDefinition& param_definition, float value) {
  if (param_definition.max_value.has_value()) {
    value = std::min(value, *param_definition.max_value);
  }
  if (param_definition.min_value.has_value()) {
    value = std::max(value, *param_definition.min_value);
  }
  return value;
}

}  // namespace

InstrumentController::InstrumentController(
    InstrumentDefinition definition,
    const InstrumentParamDefinitions& param_definitions,
    InstrumentNoteOffCallback note_off_callback,
    InstrumentNoteOnCallback note_on_callback)
    : definition_(std::move(definition)),
      note_off_callback_(std::move(note_off_callback)),
      note_on_callback_(std::move(note_on_callback)) {
  params_.reserve(param_definitions.size());
  for (const auto& param_definition : param_definitions) {
    params_.emplace(
        param_definition.id,
        std::pair{param_definition,
                  Sanitize(param_definition, param_definition.default_value)});
  }
}

InstrumentController::~InstrumentController() {
  if (note_off_callback_) {
    for (const auto& pitch : pitches_) {
      note_off_callback_(pitch);
    }
  }
}

std::vector<float> InstrumentController::GetAllNotes() const {
  return std::vector<float>{pitches_.begin(), pitches_.end()};
}

std::vector<InstrumentParam> InstrumentController::GetAllParams() const {
  std::vector<InstrumentParam> params;
  params.reserve(params_.size());
  std::transform(params_.begin(), params_.end(), std::back_inserter(params),
                 [](const auto& param) {
                   return InstrumentParam{param.first, param.second.second};
                 });
  return params;
}

InstrumentDefinition InstrumentController::GetDefinition() const {
  return definition_;
}

const float* InstrumentController::GetParam(int id) const {
  if (const auto* param = FindOrNull(params_, id)) {
    return &param->second;
  }
  return nullptr;
}

bool InstrumentController::IsNoteOn(float pitch) const {
  return pitches_.find(pitch) != pitches_.end();
}

void InstrumentController::Schedule(double timestamp,
                                    InstrumentControllerEvent event) {
  events_.emplace(timestamp, std::move(event));
}

void InstrumentController::Schedule(InstrumentControllerEvents events) {
  events_.merge(std::move(events));
}

InstrumentProcessorEvents InstrumentController::Update(double timestamp) {
  InstrumentProcessorEvents events;

  auto begin = events_.begin();
  auto end = events_.upper_bound(timestamp);
  for (auto it = begin; it != end; ++it) {
    std::visit(
        InstrumentEventVisitor{
            [&](ResetAllParams& /*reset_all_params*/) {
              for (auto& [id, param] : params_) {
                param.second = Sanitize(param.first, param.first.default_value);
                events.emplace(it->first, SetParam{id, param.second});
              }
            },
            [&](ResetParam& reset_param) {
              if (auto* param = FindOrNull(params_, reset_param.id)) {
                param->second =
                    Sanitize(param->first, param->first.default_value);
                events.emplace(it->first,
                               SetParam{reset_param.id, param->second});
              }
            },
            [&](SetAllNotesOff& /*all_notes_off*/) {
              for (const auto& pitch : pitches_) {
                if (note_off_callback_) {
                  note_off_callback_(pitch);
                }
                events.emplace(it->first, SetNoteOff{pitch});
              }
              pitches_.clear();
            },
            [&](SetCustomData& set_custom_data) {
              events.emplace(it->first, std::move(set_custom_data));
            },
            [&](SetNoteOff& set_note_off) {
              if (pitches_.erase(set_note_off.pitch) > 0) {
                if (note_off_callback_) {
                  note_off_callback_(set_note_off.pitch);
                }
                events.emplace(it->first, std::move(set_note_off));
              }
            },
            [&](SetNoteOn& set_note_on) {
              if (pitches_.emplace(set_note_on.pitch).second) {
                if (note_on_callback_) {
                  note_on_callback_(set_note_on.pitch, set_note_on.intensity);
                }
                events.emplace(it->first, std::move(set_note_on));
              }
            },
            [&](SetParam& set_param) {
              if (auto* param = FindOrNull(params_, set_param.id)) {
                param->second = Sanitize(param->first, set_param.value);
                set_param.value = param->second;
                events.emplace(it->first, std::move(set_param));
              }
            }},
        it->second);
  }
  events_.erase(begin, end);

  return events;
}

}  // namespace barelyapi
