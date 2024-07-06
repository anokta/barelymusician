#include "barelymusician/internal/musician.h"

#include <algorithm>
#include <cassert>
#include <limits>
#include <memory>
#include <utility>

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
Performer* Musician::CreatePerformer() noexcept {
  auto performer = std::make_unique<Performer>();
  const auto [it, success] = performers_.emplace(performer.get(), std::move(performer));
  assert(success);
  return it->second.get();
}

// NOLINTNEXTLINE(bugprone-exception-escape)
bool Musician::DestroyEffect(Effect* effect) noexcept { return effects_.erase(effect) > 0; }

// NOLINTNEXTLINE(bugprone-exception-escape)
bool Musician::DestroyInstrument(Instrument* instrument) noexcept {
  return instruments_.erase(instrument) > 0;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
bool Musician::DestroyPerformer(Performer* performer) noexcept {
  return performers_.erase(performer) > 0;
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

void Musician::SetTempo(double tempo) noexcept { tempo_ = std::max(tempo, 0.0); }

// NOLINTNEXTLINE(bugprone-exception-escape)
void Musician::Update(double timestamp) noexcept {
  while (timestamp_ < timestamp) {
    if (tempo_ > 0.0) {
      std::pair<double, int> update_duration = {GetBeatsFromSeconds(timestamp - timestamp_),
                                                std::numeric_limits<int>::lowest()};
      bool has_tasks_to_process = false;
      for (const auto& [performer_ptr, performer] : performers_) {
        assert(performer_ptr);
        if (const auto maybe_duration = performer_ptr->GetDurationToNextTask();
            maybe_duration && maybe_duration < update_duration) {
          has_tasks_to_process = true;
          update_duration = *maybe_duration;
        }
      }
      assert(update_duration.first > 0.0 || has_tasks_to_process);

      if (update_duration.first > 0) {
        for (const auto& [performer_ptr, performer] : performers_) {
          performer_ptr->Update(update_duration.first);
        }

        timestamp_ += GetSecondsFromBeats(update_duration.first);
        for (const auto& [effect_ptr, effect] : effects_) {
          effect_ptr->Update(timestamp_);
        }
        for (const auto& [instrument_ptr, instrument] : instruments_) {
          instrument_ptr->Update(timestamp_);
        }
      }

      if (has_tasks_to_process) {
        for (const auto& [performer_ptr, performer] : performers_) {
          performer_ptr->ProcessNextTaskAtPosition();
        }
      }
    } else if (timestamp_ < timestamp) {
      timestamp_ = timestamp;
      for (const auto& [effect_ptr, effect] : effects_) {
        effect_ptr->Update(timestamp_);
      }
      for (const auto& [instrument_ptr, instrument] : instruments_) {
        instrument_ptr->Update(timestamp_);
      }
    }
  }
}

}  // namespace barely::internal
