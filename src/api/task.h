#ifndef BARELYMUSICIAN_API_TASK_H_
#define BARELYMUSICIAN_API_TASK_H_

#include <barelymusician.h>

#include "common/callback.h"

/// Implementation of a task.
struct BarelyTask {
 public:
  /// Process callback alias.
  using ProcessCallback = barely::Callback<BarelyTask_ProcessCallback>;

  /// Constructs a new `BarelyTask`.
  ///
  /// @param performer Performer.
  /// @param position Task position.
  /// @param duration Task duration.
  /// @param callback Task process callback.
  BarelyTask(BarelyPerformer& performer, double position, double duration,
             ProcessCallback callback) noexcept;

  /// Destroys `BarelyTask`.
  ~BarelyTask() noexcept;

  /// Non-copyable and non-movable.
  BarelyTask(const BarelyTask& other) noexcept = delete;
  BarelyTask& operator=(const BarelyTask& other) noexcept = delete;
  BarelyTask(BarelyTask&& other) noexcept = delete;
  BarelyTask& operator=(BarelyTask&& other) noexcept = delete;

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
  /// @param position Position in beats.
  /// @return True if inside, false otherwise.
  bool IsInside(double position) const noexcept {
    return position >= position_ && position < GetEndPosition();
  }

  /// Processes the task.
  ///
  /// @param state Task state.
  void Process(BarelyTaskState state) noexcept { process_callback_(state); }

  /// Sets whether the task is currently active or not.
  ///
  /// @param is_active True if active, false otherwise.
  void SetActive(bool is_active) noexcept {
    is_active_ = is_active;
    Process(is_active_ ? BarelyTaskState_kBegin : BarelyTaskState_kEnd);
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
  BarelyPerformer& performer_;

  // Position in beats.
  double position_;

  // Duration in beats.
  double duration_;

  // Process callback.
  ProcessCallback process_callback_;

  // Denotes whether the task is active or not.
  bool is_active_ = false;
};

#endif  // BARELYMUSICIAN_API_TASK_H_
