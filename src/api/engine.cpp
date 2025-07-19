#include "api/engine.h"

#include <barelymusician.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <span>
#include <utility>
#include <vector>

#include "api/instrument.h"
#include "api/performer.h"
#include "common/time.h"

// NOLINTNEXTLINE(bugprone-exception-escape)
BarelyEngine::BarelyEngine(int sample_rate, float reference_frequency) noexcept
    : sample_rate_(sample_rate), reference_frequency_(reference_frequency) {
  assert(sample_rate >= 0);
  assert(reference_frequency >= 0.0f);
}

BarelyEngine::~BarelyEngine() noexcept { mutable_instruments_.Update({}); }

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyEngine::AddInstrument(BarelyInstrumentHandle instrument) noexcept {
  [[maybe_unused]] const bool success = instruments_.emplace(instrument).second;
  assert(success);
  UpdateMutableInstruments();
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyEngine::AddPerformer(BarelyPerformer* performer) noexcept {
  [[maybe_unused]] const bool success = performers_.emplace(performer).second;
  assert(success);
}

void BarelyEngine::Process(std::span<float> output_samples, double timestamp) noexcept {
  std::fill_n(output_samples.data(), output_samples.size(), 0.0f);
  const int64_t process_sample = barely::SecondsToSamples(sample_rate_, timestamp);
  auto instruments = mutable_instruments_.GetScopedView();
  for (auto* instrument : *instruments) {
    instrument->Process(output_samples, process_sample);
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyEngine::RemoveInstrument(BarelyInstrumentHandle instrument) noexcept {
  instruments_.erase(instrument);
  UpdateMutableInstruments();
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyEngine::RemovePerformer(BarelyPerformer* performer) noexcept {
  performers_.erase(performer);
}

void BarelyEngine::SetTempo(double tempo) noexcept { tempo_ = std::max(tempo, 0.0); }

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyEngine::Update(double timestamp) noexcept {
  while (timestamp_ < timestamp) {
    if (tempo_ > 0.0) {
      double update_duration = barely::SecondsToBeats(tempo_, timestamp - timestamp_);
      bool has_tasks_to_process = false;
      for (auto* performer : performers_) {
        if (const auto maybe_duration = performer->GetNextDuration();
            maybe_duration.has_value() && *maybe_duration < update_duration) {
          has_tasks_to_process = true;
          update_duration = *maybe_duration;
        }
      }
      assert(update_duration > 0.0 || has_tasks_to_process);

      if (update_duration > 0) {
        for (auto* performer : performers_) {
          performer->Update(update_duration);
        }

        timestamp_ += barely::BeatsToSeconds(tempo_, update_duration);
        const int64_t update_sample = barely::SecondsToSamples(sample_rate_, timestamp_);
        for (auto* instrument : instruments_) {
          instrument->Update(update_sample);
        }
      }

      if (has_tasks_to_process) {
        for (auto* performer : performers_) {
          performer->ProcessAllTasksAtPosition();
        }
      }
    } else if (timestamp_ < timestamp) {
      timestamp_ = timestamp;
      const int64_t update_sample = barely::SecondsToSamples(sample_rate_, timestamp_);
      for (auto* instrument : instruments_) {
        instrument->Update(update_sample);
      }
    }
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyEngine::UpdateMutableInstruments() noexcept {
  std::vector<BarelyInstrument*> new_mutable_instruments;
  new_mutable_instruments.reserve(instruments_.size());
  for (auto* instrument : instruments_) {
    new_mutable_instruments.emplace_back(instrument);
  }
  mutable_instruments_.Update(std::move(new_mutable_instruments));
}
