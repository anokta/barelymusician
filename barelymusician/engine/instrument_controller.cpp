#include "barelymusician/engine/instrument_controller.h"

#include <map>
#include <utility>
#include <variant>
#include <vector>

#include "barelymusician/common/visitor.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_event.h"

namespace barelyapi {

namespace {

// Dummy note off callback that does nothing.
void NoopNoteOffCallback(float /*pitch*/, double /*timestamp*/) {}

// Dummy note on callback that does nothing.
void NoopNoteOnCallback(float /*pitch*/, float /*intensity*/,
                        double /*timestamp*/) {}

}  // namespace

InstrumentController::InstrumentController(InstrumentDefinition definition)
    : gain_(1.0f),
      is_muted_(false),
      note_off_callback_(&NoopNoteOffCallback),
      note_on_callback_(&NoopNoteOnCallback) {
  params_.reserve(definition.param_definitions.size());
  for (const auto& param_definition : definition.param_definitions) {
    params_.emplace_back(param_definition);
  }
}

std::multimap<double, InstrumentEvent> InstrumentController::ExtractEvents() {
  return std::exchange(events_, {});
}

float InstrumentController::GetGain() const { return gain_; }

const Param* InstrumentController::GetParam(int index) const {
  if (index >= 0 && index < params_.size()) {
    return &params_[index];
  }
  return nullptr;
}

bool InstrumentController::IsMuted() const { return is_muted_; }

bool InstrumentController::IsNoteOn(float pitch) const {
  return pitches_.contains(pitch);
}

bool InstrumentController::ProcessEvent(InstrumentEvent event,
                                        double timestamp) {
  bool success = true;
  std::visit(Visitor{[&](SetDataEvent& set_data_event) noexcept {
                       SetData(set_data_event.data, timestamp);
                     },
                     [&](SetGainEvent& set_gain_event) noexcept {
                       SetGain(set_gain_event.gain, timestamp);
                     },
                     [&](SetParamEvent& set_param_event) noexcept {
                       success = SetParam(set_param_event.index,
                                          set_param_event.value, timestamp);
                     },
                     [&](StartNoteEvent& start_note_event) noexcept {
                       StartNote(start_note_event.pitch,
                                 start_note_event.intensity, timestamp);
                     },
                     [&](StopNoteEvent& stop_note_event) noexcept {
                       StopNote(stop_note_event.pitch, timestamp);
                     }},
             event);
  return true;
}

void InstrumentController::ResetAllParams(double timestamp) {
  for (int index = 0; index < static_cast<int>(params_.size()); ++index) {
    if (params_[index].ResetValue()) {
      events_.emplace_hint(events_.end(), timestamp,
                           SetParamEvent{index, params_[index].GetValue()});
    }
  }
}

bool InstrumentController::ResetParam(int index, double timestamp) {
  if (index >= 0 && index < params_.size()) {
    if (params_[index].ResetValue()) {
      events_.emplace_hint(events_.end(), timestamp,
                           SetParamEvent{index, params_[index].GetValue()});
    }
    return true;
  }
  return false;
}

void InstrumentController::SetData(void* data, double timestamp) {
  events_.emplace_hint(events_.end(), timestamp, SetDataEvent{data});
}

void InstrumentController::SetGain(float gain, double timestamp) {
  if (gain_ != gain) {
    gain_ = gain;
    if (!is_muted_) {
      events_.emplace_hint(events_.end(), timestamp, SetGainEvent{gain_});
    }
  }
}

void InstrumentController::SetMuted(bool is_muted, double timestamp) {
  if (is_muted_ != is_muted) {
    is_muted_ = is_muted;
    events_.emplace_hint(events_.end(), timestamp,
                         SetGainEvent{is_muted_ ? 0.0f : gain_});
  }
}

void InstrumentController::SetNoteOffCallback(
    NoteOffCallback note_off_callback) {
  note_off_callback_ =
      note_off_callback ? std::move(note_off_callback) : &NoopNoteOffCallback;
}

void InstrumentController::SetNoteOnCallback(NoteOnCallback note_on_callback) {
  note_on_callback_ =
      note_on_callback ? std::move(note_on_callback) : &NoopNoteOnCallback;
}

bool InstrumentController::SetParam(int index, float value, double timestamp) {
  if (index >= 0 && index < params_.size()) {
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
  if (pitches_.insert(pitch).second) {
    note_on_callback_(pitch, intensity, timestamp);
    events_.emplace_hint(events_.end(), timestamp,
                         StartNoteEvent{pitch, intensity});
  }
}

void InstrumentController::StopAllNotes(double timestamp) {
  for (const float pitch : std::exchange(pitches_, {})) {
    note_off_callback_(pitch, timestamp);
    events_.emplace_hint(events_.end(), timestamp, StopNoteEvent{pitch});
  }
}

void InstrumentController::StopNote(float pitch, double timestamp) {
  if (pitches_.erase(pitch) > 0) {
    note_off_callback_(pitch, timestamp);
    events_.emplace_hint(events_.end(), timestamp, StopNoteEvent{pitch});
  }
}

}  // namespace barelyapi
