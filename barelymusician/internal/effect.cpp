#include "barelymusician/internal/effect.h"

#include <cstddef>
#include <vector>

#include "barelymusician/barelymusician.h"

namespace barely::internal {

Effect::Effect(const EffectDefinition& definition, int frame_rate) noexcept
    : destroy_callback_(definition.destroy_callback),
      process_callback_(definition.process_callback),
      set_control_callback_(definition.set_control_callback),
      set_data_callback_(definition.set_data_callback) {
  assert(frame_rate > 0);
  if (definition.create_callback) {
    definition.create_callback(&state_, frame_rate);
  }
}

Effect::~Effect() noexcept {
  if (destroy_callback_) {
    destroy_callback_(&state_);
  }
}

void Effect::Process(double* output_samples, int output_channel_count,
                     int output_frame_count) noexcept {
  if (process_callback_) {
    process_callback_(&state_, output_samples, output_channel_count,
                      output_frame_count);
  }
}

void Effect::SetControl(int index, double value,
                        double slope_per_frame) noexcept {
  if (set_control_callback_) {
    set_control_callback_(&state_, index, value, slope_per_frame);
  }
}

void Effect::SetData(std::vector<std::byte>& data) noexcept {
  data_.swap(data);
  if (set_data_callback_) {
    set_data_callback_(&state_, data_.data(), static_cast<int>(data_.size()));
  }
}

}  // namespace barely::internal
