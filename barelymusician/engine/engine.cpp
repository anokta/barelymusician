
#include "barelymusician/engine/engine.h"

#include <cassert>
#include <limits>
#include <utility>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/engine/id.h"
#include "barelymusician/engine/instrument.h"

// TODO(#109): Reenable after API cleanup.
// #include "barelymusician/engine/performer.h"

namespace barely::internal {

namespace {

// Converts seconds to minutes.
constexpr double kMinutesFromSeconds = 1.0 / 60.0;

// Converts minutes to seconds.
constexpr double kSecondsFromMinutes = 60.0;

}  // namespace

Engine::~Engine() noexcept {
  for (auto& [instrument_id, instrument] : instruments_) {
    instrument->SetAllNotesOff(timestamp_);
  }
  instrument_refs_.Update({});
}

// NOLINTNEXTLINE(bugprone-exception-escape)
bool Engine::CreateInstrument(Id instrument_id,
                              Instrument::Definition definition,
                              int frame_rate) noexcept {
  assert(instrument_id > kInvalid);
  assert(frame_rate >= 0);
  if (instruments_
          .emplace(instrument_id,
                   std::make_unique<Instrument>(definition, frame_rate))
          .second) {
    UpdateInstrumentReferenceMap();
    return true;
  }
  return false;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
bool Engine::CreatePerformer([[maybe_unused]] Id performer_id,
                             [[maybe_unused]] int priority) noexcept {
  assert(performer_id > kInvalid);
  // if (const auto [it, success] =
  //         performers_.emplace(std::pair{priority, performer_id},
  //         Performer{});
  //     success) {
  //   performer_refs_.emplace(performer_id, std::pair{priority, &it->second});
  //   return true;
  // }
  return false;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
bool Engine::DestroyInstrument(Id instrument_id) noexcept {
  if (const auto it = instruments_.find(instrument_id);
      it != instruments_.end()) {
    auto instrument = std::move(it->second);
    instrument->SetAllNotesOff(timestamp_);
    instruments_.erase(it);
    UpdateInstrumentReferenceMap();
    return true;
  }
  return false;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
bool Engine::DestroyPerformer([[maybe_unused]] Id performer_id) noexcept {
  // if (const auto it = performer_refs_.find(performer_id);
  //     it != performer_refs_.end()) {
  //   performers_.erase(std::pair{it->second.first, performer_id});
  //   performer_refs_.erase(it);
  //   return true;
  // }
  return false;
}

double Engine::GetBeats(double seconds) const noexcept {
  return tempo_ * seconds * kMinutesFromSeconds;
}

Instrument* Engine::GetInstrument(Id instrument_id) noexcept {
  if (auto* instrument = FindOrNull(instruments_, instrument_id)) {
    return instrument->get();
  }
  return nullptr;
}

Performer* Engine::GetPerformer([[maybe_unused]] Id performer_id) noexcept {
  // if (auto* performer_ref = FindOrNull(performer_refs_, performer_id)) {
  //   return performer_ref->second;
  // }
  return nullptr;
}

double Engine::GetSeconds(double beats) const noexcept {
  return (tempo_ > 0.0)  ? beats * kSecondsFromMinutes / tempo_
         : (beats > 0.0) ? std::numeric_limits<double>::max()
         : (beats < 0.0) ? std::numeric_limits<double>::lowest()
                         : 0.0;
}

double Engine::GetTempo() const noexcept { return tempo_; }

void Engine::SetTempo(double tempo) noexcept {
  assert(tempo_ >= 0.0);
  tempo_ = tempo;
}

double Engine::GetTimestamp() const noexcept { return timestamp_; }

bool Engine::ProcessInstrument(Id instrument_id, double* output_samples,
                               int output_channel_count, int output_frame_count,
                               double timestamp) noexcept {
  assert(output_samples || output_channel_count == 0 ||
         output_frame_count == 0);
  assert(output_channel_count >= 0);
  assert(output_frame_count >= 0);
  assert(timestamp >= 0.0);
  auto instrument_refs = instrument_refs_.GetScopedView();
  if (const auto* instrument_ref =
          FindOrNull(*instrument_refs, instrument_id)) {
    assert(*instrument_ref);
    (*instrument_ref)
        ->Process(output_samples, output_channel_count, output_frame_count,
                  timestamp);
    return true;
  }
  return false;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Engine::Update(double timestamp) noexcept {
  assert(timestamp >= 0.0);

  // TODO(#109): Reenable after API cleanup.
  // while (timestamp_ < timestamp) {
  //   double update_duration = GetBeats(timestamp - timestamp_);
  //   bool has_tasks_to_trigger = false;
  //   for (const auto& [priority_id_pair, performer] : performers_) {
  //     if (const double duration = performer.GetDurationToNextTask();
  //         duration < update_duration) {
  //       update_duration = duration;
  //       has_tasks_to_trigger = true;
  //     }
  //   }

  //   timestamp_ += GetSeconds(update_duration);
  //   for (auto& [priority_id_pair, performer] : performers_) {
  //     performer.Update(update_duration);
  //   }

  //   if (has_tasks_to_trigger) {
  //     for (auto& [priority_id_pair, performer] : performers_) {
  //       if (update_duration + performer.GetDurationToNextTask() <=
  //           performer.GetPosition()) {
  //         performer.TriggerAllTasksAtCurrentPosition();
  //       }
  //     }
  //   }
  // }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Engine::UpdateInstrumentReferenceMap() noexcept {
  InstrumentReferenceMap new_instrument_refs;
  new_instrument_refs.reserve(instruments_.size());
  for (const auto& [instrument_id, instrument] : instruments_) {
    new_instrument_refs.emplace(instrument_id, instrument.get());
  }
  instrument_refs_.Update(std::move(new_instrument_refs));
}

}  // namespace barely::internal
