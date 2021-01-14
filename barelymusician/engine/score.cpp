#include "barelymusician/engine/score.h"

#include "barelymusician/base/logging.h"

namespace barelyapi {

int Score::AddNoteEvent(double position, double duration, float pitch,
                        float intensity) {
  DCHECK_GE(position, 0.0);
  DCHECK_GE(duration, 0.0);
  const int event_id = id_generator_.Next();
  events_.emplace(event_id, std::vector<double>{position, position + duration});
  data_.emplace(std::pair{position, event_id}, NoteOn{pitch, intensity});
  data_.emplace(std::pair{position + duration, event_id}, NoteOff{pitch});
  return event_id;
}

void Score::ForEachEventInRange(double begin_position, double end_position,
                                const ScoreEventCallback& callback) const {
  DCHECK_GE(begin_position, 0.0);
  DCHECK_GE(end_position, 0.0);
  if (begin_position < end_position && callback) {
    const auto begin = data_.lower_bound({begin_position, 0});
    const auto end = data_.lower_bound({end_position, 0});
    for (auto it = begin; it != end; ++it) {
      callback(it->first.first, it->second);
    }
  }
}

bool Score::IsEmpty() const { return events_.empty(); }

void Score::RemoveAllEvents() {
  data_.clear();
  events_.clear();
}

bool Score::RemoveEvent(int event_id) {
  if (auto it = events_.find(event_id); it != events_.end()) {
    for (const double position : it->second) {
      data_.erase(std::pair(position, event_id));
    }
    events_.erase(it);
    return true;
  }
  return false;
}

}  // namespace barelyapi
