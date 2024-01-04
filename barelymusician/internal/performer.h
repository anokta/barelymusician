#ifndef BARELYMUSICIAN_INTERNAL_PERFORMER_H_
#define BARELYMUSICIAN_INTERNAL_PERFORMER_H_

#include <compare>
#include <map>
#include <optional>
#include <set>
#include <utility>

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/task.h"

namespace barely::internal {

/// Class that wraps a performer.
class Performer {
 public:
  /// Adds a task.
  ///
  /// @param task Task.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void AddTask(Task& task) noexcept;

  /// Cancels all one-off tasks.
  void CancelAllOneOffTasks() noexcept;

  /// Returns the duration to next task.
  ///
  /// @return Optional pair of duration in beats and process order.
  [[nodiscard]] std::optional<std::pair<double, int>> GetDurationToNextTask() const noexcept;

  /// Returns loop begin position.
  ///
  /// @return Loop begin position in beats.
  [[nodiscard]] double GetLoopBeginPosition() const noexcept;

  /// Returns loop length.
  ///
  /// @return Loop length in beats.
  [[nodiscard]] double GetLoopLength() const noexcept;

  /// Returns position.
  ///
  /// @return Position in beats.
  [[nodiscard]] double GetPosition() const noexcept;

  /// Returns whether performer is looping or not.
  ///
  /// @return True if looping, false otherwise.
  [[nodiscard]] bool IsLooping() const noexcept;

  /// Returns whether performer is playing or not.
  ///
  /// @return True if playing, false otherwise.
  [[nodiscard]] bool IsPlaying() const noexcept;

  /// Processes the next task at the current position.
  void ProcessNextTaskAtPosition() noexcept;

  /// Destroys a task.
  ///
  /// @param task Task.
  void RemoveTask(Task& task) noexcept;

  /// Schedules a one-off task.
  ///
  /// @param definition Task definition.
  /// @param position Task position in beats.
  /// @param process_order Task process order.
  /// @param user_data Pointer to user data.
  void ScheduleOneOffTask(TaskDefinition definition, double position, int process_order,
                          void* user_data) noexcept;

  /// Sets loop begin position.
  ///
  /// @param loop_begin_position Loop begin position in beats.
  void SetLoopBeginPosition(double loop_begin_position) noexcept;

  /// Sets loop length.
  ///
  /// @param loop_length Loop length in beats.
  void SetLoopLength(double loop_length) noexcept;

  /// Sets whether performer should be looping or not.
  ///
  /// @param is_looping True if looping.
  void SetLooping(bool is_looping) noexcept;

  /// Sets position.
  ///
  /// @param position Position in beats.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetPosition(double position) noexcept;

  /// Sets task position.
  ///
  /// @param task Task.
  /// @param position Task position.
  void SetTaskPosition(Task& task, double position) noexcept;

  /// Sets task process order.
  ///
  /// @param task Task.
  /// @param process_order Task process order.
  void SetTaskProcessOrder(Task& task, int process_order) noexcept;

  /// Stops performer.
  void Start() noexcept;

  /// Stops performer.
  void Stop() noexcept;

  /// Updates performer by duration.
  ///
  /// @param duration Duration in beats.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Update(double duration) noexcept;

 private:
  // Recurring task alias.
  using RecurringTask = ::std::pair<std::pair<double, int>, Task*>;

  // Returns an iterator to the next recurring task to process.
  [[nodiscard]] std::set<RecurringTask>::const_iterator GetNextRecurringTask() const noexcept;

  // Loops around a given `position`.
  [[nodiscard]] double LoopAround(double position) const noexcept;

  // Decremments the last processed recurring task iterator to its predecessor.
  void PrevLastProcessedRecurringTaskIt() noexcept;

  // Denotes whether performer is looping or not.
  bool is_looping_ = false;

  // Denotes whether performer is playing or not.
  bool is_playing_ = false;

  // Loop begin position in beats.
  double loop_begin_position_ = 0.0;

  // Loop length in beats.
  double loop_length_ = 1.0;

  // Position in beats.
  double position_ = 0.0;

  // Sorted map of tasks by task position-identifier pairs.
  std::multimap<std::pair<double, int>, Task> one_off_tasks_;
  std::set<RecurringTask> recurring_tasks_;

  // Last processed recurring task iterator.
  std::optional<std::set<RecurringTask>::const_iterator> last_processed_recurring_task_it_;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_PERFORMER_H_
