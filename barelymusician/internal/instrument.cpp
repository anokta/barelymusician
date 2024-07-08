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
      controls_(BuildControls(static_cast<const ControlDefinition*>(definition.control_definitions),
                              definition.control_definition_count,
                              [this](int id, double value) {
                                message_queue_.Add(update_frame_, ControlMessage{id, value});
                              })),
      update_frame_(FramesFromSeconds(frame_rate_, initial_timestamp)) {
  assert(frame_rate > 0);
  if (definition.create_callback) {
    definition.create_callback(&state_, frame_rate);
  }
  if (set_control_callback_) {
    for (const auto& [id, control] : controls_) {
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

Control* Instrument::GetControl(int id) noexcept { return FindOrNull(controls_, id); }

Control* Instrument::GetNoteControl(double pitch, int id) noexcept {
  if (auto* note_controls = FindOrNull(note_controls_, pitch)) {
    return FindOrNull(*note_controls, id);
  }
  return nullptr;
}

bool Instrument::IsNoteOn(double pitch) const noexcept {
  // TODO(#111): Use `contains` instead of `find`.
  return note_controls_.find(pitch) != note_controls_.end();
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

// NOLINTNEXTLINE(bugprone-exception-escape)
void Instrument::SetAllNotesOff() noexcept {
  for (const auto& [pitch, note_controls] : std::exchange(note_controls_, {})) {
    note_off_event_.Process(pitch);
    message_queue_.Add(update_frame_, NoteOffMessage{pitch});
  }
}

void Instrument::SetData(std::vector<std::byte> data) noexcept {
  message_queue_.Add(update_frame_, DataMessage{std::move(data)});
}

void Instrument::SetNoteOff(double pitch) noexcept {
  if (note_controls_.erase(pitch) > 0) {
    note_off_event_.Process(pitch);
    message_queue_.Add(update_frame_, NoteOffMessage{pitch});
  }
}

void Instrument::SetNoteOffEvent(NoteOffEventDefinition definition, void* user_data) noexcept {
  note_off_event_ = {definition, user_data};
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Instrument::SetNoteOn(double pitch, double intensity) noexcept {
  if (note_controls_
          .try_emplace(pitch, BuildControls(note_control_definitions_.data(),
                                            static_cast<int>(note_control_definitions_.size()),
                                            [this, pitch](int id, double value) {
                                              message_queue_.Add(
                                                  update_frame_,
                                                  NoteControlMessage{pitch, id, value});
                                            }))
          .second) {
    note_on_event_.Process(pitch, intensity);
    message_queue_.Add(update_frame_, NoteOnMessage{pitch, intensity});
    for (const auto& definition : note_control_definitions_) {
      message_queue_.Add(update_frame_,
                         NoteControlMessage{pitch, definition.id, definition.default_value});
    }
  }
}

void Instrument::SetNoteOnEvent(NoteOnEventDefinition definition, void* user_data) noexcept {
  note_on_event_ = {definition, user_data};
}

void Instrument::Update(double timestamp) noexcept {
  update_frame_ = FramesFromSeconds(frame_rate_, timestamp);
}

}  // namespace barely::internal
