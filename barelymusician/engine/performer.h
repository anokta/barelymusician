#ifndef BARELYMUSICIAN_ENGINE_PERFORMER_H_
#define BARELYMUSICIAN_ENGINE_PERFORMER_H_

#include <map>
#include <optional>
#include <unordered_map>
#include <utility>

#include "barelymusician/barelymusician.h"
#include "barelymusician/engine/id.h"

namespace barely::internal {

/// Class that wraps performer.
class Performer {
 public:
  /// Task definition alias.
  using TaskDefinition = barely::TaskDefinition;

  /// Creates new task at position.
  ///
  /// @param id Task identifier.
  /// @param definition Task definition.
  /// @param position Task position.
  /// @param is_one_off True if task is one-off, false otherwise.
  /// @return True if successful, false otherwise.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  bool CreateTask(Id id, TaskDefinition definition, double position,
                  bool is_one_off) noexcept;

  /// Destroys task.
  ///
  /// @param id Task identifier.
  /// @return True if successful, false otherwise.
  bool DestroyTask(Id id) noexcept;

  /// Returns duration to next task.
  ///
  /// @return Duration in beats.
  [[nodiscard]] double GetDurationToNextTask() const noexcept;

  /// Returns task position.
  ///
  /// @param id Task identifier.
  /// @return Pointer to task position.
  [[nodiscard]] const double* GetTaskPosition(Id id) const noexcept;

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

  /// Sets task position.
  ///
  /// @param id Task identifier.
  /// @param position Task position.
  bool SetTaskPosition(Id id, double position) noexcept;

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

  /// Stops performer.
  void Start() noexcept;

  /// Stops performer.
  void Stop() noexcept;

  /// Triggers all tasks at current position.
  void TriggerAllTasksAtCurrentPosition() noexcept;

  /// Updates performer by duration.
  ///
  /// @param duration Duration in beats.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Update(double duration) noexcept;

 private:
  // Returns next task callback.
  [[nodiscard]] std::map<std::pair<double, Id>, TaskCallback>::const_iterator
  GetNextTaskCallback() const noexcept;

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

  // Sorted map of task callbacks by task position-identifier pairs.
  std::map<std::pair<double, Id>, TaskCallback> callbacks_;

  // Map of task positions by task identifiers.
  std::unordered_map<Id, double> positions_;

  // Sorted map of one-off task callbacks by task delays.
  std::multimap<double, TaskCallback> one_off_callbacks_;

  // Last triggered position.
  std::optional<double> last_triggered_position_;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_ENGINE_PERFORMER_H_
