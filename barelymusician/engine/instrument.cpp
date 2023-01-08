#include "barelymusician/engine/instrument.h"

#include <cassert>
#include <cstddef>
#include <utility>
#include <variant>
#include <vector>

#include "barelymusician/common/find_or_null.h"
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
Instrument::Instrument(const InstrumentDefinition& definition,
                       int frame_rate) noexcept
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
                              definition.control_definition_count)) {
  assert(frame_rate >= 0);
  if (definition.create_callback) {
    definition.create_callback(&state_, frame_rate);
  }
  if (set_control_callback_) {
    for (int index = 0; index < definition.control_definition_count; ++index) {
      set_control_callback_(&state_, index, controls_[index].Get(), 0.0);
    }
  }
}

Instrument::~Instrument() noexcept {
  if (destroy_callback_) {
    destroy_callback_(&state_);
  }
}

StatusOr<std::reference_wrapper<const Control>> Instrument::GetControl(
    int index) const noexcept {
  assert(index >= 0);
  if (index < static_cast<int>(controls_.size())) {
    return {controls_[index]};
  }
  return {Status::kInvalidArgument};
}

StatusOr<std::reference_wrapper<const Control>> Instrument::GetNoteControl(
    double pitch, int index) const noexcept {
  assert(index >= 0);
  if (const auto* note_controls = FindOrNull(note_controls_, pitch)) {
    if (index < static_cast<int>(note_controls->size())) {
      return {(*note_controls)[index]};
    }
    return {Status::kInvalidArgument};
  }
  return {Status::kNotFound};
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
  int frame = 0;
  // Process *all* messages before the end timestamp.
  const double end_timestamp = timestamp + GetSeconds(output_frame_count);
  for (auto* message = message_queue_.GetNext(end_timestamp); message;
       message = message_queue_.GetNext(end_timestamp)) {
    const int message_frame = GetFrames(message->first - timestamp);
    if (frame < message_frame) {
      if (process_callback_) {
        process_callback_(&state_,
                          &output_samples[output_channel_count * frame],
                          output_channel_count, message_frame - frame);
      }
      frame = message_frame;
    }
    std::visit(
        MessageVisitor{
            [this](ControlMessage& control_message) noexcept {
              if (set_control_callback_) {
                set_control_callback_(
                    &state_, control_message.index, control_message.value,
                    GetSlopePerFrame(control_message.slope_per_second));
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
                    note_control_message.index, note_control_message.value,
                    GetSlopePerFrame(note_control_message.slope_per_second));
              }
            },
            [this](NoteOffMessage& note_off_message) noexcept {
              if (set_note_off_callback_) {
                set_note_off_callback_(&state_, note_off_message.pitch);
              }
            },
            [this](NoteOnMessage& note_on_message) noexcept {
              if (set_note_on_callback_) {
                set_note_on_callback_(&state_, note_on_message.pitch);
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
        control_event_callback_(index, control.Get());
      }
      message_queue_.Add(timestamp_, ControlMessage{index, control.Get(), 0.0});
    }
  }
}

Status Instrument::ResetAllNoteControls(double pitch) noexcept {
  if (auto* note_controls = FindOrNull(note_controls_, pitch)) {
    for (int index = 0; index < static_cast<int>(note_controls->size());
         ++index) {
      if (auto& note_control = (*note_controls)[index]; note_control.Reset()) {
        if (note_control_event_callback_) {
          note_control_event_callback_(pitch, index, note_control.Get());
        }
        message_queue_.Add(
            timestamp_,
            NoteControlMessage{pitch, index, note_control.Get(), 0.0});
      }
    }
    return Status::kOk;
  }
  return Status::kNotFound;
}

Status Instrument::ResetControl(int index) noexcept {
  assert(index >= 0);
  if (index < static_cast<int>(controls_.size())) {
    if (auto& control = controls_[index]; control.Reset()) {
      if (note_control_event_callback_) {
        control_event_callback_(index, control.Get());
      }
      message_queue_.Add(timestamp_, ControlMessage{index, control.Get(), 0.0});
    }
    return Status::kOk;
  }
  return Status::kInvalidArgument;
}

Status Instrument::ResetNoteControl(double pitch, int index) noexcept {
  assert(index >= 0);
  if (auto* note_controls = FindOrNull(note_controls_, pitch)) {
    if (index < static_cast<int>(note_controls->size())) {
      if (auto& note_control = (*note_controls)[index]; note_control.Reset()) {
        if (note_control_event_callback_) {
          note_control_event_callback_(pitch, index, note_control.Get());
        }
        message_queue_.Add(
            timestamp_,
            NoteControlMessage{pitch, index, note_control.Get(), 0.0});
      }
      return Status::kOk;
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
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
                              double slope_per_second) noexcept {
  assert(index >= 0);
  if (index < static_cast<int>(controls_.size())) {
    if (auto& control = controls_[index];
        control.Set(value, slope_per_second)) {
      if (note_control_event_callback_) {
        control_event_callback_(index, control.Get());
      }
      message_queue_.Add(
          timestamp_, ControlMessage{index, control.Get(), slope_per_second});
    }
    return Status::kOk;
  }
  return Status::kInvalidArgument;
}

void Instrument::SetControlEventCallback(
    ControlEventCallback callback) noexcept {
  control_event_callback_ = std::move(callback);
}

void Instrument::SetData(std::vector<std::byte> data) noexcept {
  message_queue_.Add(timestamp_, DataMessage{std::move(data)});
}

Status Instrument::SetNoteControl(double pitch, int index, double value,
                                  double slope_per_second) noexcept {
  assert(index >= 0);
  if (auto* note_controls = FindOrNull(note_controls_, pitch)) {
    if (index < static_cast<int>(controls_.size())) {
      if (auto& note_control = (*note_controls)[index];
          note_control.Set(value, slope_per_second)) {
        if (note_control_event_callback_) {
          note_control_event_callback_(pitch, index, note_control.Get());
        }
        message_queue_.Add(timestamp_,
                           NoteControlMessage{pitch, index, note_control.Get(),
                                              slope_per_second});
      }
      return Status::kOk;
    }
    return Status::kInvalidArgument;
  }
  return Status::kNotFound;
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
void Instrument::SetNoteOn(double pitch) noexcept {
  if (note_controls_.try_emplace(pitch, default_note_controls_).second) {
    if (note_on_event_callback_) {
      note_on_event_callback_(pitch);
    }
    message_queue_.Add(timestamp_, NoteOnMessage{pitch});
  }
}

void Instrument::SetNoteOnEventCallback(NoteOnEventCallback callback) noexcept {
  note_on_event_callback_ = std::move(callback);
}

void Instrument::Update(double timestamp) noexcept {
  assert(timestamp_ <= timestamp);
  if (timestamp_ < timestamp) {
    const double elapsed_seconds = timestamp - timestamp_;
    // Update controls.
    for (int index = 0; index < static_cast<int>(controls_.size()); ++index) {
      if (auto& control = controls_[index];
          control.UpdateBy(elapsed_seconds) && control_event_callback_) {
        control_event_callback_(index, control.Get());
      }
    }
    // Update note controls.
    for (auto& [pitch, note_controls] : note_controls_) {
      for (int index = 0; index < static_cast<int>(note_controls.size());
           ++index) {
        if (auto& note_control = note_controls[index];
            note_control.UpdateBy(elapsed_seconds) &&
            note_control_event_callback_) {
          note_control_event_callback_(pitch, index, note_control.Get());
        }
      }
    }
    timestamp_ = timestamp;
  }
}

int Instrument::GetFrames(double seconds) const noexcept {
  return frame_rate_ > 0
             ? static_cast<int>(seconds * static_cast<double>(frame_rate_))
             : 0;
}

double Instrument::GetSeconds(int frames) const noexcept {
  return frame_rate_ > 0
             ? static_cast<double>(frames) / static_cast<double>(frame_rate_)
             : 0.0;
}

double Instrument::GetSlopePerFrame(double slope_per_second) const noexcept {
  return frame_rate_ > 0 ? slope_per_second / static_cast<double>(frame_rate_)
                         : 0.0;
}

}  // namespace barely::internal
