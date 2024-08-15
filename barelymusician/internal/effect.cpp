#include "barelymusician/internal/effect.h"

#include <cassert>
#include <cstddef>
#include <variant>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/find_or_null.h"
#include "barelymusician/internal/control.h"
#include "barelymusician/internal/message.h"

namespace barely::internal {

Effect::Effect(const EffectDefinition& definition, int frame_rate, int64_t update_frame) noexcept
    : destroy_callback_(definition.destroy_callback),
      process_callback_(definition.process_callback),
      set_control_callback_(definition.set_control_callback),
      set_data_callback_(definition.set_data_callback),
      control_map_(
          BuildControlMap(static_cast<const ControlDefinition*>(definition.control_definitions),
                          definition.control_definition_count,
                          [&](int id, double value) {
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

Effect::~Effect() noexcept {
  if (destroy_callback_) {
    destroy_callback_(&state_);
  }
}

Control* Effect::GetControl(int id) noexcept { return FindOrNull(control_map_, id); }

const Control* Effect::GetControl(int id) const noexcept { return FindOrNull(control_map_, id); }

// NOLINTNEXTLINE(bugprone-exception-escape)
bool Effect::Process(double* output_samples, int output_channel_count, int output_frame_count,
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
  for (auto& [id, control] : control_map_) {
    control.ResetValue();
  }
}

void Effect::SetControlEvent(ControlEventDefinition definition, void* user_data) noexcept {
  control_event_ = {definition, user_data};
}

void Effect::SetData(std::vector<std::byte> data) noexcept {
  message_queue_.Add(update_frame_, DataMessage{std::move(data)});
}

void Effect::Update(int64_t update_frame) noexcept {
  assert(update_frame >= update_frame_);
  update_frame_ = update_frame;
}

}  // namespace barely::internal
