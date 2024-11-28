#include "engine/musician.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <limits>
#include <memory>
#include <utility>

#include "engine/config.h"
#include "engine/instrument.h"
#include "engine/performer.h"

namespace barely::internal {

namespace {

// Converts seconds to minutes.
constexpr double kMinutesFromSeconds = 1.0 / 60.0;

// Converts minutes to seconds.
constexpr double kSecondsFromMinutes = 60.0;

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
Musician::Musician(int sample_rate) noexcept
    : instrument_pool_(kMaxInstrumentCount),
      performer_pool_(kMaxPerformerCount),
      sample_rate_(sample_rate) {
  instruments_.reserve(kMaxInstrumentCount);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
Instrument* Musician::AddInstrument() noexcept {
  Instrument* instrument = instrument_pool_.Construct(sample_rate_, reference_frequency_,
                                                      GetSamplesFromSeconds(timestamp_));
  [[maybe_unused]] const bool success = instruments_.emplace(instrument).second;
  assert(success);
  return instrument;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
Performer* Musician::AddPerformer(int process_order) noexcept {
  Performer* performer = performer_pool_.Construct(process_order);
  [[maybe_unused]] const bool success = performers_.emplace(process_order, performer).second;
  assert(success);
  return performer;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Musician::RemoveInstrument(Instrument* instrument) noexcept {
  assert(instrument != nullptr);
  [[maybe_unused]] const bool success = (instruments_.erase(instrument) == 1);
  assert(success);
  instrument_pool_.Destruct(instrument);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Musician::RemovePerformer(Performer* performer) noexcept {
  assert(performer != nullptr);
  [[maybe_unused]] const bool success =
      (performers_.erase({performer->GetProcessOrder(), performer}) == 1);
  assert(success);
  performer_pool_.Destruct(performer);
}

double Musician::GetBeatsFromSeconds(double seconds) const noexcept {
  return tempo_ * seconds * kMinutesFromSeconds;
}

float Musician::GetReferenceFrequency() const noexcept { return reference_frequency_; }

int64_t Musician::GetSamplesFromSeconds(double seconds) const noexcept {
  return static_cast<int64_t>(seconds * static_cast<double>(sample_rate_));
}

double Musician::GetSecondsFromBeats(double beats) const noexcept {
  return (tempo_ > 0.0) ? beats * kSecondsFromMinutes / tempo_
                        : (beats > 0.0 ? std::numeric_limits<double>::max()
                                       : std::numeric_limits<double>::lowest());
}

double Musician::GetTempo() const noexcept { return tempo_; }

double Musician::GetTimestamp() const noexcept { return timestamp_; }

void Musician::SetReferenceFrequency(float reference_frequency) noexcept {
  reference_frequency = std::max(reference_frequency, 0.0f);
  if (reference_frequency_ != reference_frequency) {
    reference_frequency_ = reference_frequency;
    for (auto& instrument : instruments_) {
      instrument->SetReferenceFrequency(reference_frequency_);
    }
  }
}

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
        const int64_t update_sample = GetSamplesFromSeconds(timestamp_);
        for (const auto& instrument : instruments_) {
          instrument->Update(update_sample);
        }
      }

      if (has_tasks_to_process) {
        for (const auto& [_, performer] : performers_) {
          performer->ProcessNextTaskAtPosition();
        }
      }
    } else if (timestamp_ < timestamp) {
      timestamp_ = timestamp;
      const int64_t update_sample = GetSamplesFromSeconds(timestamp_);
      for (const auto& instrument : instruments_) {
        instrument->Update(update_sample);
      }
    }
  }
}

}  // namespace barely::internal
