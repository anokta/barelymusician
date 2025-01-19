#ifndef BARELYMUSICIAN_ENGINE_PERFORMER_H_
#define BARELYMUSICIAN_ENGINE_PERFORMER_H_

#include <compare>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <utility>

#include "barelymusician.h"
#include "engine/callback.h"

namespace barely::internal {

/// Class that wraps a performer.
class Performer {
 public:
  /// Beat callback alias.
  using BeatCallback = Callback<BarelyPerformer_BeatCallback>;

  /// Task.
  class Task {
   public:
    /// Process callback alias.
    using ProcessCallback = Callback<BarelyTask_ProcessCallback>;

    /// Constructs a new `Task`.
    ///
    /// @param performer Performer.
    /// @param position Task position.
    /// @param duration Task duration.
    /// @param callback Task process callback.
    Task(Performer& performer, double position, double duration, ProcessCallback callback) noexcept
        : performer_(performer),
          position_(position),
          duration_(duration),
          process_callback_(callback) {}

    /// Returns the duration.
    ///
    /// @return Duration in beats.
    double GetDuration() const noexcept { return duration_; }

    /// Returns the position.
    ///
    /// @return Position in beats.
    double GetPosition() const noexcept { return position_; }

    /// Returns the end position.
    ///
    /// @return End position in beats.
    double GetEndPosition() const noexcept { return position_ + duration_; }

    /// Returns whether the task is currently active or not.
    ///
    /// @return True if active, false otherwise.
    bool IsActive() const noexcept { return is_active_; }

    /// Returns whether a position is inside the task boundaries.
    ///
    /// @return True if inside, false otherwise.
    bool IsInside(double position) const noexcept {
      return position >= position_ && position < GetEndPosition();
    }

    /// Processes the task.
    ///
    /// @param state Task state.
    void Process(TaskState state) noexcept {
      process_callback_(static_cast<BarelyTaskState>(state));
    }

    /// Sets whether the task is currently active or not.
    ///
    /// @param is_active True if active, false otherwise.
    void SetActive(bool is_active) noexcept {
      is_active_ = is_active;
      Process(is_active_ ? TaskState::kBegin : TaskState::kEnd);
    }

    /// Sets the duration.
    ///
    /// @param duration Duration in beats.
    void SetDuration(double duration) noexcept;

    /// Sets the position.
    ///
    /// @param position Position in beats.
    void SetPosition(double position) noexcept;

    /// Sets the process callback.
    ///
    /// @param callback Task process callback.
    void SetProcessCallback(ProcessCallback callback) noexcept;

   private:
    // Performer.
    Performer& performer_;

    // Position in beats.
    double position_;

    // Duration in beats.
    double duration_;

    // Process callback.
    ProcessCallback process_callback_;

    // Denotes whether the task is active or not.
    bool is_active_ = false;

    // TODO(#147): Temp hack to allow destroying by handle.
   public:
    BarelyPerformerHandle performer = nullptr;
  };

  /// Creates a new task.
  ///
  /// @param position Task position in beats.
  /// @param duration Task duration in beats.
  /// @param callback Task process callback.
  /// @return Pointer to task.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  Task* CreateTask(double position, double duration, Task::ProcessCallback callback) noexcept;

  /// Destroys a task.
  ///
  /// @param task Pointer to task.
  void DestroyTask(Task* task) noexcept;

  /// Returns loop begin position.
  ///
  /// @return Loop begin position in beats.
  [[nodiscard]] double GetLoopBeginPosition() const noexcept { return loop_begin_position_; }

  /// Returns loop end position.
  ///
  /// @return Loop end position in beats.
  [[nodiscard]] double GetLoopEndPosition() const noexcept {
    return loop_begin_position_ + loop_length_;
  }

  /// Returns loop length.
  ///
  /// @return Loop length in beats.
  [[nodiscard]] double GetLoopLength() const noexcept { return loop_length_; }

  /// Returns position.
  ///
  /// @return Position in beats.
  [[nodiscard]] double GetPosition() const noexcept { return position_; }

  /// Returns whether performer is looping or not.
  ///
  /// @return True if looping, false otherwise.
  [[nodiscard]] bool IsLooping() const noexcept { return is_looping_; }

  /// Returns whether performer is playing or not.
  ///
  /// @return True if playing, false otherwise.
  [[nodiscard]] bool IsPlaying() const noexcept { return is_playing_; }

  /// Returns the duration to next callback.
  ///
  /// @return Optional duration in beats.
  [[nodiscard]] std::optional<double> GetNextDuration() const noexcept;

  /// Processes all tasks at the current position.
  void ProcessAllTasksAtPosition() noexcept;

  /// Sets the beat callback.
  ///
  /// @param callback Beat callback.
  void SetBeatCallback(BeatCallback callback) noexcept;

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

  /// Sets whether the task active is active or not.
  ///
  /// @param it Iterator to task.
  /// @param is_active True if active, false otherwise.
  void SetTaskActive(std::set<std::pair<double, Task*>>::iterator it, bool is_active) noexcept;

  /// Sets task duration.
  ///
  /// @param task Pointer to task.
  /// @param old_duration Old task duration.
  void SetTaskDuration(Task* task, double old_position) noexcept;

  /// Sets task position.
  ///
  /// @param task Pointer to task.
  /// @param old_position Old task position.
  void SetTaskPosition(Task* task, double old_position) noexcept;

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
  //  Returns an iterator to the next inactive task to process.
  [[nodiscard]] std::set<std::pair<double, Task*>>::const_iterator GetNextInactiveTask()
      const noexcept;

  // Loops around a given `position`.
  [[nodiscard]] double LoopAround(double position) const noexcept;

  // Beat callback.
  BeatCallback beat_callback_ = {};

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

  // Map of tasks with their position-pointer pairs.
  std::unordered_map<Task*, std::unique_ptr<Task>> tasks_;
  std::set<std::pair<double, Task*>> active_tasks_;
  std::set<std::pair<double, Task*>> inactive_tasks_;

  // Last processed task iterator.
  // std::optional<TaskMap::const_iterator> last_processed_task_it_;

  std::optional<double> last_beat_position_ = std::nullopt;

  // TODO(#147): Temp hack to allow destroying by handle.
 public:
  BarelyMusicianHandle musician = nullptr;
};

}  // namespace barely::internal

struct BarelyPerformer : public barely::internal::Performer {};
static_assert(sizeof(BarelyPerformer) == sizeof(barely::internal::Performer));

struct BarelyTask : public barely::internal::Performer::Task {};
static_assert(sizeof(BarelyTask) == sizeof(barely::internal::Performer::Task));

#endif  // BARELYMUSICIAN_ENGINE_PERFORMER_H_
