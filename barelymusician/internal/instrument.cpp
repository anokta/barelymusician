#include "barelymusician/internal/instrument.h"

#include <cassert>
#include <cstddef>
#include <utility>
#include <variant>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/find_or_null.h"
#include "barelymusician/internal/control.h"
#include "barelymusician/internal/message.h"
#include "barelymusician/internal/seconds.h"

namespace barely::internal {

// NOLINTNEXTLINE(bugprone-exception-escape)
Instrument::Instrument(const InstrumentDefinition& definition, int frame_rate,
                       double initial_timestamp) noexcept
    : destroy_callback_(definition.destroy_callback),
      process_callback_(definition.process_callback),
      set_control_callback_(definition.set_control_callback),
      set_data_callback_(definition.set_data_callback),
      set_note_control_callback_(definition.set_note_control_callback),
      set_note_off_callback_(definition.set_note_off_callback),
      set_note_on_callback_(definition.set_note_on_callback),
      frame_rate_(frame_rate),
      note_control_definitions_(
          definition.note_control_definitions,
          definition.note_control_definitions + definition.note_control_definition_count),
      control_map_(
          BuildControlMap(static_cast<const ControlDefinition*>(definition.control_definitions),
                          definition.control_definition_count,
                          [this](int control_id, double value) {
                            message_queue_.Add(update_frame_, ControlMessage{control_id, value});
                          })),
      update_frame_(FramesFromSeconds(frame_rate_, initial_timestamp)) {
  assert(frame_rate > 0);
  if (definition.create_callback) {
    definition.create_callback(&state_, frame_rate);
  }
  if (set_control_callback_) {
    for (const auto& [control_id, control] : control_map_) {
      set_control_callback_(&state_, control_id, control.GetValue());
    }
  }
}

Instrument::~Instrument() noexcept {
  if (destroy_callback_) {
    destroy_callback_(&state_);
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Instrument::AddNote(Note* note) noexcept {
  assert(note != nullptr);
  notes_.emplace(note);
  message_queue_.Add(update_frame_,
                     NoteOnMessage{note->GetId(), note->GetPitch(), note->GetIntensity()});
  for (const auto& definition : note_control_definitions_) {
    message_queue_.Add(update_frame_, NoteControlMessage{note->GetId(), definition.control_id,
                                                         definition.default_value});
  }
}

ControlMap Instrument::BuildNoteControlMap(int note_id) noexcept {
  return BuildControlMap(
      note_control_definitions_.data(), static_cast<int>(note_control_definitions_.size()),
      [this, note_id](int control_id, double value) {
        message_queue_.Add(update_frame_, NoteControlMessage{note_id, control_id, value});
      });
}

Control* Instrument::GetControl(int control_id) noexcept {
  return FindOrNull(control_map_, control_id);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
bool Instrument::Process(double* output_samples, int output_channel_count, int output_frame_count,
                         double timestamp) noexcept {
  if ((!output_samples && output_channel_count > 0 && output_frame_count > 0) ||
      output_channel_count < 0 || output_frame_count < 0) {
    return false;
  }
  int frame = 0;
  // Process *all* messages before the end frame.
  const int64_t begin_frame = FramesFromSeconds(frame_rate_, timestamp);
  const int64_t end_frame = begin_frame + output_frame_count;
  for (auto* message = message_queue_.GetNext(end_frame); message;
       message = message_queue_.GetNext(end_frame)) {
    if (const int message_frame = static_cast<int>(message->first - begin_frame);
        frame < message_frame) {
      if (process_callback_) {
        process_callback_(&state_, &output_samples[frame * output_channel_count],
                          output_channel_count, message_frame - frame);
      }
      frame = message_frame;
    }
    std::visit(MessageVisitor{
                   [this](ControlMessage& control_message) noexcept {
                     if (set_control_callback_) {
                       set_control_callback_(&state_, control_message.control_id,
                                             control_message.value);
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
                       set_note_control_callback_(&state_, note_control_message.note_id,
                                                  note_control_message.control_id,
                                                  note_control_message.value);
                     }
                   },
                   [this](NoteOffMessage& note_off_message) noexcept {
                     if (set_note_off_callback_) {
                       set_note_off_callback_(&state_, note_off_message.note_id);
                     }
                   },
                   [this](NoteOnMessage& note_on_message) noexcept {
                     if (set_note_on_callback_) {
                       set_note_on_callback_(&state_, note_on_message.note_id,
                                             note_on_message.pitch, note_on_message.intensity);
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

void Instrument::RemoveNote(Note* note) noexcept {
  assert(note);
  notes_.erase(note);
  message_queue_.Add(update_frame_, NoteOffMessage{note->GetId()});
}

void Instrument::SetData(std::vector<std::byte> data) noexcept {
  message_queue_.Add(update_frame_, DataMessage{std::move(data)});
}

void Instrument::Update(double timestamp) noexcept {
  update_frame_ = FramesFromSeconds(frame_rate_, timestamp);
}

}  // namespace barely::internal
