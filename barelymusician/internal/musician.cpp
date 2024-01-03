
#include "barelymusician/internal/musician.h"

#include <algorithm>
#include <cassert>
#include <limits>
#include <optional>
#include <utility>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/seconds.h"
#include "barelymusician/internal/instrument.h"
#include "barelymusician/internal/observable.h"
#include "barelymusician/internal/performer.h"

namespace barely::internal {

// NOLINTNEXTLINE(bugprone-exception-escape)
Observable<Instrument> Musician::CreateInstrument(InstrumentDefinition definition,
                                                  int frame_rate) noexcept {
  Observable<Instrument> instrument(definition, frame_rate, tempo_, timestamp_);
  [[maybe_unused]] const bool success = instruments_.insert(instrument.get()).second;
  assert(success);
  return instrument;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
Observable<Performer> Musician::CreatePerformer() noexcept {
  Observable<Performer> performer;
  [[maybe_unused]] const bool success = performers_.insert(performer.get()).second;
  assert(success);
  return performer;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Musician::DestroyInstrument(const Observable<Instrument>& instrument) noexcept {
  [[maybe_unused]] const bool success = instruments_.erase(instrument.get()) > 0;
  assert(success);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Musician::DestroyPerformer(const Observable<Performer>& performer) noexcept {
  [[maybe_unused]] const bool success = performers_.erase(performer.get()) > 0;
  assert(success);
}

double Musician::GetTempo() const noexcept { return tempo_; }

double Musician::GetTimestamp() const noexcept { return timestamp_; }

void Musician::SetTempo(double tempo) noexcept {
  tempo = std::max(tempo, 0.0);
  if (tempo_ == tempo) {
    return;
  }
  tempo_ = tempo;
  for (auto& instrument : instruments_) {
    instrument->SetTempo(tempo_);
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Musician::Update(double timestamp) noexcept {
  while (timestamp_ < timestamp) {
    if (tempo_ > 0.0) {
      std::pair<double, int> update_duration = {BeatsFromSeconds(tempo_, timestamp - timestamp_),
                                                std::numeric_limits<int>::max()};
      bool has_tasks_to_process = false;
      for (const auto& performer : performers_) {
        assert(performer);
        if (const auto maybe_duration = performer->GetDurationToNextTask();
            maybe_duration && maybe_duration < update_duration) {
          has_tasks_to_process = true;
          update_duration = *maybe_duration;
        }
      }

      if (update_duration.first > 0) {
        for (const auto& performer : performers_) {
          assert(performer);
          performer->Update(update_duration.first);
        }

        timestamp_ += SecondsFromBeats(tempo_, update_duration.first);
        for (auto& instrument : instruments_) {
          assert(instrument);
          instrument->Update(timestamp_);
        }
      }

      if (has_tasks_to_process && timestamp_ < timestamp) {
        for (const auto& performer : performers_) {
          assert(performer);
          performer->ProcessNextTaskAtPosition();
        }
      }
    } else if (timestamp_ < timestamp) {
      timestamp_ = timestamp;
      for (auto& instrument : instruments_) {
        assert(instrument);
        instrument->Update(timestamp_);
      }
    }
  }
}

}  // namespace barely::internal