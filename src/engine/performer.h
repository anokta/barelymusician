#ifndef BARELYMUSICIAN_ENGINE_PERFORMER_H_
#define BARELYMUSICIAN_ENGINE_PERFORMER_H_

#include <compare>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <utility>

#include "barelymusician.h"
#include "engine/event.h"
#include "engine/pool.h"

namespace barely::internal {

/// Class that wraps a performer.
class Performer {
 public:
  /// Task.
  struct Task : public Event<TaskEvent> {
    /// Constructs a new `Task`.
    ///
    /// @param performer Performer.
    /// @param task_event Task event.
    /// @param position Task position.
    /// @param set_position_callback Set position callback.
    Task(Performer& performer, const TaskEvent& task_event, double position) noexcept;

    /// Returns the position.
    ///
    /// @return Position in beats.
    double GetPosition() const noexcept { return position_; }

    /// Sets the position.
    ///
    /// @param position Position in beats.
    void SetPosition(double position) noexcept;

   private:
    // Performer.
    Performer& performer_;

    // Position.
    double position_;
  };

  /// Constructs a new `Performer`.
  ///
  /// @param process_order Process order.
  explicit Performer(int process_order) noexcept;

  /// Adds a task.
  ///
  /// @param task_event Task event.
  /// @param position Task position.
  /// @return Pointer to task.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  Task* AddTask(const TaskEvent& task_event, double position) noexcept;

  /// Returns the duration to next task.
  ///
  /// @return Optional duration in beats.
  [[nodiscard]] std::optional<double> GetDurationToNextTask() const noexcept;

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

  /// Returns process order.
  ///
  /// @return Process order.
  [[nodiscard]] int GetProcessOrder() const noexcept;

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

  /// Removes a task.
  ///
  /// @param task Pointer to task.
  void RemoveTask(Task* task) noexcept;

  /// Sets the beat event.
  ///
  /// @param beat_event Beat event.
  void SetBeatEvent(const BeatEvent& beat_event) noexcept;

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
  /// @param task Task handle.
  /// @param position Task position.
  void SetTaskPosition(Task* task, double position) noexcept;

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
  // Recurring task set alias.
  using RecurringTaskSet = std::set<std::pair<double, Task*>>;

  // Returns an iterator to the next recurring task to process.
  [[nodiscard]] RecurringTaskSet::const_iterator GetNextRecurringTask() const noexcept;

  // Loops around a given `position`.
  [[nodiscard]] double LoopAround(double position) const noexcept;

  // Decremments the last processed recurring task iterator to its predecessor.
  void PrevLastProcessedRecurringTaskIt() noexcept;

  // Beat event.
  BeatEvent beat_event_ = {};

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

  // Process order.
  int process_order_ = 0;

  // Map of tasks.
  Pool<Task> recurring_task_pool_;
  RecurringTaskSet recurring_tasks_;

  // Last processed recurring task iterator.
  std::optional<RecurringTaskSet::const_iterator> last_processed_recurring_task_it_;

  std::optional<double> last_beat_position_;
};

}  // namespace barely::internal

struct BarelyPerformer : public barely::internal::Performer {};
static_assert(sizeof(BarelyPerformer) == sizeof(barely::internal::Performer));

struct BarelyTask : public barely::internal::Performer::Task {};
static_assert(sizeof(BarelyTask) == sizeof(barely::internal::Performer::Task));

#endif  // BARELYMUSICIAN_ENGINE_PERFORMER_H_
