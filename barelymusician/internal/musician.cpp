#include "barelymusician/internal/musician.h"

#include <algorithm>
#include <cassert>
#include <limits>

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/effect.h"
#include "barelymusician/internal/instrument.h"
#include "barelymusician/internal/performer.h"
#include "barelymusician/internal/seconds.h"

namespace barely::internal {

Musician::Musician(int frame_rate) noexcept : frame_rate_(frame_rate) {}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Musician::AddEffect(Effect* effect) noexcept {
  assert(effect);
  effects_.emplace(effect);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Musician::AddInstrument(Instrument* instrument) noexcept {
  assert(instrument);
  instruments_.emplace(instrument);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Musician::AddPerformer(Performer* performer) noexcept {
  assert(performer);
  performers_.emplace(performer->GetProcessOrder(), performer);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Musician::RemoveEffect(Effect* effect) noexcept {
  assert(effect);
  effects_.erase(effect);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Musician::RemoveInstrument(Instrument* instrument) noexcept {
  assert(instrument);
  instruments_.erase(instrument);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Musician::RemovePerformer(Performer* performer) noexcept {
  assert(performer);
  performers_.erase({performer->GetProcessOrder(), performer});
}

double Musician::GetBeatsFromSeconds(double seconds) const noexcept {
  return BeatsFromSeconds(tempo_, seconds);
}

int Musician::GetFrameRate() const noexcept { return frame_rate_; }

double Musician::GetSecondsFromBeats(double beats) const noexcept {
  return (tempo_ > 0.0) ? SecondsFromBeats(tempo_, beats)
                        : (beats > 0.0 ? std::numeric_limits<double>::max()
                                       : std::numeric_limits<double>::lowest());
}

double Musician::GetTempo() const noexcept { return tempo_; }

double Musician::GetTimestamp() const noexcept { return timestamp_; }

void Musician::SetTempo(double tempo) noexcept { tempo_ = std::max(tempo, 0.0); }

// NOLINTNEXTLINE(bugprone-exception-escape)
void Musician::Update(double timestamp) noexcept {
  while (timestamp_ < timestamp) {
    if (tempo_ > 0.0) {
      double update_duration = GetBeatsFromSeconds(timestamp - timestamp_);
      bool has_tasks_to_process = false;
      for (const auto& [_, performer] : performers_) {
        if (const auto maybe_duration = performer->GetDurationToNextTask();
            maybe_duration && maybe_duration < update_duration) {
          has_tasks_to_process = true;
          update_duration = *maybe_duration;
        }
      }
      assert(update_duration > 0.0 || has_tasks_to_process);

      if (update_duration > 0) {
        for (const auto& [_, performer] : performers_) {
          performer->Update(update_duration);
        }

        timestamp_ += GetSecondsFromBeats(update_duration);
        for (const auto& effect : effects_) {
          effect->Update(timestamp_);
        }
        for (const auto& instrument : instruments_) {
          instrument->Update(timestamp_);
        }
      }

      if (has_tasks_to_process) {
        for (const auto& [_, performer] : performers_) {
          performer->ProcessNextTaskAtPosition();
        }
      }
    } else if (timestamp_ < timestamp) {
      timestamp_ = timestamp;
      for (const auto& effect : effects_) {
        effect->Update(timestamp_);
      }
      for (const auto& instrument : instruments_) {
        instrument->Update(timestamp_);
      }
    }
  }
}

}  // namespace barely::internal
