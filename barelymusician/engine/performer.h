#ifndef BARELYMUSICIAN_ENGINE_PERFORMER_H_
#define BARELYMUSICIAN_ENGINE_PERFORMER_H_

#include <map>
#include <optional>
#include <unordered_map>
#include <utility>

#include "barelymusician/engine/id.h"
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
  /// @param is_one_off True if task is one-off, false otherwise.
  /// @param user_data Pointer to user data.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void CreateTask(Id task_id, TaskDefinition definition, double position,
                  bool is_one_off, void* user_data) noexcept;

  /// Destroys task.
  ///
  /// @param task_id Task identifier.
  /// @return Status.
  Status DestroyTask(Id task_id) noexcept;

  // TODO(#109): Refactor to match `Task` functionality.
  // /// Returns duration to next task.
  // ///
  // /// @return Duration in beats.
  // [[nodiscard]] double GetDurationToNextTask() const noexcept;

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

  /// Returns whether performer is looping or not.
  ///
  /// @return True if looping, false otherwise.
  [[nodiscard]] bool IsLooping() const noexcept;

  /// Returns whether performer is playing or not.
  ///
  /// @return True if playing, false otherwise.
  [[nodiscard]] bool IsPlaying() const noexcept;

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

  /// Stops performer.
  void Start() noexcept;

  /// Stops performer.
  void Stop() noexcept;

  // TODO(#109): Refactor to match `Task` functionality.
  // /// Triggers all tasks at current position.
  // void TriggerAllTasksAtCurrentPosition() noexcept;

  // /// Updates performer by duration.
  // ///
  // /// @param duration Duration in beats.
  // // NOLINTNEXTLINE(bugprone-exception-escape)
  // void Update(double duration) noexcept;

 private:
  // TODO(#109): Refactor to match `Task` functionality.
  // // Returns next task callback.
  // [[nodiscard]] std::map<std::pair<double, Id>, TaskCallback>::const_iterator
  // GetNextTaskCallback() const noexcept;

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

  // Map of task positions by task identifiers.
  std::unordered_map<Id, std::pair<double, bool>> position_type_pairs_;

  // Sorted map of tasks by task position-identifier pairs.
  std::map<std::pair<double, Id>, Task> one_off_tasks_;
  std::map<std::pair<double, Id>, Task> tasks_;

  // // Last triggered position.
  // std::optional<double> last_triggered_position_;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_ENGINE_PERFORMER_H_
