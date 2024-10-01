#include "barelymusician/internal/instrument.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <variant>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/find_or_null.h"
#include "barelymusician/internal/control.h"
#include "barelymusician/internal/message.h"

namespace barely::internal {

// NOLINTNEXTLINE(bugprone-exception-escape)
Instrument::Instrument(const InstrumentDefinition& definition, int frame_rate,
                       int64_t update_frame) noexcept
    : destroy_callback_(definition.destroy_callback),
      process_callback_(definition.process_callback),
      set_control_callback_(definition.set_control_callback),
      set_data_callback_(definition.set_data_callback),
      set_note_control_callback_(definition.set_note_control_callback),
      set_note_off_callback_(definition.set_note_off_callback),
      set_note_on_callback_(definition.set_note_on_callback),
      set_tuning_callback_(definition.set_tuning_callback),
      note_control_definitions_(
          definition.note_control_definitions,
          definition.note_control_definitions + definition.note_control_definition_count),
      control_map_(
          BuildControlMap(static_cast<const ControlDefinition*>(definition.control_definitions),
                          definition.control_definition_count,
                          [this](int id, double value) {
                            control_event_.Process(id, value);
                            message_queue_.Add(update_frame_, ControlMessage{id, value});
                          })),
      update_frame_(update_frame) {
  assert(frame_rate > 0);
  if (definition.create_callback) {
    definition.create_callback(&state_, frame_rate);
  }
  if (set_control_callback_) {
    for (const auto& [id, control] : control_map_) {
      set_control_callback_(&state_, id, control.GetValue());
    }
  }
}

Instrument::~Instrument() noexcept {
  SetAllNotesOff();
  if (destroy_callback_) {
    destroy_callback_(&state_);
  }
}

Control* Instrument::GetControl(int id) noexcept { return FindOrNull(control_map_, id); }

const Control* Instrument::GetControl(int id) const noexcept {
  return FindOrNull(control_map_, id);
}

Control* Instrument::GetNoteControl(int pitch, int id) noexcept {
  if (auto* note_control_map = FindOrNull(note_control_maps_, pitch)) {
    return FindOrNull(*note_control_map, id);
  }
  return nullptr;
}

const Control* Instrument::GetNoteControl(int pitch, int id) const noexcept {
  if (const auto* note_control_map = FindOrNull(note_control_maps_, pitch)) {
    return FindOrNull(*note_control_map, id);
  }
  return nullptr;
}

bool Instrument::IsNoteOn(int pitch) const noexcept { return note_control_maps_.contains(pitch); }

// NOLINTNEXTLINE(bugprone-exception-escape)
bool Instrument::Process(double* output_samples, int output_channel_count, int output_frame_count,
                         int64_t process_frame) noexcept {
  if ((!output_samples && output_channel_count > 0 && output_frame_count > 0) ||
      output_channel_count < 0 || output_frame_count < 0) {
    return false;
  }
  int frame = 0;
  // Process *all* messages before the end frame.
  const int64_t end_frame = process_frame + output_frame_count;
  for (auto* message = message_queue_.GetNext(end_frame); message;
       message = message_queue_.GetNext(end_frame)) {
    if (const int message_frame = static_cast<int>(message->first - process_frame);
        frame < message_frame) {
      if (process_callback_) {
        process_callback_(&state_, &output_samples[frame * output_channel_count],
                          output_channel_count, message_frame - frame);
      }
      frame = message_frame;
    }
    std::visit(
        MessageVisitor{
            [this](ControlMessage& control_message) noexcept {
              if (set_control_callback_) {
                set_control_callback_(&state_, control_message.id, control_message.value);
              }
            },
            [this](DataMessage& data_message) noexcept {
              if (set_data_callback_) {
                data_.swap(data_message.data);
                set_data_callback_(&state_, data_.data(), static_cast<int>(data_.size()));
              }
            },
            [this](NoteControlMessage& note_control_message) noexcept {
              if (set_note_control_callback_) {
                set_note_control_callback_(&state_, note_control_message.pitch,
                                           note_control_message.id, note_control_message.value);
              }
            },
            [this](NoteOffMessage& note_off_message) noexcept {
              if (set_note_off_callback_) {
                set_note_off_callback_(&state_, note_off_message.pitch);
              }
            },
            [this](NoteOnMessage& note_on_message) noexcept {
              if (set_note_on_callback_) {
                set_note_on_callback_(&state_, note_on_message.pitch, note_on_message.intensity);
              }
            },
            [this](TuningMessage& tuning_message) noexcept {
              if (set_tuning_callback_) {
                tuning_or_ = std::move(tuning_message.tuning_or);
                set_tuning_callback_(
                    &state_, tuning_or_.has_value() ? &tuning_or_->GetDefinition() : nullptr);
              }
            }},
        message->second);
  }
  // Process the rest of the buffer.
  if (frame < output_frame_count) {
    if (process_callback_) {
      process_callback_(&state_, &output_samples[frame * output_channel_count],
                        output_channel_count, output_frame_count - frame);
    }
  }
  return true;
}

void Instrument::ResetAllControls() noexcept {
  for (auto& [id, control] : control_map_) {
    control.ResetValue();
  }
}

bool Instrument::ResetAllNoteControls(int pitch) noexcept {
  if (auto* note_control_map = FindOrNull(note_control_maps_, pitch)) {
    for (auto& [id, note_control] : *note_control_map) {
      note_control.ResetValue();
    }
    return true;
  }
  return false;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Instrument::SetAllNotesOff() noexcept {
  for (const auto& [note, _] : std::exchange(note_control_maps_, {})) {
    note_off_event_.Process(note);
    message_queue_.Add(update_frame_, NoteOffMessage{note});
  }
}

void Instrument::SetControlEvent(ControlEventDefinition definition, void* user_data) noexcept {
  control_event_ = {definition, user_data};
}

void Instrument::SetData(std::vector<std::byte> data) noexcept {
  message_queue_.Add(update_frame_, DataMessage{std::move(data)});
}

void Instrument::SetNoteControlEvent(NoteControlEventDefinition definition,
                                     void* user_data) noexcept {
  note_control_event_ = {definition, user_data};
}

void Instrument::SetNoteOff(int pitch) noexcept {
  if (note_control_maps_.erase(pitch) > 0) {
    note_off_event_.Process(pitch);
    message_queue_.Add(update_frame_, NoteOffMessage{pitch});
  }
}

void Instrument::SetNoteOffEvent(NoteOffEventDefinition definition, void* user_data) noexcept {
  note_off_event_ = {definition, user_data};
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Instrument::SetNoteOn(int pitch, double intensity) noexcept {
  if (const auto [it, success] = note_control_maps_.try_emplace(
          pitch, BuildControlMap(note_control_definitions_.data(),
                                 static_cast<int>(note_control_definitions_.size()),
                                 [this, pitch](int id, double value) {
                                   note_control_event_.Process(pitch, id, value);
                                   message_queue_.Add(update_frame_,
                                                      NoteControlMessage{pitch, id, value});
                                 }));
      success) {
    note_on_event_.Process(pitch, intensity);
    message_queue_.Add(update_frame_, NoteOnMessage{pitch, intensity});
    for (const auto& [id, note_control] : it->second) {
      message_queue_.Add(update_frame_, NoteControlMessage{pitch, id, note_control.GetValue()});
    }
  }
}

void Instrument::SetNoteOnEvent(NoteOnEventDefinition definition, void* user_data) noexcept {
  note_on_event_ = {definition, user_data};
}

void Instrument::SetTuning(std::optional<Tuning> tuning_or) noexcept {
  message_queue_.Add(update_frame_, TuningMessage{std::move(tuning_or)});
}

void Instrument::Update(int64_t update_frame) noexcept {
  assert(update_frame >= update_frame_);
  update_frame_ = update_frame;
}

}  // namespace barely::internal
