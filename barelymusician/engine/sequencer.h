#ifndef BARELYMUSICIAN_ENGINE_SEQUENCER_H_
#define BARELYMUSICIAN_ENGINE_SEQUENCER_H_

#include <functional>
#include <map>
#include <optional>
#include <unordered_map>
#include <utility>

#include "barelymusician/barelymusician.h"
#include "barelymusician/engine/fixed.h"
#include "barelymusician/engine/id.h"

namespace barely::internal {

/// Class that wraps sequencer.
class Sequencer {
 public:
  /// Event callback alias.
  using EventCallback = barely::Sequencer::EventCallback;

  /// Adds new event at position.
  ///
  /// @param id Event identifier.
  /// @param position Event position.
  /// @param callback Event callback.
  /// @return True if successful, false otherwise.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  bool AddEvent(Id id, Fixed position, EventCallback callback) noexcept;

  /// Returns duration to next event.
  ///
  /// @return Duration in beats.
  [[nodiscard]] Fixed GetDurationToNextEvent() const noexcept;

  /// Returns event callback.
  ///
  /// @param id Event identifier.
  /// @return Pointer to event callback.
  [[nodiscard]] const EventCallback* GetEventCallback(Id id) const noexcept;

  /// Returns event position.
  ///
  /// @param id Event identifier.
  /// @return Pointer to event position.
  [[nodiscard]] const Fixed* GetEventPosition(Id id) const noexcept;

  /// Returns loop begin position.
  ///
  /// @return Loop begin position in beats.
  [[nodiscard]] Fixed GetLoopBeginPosition() const noexcept;

  /// Returns loop length.
  ///
  /// @return Loop length in beats.
  [[nodiscard]] Fixed GetLoopLength() const noexcept;

  /// Returns position.
  ///
  /// @return Position in beats.
  [[nodiscard]] Fixed GetPosition() const noexcept;

  /// Returns whether sequencer is looping or not.
  ///
  /// @return True if looping, false otherwise.
  [[nodiscard]] bool IsLooping() const noexcept;

  /// Returns whether sequencer is playing or not.
  ///
  /// @return True if playing, false otherwise.
  [[nodiscard]] bool IsPlaying() const noexcept;

  /// Adds new event at position.
  ///
  /// @param id Event identifier.
  /// @return True if successful, false otherwise.
  bool RemoveEvent(Id id) noexcept;

  /// Schedules one-off event at position.
  ///
  /// @param position Position in beats.
  /// @param callback Event callback.
  /// @return True if successful, false otherwise.
  bool ScheduleOneOffEvent(Fixed position, EventCallback callback) noexcept;

  /// Sets event callback.
  ///
  /// @param id Event identifier.
  /// @param callback Event callback.
  bool SetEventCallback(Id id, EventCallback callback) noexcept;

  /// Sets event position.
  ///
  /// @param id Event identifier.
  /// @param position Event position.
  bool SetEventPosition(Id id, Fixed position) noexcept;

  /// Sets loop begin position.
  ///
  /// @param loop_begin_position Loop begin position in beats.
  void SetLoopBeginPosition(Fixed loop_begin_position) noexcept;

  /// Sets loop length.
  ///
  /// @param loop_length Loop length in beats.
  void SetLoopLength(Fixed loop_length) noexcept;

  /// Sets whether sequencer should be looping or not.
  ///
  /// @param is_looping True if looping.
  void SetLooping(bool is_looping) noexcept;

  /// Sets position.
  ///
  /// @param position Position in beats.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetPosition(Fixed position) noexcept;

  /// Stops sequencer.
  void Start() noexcept;

  /// Stops sequencer.
  void Stop() noexcept;

  /// Triggers all events at current position.
  void TriggerAllEventsAtCurrentPosition() noexcept;

  /// Updates sequencer by duration.
  ///
  /// @param duration Duration in beats.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Update(Fixed duration) noexcept;

 private:
  // Returns next event callback.
  std::map<std::pair<Fixed, Id>, EventCallback>::const_iterator
  GetNextEventCallback() const noexcept;

  // Denotes whether sequencer is looping or not.
  bool is_looping_ = false;

  // Denotes whether sequencer is playing or not.
  bool is_playing_ = false;

  // Loop begin position in beats.
  Fixed loop_begin_position_ = Fixed(0);

  // Loop length in beats.
  Fixed loop_length_ = Fixed(1);

  // Position in beats.
  Fixed position_ = Fixed(0);

  // Sorted map of event callbacks by event position-identifier pairs.
  std::map<std::pair<Fixed, Id>, EventCallback> callbacks_;

  // Map of event positions by event identifiers.
  std::unordered_map<Id, Fixed> positions_;

  // Sorted map of one-off event callbacks by event delays.
  std::multimap<Fixed, EventCallback> one_off_callbacks_;

  // Last triggered position.
  std::optional<Fixed> last_triggered_position_;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_ENGINE_SEQUENCER_H_
