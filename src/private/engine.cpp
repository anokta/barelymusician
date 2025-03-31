#include "private/engine.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <limits>
#include <memory>
#include <span>
#include <utility>

#include "barelymusician.h"
#include "private/instrument.h"
#include "private/performer.h"

namespace barely {

namespace {

// Converts minutes to seconds.
constexpr double kMinutesToSeconds = 60.0;

// Converts seconds to minutes.
constexpr double kSecondsToMinutes = 1.0 / kMinutesToSeconds;

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
EngineImpl::EngineImpl(int sample_rate) noexcept : sample_rate_(sample_rate) {}

double EngineImpl::BeatsToSeconds(double beats) const noexcept {
  return (tempo_ > 0.0) ? beats * kMinutesToSeconds / tempo_
                        : (beats > 0.0 ? std::numeric_limits<double>::max()
                                       : std::numeric_limits<double>::lowest());
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void EngineImpl::CreateInstrument(BarelyInstrumentHandle instrument) noexcept {
  [[maybe_unused]] const bool success = instruments_.emplace(instrument).second;
  assert(success);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
PerformerImpl* EngineImpl::CreatePerformer() noexcept {
  auto performer = std::make_unique<PerformerImpl>();
  auto* performer_ptr = performer.get();
  [[maybe_unused]] const bool success =
      performers_.emplace(performer_ptr, std::move(performer)).second;
  assert(success);
  return performer_ptr;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void EngineImpl::DestroyInstrument(BarelyInstrumentHandle instrument) noexcept {
  instruments_.erase(instrument);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void EngineImpl::DestroyPerformer(PerformerImpl* performer) noexcept {
  performers_.erase(performer);
}

float EngineImpl::GetReferenceFrequency() const noexcept { return reference_frequency_; }

int EngineImpl::GetSampleRate() const noexcept { return sample_rate_; }

double EngineImpl::GetTempo() const noexcept { return tempo_; }

double EngineImpl::GetTimestamp() const noexcept { return timestamp_; }

double EngineImpl::SecondsToBeats(double seconds) const noexcept {
  return tempo_ * seconds * kSecondsToMinutes;
}

int64_t EngineImpl::SecondsToSamples(double seconds) const noexcept {
  return static_cast<int64_t>(seconds * static_cast<double>(sample_rate_));
}

void EngineImpl::SetReferenceFrequency(float reference_frequency) noexcept {
  reference_frequency = std::max(reference_frequency, 0.0f);
  if (reference_frequency_ != reference_frequency) {
    reference_frequency_ = reference_frequency;
    for (auto instrument : instruments_) {
      instrument->SetReferenceFrequency(reference_frequency_);
    }
  }
}

void EngineImpl::SetTempo(double tempo) noexcept { tempo_ = std::max(tempo, 0.0); }

// NOLINTNEXTLINE(bugprone-exception-escape)
void EngineImpl::Update(double timestamp) noexcept {
  while (timestamp_ < timestamp) {
    if (tempo_ > 0.0) {
      double update_duration = SecondsToBeats(timestamp - timestamp_);
      bool has_tasks_to_process = false;
      for (const auto& [performer, _] : performers_) {
        if (const auto maybe_duration = performer->GetNextDuration();
            maybe_duration.has_value() && *maybe_duration < update_duration) {
          has_tasks_to_process = true;
          update_duration = *maybe_duration;
        }
      }
      assert(update_duration > 0.0 || has_tasks_to_process);

      if (update_duration > 0) {
        for (const auto& [performer, _] : performers_) {
          performer->Update(update_duration);
        }

        timestamp_ += BeatsToSeconds(update_duration);
        const int64_t update_sample = SecondsToSamples(timestamp_);
        for (const auto instrument : instruments_) {
          instrument->Update(update_sample);
        }
      }

      if (has_tasks_to_process) {
        for (const auto& [performer, _] : performers_) {
          performer->ProcessAllTasksAtPosition();
        }
      }
    } else if (timestamp_ < timestamp) {
      timestamp_ = timestamp;
      const int64_t update_sample = SecondsToSamples(timestamp_);
      for (const auto instrument : instruments_) {
        instrument->Update(update_sample);
      }
    }
  }
}

}  // namespace barely
