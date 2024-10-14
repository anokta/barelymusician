#include "barelymusician/internal/musician.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <limits>

#include "barelymusician/internal/instrument_controller.h"
#include "barelymusician/internal/performer.h"

namespace barely::internal {

namespace {

// Converts seconds to minutes.
constexpr double kMinutesFromSeconds = 1.0 / 60.0;

// Converts minutes to seconds.
constexpr double kSecondsFromMinutes = 60.0;

}  // namespace

Musician::Musician(int frame_rate, double reference_frequency) noexcept
    : frame_rate_(frame_rate), reference_frequency_(reference_frequency) {}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Musician::AddInstrument(InstrumentController* instrument) noexcept {
  assert(instrument);
  instruments_.emplace(instrument);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Musician::AddPerformer(Performer* performer) noexcept {
  assert(performer);
  performers_.emplace(performer->GetProcessOrder(), performer);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Musician::RemoveInstrument(InstrumentController* instrument) noexcept {
  assert(instrument);
  instruments_.erase(instrument);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Musician::RemovePerformer(Performer* performer) noexcept {
  assert(performer);
  performers_.erase({performer->GetProcessOrder(), performer});
}

double Musician::GetBeatsFromSeconds(double seconds) const noexcept {
  return tempo_ * seconds * kMinutesFromSeconds;
}

int Musician::GetFrameRate() const noexcept { return frame_rate_; }

double Musician::GetReferenceFrequency() const noexcept { return reference_frequency_; }

int64_t Musician::GetFramesFromSeconds(double seconds) const noexcept {
  return static_cast<int64_t>(seconds * static_cast<double>(frame_rate_));
}

double Musician::GetSecondsFromBeats(double beats) const noexcept {
  return (tempo_ > 0.0) ? beats * kSecondsFromMinutes / tempo_
                        : (beats > 0.0 ? std::numeric_limits<double>::max()
                                       : std::numeric_limits<double>::lowest());
}

double Musician::GetTempo() const noexcept { return tempo_; }

double Musician::GetTimestamp() const noexcept { return timestamp_; }

int64_t Musician::GetUpdateFrame() const noexcept { return update_frame_; }

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
        update_frame_ = GetFramesFromSeconds(timestamp_);
        for (const auto& instrument : instruments_) {
          instrument->Update(update_frame_);
        }
      }

      if (has_tasks_to_process) {
        for (const auto& [_, performer] : performers_) {
          performer->ProcessNextTaskAtPosition();
        }
      }
    } else if (timestamp_ < timestamp) {
      timestamp_ = timestamp;
      update_frame_ = GetFramesFromSeconds(timestamp_);
      for (const auto& instrument : instruments_) {
        instrument->Update(update_frame_);
      }
    }
  }
}

}  // namespace barely::internal
