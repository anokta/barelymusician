#include "barelymusician/internal/musician.h"

#include <algorithm>
#include <cassert>
#include <limits>
#include <optional>
#include <utility>

#include "barelymusician/internal/instrument.h"
#include "barelymusician/internal/performer.h"
#include "barelymusician/internal/seconds.h"

namespace barely::internal {

// NOLINTNEXTLINE(bugprone-exception-escape)
void Musician::AddInstrument(Instrument& instrument) noexcept {
  [[maybe_unused]] const bool success = instruments_.insert(&instrument).second;
  assert(success);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Musician::AddPerformer(Performer& performer) noexcept {
  [[maybe_unused]] const bool success = performers_.insert(&performer).second;
  assert(success);
}

double Musician::GetBeatsFromSeconds(double seconds) noexcept {
  return BeatsFromSeconds(tempo_, seconds);
}

double Musician::GetSecondsFromBeats(double beats) noexcept {
  return (tempo_ > 0.0) ? SecondsFromBeats(tempo_, beats)
                        : (beats > 0.0 ? std::numeric_limits<double>::max()
                                       : std::numeric_limits<double>::lowest());
}

double Musician::GetTempo() const noexcept { return tempo_; }

double Musician::GetTimestamp() const noexcept { return timestamp_; }

// NOLINTNEXTLINE(bugprone-exception-escape)
void Musician::RemoveInstrument(Instrument& instrument) noexcept {
  [[maybe_unused]] const bool success = instruments_.erase(&instrument) > 0;
  assert(success);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Musician::RemovePerformer(Performer& performer) noexcept {
  [[maybe_unused]] const bool success = performers_.erase(&performer) > 0;
  assert(success);
}

void Musician::SetTempo(double tempo) noexcept { tempo_ = std::max(tempo, 0.0); }

// NOLINTNEXTLINE(bugprone-exception-escape)
void Musician::Update(double timestamp) noexcept {
  while (timestamp_ < timestamp) {
    if (tempo_ > 0.0) {
      std::pair<double, int> update_duration = {GetBeatsFromSeconds(timestamp - timestamp_),
                                                std::numeric_limits<int>::lowest()};
      bool has_tasks_to_process = false;
      for (const auto& performer : performers_) {
        assert(performer);
        if (const auto maybe_duration = performer->GetDurationToNextTask();
            maybe_duration && maybe_duration < update_duration) {
          has_tasks_to_process = true;
          update_duration = *maybe_duration;
        }
      }
      assert(update_duration.first > 0.0 || has_tasks_to_process);

      if (update_duration.first > 0) {
        for (const auto& performer : performers_) {
          assert(performer);
          performer->Update(update_duration.first);
        }

        timestamp_ += GetSecondsFromBeats(update_duration.first);
        for (const auto& instrument : instruments_) {
          assert(instrument);
          instrument->Update(timestamp_);
        }
      }

      if (has_tasks_to_process) {
        for (const auto& performer : performers_) {
          assert(performer);
          performer->ProcessNextTaskAtPosition();
        }
      }
    } else if (timestamp_ < timestamp) {
      timestamp_ = timestamp;
      for (const auto& instrument : instruments_) {
        assert(instrument);
        instrument->Update(timestamp_);
      }
    }
  }
}

}  // namespace barely::internal
