
#include "barelymusician/internal/task.h"

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/event.h"

namespace barely::internal {

Task::Task(const TaskDefinition& definition, Rational position, int process_order,
           void* user_data) noexcept
    : Event<TaskDefinition>(definition, user_data),
      position_(position),
      process_order_(process_order) {}

Rational Task::GetPosition() const noexcept { return position_; }

int Task::GetProcessOrder() const noexcept { return process_order_; }

void Task::SetPosition(Rational position) noexcept { position_ = position; }

void Task::SetProcessOrder(int process_order) noexcept { process_order_ = process_order; }

}  // namespace barely::internal
