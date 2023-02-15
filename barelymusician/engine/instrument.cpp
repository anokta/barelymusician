#include "barelymusician/engine/instrument.h"

#include <cassert>
#include <cstddef>
#include <utility>
#include <variant>
#include <vector>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/common/seconds.h"
#include "barelymusician/engine/control.h"
#include "barelymusician/engine/message.h"
#include "barelymusician/engine/status.h"

namespace barely::internal {

namespace {

// Builds corresponding controls for a given list of control `definitions`.
std::vector<Control> BuildControls(const auto* definitions,
                                   int definition_count) noexcept {
  std::vector<Control> controls;
  controls.reserve(definition_count);
  for (int index = 0; index < definition_count; ++index) {
    controls.emplace_back(definitions[index]);
  }
  return controls;
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
Instrument::Instrument(const InstrumentDefinition& definition, int frame_rate,
                       double initial_tempo, double initial_timestamp) noexcept
    : destroy_callback_(definition.destroy_callback),
      process_callback_(definition.process_callback),
      set_control_callback_(definition.set_control_callback),
      set_data_callback_(definition.set_data_callback),
      set_note_control_callback_(definition.set_note_control_callback),
      set_note_off_callback_(definition.set_note_off_callback),
      set_note_on_callback_(definition.set_note_on_callback),
      frame_rate_(frame_rate),
      default_note_controls_(
          BuildControls(definition.note_control_definitions,
                        definition.note_control_definition_count)),
      controls_(BuildControls(definition.control_definitions,
                              definition.control_definition_count)),
      tempo_(initial_tempo),
      timestamp_(initial_timestamp) {
  assert(frame_rate > 0);
  assert(initial_tempo > 0.0);
  assert(initial_tempo > 0.0);
  if (definition.create_callback) {
    definition.create_callback(&state_, frame_rate);
  }
  if (set_control_callback_) {
    for (int index = 0; index < definition.control_definition_count; ++index) {
      set_control_callback_(&state_, index, controls_[index].GetValue(), 0.0);
    }
  }
}

Instrument::~Instrument() noexcept {
  if (destroy_callback_) {
    destroy_callback_(&state_);
  }
}

StatusOr<double> Instrument::GetControl(int index) const noexcept {
  if (index >= 0 && index < static_cast<int>(controls_.size())) {
    return controls_[index].GetValue();
  }
  return Status::InvalidArgument();
}

StatusOr<double> Instrument::GetNoteControl(double pitch,
                                            int index) const noexcept {
  assert(index >= 0);
  if (const auto* note_controls = FindOrNull(note_controls_, pitch)) {
    if (index >= 0 && index < static_cast<int>(note_controls->size())) {
      return (*note_controls)[index].GetValue();
    }
    return Status::InvalidArgument();
  }
  return Status::NotFound();
}

bool Instrument::IsNoteOn(double pitch) const noexcept {
  return note_controls_.contains(pitch);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Instrument::Process(double* output_samples, int output_channel_count,
                         int output_frame_count, double timestamp) noexcept {
  assert(output_samples || output_channel_count == 0 ||
         output_frame_count == 0);
  assert(output_channel_count >= 0);
  assert(output_frame_count >= 0);
  assert(timestamp >= 0.0);
  int frame = 0;
  // Process *all* messages before the end timestamp.
  const double end_timestamp =
      timestamp + SecondsFromFrames(frame_rate_, output_frame_count);
  for (auto* message = message_queue_.GetNext(end_timestamp); message;
       message = message_queue_.GetNext(end_timestamp)) {
    const int message_frame =
        FramesFromSeconds(frame_rate_, message->first - timestamp);
    if (frame < message_frame) {
      if (process_callback_) {
        process_callback_(&state_,
                          &output_samples[output_channel_count * frame],
                          output_channel_count, message_frame - frame);
      }
      frame = message_frame;
    }
    std::visit(MessageVisitor{
                   [this](ControlMessage& control_message) noexcept {
                     if (set_control_callback_) {
                       set_control_callback_(&state_, control_message.index,
                                             control_message.value,
                                             control_message.slope_per_frame);
                     }
                   },
                   [this](DataMessage& data_message) noexcept {
                     if (set_data_callback_) {
                       data_.swap(data_message.data);
                       set_data_callback_(&state_, data_.data(),
                                          static_cast<int>(data_.size()));
                     }
                   },
                   [this](NoteControlMessage& note_control_message) noexcept {
                     if (set_note_control_callback_) {
                       set_note_control_callback_(
                           &state_, note_control_message.pitch,
                           note_control_message.index,
                           note_control_message.value,
                           note_control_message.slope_per_frame);
                     }
                   },
                   [this](NoteOffMessage& note_off_message) noexcept {
                     if (set_note_off_callback_) {
                       set_note_off_callback_(&state_, note_off_message.pitch);
                     }
                   },
                   [this](NoteOnMessage& note_on_message) noexcept {
                     if (set_note_on_callback_) {
                       set_note_on_callback_(&state_, note_on_message.pitch,
                                             note_on_message.intensity);
                     }
                   }},
               message->second);
  }
  // Process the rest of the buffer.
  if (frame < output_frame_count && process_callback_) {
    process_callback_(&state_, &output_samples[output_channel_count * frame],
                      output_channel_count, output_frame_count - frame);
  }
}

void Instrument::ResetAllControls() noexcept {
  for (int index = 0; index < static_cast<int>(controls_.size()); ++index) {
    if (auto& control = controls_[index]; control.Reset()) {
      if (note_control_event_callback_) {
        control_event_callback_(index, control.GetValue());
      }
      message_queue_.Add(timestamp_,
                         ControlMessage{index, control.GetValue(), 0.0});
    }
  }
}

Status Instrument::ResetAllNoteControls(double pitch) noexcept {
  if (auto* note_controls = FindOrNull(note_controls_, pitch)) {
    for (int index = 0; index < static_cast<int>(note_controls->size());
         ++index) {
      if (auto& note_control = (*note_controls)[index]; note_control.Reset()) {
        if (note_control_event_callback_) {
          note_control_event_callback_(pitch, index, note_control.GetValue());
        }
        message_queue_.Add(
            timestamp_,
            NoteControlMessage{pitch, index, note_control.GetValue(), 0.0});
      }
    }
    return Status::Ok();
  }
  return Status::NotFound();
}

Status Instrument::ResetControl(int index) noexcept {
  if (index >= 0 && index < static_cast<int>(controls_.size())) {
    if (auto& control = controls_[index]; control.Reset()) {
      if (note_control_event_callback_) {
        control_event_callback_(index, control.GetValue());
      }
      message_queue_.Add(timestamp_,
                         ControlMessage{index, control.GetValue(), 0.0});
    }
    return Status::Ok();
  }
  return Status::InvalidArgument();
}

Status Instrument::ResetNoteControl(double pitch, int index) noexcept {
  assert(index >= 0);
  if (auto* note_controls = FindOrNull(note_controls_, pitch)) {
    if (index >= 0 && index < static_cast<int>(note_controls->size())) {
      if (auto& note_control = (*note_controls)[index]; note_control.Reset()) {
        if (note_control_event_callback_) {
          note_control_event_callback_(pitch, index, note_control.GetValue());
        }
        message_queue_.Add(
            timestamp_,
            NoteControlMessage{pitch, index, note_control.GetValue(), 0.0});
      }
      return Status::Ok();
    }
    return Status::InvalidArgument();
  }
  return Status::NotFound();
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Instrument::SetAllNotesOff() noexcept {
  for (const auto& [pitch, note_controls] : std::exchange(note_controls_, {})) {
    if (note_off_event_callback_) {
      note_off_event_callback_(pitch);
    }
    message_queue_.Add(timestamp_, NoteOffMessage{pitch});
  }
}

Status Instrument::SetControl(int index, double value,
                              double slope_per_beat) noexcept {
  if (index >= 0 && index < static_cast<int>(controls_.size())) {
    if (auto& control = controls_[index]; control.Set(value, slope_per_beat)) {
      if (note_control_event_callback_) {
        control_event_callback_(index, control.GetValue());
      }
      message_queue_.Add(timestamp_,
                         ControlMessage{index, control.GetValue(),
                                        GetSlopePerFrame(slope_per_beat)});
    }
    return Status::Ok();
  }
  return Status::InvalidArgument();
}

void Instrument::SetControlEventCallback(
    ControlEventCallback callback) noexcept {
  control_event_callback_ = std::move(callback);
}

void Instrument::SetData(std::vector<std::byte> data) noexcept {
  message_queue_.Add(timestamp_, DataMessage{std::move(data)});
}

Status Instrument::SetNoteControl(double pitch, int index, double value,
                                  double slope_per_beat) noexcept {
  if (auto* note_controls = FindOrNull(note_controls_, pitch)) {
    if (index >= 0 && index < static_cast<int>(controls_.size())) {
      if (auto& note_control = (*note_controls)[index];
          note_control.Set(value, slope_per_beat)) {
        if (note_control_event_callback_) {
          note_control_event_callback_(pitch, index, note_control.GetValue());
        }
        message_queue_.Add(
            timestamp_,
            NoteControlMessage{pitch, index, note_control.GetValue(),
                               GetSlopePerFrame(slope_per_beat)});
      }
      return Status::Ok();
    }
    return Status::InvalidArgument();
  }
  return Status::NotFound();
}

void Instrument::SetNoteControlEventCallback(
    NoteControlEventCallback callback) noexcept {
  note_control_event_callback_ = std::move(callback);
}

void Instrument::SetNoteOff(double pitch) noexcept {
  if (note_controls_.erase(pitch) > 0) {
    if (note_off_event_callback_) {
      note_off_event_callback_(pitch);
    }
    message_queue_.Add(timestamp_, NoteOffMessage{pitch});
  }
}

void Instrument::SetNoteOffEventCallback(
    NoteOffEventCallback callback) noexcept {
  note_off_event_callback_ = std::move(callback);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Instrument::SetNoteOn(double pitch, double intensity) noexcept {
  if (note_controls_.try_emplace(pitch, default_note_controls_).second) {
    if (note_on_event_callback_) {
      note_on_event_callback_(pitch, intensity);
    }
    message_queue_.Add(timestamp_, NoteOnMessage{pitch, intensity});
    for (int index = 0; index < static_cast<int>(default_note_controls_.size());
         ++index) {
      message_queue_.Add(
          timestamp_,
          NoteControlMessage{pitch, index,
                             default_note_controls_[index].GetValue()});
    }
  }
}

void Instrument::SetNoteOnEventCallback(NoteOnEventCallback callback) noexcept {
  note_on_event_callback_ = std::move(callback);
}

void Instrument::SetTempo(double tempo) noexcept {
  assert(tempo_ != tempo);
  tempo_ = tempo;
  // Update controls.
  for (int index = 0; index < static_cast<int>(controls_.size()); ++index) {
    if (const auto& control = controls_[index];
        control.GetSlopePerBeat() != 0.0) {
      message_queue_.Add(
          timestamp_,
          ControlMessage{index, control.GetValue(),
                         GetSlopePerFrame(control.GetSlopePerBeat())});
    }
  }
  // Update note controls.
  for (auto& [pitch, note_controls] : note_controls_) {
    for (int index = 0; index < static_cast<int>(note_controls.size());
         ++index) {
      if (const auto& note_control = note_controls[index];
          note_control.GetSlopePerBeat() != 0.0) {
        message_queue_.Add(
            timestamp_, NoteControlMessage{
                            pitch, index, note_control.GetValue(),
                            GetSlopePerFrame(note_control.GetSlopePerBeat())});
      }
    }
  }
}

void Instrument::Update(double timestamp) noexcept {
  assert(timestamp_ <= timestamp);
  if (timestamp_ == timestamp) return;
  const double duration = BeatsFromSeconds(tempo_, timestamp - timestamp_);
  // Update controls.
  for (int index = 0; index < static_cast<int>(controls_.size()); ++index) {
    if (auto& control = controls_[index];
        control.Update(duration) && control_event_callback_) {
      control_event_callback_(index, control.GetValue());
    }
  }
  // Update note controls.
  for (auto& [pitch, note_controls] : note_controls_) {
    for (int index = 0; index < static_cast<int>(note_controls.size());
         ++index) {
      if (auto& note_control = note_controls[index];
          note_control.Update(duration) && note_control_event_callback_) {
        note_control_event_callback_(pitch, index, note_control.GetValue());
      }
    }
  }
  timestamp_ = timestamp;
}

double Instrument::GetSlopePerFrame(double slope_per_beat) const noexcept {
  return tempo_ > 0.0 && frame_rate_ > 0
             ? BeatsFromSeconds(tempo_, slope_per_beat) /
                   static_cast<double>(frame_rate_)
             : 0.0;
}

}  // namespace barely::internal
