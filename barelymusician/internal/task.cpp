
#include "barelymusician/internal/task.h"

#include <cassert>
#include <utility>

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/event.h"

namespace barely::internal {

Task::Task(const TaskDefinition& definition, double position, void* user_data,
           SetPositionCallback set_position_callback) noexcept
    : Event<TaskDefinition>(definition, user_data),
      position_(position),
      set_position_callback_(std::move(set_position_callback)) {
  assert(set_position_callback_);
}

double Task::GetPosition() const noexcept { return position_; }

void Task::SetPosition(double position) noexcept {
  if (position != position_) {
    set_position_callback_(this, position);
    position_ = position;
  }
}

}  // namespace barely::internal
