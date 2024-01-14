
#include "barelymusician/internal/musician.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <limits>
#include <optional>
#include <utility>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/rational.h"
#include "barelymusician/internal/instrument.h"
#include "barelymusician/internal/performer.h"

namespace barely::internal {

namespace {

/// Converts minutes to seconds.
constexpr int kSecondsFromMinutes = 60;

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
Musician::Musician(int frame_rate) noexcept : frame_rate_(frame_rate) { assert(frame_rate > 0); }

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

int Musician::GetFrameRate() const noexcept { return frame_rate_; }

int Musician::GetTempo() const noexcept { return tempo_; }

std::int64_t Musician::GetTimestamp() const noexcept { return timestamp_; }

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

void Musician::SetTempo(int tempo) noexcept {
  tempo = std::max(tempo, 0);
  if (tempo_ == tempo) {
    return;
  }
  tempo_ = tempo;
  for (const auto& instrument : instruments_) {
    instrument->SetTempo(tempo_);
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Musician::Update(std::int64_t timestamp) noexcept {
  // Keep track of the fractional part of the timestamp to compensate for update intervals beyond
  // the timestamp granularity.
  Rational timestamp_fraction = 0;
  while (timestamp_ < timestamp) {
    if (tempo_ > 0) {
      std::pair<Rational, int> update_duration = {
          (timestamp - timestamp_ - timestamp_fraction) *
              Rational(tempo_, frame_rate_ * kSecondsFromMinutes),
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
      assert(update_duration.first > 0 || has_tasks_to_process);

      if (update_duration.first > 0) {
        for (const auto& performer : performers_) {
          assert(performer);
          performer->Update(update_duration.first);
        }

        const Rational update_interval =
            frame_rate_ * kSecondsFromMinutes * update_duration.first / tempo_ + timestamp_fraction;
        timestamp_fraction = (update_interval % 1);
        timestamp_ += static_cast<std::int64_t>(update_interval);

        for (const auto& instrument : instruments_) {
          assert(instrument);
          instrument->Update(timestamp_, update_duration.first);
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
        instrument->Update(timestamp_, /*duration=*/0);
      }
    }
  }
}

}  // namespace barely::internal
