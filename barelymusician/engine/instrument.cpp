#include "barelymusician/engine/instrument.h"

#include <cassert>
#include <utility>
#include <variant>

#include "barelymusician/engine/message.h"

namespace barely::internal {

// NOLINTNEXTLINE(bugprone-exception-escape)
Instrument::Instrument(const Definition& definition, int frame_rate) noexcept
    : destroy_callback_(definition.destroy_callback),
      process_callback_(definition.process_callback),
      set_control_callback_(definition.set_control_callback),
      set_data_callback_(definition.set_data_callback),
      set_note_off_callback_(definition.set_note_off_callback),
      set_note_on_callback_(definition.set_note_on_callback),
      frame_rate_(frame_rate) {
  assert(frame_rate >= 0);
  controls_.reserve(definition.control_definition_count);
  for (int index = 0; index < definition.control_definition_count; ++index) {
    controls_.emplace_back(definition.control_definitions[index]);
  }
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

const Control* Instrument::GetControl(int index) const noexcept {
  assert(index >= 0);
  if (index < static_cast<int>(controls_.size())) {
    return &controls_[index];
  }
  return nullptr;
}

bool Instrument::IsNoteOn(double pitch) const noexcept {
  return pitches_.contains(pitch);
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
    std::visit(MessageVisitor{
                   [this](ControlMessage& control_message) noexcept {
                     if (set_control_callback_) {
                       set_control_callback_(
                           &state_, control_message.index,
                           control_message.value,
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

void Instrument::ResetAllControls(double timestamp) noexcept {
  assert(timestamp >= 0.0);
  for (int index = 0; index < static_cast<int>(controls_.size()); ++index) {
    if (controls_[index].ResetValue()) {
      message_queue_.Add(
          timestamp, ControlMessage{index, controls_[index].GetValue(), 0.0});
    }
  }
}

bool Instrument::ResetControl(int index, double timestamp) noexcept {
  assert(index >= 0);
  assert(timestamp >= 0.0);
  if (index < static_cast<int>(controls_.size())) {
    if (controls_[index].ResetValue()) {
      message_queue_.Add(
          timestamp, ControlMessage{index, controls_[index].GetValue(), 0.0});
    }
    return true;
  }
  return false;
}

bool Instrument::SetControl(int index, double value, double slope_per_second,
                            double timestamp) noexcept {
  assert(index >= 0);
  assert(timestamp >= 0.0);
  if (index < static_cast<int>(controls_.size())) {
    if (controls_[index].SetValue(value)) {
      message_queue_.Add(
          timestamp,
          ControlMessage{index, controls_[index].GetValue(), slope_per_second});
    }
    return true;
  }
  return false;
}

void Instrument::SetData(std::vector<std::byte> data,
                         double timestamp) noexcept {
  assert(timestamp >= 0.0);
  message_queue_.Add(timestamp, DataMessage{std::move(data)});
}

void Instrument::SetNoteOffCallback(NoteOffCallback callback) noexcept {
  note_off_callback_ = std::move(callback);
}

void Instrument::SetNoteOnCallback(NoteOnCallback callback) noexcept {
  note_on_callback_ = std::move(callback);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Instrument::StartNote(double pitch, double timestamp) noexcept {
  assert(timestamp >= 0.0);
  if (pitches_.insert(pitch).second) {
    if (note_on_callback_) {
      note_on_callback_(pitch);
    }
    message_queue_.Add(timestamp, NoteOnMessage{pitch});
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Instrument::StopAllNotes(double timestamp) noexcept {
  assert(timestamp >= 0.0);
  for (const double pitch : std::exchange(pitches_, {})) {
    if (note_off_callback_) {
      note_off_callback_(pitch);
    }
    message_queue_.Add(timestamp, NoteOffMessage{pitch});
  }
}

void Instrument::StopNote(double pitch, double timestamp) noexcept {
  assert(timestamp >= 0.0);
  if (pitches_.erase(pitch) > 0) {
    if (note_off_callback_) {
      note_off_callback_(pitch);
    }
    message_queue_.Add(timestamp, NoteOffMessage{pitch});
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
