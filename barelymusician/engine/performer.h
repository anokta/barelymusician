#ifndef BARELYMUSICIAN_ENGINE_PERFORMER_H_
#define BARELYMUSICIAN_ENGINE_PERFORMER_H_

#include <map>
#include <memory>
#include <optional>
#include <unordered_map>
#include <utility>

#include "barelymusician/engine/id.h"
#include "barelymusician/engine/number.h"
#include "barelymusician/engine/status.h"
#include "barelymusician/engine/task.h"

namespace barely::internal {

/// Class that wraps performer.
class Performer {
 public:
  /// Creates new task at position.
  ///
  /// @param task_id Task identifier.
  /// @param definition Task definition.
  /// @param position Task position in beats.
  /// @param type Task type.
  /// @param user_data Pointer to user data.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void CreateTask(Id task_id, TaskDefinition definition, Real position,
                  TaskType type, void* user_data) noexcept;

  /// Destroys task.
  ///
  /// @param task_id Task identifier.
  /// @return Status.
  Status DestroyTask(Id task_id) noexcept;

  /// Returns duration to next task.
  ///
  /// @return Duration in beats.
  [[nodiscard]] std::optional<Real> GetDurationToNextTask() const noexcept;

  /// Returns loop begin position.
  ///
  /// @return Loop begin position in beats.
  [[nodiscard]] Real GetLoopBeginPosition() const noexcept;

  /// Returns loop length.
  ///
  /// @return Loop length in beats.
  [[nodiscard]] Real GetLoopLength() const noexcept;

  /// Returns position.
  ///
  /// @return Position in beats.
  [[nodiscard]] Real GetPosition() const noexcept;

  /// Returns task position.
  ///
  /// @param task_id Task identifier.
  /// @return Position or error status.
  [[nodiscard]] StatusOr<Real> GetTaskPosition(Id task_id) const noexcept;

  /// Returns whether performer is looping or not.
  ///
  /// @return True if looping, false otherwise.
  [[nodiscard]] bool IsLooping() const noexcept;

  /// Returns whether performer is playing or not.
  ///
  /// @return True if playing, false otherwise.
  [[nodiscard]] bool IsPlaying() const noexcept;

  /// Processes all tasks at the current position.
  void ProcessAllTasksAtCurrentPosition() noexcept;

  /// Sets loop begin position.
  ///
  /// @param loop_begin_position Loop begin position in beats.
  void SetLoopBeginPosition(Real loop_begin_position) noexcept;

  /// Sets loop length.
  ///
  /// @param loop_length Loop length in beats.
  void SetLoopLength(Real loop_length) noexcept;

  /// Sets whether performer should be looping or not.
  ///
  /// @param is_looping True if looping.
  void SetLooping(bool is_looping) noexcept;

  /// Sets position.
  ///
  /// @param position Position in beats.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetPosition(Real position) noexcept;

  /// Sets task position.
  ///
  /// @param task_id Task identifier.
  /// @param position Task position.
  /// @return Status.
  Status SetTaskPosition(Id task_id, Real position) noexcept;

  /// Stops performer.
  void Start() noexcept;

  /// Stops performer.
  void Stop() noexcept;

  /// Updates performer by duration.
  ///
  /// @param duration Duration in beats.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Update(Real duration) noexcept;

  /// Updates performer to next task position.
  void UpdateToNextTask() noexcept;

 private:
  // Task info.
  struct TaskInfo {
    // Position.
    Real position;

    // Type.
    TaskType type;
  };

  // Returns an iterator to the next recurring task to process.
  using TaskMap = std::map<std::pair<Real, Id>, std::unique_ptr<Task>>;
  [[nodiscard]] TaskMap::const_iterator GetNextRecurringTask() const noexcept;

  // Denotes whether performer is looping or not.
  bool is_looping_ = false;

  // Denotes whether performer is playing or not.
  bool is_playing_ = false;

  // Loop begin position in beats.
  Real loop_begin_position_ = 0.0;

  // Loop length in beats.
  Real loop_length_ = 1.0;

  // Position in beats.
  Real position_ = 0.0;

  // Map of task infos by task identifiers.
  std::unordered_map<Id, TaskInfo> infos_;

  // Sorted map of tasks by task position-identifier pairs.
  TaskMap one_off_tasks_;
  TaskMap recurring_tasks_;

  // Last processed recurring task position.
  std::optional<Real> last_processed_position_;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_ENGINE_PERFORMER_H_
