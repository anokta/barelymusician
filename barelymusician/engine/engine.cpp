
#include "barelymusician/engine/engine.h"

#include <cassert>
#include <functional>
#include <utility>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/engine/id.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/engine/performer.h"
#include "barelymusician/engine/status.h"

namespace barely::internal {

namespace {

// Converts seconds to minutes.
constexpr double kMinutesFromSeconds = 1.0 / 60.0;

// Converts minutes to seconds.
constexpr double kSecondsFromMinutes = 60.0;

/// Returns corresponding beats for given `seconds` at `tempo`.
double GetBeats(double tempo, double seconds) noexcept {
  assert(tempo > 0.0);
  return tempo * seconds * kMinutesFromSeconds;
}

/// Returns corresponding seconds for given `beats` at `tempo`.
double GetSeconds(double tempo, double beats) noexcept {
  assert(tempo > 0.0);
  return beats * kSecondsFromMinutes / tempo;
}

}  // namespace

Engine::~Engine() noexcept {
  for (auto& [instrument_id, instrument] : instruments_) {
    instrument->SetAllNotesOff();
  }
  instrument_refs_.Update({});
}

// NOLINTNEXTLINE(bugprone-exception-escape)
StatusOr<Id> Engine::CreateInstrument(InstrumentDefinition definition,
                                      int frame_rate) noexcept {
  if (frame_rate < 0) return {Status::kInvalidArgument};
  const Id instrument_id = GenerateNextId();
  const auto [it, success] = instruments_.emplace(
      instrument_id, std::make_unique<Instrument>(definition, frame_rate));
  assert(success);
  it->second->Update(timestamp_);
  UpdateInstrumentReferenceMap();
  return instrument_id;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
StatusOr<Id> Engine::CreatePerformer(int order) noexcept {
  const Id performer_id = GenerateNextId();
  auto [it, success] =
      performers_.emplace(std::pair{order, performer_id}, Performer{});
  assert(success);
  success = performer_refs_
                .emplace(performer_id, std::pair{order, std::ref(it->second)})
                .second;
  assert(success);
  return performer_id;
}

StatusOr<Id> Engine::CreatePerformerTask(
    [[maybe_unused]] Id performer_id,
    [[maybe_unused]] TaskDefinition definition,
    [[maybe_unused]] double position, [[maybe_unused]] bool is_one_off,
    [[maybe_unused]] void* user_data) noexcept {
  if (performer_id == kInvalid) return Status::kInvalidArgument;
  if (position < 0.0) return Status::kInvalidArgument;
  // TODO(#109): Implement this.
  return {Status::kUnimplemented};
}

// NOLINTNEXTLINE(bugprone-exception-escape)
Status Engine::DestroyInstrument(Id instrument_id) noexcept {
  if (instrument_id == kInvalid) return Status::kInvalidArgument;
  if (const auto it = instruments_.find(instrument_id);
      it != instruments_.end()) {
    auto instrument = std::move(it->second);
    instrument->SetAllNotesOff();
    instruments_.erase(it);
    UpdateInstrumentReferenceMap();
    return Status::kOk;
  }
  return Status::kNotFound;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
Status Engine::DestroyPerformer(Id performer_id) noexcept {
  if (performer_id == kInvalid) return Status::kInvalidArgument;
  if (const auto it = performer_refs_.find(performer_id);
      it != performer_refs_.end()) {
    [[maybe_unused]] const bool success =
        performers_.erase(std::pair{it->second.first, performer_id}) > 0;
    assert(success);
    performer_refs_.erase(it);
    return Status::kOk;
  }
  return Status::kNotFound;
}

StatusOr<std::reference_wrapper<Instrument>> Engine::GetInstrument(
    Id instrument_id) noexcept {
  if (instrument_id == kInvalid) return {Status::kInvalidArgument};
  if (auto* instrument = FindOrNull(instruments_, instrument_id)) {
    return std::ref(*(*instrument));
  }
  return {Status::kNotFound};
}

StatusOr<std::reference_wrapper<Performer>> Engine::GetPerformer(
    [[maybe_unused]] Id performer_id) noexcept {
  if (performer_id == kInvalid) return {Status::kInvalidArgument};
  if (auto* performer_ref = FindOrNull(performer_refs_, performer_id)) {
    return performer_ref->second;
  }
  return {Status::kNotFound};
}

double Engine::GetTempo() const noexcept { return tempo_; }

double Engine::GetTimestamp() const noexcept { return timestamp_; }

Status Engine::ProcessInstrument(Id instrument_id, double* output_samples,
                                 int output_channel_count,
                                 int output_frame_count,
                                 double timestamp) noexcept {
  if (instrument_id == kInvalid) return Status::kInvalidArgument;
  if ((!output_samples && output_channel_count > 0 && output_frame_count) ||
      output_channel_count < 0 || output_frame_count < 0 || timestamp < 0.0) {
    return Status::kInvalidArgument;
  }
  auto instrument_refs = instrument_refs_.GetScopedView();
  if (const auto* instrument_ref =
          FindOrNull(*instrument_refs, instrument_id)) {
    assert(*instrument_ref);
    (*instrument_ref)
        ->Process(output_samples, output_channel_count, output_frame_count,
                  timestamp);
    return Status::kOk;
  }
  return Status::kNotFound;
}

void Engine::SetTempo(double tempo) noexcept { tempo_ = std::max(tempo, 0.0); }

// NOLINTNEXTLINE(bugprone-exception-escape)
void Engine::Update(double timestamp) noexcept {
  assert(timestamp >= 0.0);

  // while (timestamp_ < timestamp) {
  //   if (tempo_ > 0.0) {
  //     double update_duration = GetBeats(tempo_, timestamp - timestamp_);
  //   } else {
  //     // Playback is stopped.
  //     timestamp_ = timestamp;
  //   }
  //   for (auto& [instrument_id, instrument] : instruments_) {
  //     instrument->Update(timestamp);
  //   }
  // }

  for (auto& [instrument_id, instrument] : instruments_) {
    instrument->Update(timestamp);
  }

  // TODO(#109): Reenable after API cleanup. Check for `tempo_ > 0.0`.
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

Id Engine::GenerateNextId() noexcept {
  assert(id_counter_ >= 0);
  return ++id_counter_;
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
