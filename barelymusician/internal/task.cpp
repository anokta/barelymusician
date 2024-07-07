
#include "barelymusician/internal/task.h"

#include <cassert>

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/event.h"

namespace barely::internal {

Task::Task(const TaskDefinition& definition, double position, int process_order, void* user_data,
           SetPositionCallback set_position_callback,
           SetProcessOrderCallback set_process_order_callback) noexcept
    : Event<TaskDefinition>(definition, user_data),
      position_(position),
      process_order_(process_order),
      set_position_callback_(set_position_callback),
      set_process_order_callback_(set_process_order_callback) {
  assert(set_position_callback_);
  assert(set_process_order_callback_);
}

double Task::GetPosition() const noexcept { return position_; }

int Task::GetProcessOrder() const noexcept { return process_order_; }

void Task::SetPosition(double position) noexcept {
  if (position != position_) {
    set_position_callback_(this, position);
    position_ = position;
  }
}

void Task::SetProcessOrder(int process_order) noexcept {
  if (process_order != process_order_) {
    set_process_order_callback_(this, process_order);
    process_order_ = process_order;
  }
}

}  // namespace barely::internal
