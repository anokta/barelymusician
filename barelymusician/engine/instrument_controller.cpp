#include "barelymusician/engine/instrument_controller.h"

#include <algorithm>
#include <iterator>
#include <utility>
#include <variant>
#include <vector>

#include "barelymusician/common/common_utils.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_event.h"
#include "barelymusician/engine/instrument_param.h"

namespace barelyapi {

namespace {

// TODO: Move to InstrumentParam *class*.
void SetValue(float value, InstrumentParam* param) {
  if (param->definition.max_value.has_value()) {
    value = std::min(value, *param->definition.max_value);
  }
  if (param->definition.min_value.has_value()) {
    value = std::max(value, *param->definition.min_value);
  }
  param->value = value;
}

InstrumentParam BuildParam(InstrumentParamDefinition definition) {
  InstrumentParam param{std::move(definition)};
  SetValue(param.definition.default_value, &param);
  return param;
}

}  // namespace

InstrumentController::InstrumentController(
    InstrumentDefinition definition,
    InstrumentParamDefinitions param_definitions,
    NoteOffCallback note_off_callback, NoteOnCallback note_on_callback)
    : definition_(std::move(definition)),
      note_off_callback_(std::move(note_off_callback)),
      note_on_callback_(std::move(note_on_callback)) {
  params_.reserve(param_definitions.size());
  for (auto& param_definition : param_definitions) {
    params_.emplace(param_definition.id,
                    BuildParam(std::move(param_definition)));
  }
}

InstrumentController::~InstrumentController() {
  if (note_off_callback_) {
    for (const auto& pitch : pitches_) {
      note_off_callback_(pitch);
    }
  }
}

const std::unordered_set<float>& InstrumentController::GetAllNotes() const {
  return pitches_;
}

const std::unordered_map<int, InstrumentParam>&
InstrumentController::GetAllParams() const {
  return params_;
}

InstrumentDefinition InstrumentController::GetDefinition() const {
  return definition_;
}

const InstrumentParam* InstrumentController::GetParam(int id) const {
  return FindOrNull(params_, id);
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
                SetValue(param.definition.default_value, &param);
                events.emplace(it->first, SetParam{id, param.value});
              }
            },
            [&](ResetParam& reset_param) {
              if (auto* param = FindOrNull(params_, reset_param.id)) {
                SetValue(param->definition.default_value, param);
                events.emplace(it->first,
                               SetParam{reset_param.id, param->value});
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
                SetValue(set_param.value, param);
                events.emplace(it->first, SetParam{set_param.id, param->value});
              }
            }},
        it->second);
  }
  events_.erase(begin, end);

  return events;
}

}  // namespace barelyapi
