#include "barelymusician/internal/effect.h"

#include <cassert>
#include <cstddef>
#include <unordered_map>
#include <variant>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/find_or_null.h"
#include "barelymusician/internal/control.h"
#include "barelymusician/internal/message.h"
#include "barelymusician/internal/seconds.h"

namespace barely::internal {

Effect::Effect(const EffectDefinition& definition, int frame_rate,
               double initial_timestamp) noexcept
    : destroy_callback_(definition.destroy_callback),
      process_callback_(definition.process_callback),
      set_control_callback_(definition.set_control_callback),
      set_data_callback_(definition.set_data_callback),
      frame_rate_(frame_rate),
      controls_(BuildControls(static_cast<const ControlDefinition*>(definition.control_definitions),
                              definition.control_definition_count)),
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

Effect::~Effect() noexcept {
  if (destroy_callback_) {
    destroy_callback_(&state_);
  }
}

Control* Effect::GetControl(int index) noexcept {
  if (index >= 0 && index < static_cast<int>(controls_.size())) {
    return &controls_[index];
  }
  return nullptr;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
bool Effect::Process(double* output_samples, int output_channel_count, int output_frame_count,
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
                       set_control_callback_(&state_, control_message.id, control_message.value);
                     }
                   },
                   [this](DataMessage& data_message) noexcept {
                     if (set_data_callback_) {
                       data_.swap(data_message.data);
                       set_data_callback_(&state_, data_.data(), static_cast<int>(data_.size()));
                     }
                   },
                   [](const auto&) noexcept { assert(false); }},
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

void Effect::ResetAllControls() noexcept {
  for (auto& [id, control] : controls_) {
    if (control.Reset()) {
      message_queue_.Add(update_frame_, ControlMessage{id, control.GetValue()});
    }
  }
}

bool Effect::ResetControl(int index) noexcept {
  if (index >= 0 && index < static_cast<int>(controls_.size())) {
    if (auto& control = controls_[index]; control.Reset()) {
      message_queue_.Add(update_frame_, ControlMessage{index, control.GetValue()});
    }
    return true;
  }
  return false;
}

bool Effect::SetControl(int index, double value) noexcept {
  if (index >= 0 && index < static_cast<int>(controls_.size())) {
    if (auto& control = controls_[index]; control.Set(value)) {
      message_queue_.Add(update_frame_, ControlMessage{index, control.GetValue()});
    }
    return true;
  }
  return false;
}

void Effect::SetData(std::vector<std::byte> data) noexcept {
  message_queue_.Add(update_frame_, DataMessage{std::move(data)});
}

void Effect::Update(double timestamp) noexcept {
  update_frame_ = FramesFromSeconds(frame_rate_, timestamp);
}

}  // namespace barely::internal
