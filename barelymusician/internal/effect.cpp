#include "barelymusician/internal/effect.h"

#include <cassert>
#include <cstddef>
#include <unordered_map>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/control.h"

namespace barely::internal {

Effect::Effect(const EffectDefinition& definition, int frame_rate, int process_order) noexcept
    : destroy_callback_(definition.destroy_callback),
      process_callback_(definition.process_callback),
      set_control_callback_(definition.set_control_callback),
      set_data_callback_(definition.set_data_callback),
      controls_(BuildControls(static_cast<const ControlDefinition*>(definition.control_definitions),
                              definition.control_definition_count)),
      process_order_(process_order) {
  assert(frame_rate > 0);
  if (definition.create_callback) {
    definition.create_callback(&state_, frame_rate);
  }
  if (set_control_callback_) {
    for (const auto& [id, control] : controls_) {
      set_control_callback_(&state_, id, control.GetValue(), 0.0);
    }
  }
}

Effect::~Effect() noexcept {
  if (destroy_callback_) {
    destroy_callback_(&state_);
  }
}

std::unordered_map<int, Control>& Effect::GetAllControls() noexcept { return controls_; }

Control* Effect::GetControl(int index) noexcept {
  if (index >= 0 && index < static_cast<int>(controls_.size())) {
    return &controls_[index];
  }
  return nullptr;
}

int Effect::GetProcessOrder() const noexcept { return process_order_; }

void Effect::Process(double* output_samples, int output_channel_count,
                     int output_frame_count) noexcept {
  if (process_callback_) {
    process_callback_(&state_, output_samples, output_channel_count, output_frame_count);
  }
}

void Effect::ProcessControlEvent(int index) noexcept {
  assert(index >= 0 && index < static_cast<int>(controls_.size()));
  control_event_.Process(index, controls_[index].GetValue());
}

void Effect::ProcessControlMessage(int index, double value, double slope_per_frame) noexcept {
  if (set_control_callback_) {
    set_control_callback_(&state_, index, value, slope_per_frame);
  }
}

void Effect::ProcessDataMessage(std::vector<std::byte>& data) noexcept {
  data_.swap(data);
  if (set_data_callback_) {
    set_data_callback_(&state_, data_.data(), static_cast<int>(data_.size()));
  }
}

void Effect::SetControlEvent(ControlEventDefinition definition, void* user_data) noexcept {
  control_event_ = {definition, user_data};
}

bool Effect::SetProcessOrder(int process_order) noexcept {
  if (process_order != process_order_) {
    process_order_ = process_order;
    return true;
  }
  return false;
}

}  // namespace barely::internal
