#include "barelymusician/engine/instrument_controller.h"

#include <map>
#include <utility>
#include <variant>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/visitor.h"
#include "barelymusician/engine/instrument_event.h"

namespace barelyapi {

InstrumentController::InstrumentController(
    const BarelyInstrumentDefinition& definition) {
  parameters_.reserve(definition.num_parameter_definitions);
  for (int index = 0; index < definition.num_parameter_definitions; ++index) {
    parameters_.emplace_back(definition.parameter_definitions[index]);
  }
}

std::multimap<double, InstrumentEvent>& InstrumentController::GetEvents() {
  return events_;
}

float InstrumentController::GetGain() const { return gain_; }

const Parameter* InstrumentController::GetParameter(int index) const {
  if (index >= 0 && index < static_cast<int>(parameters_.size())) {
    return &parameters_[index];
  }
  return nullptr;
}

bool InstrumentController::IsMuted() const { return is_muted_; }

bool InstrumentController::IsNoteOn(float pitch) const {
  return pitches_.contains(pitch);
}

void InstrumentController::ProcessEvent(const InstrumentEvent& event,
                                        double timestamp) {
  std::visit(
      Visitor{[&](const SetDataEvent& set_data_event) noexcept {
                SetData(set_data_event.data, timestamp);
              },
              [&](const SetGainEvent& set_gain_event) noexcept {
                SetGain(set_gain_event.gain, timestamp);
              },
              [&](const SetParameterEvent& set_parameter_event) noexcept {
                SetParameter(set_parameter_event.index,
                             set_parameter_event.value, timestamp);
              },
              [&](const StartNoteEvent& start_note_event) noexcept {
                StartNote(start_note_event.pitch, start_note_event.intensity,
                          timestamp);
              },
              [&](const StopNoteEvent& stop_note_event) noexcept {
                StopNote(stop_note_event.pitch, timestamp);
              }},
      event);
}

void InstrumentController::ResetAllParameters(double timestamp) {
  for (int index = 0; index < static_cast<int>(parameters_.size()); ++index) {
    if (parameters_[index].ResetValue()) {
      events_.emplace_hint(
          events_.end(), timestamp,
          SetParameterEvent{index, parameters_[index].GetValue()});
    }
  }
}

bool InstrumentController::ResetParameter(int index, double timestamp) {
  if (index >= 0 && index < static_cast<int>(parameters_.size())) {
    if (parameters_[index].ResetValue()) {
      events_.emplace_hint(
          events_.end(), timestamp,
          SetParameterEvent{index, parameters_[index].GetValue()});
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
    BarelyInstrument_NoteOffCallback note_off_callback, void* user_data) {
  if (note_off_callback) {
    note_off_callback_ = [note_off_callback, user_data](float pitch,
                                                        double timestamp) {
      if (note_off_callback) {
        note_off_callback(pitch, timestamp, user_data);
      }
    };
  } else {
    note_off_callback_ = nullptr;
  }
}

void InstrumentController::SetNoteOnCallback(
    BarelyInstrument_NoteOnCallback note_on_callback, void* user_data) {
  if (note_on_callback) {
    note_on_callback_ = [note_on_callback, user_data](
                            float pitch, float intensity, double timestamp) {
      note_on_callback(pitch, intensity, timestamp, user_data);
    };
  } else {
    note_on_callback_ = nullptr;
  }
}

bool InstrumentController::SetParameter(int index, float value,
                                        double timestamp) {
  if (index >= 0 && index < static_cast<int>(parameters_.size())) {
    if (parameters_[index].SetValue(value)) {
      events_.emplace_hint(
          events_.end(), timestamp,
          SetParameterEvent{index, parameters_[index].GetValue()});
    }
    return true;
  }
  return false;
}

void InstrumentController::StartNote(float pitch, float intensity,
                                     double timestamp) {
  if (pitches_.insert(pitch).second) {
    if (note_on_callback_) {
      note_on_callback_(pitch, intensity, timestamp);
    }
    events_.emplace_hint(events_.end(), timestamp,
                         StartNoteEvent{pitch, intensity});
  }
}

void InstrumentController::StopAllNotes(double timestamp) {
  for (const float pitch : std::exchange(pitches_, {})) {
    if (note_off_callback_) {
      note_off_callback_(pitch, timestamp);
    }
    events_.emplace_hint(events_.end(), timestamp, StopNoteEvent{pitch});
  }
}

void InstrumentController::StopNote(float pitch, double timestamp) {
  if (pitches_.erase(pitch) > 0) {
    if (note_off_callback_) {
      note_off_callback_(pitch, timestamp);
    }
    events_.emplace_hint(events_.end(), timestamp, StopNoteEvent{pitch});
  }
}

}  // namespace barelyapi
