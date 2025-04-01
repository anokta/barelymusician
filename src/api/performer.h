#ifndef BARELYMUSICIAN_API_PERFORMER_H_
#define BARELYMUSICIAN_API_PERFORMER_H_

#include <barelymusician.h>

#include <cassert>
#include <compare>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <unordered_set>
#include <utility>

#include "api/task.h"
#include "common/callback.h"

/// Implementation of a performer.
struct BarelyPerformer {
 public:
  /// Beat callback alias.
  using BeatCallback = barely::Callback<BarelyPerformer_BeatCallback>;

  /// Constructs a new `BarelyPerformer`.
  ///
  /// @param engine Engine.
  explicit BarelyPerformer(BarelyEngine& engine) noexcept;

  /// Destroys `BarelyPerformer`.
  ~BarelyPerformer() noexcept;

  /// Non-copyable and non-movable.
  BarelyPerformer(const BarelyPerformer& other) noexcept = delete;
  BarelyPerformer& operator=(const BarelyPerformer& other) noexcept = delete;
  BarelyPerformer(BarelyPerformer&& other) noexcept = delete;
  BarelyPerformer& operator=(BarelyPerformer&& other) noexcept = delete;

  /// Adds a new task.
  ///
  /// @param task Pointer to task.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void AddTask(BarelyTask* task) noexcept;

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

  /// Removes a task.
  ///
  /// @param task Pointer to task.
  void RemoveTask(BarelyTask* task) noexcept;

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

  /// Sets task duration.
  ///
  /// @param task Pointer to task.
  /// @param old_duration Old task duration.
  void SetTaskDuration(BarelyTask* task, double old_duration) noexcept;

  /// Sets task position.
  ///
  /// @param task Pointer to task.
  /// @param old_position Old task position.
  void SetTaskPosition(BarelyTask* task, double old_position) noexcept;

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
  [[nodiscard]] std::set<std::pair<double, BarelyTask*>>::const_iterator GetNextInactiveTask()
      const noexcept;

  // Loops around a given `position`.
  [[nodiscard]] double LoopAround(double position) const noexcept;

  /// Sets the active status of a task.
  void SetTaskActive(const std::set<std::pair<double, BarelyTask*>>::iterator& it,
                     bool is_active) noexcept;

  /// Updates the key of an active task.
  void UpdateActiveTaskKey(double old_end_position, BarelyTask* task) noexcept;

  /// Updates the key of an inactive task.
  void UpdateInactiveTaskKey(double old_position, BarelyTask* task) noexcept;

  // Engine.
  BarelyEngine& engine_;

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

  // Set of task position-pointer pairs.
  std::set<std::pair<double, BarelyTask*>> active_tasks_;
  std::set<std::pair<double, BarelyTask*>> inactive_tasks_;

  std::optional<double> last_beat_position_ = std::nullopt;
};

#endif  // BARELYMUSICIAN_API_PERFORMER_H_
