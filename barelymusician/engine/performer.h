#ifndef BARELYMUSICIAN_ENGINE_PERFORMER_H_
#define BARELYMUSICIAN_ENGINE_PERFORMER_H_

#include <map>
#include <memory>
#include <optional>
#include <tuple>
#include <unordered_map>
#include <utility>

#include "barelymusician/engine/id.h"
#include "barelymusician/engine/status.h"
#include "barelymusician/engine/task.h"

namespace barely::internal {

/// Class that wraps a performer.
class Performer {
 public:
  /// Creates a new task at position.
  ///
  /// @param task_id Task identifier.
  /// @param definition Task definition.
  /// @param is_one_off True if one-off task, false otherwise.
  /// @param position Task position in beats.
  /// @param process_order Task process order.
  /// @param user_data Pointer to user data.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void CreateTask(Id task_id, TaskDefinition definition, bool is_one_off,
                  double position, int process_order, void* user_data) noexcept;

  /// Destroys a task.
  ///
  /// @param task_id Task identifier.
  /// @return Status.
  Status DestroyTask(Id task_id) noexcept;

  /// Returns the duration to next task.
  ///
  /// @return Pair of duration in beats and process order.
  [[nodiscard]] std::optional<std::pair<double, int>> GetDurationToNextTask()
      const noexcept;

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

  /// Returns task position.
  ///
  /// @param task_id Task identifier.
  /// @return Position or error status.
  [[nodiscard]] StatusOr<double> GetTaskPosition(Id task_id) const noexcept;

  /// Returns task process order.
  ///
  /// @param task_id Task identifier.
  /// @return Process order or error status.
  [[nodiscard]] StatusOr<int> GetTaskProcessOrder(Id task_id) const noexcept;

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
  /// @param task_id Task identifier.
  /// @param position Task position.
  /// @return Status.
  Status SetTaskPosition(Id task_id, double position) noexcept;

  /// Sets task process order.
  ///
  /// @param task_id Task identifier.
  /// @param process_order Task process order.
  /// @return Status.
  Status SetTaskProcessOrder(Id task_id, int process_order) noexcept;

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
  // Task map alias.
  using TaskMap = std::map<std::tuple<double, int, Id>, std::unique_ptr<Task>>;

  // Task info.
  struct TaskInfo {
    // True if one-off task, false if recurring.
    bool is_one_off;

    // Position.
    double position;

    // Process order.
    int process_order;
  };

  // Returns an iterator to the next recurring task to process.
  [[nodiscard]] TaskMap::const_iterator GetNextRecurringTask() const noexcept;

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

  // Map of task infos by task identifiers.
  std::unordered_map<Id, TaskInfo> infos_;

  // Sorted map of tasks by task position-identifier pairs.
  TaskMap one_off_tasks_;
  TaskMap recurring_tasks_;

  // Last processed recurring task position.
  std::optional<double> last_processed_position_;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_ENGINE_PERFORMER_H_
