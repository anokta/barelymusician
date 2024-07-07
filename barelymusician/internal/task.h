#ifndef BARELYMUSICIAN_INTERNAL_TASK_H_
#define BARELYMUSICIAN_INTERNAL_TASK_H_

#include <functional>

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/event.h"

namespace barely::internal {

/// Class that wraps a task.
class Task : public Event<TaskDefinition> {
 public:
  /// Set position callback alias.
  using SetPositionCallback = std::function<void(Task*, double)>;

  /// Set process order callback alias.
  using SetProcessOrderCallback = std::function<void(Task*, int)>;

  /// Constructs a new `Task`.
  ///
  /// @param definition Task definition.
  /// @param position Task position.
  /// @param process_order Task process order.
  /// @param user_data Pointer to user data.
  /// @param set_position_callback Set position callback.
  /// @param set_process_order_callback Set process order callback.
  Task(const TaskDefinition& definition, double position, int process_order, void* user_data,
       SetPositionCallback set_position_callback,
       SetProcessOrderCallback set_process_order_callback) noexcept;

  /// Returns the position.
  ///
  /// @return Position in beats.
  double GetPosition() const noexcept;

  /// Returns the process order.
  ///
  /// @return Process order.
  int GetProcessOrder() const noexcept;

  /// Sets the position.
  ///
  /// @param position Position in beats.
  void SetPosition(double position) noexcept;

  /// Returns the process order.
  ///
  /// @param process_order Process order.
  void SetProcessOrder(int process_order) noexcept;

 private:
  // Position.
  double position_;

  // Process order.
  int process_order_;

  // Set position callback.
  SetPositionCallback set_position_callback_;

  // Set process order callback.
  SetProcessOrderCallback set_process_order_callback_;
};

}  // namespace barely::internal

#endif
