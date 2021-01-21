#ifndef BARELYMUSICIAN_ENGINE_TRACK_H_
#define BARELYMUSICIAN_ENGINE_TRACK_H_

#include <functional>
#include <map>
#include <unordered_map>
#include <utility>
#include <vector>

#include "barelymusician/common/id_generator.h"
#include "barelymusician/instrument/instrument_data.h"

namespace barelyapi {

// Track event callback signature.
using TrackEventCallback =
    std::function<void(double position, const InstrumentData& data)>;

// Music track that stores scheduled instrument events.
struct Track {
 public:
  /// Adds note event.
  ///
  /// @param position Note position.
  /// @param duration Note duration.
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  /// @return Note event id.
  int AddNoteEvent(double position, double duration, float pitch,
                   float intensity);

  /// Calls given function for each event in given range.
  ///
  /// @param begin_position Begin position.
  /// @param end_position End position.
  /// @param callback Callback function.
  void ForEachEventInRange(double begin_position, double end_position,
                           const TrackEventCallback& callback) const;

  /// Returns whether the track is empty or not.
  ///
  /// @return True if empty.
  bool IsEmpty() const;

  /// Removes all events.
  void RemoveAllEvents();

  /// Removes all events in given range.
  ///
  /// @param begin_position Begin position.
  /// @param end_position End position.
  void RemoveAllEventsInRange(double begin_position, double end_position);

  /// Removes event.
  ///
  /// @param event_id Event id.
  /// @return True if successful.
  bool RemoveEvent(int event_id);

 private:
  // List of scheduled event instrument data.
  std::multimap<std::pair<double, int>, InstrumentData> data_;

  // List of event positions.
  std::unordered_map<int, std::vector<double>> events_;

  // Event id generator.
  IdGenerator id_generator_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_TRACK_H_
