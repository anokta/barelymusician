#ifndef BARELYMUSICIAN_INTERNAL_TASK_H_
#define BARELYMUSICIAN_INTERNAL_TASK_H_

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/event.h"

namespace barely::internal {

/// Class that wraps a task.
class Task : public Event<TaskDefinition> {
 public:
  /// Constructs a new `Task`.
  ///
  /// @param definition Task definition.
  /// @param position Task position.
  /// @param process_order Task process order.
  /// @param user_data Pointer to user data.
  Task(const TaskDefinition& definition, Rational position, int process_order,
       void* user_data) noexcept;

  /// Returns the position.
  ///
  /// @return Position in beats.
  Rational GetPosition() const noexcept;

  /// Returns the process order.
  ///
  /// @return Process order.
  int GetProcessOrder() const noexcept;

  /// Sets the position.
  ///
  /// @param position Position in beats.
  void SetPosition(Rational position) noexcept;

  /// Returns the process order.
  ///
  /// @param process_order Process order.
  void SetProcessOrder(int process_order) noexcept;

 private:
  // Position.
  Rational position_;

  // Process order.
  int process_order_;
};

}  // namespace barely::internal

#endif
