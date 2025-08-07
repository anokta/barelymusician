#include "api/engine.h"

#include <barelymusician.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <limits>
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

void BarelyEngine::Process(float* output_samples, int output_channel_count, int output_frame_count,
                           double timestamp) noexcept {
  assert(output_samples != nullptr);
  assert(output_channel_count > 0);
  assert(output_frame_count > 0);
  std::fill_n(output_samples, output_channel_count * output_frame_count, 0.0f);
  const int64_t process_frame = barely::SecondsToFrames(sample_rate_, timestamp);
  auto instruments = mutable_instruments_.GetScopedView();
  for (auto* instrument : *instruments) {
    instrument->Process(output_samples, output_channel_count, output_frame_count, process_frame);
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
      BarelyPerformer::TaskKey next_key = {barely::SecondsToBeats(tempo_, timestamp - timestamp_),
                                           std::numeric_limits<int>::min()};
      bool has_tasks_to_process = false;
      for (auto* performer : performers_) {
        if (const auto maybe_next_key = performer->GetNextTaskKey();
            maybe_next_key.has_value() && *maybe_next_key < next_key) {
          has_tasks_to_process = true;
          next_key = *maybe_next_key;
        }
      }

      const auto& [update_duration, max_priority] = next_key;
      assert(update_duration > 0.0 || has_tasks_to_process);

      if (update_duration > 0) {
        for (auto* performer : performers_) {
          performer->Update(update_duration);
        }

        timestamp_ += barely::BeatsToSeconds(tempo_, update_duration);
        const int64_t update_frame = barely::SecondsToFrames(sample_rate_, timestamp_);
        for (auto* instrument : instruments_) {
          instrument->Update(update_frame);
        }
      }

      if (has_tasks_to_process) {
        for (auto* performer : performers_) {
          performer->ProcessAllTasksAtPosition(max_priority);
        }
      }
    } else if (timestamp_ < timestamp) {
      timestamp_ = timestamp;
      const int64_t update_frame = barely::SecondsToFrames(sample_rate_, timestamp_);
      for (auto* instrument : instruments_) {
        instrument->Update(update_frame);
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
