#include "barelymusician/engine/instrument_controller.h"

#include <cassert>
#include <map>
#include <utility>
#include <variant>

#include "barelymusician/common/visitor.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_event.h"

namespace barelyapi {

InstrumentController::InstrumentController(InstrumentDefinition definition,
                                           NoteOffCallback note_off_callback,
                                           NoteOnCallback note_on_callback)
    : gain_(1.0f),
      is_muted_(false),
      note_off_callback_(std::move(note_off_callback)),
      note_on_callback_(std::move(note_on_callback)) {
  assert(note_off_callback_);
  assert(note_on_callback_);
  params_.reserve(definition.param_definitions.size());
  for (auto& param_definition : definition.param_definitions) {
    params_.push_back(Param{std::move(param_definition)});
  }
}

std::multimap<double, InstrumentEvent>& InstrumentController::GetEvents() {
  return events_;
}

float InstrumentController::GetGain() const { return gain_; }

const Param* InstrumentController::GetParam(int index) const {
  assert(index >= 0);
  if (index < static_cast<int>(params_.size())) {
    return &params_[index];
  }
  return nullptr;
}

bool InstrumentController::IsMuted() const { return is_muted_; }

bool InstrumentController::IsNoteOn(float pitch) const {
  return pitches_.contains(pitch);
}

void InstrumentController::ProcessEvent(InstrumentEvent event,
                                        double timestamp) {
  std::visit(Visitor{[&](const SetDataEvent& set_data_event) noexcept {
                       SetData(set_data_event.data, timestamp);
                     },
                     [&](const SetGainEvent& set_gain_event) noexcept {
                       SetGain(set_gain_event.gain, timestamp);
                     },
                     [&](const SetParamEvent& set_param_event) noexcept {
                       SetParam(set_param_event.index, set_param_event.value,
                                timestamp);
                     },
                     [&](const StartNoteEvent& start_note_event) noexcept {
                       StartNote(start_note_event.pitch,
                                 start_note_event.intensity, timestamp);
                     },
                     [&](const StopNoteEvent& stop_note_event) noexcept {
                       StopNote(stop_note_event.pitch, timestamp);
                     }},
             event);
}

void InstrumentController::ResetAllParams(double timestamp) {
  assert(timestamp >= 0.0);
  for (int index = 0; index < static_cast<int>(params_.size()); ++index) {
    if (params_[index].ResetValue()) {
      events_.emplace_hint(events_.end(), timestamp,
                           SetParamEvent{index, params_[index].GetValue()});
    }
  }
}

bool InstrumentController::ResetParam(int index, double timestamp) {
  assert(index >= 0);
  assert(timestamp >= 0.0);
  if (index < static_cast<int>(params_.size())) {
    if (params_[index].ResetValue()) {
      events_.emplace_hint(events_.end(), timestamp,
                           SetParamEvent{index, params_[index].GetValue()});
    }
    return true;
  }
  return false;
}

void InstrumentController::SetData(void* data, double timestamp) {
  assert(timestamp >= 0.0);
  events_.emplace_hint(events_.end(), timestamp, SetDataEvent{data});
}

void InstrumentController::SetGain(float gain, double timestamp) {
  assert(timestamp >= 0.0);
  if (gain_ != gain) {
    gain_ = gain;
    if (!is_muted_) {
      events_.emplace_hint(events_.end(), timestamp, SetGainEvent{gain_});
    }
  }
}

void InstrumentController::SetMuted(bool is_muted, double timestamp) {
  assert(timestamp >= 0.0);
  if (is_muted_ != is_muted) {
    is_muted_ = is_muted;
    events_.emplace_hint(events_.end(), timestamp,
                         SetGainEvent{is_muted_ ? 0.0f : gain_});
  }
}

void InstrumentController::SetNoteOffCallback(
    NoteOffCallback note_off_callback) {
  assert(note_off_callback);
  note_off_callback_ = std::move(note_off_callback);
}

void InstrumentController::SetNoteOnCallback(NoteOnCallback note_on_callback) {
  assert(note_on_callback);
  note_on_callback_ = std::move(note_on_callback);
}

bool InstrumentController::SetParam(int index, float value, double timestamp) {
  assert(index >= 0);
  assert(timestamp >= 0.0);
  if (index < static_cast<int>(params_.size())) {
    if (params_[index].SetValue(value)) {
      events_.emplace_hint(events_.end(), timestamp,
                           SetParamEvent{index, params_[index].GetValue()});
    }
    return true;
  }
  return false;
}

void InstrumentController::StartNote(float pitch, float intensity,
                                     double timestamp) {
  assert(timestamp >= 0.0);
  if (pitches_.insert(pitch).second) {
    note_on_callback_(pitch, intensity, timestamp);
    events_.emplace_hint(events_.end(), timestamp,
                         StartNoteEvent{pitch, intensity});
  }
}

void InstrumentController::StopAllNotes(double timestamp) {
  assert(timestamp >= 0.0);
  for (const float pitch : std::exchange(pitches_, {})) {
    note_off_callback_(pitch, timestamp);
    events_.emplace_hint(events_.end(), timestamp, StopNoteEvent{pitch});
  }
}

void InstrumentController::StopNote(float pitch, double timestamp) {
  assert(timestamp >= 0.0);
  if (pitches_.erase(pitch) > 0) {
    note_off_callback_(pitch, timestamp);
    events_.emplace_hint(events_.end(), timestamp, StopNoteEvent{pitch});
  }
}

}  // namespace barelyapi
