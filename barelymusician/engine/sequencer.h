#ifndef BARELYMUSICIAN_ENGINE_SEQUENCER_H_
#define BARELYMUSICIAN_ENGINE_SEQUENCER_H_

#include <functional>
#include <map>
#include <optional>
#include <unordered_map>
#include <utility>

#include "barelymusician/engine/id.h"

namespace barely::internal {

/// Class that wraps sequencer.
class Sequencer {
 public:
  /// Event callback signature.
  ///
  /// @param position Position in beats.
  using EventCallback = std::function<void(double position)>;

  /// Adds new event at position.
  ///
  /// @param id Event identifier.
  /// @param position Event position.
  /// @param callback Event callback.
  /// @return True if successful, false otherwise.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  bool AddEvent(Id id, double position, EventCallback callback) noexcept;

  /// Returns duration to next event.
  ///
  /// @return Duration in beats.
  [[nodiscard]] double GetDurationToNextEvent() const noexcept;

  /// Returns event callback.
  ///
  /// @param id Event identifier.
  /// @return Pointer to event callback.
  [[nodiscard]] const EventCallback* GetEventCallback(Id id) const noexcept;

  /// Returns event position.
  ///
  /// @param id Event identifier.
  /// @return Pointer to event position.
  [[nodiscard]] const double* GetEventPosition(Id id) const noexcept;

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
  bool ScheduleOneOffEvent(double position, EventCallback callback) noexcept;

  /// Sets event callback.
  ///
  /// @param id Event identifier.
  /// @param callback Event callback.
  bool SetEventCallback(Id id, EventCallback callback) noexcept;

  /// Sets event position.
  ///
  /// @param id Event identifier.
  /// @param position Event position.
  bool SetEventPosition(Id id, double position) noexcept;

  /// Sets loop begin position.
  ///
  /// @param loop_begin_position Loop begin position in beats.
  void SetLoopBeginPosition(double loop_begin_position) noexcept;

  /// Sets loop length.
  ///
  /// @param loop_length Loop length in beats.
  void SetLoopLength(double loop_length) noexcept;

  /// Sets whether sequencer should be looping or not.
  ///
  /// @param is_looping True if looping.
  void SetLooping(bool is_looping) noexcept;

  /// Sets position.
  ///
  /// @param position Position in beats.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetPosition(double position) noexcept;

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
  void Update(double duration) noexcept;

 private:
  // Returns next event callback.
  std::map<std::pair<double, Id>, EventCallback>::const_iterator&
  GetNextEventCallback() const noexcept;

  // Denotes whether sequencer is looping or not.
  bool is_looping_ = false;

  // Denotes whether sequencer is playing or not.
  bool is_playing_ = false;

  // Loop begin position in beats.
  double loop_begin_position_ = 0.0;

  // Loop length in beats.
  double loop_length_ = 1.0;

  // Position in beats.
  double position_ = 0.0;

  // Sorted map of event callbacks by event position-identifier pairs.
  std::map<std::pair<double, Id>, EventCallback> callbacks_;

  // Map of event positions by event identifiers.
  std::unordered_map<Id, double> positions_;

  // Sorted map of one-off event callbacks by event delays.
  std::multimap<double, EventCallback> one_off_callbacks_;

  // Next event callback (cached).
  mutable std::optional<
      std::map<std::pair<double, Id>, EventCallback>::const_iterator>
      next_callback_;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_ENGINE_SEQUENCER_H_
