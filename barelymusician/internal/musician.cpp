#include "barelymusician/internal/musician.h"

#include <algorithm>
#include <cassert>
#include <limits>
#include <memory>
#include <utility>

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/effect.h"
#include "barelymusician/internal/instrument.h"
#include "barelymusician/internal/performer.h"
#include "barelymusician/internal/seconds.h"

namespace barely::internal {

// NOLINTNEXTLINE(bugprone-exception-escape)
Effect* Musician::CreateEffect(EffectDefinition definition, int frame_rate) noexcept {
  auto effect = std::make_unique<Effect>(definition, frame_rate, timestamp_);
  const auto [it, success] = effects_.emplace(effect.get(), std::move(effect));
  assert(success);
  return it->second.get();
}

// NOLINTNEXTLINE(bugprone-exception-escape)
Instrument* Musician::CreateInstrument(InstrumentDefinition definition, int frame_rate) noexcept {
  auto instrument = std::make_unique<Instrument>(definition, frame_rate, timestamp_);
  const auto [it, success] = instruments_.emplace(instrument.get(), std::move(instrument));
  assert(success);
  return it->second.get();
}

// NOLINTNEXTLINE(bugprone-exception-escape)
Performer* Musician::CreatePerformer(int process_order) noexcept {
  auto performer = std::make_unique<Performer>(process_order);
  const auto [it, success] =
      performers_.emplace(std::pair{process_order, performer.get()}, std::move(performer));
  assert(success);
  return it->second.get();
}

// NOLINTNEXTLINE(bugprone-exception-escape)
bool Musician::DestroyEffect(Effect* effect) noexcept {
  assert(effect);
  return effects_.erase(effect) > 0;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
bool Musician::DestroyInstrument(Instrument* instrument) noexcept {
  assert(instrument);
  return instruments_.erase(instrument) > 0;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
bool Musician::DestroyPerformer(Performer* performer) noexcept {
  assert(performer);
  return performers_.erase({performer->GetProcessOrder(), performer}) > 0;
}

double Musician::GetBeatsFromSeconds(double seconds) const noexcept {
  return BeatsFromSeconds(tempo_, seconds);
}

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
        for (const auto& [effect, _] : effects_) {
          effect->Update(timestamp_);
        }
        for (const auto& [instrument, _] : instruments_) {
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
      for (const auto& [effect, _] : effects_) {
        effect->Update(timestamp_);
      }
      for (const auto& [instrument, _] : instruments_) {
        instrument->Update(timestamp_);
      }
    }
  }
}

}  // namespace barely::internal
