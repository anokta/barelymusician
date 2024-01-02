
#include "barelymusician/internal/task.h"

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/event.h"

namespace barely::internal {

Task::Task(const TaskDefinition& definition, double position, int process_order,
           void* user_data) noexcept
    : Event<TaskDefinition>(definition, user_data),
      position_(position),
      process_order_(process_order) {}

double Task::GetPosition() const noexcept { return position_; }

int Task::GetProcessOrder() const noexcept { return process_order_; }

bool Task::SetPosition(double position) noexcept {
  if (position != position_) {
    position_ = position;
    return true;
  }
  return false;
}

bool Task::SetProcessOrder(int process_order) noexcept {
  if (process_order != process_order_) {
    process_order_ = process_order;
    return true;
  }
  return false;
}

}  // namespace barely::internal
